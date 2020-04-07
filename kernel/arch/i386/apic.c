/**
 * apic.c: Advanced Programmable Interrupt Controller
 */

#include "alloc.h"
#include "apic.h"
#include "asm.h"
#include "cpuid.h"
#include "interrupt.h"
#include "io.h"
#include "page.h"

static void *lapic_base_vma;
static uintptr_t lapic_base_pma;
static bool get_page_first_run = true;

static void lapic_timer_init(uint32_t period)
{
    apic_lvt_reg_t volatile *reg = lapic_base_vma;

    printk("lapic_timer_init: LAPIC base: %p, pma: %p\n", reg, page_get_pma((uintptr_t)reg));
    apic_lvt_reg_t ver = reg[APIC_VER_IDX];
    printk("lapic_timer_init: LAPIC version: %x, max lvt: %d\n", ver.ver.version, ver.ver.max_lvt_entry);

    cpuid_thermal_t thermal_info;
    cpuid_thermal(&thermal_info);

    if (!thermal_info.arat) {
        printk("lapic_timer_init: WARNING: APIC timer is not persistent\n");
    }

    printk("lapic_timer_init: period: %u\n", period);

    printk("lapic_timer_init: A TR: %p\n", reg[APIC_LVT_TR_IDX]);

    apic_lvt_reg_t timer = {
        .lvt = {
            .vector = IDT_VECTOR_TIMER,
            .timer_mode = APIC_TIMER_PERIODIC,
        }
    };
    reg[APIC_LVT_TR_IDX] = timer;

    printk("lapic_timer_init: B TR: %p\n", reg[APIC_LVT_TR_IDX]);
    printk("lapic_timer_init: B TR.vector: %u\n", reg[APIC_LVT_TR_IDX].lvt.vector);

    // Set divide configuration register
    reg[APIC_DIV_CONFIG_IDX].timer.divide = APIC_TIMER_DIV128;
    printk("lapic_timer_init: Timer: %u, %u\n", reg[APIC_DIV_CONFIG_IDX].timer.divide, APIC_TIMER_DIV16);

    // Set initial-count register
    reg[APIC_INIT_COUNT_IDX].raw = period;

    sti();
}

// Disable Intel 8259 PIC
static void pic_disable(void)
{
    // TODO place this in a header file
    const int PIC_DATA_MASTER = 0x21;
    const int PIC_DATA_SLAVE = 0xa1;

    /**
     * Supposedly this disables the Intel 8259. Good riddance, too; there's
     * absolutely zero clear documentation anywhere explaining what the below
     * code even does.
     */
    outb(PIC_DATA_MASTER, 0xff);
    outb(PIC_DATA_SLAVE, 0xff);
}

// Send EOI
void lapic_eoi(void)
{
    apic_lvt_reg_t volatile *reg = lapic_base_vma;
    reg[APIC_EOI_IDX].raw = 0;
}

uintptr_t lvt_get_page_pma(void)
{
    if (get_page_first_run) {
        get_page_first_run = false;
        return lapic_base_pma;
    }
    return PAGE_GET_DEFAULT();
}

void apic_init(void)
{
    pic_disable();
    printk("apic_init: PIC has been disabled\n");

    // TODO if the below checks fail, configure the legacy x86 PIC instead of
    // disabling it above
    cpuid_version_t ver;
    cpuid_version(&ver);

    if (!ver.apic) {
        printk("apic_init: FATAL: No local APIC available\n");
        die();
    }
    if (!ver.msr) {
        printk("apic_init: FATAL: No MSRs available\n");
        die();
    }

    apic_base_msr_t msr;

    get_msr(&msr, IA32_APIC_BASE_MSR);

    // TODO is this the right approach for an MP system? Do this earlier?
    // Halt processor if we are not on the bootstrap processor
    if (!msr.bsp) {
        printk("apic_init: Halting core (not bootstrap)\n");
        halt();
    }

    // Calculate local APIC register physical base address from MSR
    lapic_base_pma = (uintptr_t)msr.base << 12;

    // Map page lapic registers
    lapic_base_vma = kalloc(&lvt_get_page_pma, PAGE_SIZE, PAGE_SIZE);

    lapic_timer_init(0x10000);
}
