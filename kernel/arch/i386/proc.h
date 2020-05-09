/**
 * proc.h: Process management
 */

#ifndef _KERNEL_PROC_H
#define _KERNEL_PROC_H

#include "std.h"

typedef uint16_t pid_t; // Process ID type
typedef uint16_t pc_t;  // Process ID type

// Process constants
enum {
    PID_MAX = 1 << (sizeof(pid_t) * BITS_PER_BYTE),
};

typedef enum {
    PROC_DEAD = 0,      // Process is dead, i.e. non-existent
    PROC_SLEEPING,      // Process is halted, e.g. waiting for I/O
    PROC_ACTIVE,        // Process is active but waiting for execution
    PROC_RUNNING        // Process is active and currently executing
} proc_state_t;

// Process context
// TODO include all other registers available to user processes
typedef struct {
    reg_t ds;
    reg_t es;
    reg_t fs;
    reg_t gs;
    reg_t eax;
    reg_t ecx;
    reg_t edx;
    reg_t ebx;
    reg_t ebp;
    reg_t esi;
    reg_t edi;
    reg_t eip;
    reg_t cs;
    reg_t eflags;
    reg_t esp;          // Only present on privilege change
    reg_t ss;           // Only present on privilege change
} proc_ctxt_t;

// This node is for the linked list tracking process pages and page tables
typedef struct proc_page_node {
    struct proc_page_node *next;
    uintptr_t page_dir_idx;
    uintptr_t page_table_vma;
    uintptr_t page_dir_entry;
} proc_page_node_t;

// Process table entry
typedef struct {
    uint64_t inode_id;              // Inode ID of program
    time_t start_time;              // Process start time (for elapsed execution time)
    proc_page_node_t *page_tables;  // Linked list of process page tables
    proc_state_t state;             // Process state
    proc_ctxt_t ctxt;               // Process context (saved machine state)
    pc_t exec_count;                // Number of execution time slices remaining
    pc_t priority;                  // Number of execution time slices allowed
    pid_t ppid;                     // Parent Process ID
} proc_t;

// Process queue node
typedef struct pid_node {
    struct pid_node *next;
    pid_t pid;
} pid_node_t;

// Global functions
void proc_dump_queue(void);
pid_t proc_get_pid(void);
void proc_info(pid_t pid);
void proc_init(void);
void proc_load_ctxt(proc_ctxt_t *ctxt);
void proc_loop(void);
void proc_next(void);
void proc_store_ctxt(proc_ctxt_t *ctxt);

#endif // _KERNEL_PROC_H
