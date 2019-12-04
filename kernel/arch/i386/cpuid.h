/**
 * cpuid.h: CPUID and other feature-detection routines
 *
 * For documentation on the CPUID instruction, see Intel Software Developer's
 * Manual, vol. 2, Section 3.2, "CPUID"
 */

#ifndef _KERNEL_CPUID_H
#define _KERNEL_CPUID_H

#include "std.h"

// eax: 0x01
typedef struct
{
    // eax
    uint32_t    stepping_id         : 4;
    uint32_t    model               : 4;
    uint32_t    family_id           : 4;
    uint32_t    processor_type      : 2;
    uint32_t    _reserved1          : 2;
    uint32_t    extended_model_id   : 4;
    uint32_t    extended_family_id  : 8;

    // ebx
    uint8_t     brand_index;
    uint8_t     clflush_line_size;
    uint8_t     max_ids;
    uint8_t     initial_apic_id;

    // ecx
    uint32_t    sse3                : 1;
    uint32_t    pclmulqdq           : 1;
    uint32_t    dtest64             : 1;
    uint32_t    monitor             : 1;
    uint32_t    ds_cpl              : 1;
    uint32_t    vmx                 : 1;
    uint32_t    smx                 : 1;
    uint32_t    eist                : 1;
    uint32_t    tm2                 : 1;
    uint32_t    ssse3               : 1;
    uint32_t    cnxt_id             : 1;
    uint32_t    sdbg                : 1;
    uint32_t    fma                 : 1;
    uint32_t    cmpxchg16b          : 1;
    uint32_t    xtpr_update_control : 1;
    uint32_t    pdcm                : 1;
    uint32_t    _reserved2          : 1;
    uint32_t    pcid                : 1;
    uint32_t    dca                 : 1;
    uint32_t    sse4_1              : 1;
    uint32_t    sse4_2              : 1;
    uint32_t    x2apic              : 1;
    uint32_t    movbe               : 1;
    uint32_t    popcnt              : 1;
    uint32_t    tsc_deadline        : 1;
    uint32_t    aesni               : 1;
    uint32_t    xsave               : 1;
    uint32_t    osxsave             : 1;
    uint32_t    avx                 : 1;
    uint32_t    f16c                : 1;
    uint32_t    rdrand              : 1;
    uint32_t    _unused             : 1;

    // edx
    uint32_t    fpu         : 1; // Floating point unit
    uint32_t    vme         : 1; // Virtual 8086 mode enhancements
    uint32_t    de          : 1; // Debugging extensions
    uint32_t    pse         : 1; // Page size extension
    uint32_t    tsc         : 1; // Time stamp counter
    uint32_t    msr         : 1; // Model specific registers (rdmsr, wrmsr instructions)
    uint32_t    pae         : 1; // Physical address extension
    uint32_t    mce         : 1; // Machine check exception
    uint32_t    cx8         : 1; // cmpxchg8b instruction
    uint32_t    apic        : 1; // APIC on-chip
    uint32_t    _reserved3  : 1;
    uint32_t    sep         : 1; // sysenter and sysexit instructions
    uint32_t    mtrr        : 1; // Memory type range registers
    uint32_t    pge         : 1; // Page global bit
    uint32_t    mca         : 1; // Machine check architecture
    uint32_t    cmov        : 1; // Conditional move instruction
    uint32_t    pat         : 1; // Page attribute table
    uint32_t    pse_36      : 1; // 36-bit page size extension
    uint32_t    psn         : 1; // Processor serial number
    uint32_t    clfsh       : 1; // clflush instruction
    uint32_t    _reserved4  : 1;
    uint32_t    ds          : 1; // Debug store
    uint32_t    acpi        : 1; // Thermal monitor, software controlled clock facilities
    uint32_t    mmx         : 1; // Intel MMX
    uint32_t    fxsr        : 1; // fxsave, fxrstor instructions
    uint32_t    sse         : 1; // SSE
    uint32_t    sse2        : 1; // SSE2
    uint32_t    ss          : 1; // Self snoop
    uint32_t    htt         : 1; // Max APIC IDs reserved field is valid
    uint32_t    tm          : 1; // Thermal monitor
    uint32_t    _reserved5  : 1;
    uint32_t    pbe         : 1; // Pending break enable

} cpuid_version_t;

typedef struct
{
    // eax
    uint32_t    temp_sensor                 : 1; // Digital temperature sensor
    uint32_t    turbo_boost                 : 1; // Intel Turbo Boost
    uint32_t    arat                        : 1; // APIC-Timer-always-running
    uint32_t    _reserved                   : 1;
    uint32_t    pln                         : 1; // Power limit notification controls
    uint32_t    ecmd                        : 1; // Clock modulation duty cycle extension
    uint32_t    ptm                         : 1; // Package thermal management
    uint32_t    hwp                         : 1; // HWP base registers
    uint32_t    hwp_notification            : 1; // IA32_HWP_INTERRUPT MSR
    uint32_t    hwp_activity_window         : 1; // IA32_HWP_REQUEST[41:32]
    uint32_t    hwp_energy_perf_preference  : 1; // IA32_HWP_REQUEST[31:24]
    uint32_t    hwp_package_level_request   : 1; // IA32_HWP_REQUEST_PKG MSR
    uint32_t    _reserved2                  : 1;
    uint32_t    hdc                         : 1; // HDC base registers
    uint32_t    turbo_boost3                : 1; // Intel Turbo Boost 3.0
    uint32_t    hwp_capabilities            : 1; // Highest performance change
    uint32_t    hwp_peci_override           : 1; // HWP PECI override
    uint32_t    hwp_flexible                : 1; // Flexible HWP
    uint32_t    hwp_fast_access             : 1; // Fast access mode for IA32_HWP_REQUEST
    uint32_t    _reserved3                  : 1;
    uint32_t    hwp_request_ignore          : 1; // Ignore idle logical processor HWP request
    uint32_t    _reserved4                  : 11;

    // ebx
    uint32_t    num_interrupt_thresholds    : 4;// for digital thermal sensor
    uint32_t    _reserved5                  : 28;

    // ecx
    uint32_t    hardware_coordination_feedback  : 1;
    uint32_t    _reserved6                      : 2;
    uint32_t    perf_energy_bias_preference     : 1;
    uint32_t    _reserved7                      : 28;

    // edx
    uint32_t    _reserved8;
} cpuid_thermal_t;

// EAX 0
static inline void cpuid_id_string(size_t *max_input, char *id_string_buffer)
{
    asm (
        "movl $0, %%eax\n\t"
        "cpuid\n\t"
        "movl %%eax, %0\n\t"
        "movl %%ebx, %1\n\t"
        "movl %%edx, %2\n\t"
        "movl %%ecx, %3\n\t"
        : "=rm" ( *max_input ),
          "=rm" ( ((reg32_t*)id_string_buffer)[0] ),
          "=rm" ( ((reg32_t*)id_string_buffer)[1] ),
          "=rm" ( ((reg32_t*)id_string_buffer)[2] )
        : // No inputs
        : "eax", "ebx", "edx", "ecx"
    );
    id_string_buffer[3 * sizeof(reg_t)] = 0;
}

// EAX 1
static inline void cpuid_version(cpuid_version_t *version_info)
{
    asm (
        "movl $1, %%eax\n\t"
        "cpuid\n\t"
        "movl %%eax, %0\n\t"
        "movl %%ebx, %1\n\t"
        "movl %%ecx, %2\n\t"
        "movl %%edx, %3\n\t"
        : "=rm" ( ((reg32_t*)version_info)[0] ),
          "=rm" ( ((reg32_t*)version_info)[1] ),
          "=rm" ( ((reg32_t*)version_info)[2] ),
          "=rm" ( ((reg32_t*)version_info)[3] )
        : // No inputs
        : "eax", "ebx", "ecx", "edx"
    );
}

// EAX 6
static inline void cpuid_thermal(cpuid_thermal_t *thermal_info)
{
    asm (
        "movl $6, %%eax\n\t"
        "cpuid\n\t"
        "movl %%eax, %0\n\t"
        "movl %%ebx, %1\n\t"
        "movl %%ecx, %2\n\t"
        "movl %%edx, %3\n\t"
        : "=rm" ( ((reg32_t*)thermal_info)[0] ),
          "=rm" ( ((reg32_t*)thermal_info)[1] ),
          "=rm" ( ((reg32_t*)thermal_info)[2] ),
          "=rm" ( ((reg32_t*)thermal_info)[3] )
        : // No inputs
        : "eax", "ebx", "ecx", "edx"
    );
}

#endif // _KERNEL_CPUID_H
