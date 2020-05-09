/**
 * proc.c: Process management
 */

#include "asm.h"
#include "alloc.h"
#include "apic.h"
#include "gdt.h"
#include "io.h"
#include "mem.h"
#include "page.h"
#include "proc.h"

static proc_t *proc_table;              // Process table
static pid_t proc_num;                  // Number of registered processes
static pid_t PID = 0;                   // Current PID
static pid_node_t *proc_queue_start;    // Process scheduling queue (start)
static pid_node_t *proc_queue_end;      // Process scheduling queue (end)

// Static functions
static void proc_mem_map(pid_t pid);
static void proc_mem_unmap(pid_t pid);
static pid_t proc_new_pid(void);
static inline bool proc_pid_taken(pid_t pid);
static void proc_print_ctxt(proc_ctxt_t *ctxt);
static pid_t proc_register(void (*entry_point)(void), pc_t priority);
static void proc1(void);
static void proc2(void);
static void proc3(void);

pid_t proc_get_pid(void)
{
    return PID;
}

void proc_store_ctxt(proc_ctxt_t *ctxt)
{
    proc_table[PID].ctxt = *ctxt;
}

void proc_load_ctxt(proc_ctxt_t *ctxt)
{
    *ctxt = proc_table[PID].ctxt;
}

// Schedule the next process
void proc_next(void)
{
    // Perform context switch if necessary
    if (--proc_table[PID].exec_count == 0) {

        // Reset execution count
        proc_table[PID].exec_count = proc_table[PID].priority;

        // Place current pid to end of queue
        proc_queue_end->next = proc_queue_start;
        proc_queue_end = proc_queue_start;
        proc_queue_start = proc_queue_end->next;
        proc_queue_end->next = NULL;

        // Update process state
        proc_table[PID].state = PROC_ACTIVE;

        // Clean current process memory map
        proc_mem_unmap(PID);

        // Get next PID from start of the queue
        PID = proc_queue_start->pid;

        // Setup next process memory map
        proc_mem_map(PID);

        // Invalidate TLB cache
        set_cr3(get_cr3());

        // Set next process to RUNNING
        proc_table[PID].state = PROC_RUNNING;
    }
}

// Map process memory space
static void proc_mem_map(pid_t pid)
{
    // Map process page tables
    proc_page_node_t *n = proc_table[pid].page_tables;
    while (n) {
        page_set_dir_entry(n->page_dir_idx, n->page_table_vma, n->page_dir_entry);
        n = n->next;
    }
}

// Unmap process memory space
static void proc_mem_unmap(pid_t pid)
{
    // Unmap process page tables
    proc_page_node_t *n = proc_table[pid].page_tables;
    while (n) {
        // Wipe page directory entry
        page_set_dir_entry(n->page_dir_idx, (page_entry_t)0, (page_entry_t)0);
        n = n->next;
    }
}

// Return whether pid is taken
static inline bool proc_pid_taken(pid_t pid)
{
    return (bool)proc_table[pid].state;
}

// Return next available PID
static pid_t proc_new_pid(void)
{
    static pid_t next_pid = 0;
    while(proc_pid_taken(next_pid))
        next_pid++;
    return next_pid++;
}

static void proc_page_table_register(
                pid_t pid, uintptr_t table_vma, uintptr_t space_vma, uintptr_t flags)
{
    // Prepend new node to page_tables linked list
    proc_page_node_t *table_node = kmalloc(sizeof(*table_node));
    *table_node = (proc_page_node_t) {
        .next = proc_table[pid].page_tables,
        .page_dir_idx = space_vma >> 22,
        .page_dir_entry = page_get_pma(table_vma) | flags,
        .page_table_vma = table_vma,
    };
    proc_table[pid].page_tables = table_node;
}

static void proc_page_register(uintptr_t table_vma, uintptr_t vma, page_entry_t entry)
{
    ((page_entry_t*)table_vma)[vma >> 12 & 0x3ff] = entry;
}

// Add pid to process queue
static void proc_queue_add(pid_t pid)
{
    if (proc_queue_end) {
        pid_node_t *old_end = proc_queue_end;
        proc_queue_end = kmalloc(sizeof(pid_node_t));
        old_end->next = proc_queue_end;
    } else {
        proc_queue_start = kmalloc(sizeof(pid_node_t));
        proc_queue_end = proc_queue_start;
    }

    proc_queue_end->pid = pid;
    proc_queue_end->next = NULL;

    proc_num++;
}

// Register process for execution in process table
// TODO switch from entry point to inode_t parameter
static pid_t proc_register(void (*entry_point)(void), pc_t priority)
{
    pid_t pid = proc_new_pid();

    proc_t *proc = &proc_table[pid];

    proc->ctxt.eip = (uintptr_t)entry_point;
    proc->ctxt.ss = GDT_SEL_DATA_PL3;
    proc->ctxt.gs = GDT_SEL_DATA_PL3;
    proc->ctxt.fs = GDT_SEL_DATA_PL3;
    proc->ctxt.es = GDT_SEL_DATA_PL3;
    proc->ctxt.ds = GDT_SEL_DATA_PL3;
    proc->ctxt.cs = GDT_SEL_CODE_PL3;
    proc->ctxt.esp = KERNEL_START_VMA;
    proc->ctxt.eflags = 0x3202;  // Set IF flag, IOPL = 3

    proc->state = PROC_ACTIVE;
    proc->exec_count = priority;
    proc->priority = priority;

    // Register memory space

    // Map page table for user stack
    uintptr_t table = (uintptr_t)kalloc(PAGE_GET_DEFAULT, PAGE_SIZE, PAGE_SIZE);
    const uintptr_t stack_page_vma = proc->ctxt.esp - PAGE_SIZE;
    const uintptr_t user_data_flags = PAGE_PUBLIC | PAGE_WRITE | PAGE_PRESENT;

    proc_page_table_register(pid, table, stack_page_vma, user_data_flags);
    proc_page_register(table, stack_page_vma, PAGE_GET_DEFAULT() | user_data_flags);

    // Add to process queue
    proc_queue_add(pid);

    return pid;
}

// Test process
static void proc1(void)
{
    while (1) {
        printk("1");
    }
}

// Test process
static void proc2(void)
{
    while (1) {
        printk("2");
    }
}

// Test process
static void proc3(void)
{
    while (1) {
        printk("3");
    }
}

proc_ctxt_t * proc_get_ctxt(pid_t pid)
{
    return &proc_table[pid].ctxt;
}

void proc_dump_queue(void)
{
    pid_node_t *node = proc_queue_start;
    printk("proc_dump_queue:\n");
    while (node) {
        printk("    PID: %u, \n", node->pid);
        node = node->next;
    }
}

static void proc_print_ctxt(proc_ctxt_t *ctxt)
{
    printk("proc_print_ctxt:\n");
    printk("     ss: %p\n", ctxt->ss);
    printk("    esp: %p\n", ctxt->esp);
    printk("  flags: %p\n", ctxt->eflags);
    printk("     cs: %p\n", ctxt->cs);
    printk("    eip: %p\n", ctxt->eip);
    printk("    edi: %p\n", ctxt->edi);
    printk("    esi: %p\n", ctxt->esi);
    printk("    ebp: %p\n", ctxt->ebp);
    printk("    ebx: %p\n", ctxt->ebx);
    printk("    edx: %p\n", ctxt->edx);
    printk("    ecx: %p\n", ctxt->ecx);
    printk("    eax: %p\n", ctxt->eax);
    printk("     gs: %p\n", ctxt->gs);
    printk("     fs: %p\n", ctxt->fs);
    printk("     es: %p\n", ctxt->es);
    printk("     ds: %p\n", ctxt->ds);
}

void proc_info(pid_t pid)
{
    char const *state[] = {
        "DEAD",
        "SLEEPING",
        "ACTIVE",
        "RUNNING"
    };

    proc_t proc = proc_table[pid];

    printk("proc_info (%u):\n", pid);
    printk("    ppid: %u\n", proc.ppid);
    printk("    inode_id: %p\n", proc.inode_id);
    printk("    start_time: %u\n", proc.start_time);
    printk("    state: %s\n", state[proc.state]);
    printk("    exec_count: %u\n", proc.exec_count);
    proc_print_ctxt(&proc_table[pid].ctxt);
}

void proc_init(void)
{
    // Allocate process table
    proc_table = kmalloc(PID_MAX * sizeof(proc_t));

    // Initialize process queue
    proc_queue_start = NULL;
    proc_queue_end = NULL;

    // Map page for scheduler stack
    const void *stack_page = kalloc(PAGE_GET_DEFAULT, PAGE_SIZE, PAGE_SIZE);

    // Configure TSS
    TSS.esp0 = (uintptr_t)stack_page + PAGE_SIZE;

    // Register kernel at PID zero
    proc_t *proc_kernel = &proc_table[0];
    proc_kernel->ppid = 0;
    proc_kernel->state = PROC_RUNNING;
    proc_kernel->exec_count = 1;
    proc_kernel->priority = 10;

    // Add kernel to execution queue
    proc_queue_add(0);
    proc_num--; // We don't want the kernel to count as a running process

    proc_register(&proc1, 30);
    proc_register(&proc2, 10);
    proc_register(&proc3, 10);
}

// Process scheduling loop
void proc_loop(void)
{
    // TODO
    //(void)proc_dump_queue;

    // The loop runs so long as there are processes registered
    // PID 0 can be thought of as a system idler. The priority of PID 0 can be
    // increased during runtime for power management
    sti();
    while (proc_num) {
        halt();
    }
}
