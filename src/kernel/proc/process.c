/**
 * @file process.c
 * @brief Process Control Block (PCB) and Process Management
 * @author TEROS Development Team
 * @date 2025
 */

#include "process.h"
#include "tvm.h"
#include "kmalloc.h"
#include "console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// PROCESS MANAGEMENT IMPLEMENTATION
// =============================================================================

#define MAX_PROCESSES 1024
#define MAX_PROCESS_NAME 64
#define MAX_PROCESS_ARGS 16
#define MAX_PROCESS_ENV 32

// Process states (ternary)
typedef enum {
    PROCESS_STATE_RUNNING = 1,    // Positive (1)
    PROCESS_STATE_READY = 0,      // Zero (0)
    PROCESS_STATE_BLOCKED = -1    // Negative (-1)
} process_state_t;

// Process structure
typedef struct {
    uint32_t pid;                    // Process ID
    uint32_t ppid;                   // Parent Process ID
    char name[MAX_PROCESS_NAME];     // Process name
    process_state_t state;          // Process state (ternary)
    int32_t priority;               // Process priority (-1, 0, 1)
    uint32_t start_time;            // Process start time
    uint32_t cpu_time;              // CPU time used
    uint32_t memory_usage;          // Memory usage in bytes
    tvm_t* tvm;                     // Ternary Virtual Machine
    uint32_t stack_ptr;             // Stack pointer
    uint32_t heap_ptr;              // Heap pointer
    uint32_t code_start;            // Code start address
    uint32_t code_size;             // Code size
    uint32_t data_start;            // Data start address
    uint32_t data_size;             // Data size
    uint32_t bss_start;             // BSS start address
    uint32_t bss_size;              // BSS size
    char** argv;                    // Command line arguments
    char** envp;                    // Environment variables
    uint32_t argc;                  // Argument count
    uint32_t envc;                  // Environment count
    uint32_t exit_code;             // Exit code
    bool terminated;                // Termination flag
    uint32_t signal_mask;           // Signal mask
    uint32_t pending_signals;       // Pending signals
    uint32_t file_descriptors[32];  // File descriptors
    uint32_t fd_count;              // File descriptor count
} process_t;

typedef struct {
    process_t* processes[MAX_PROCESSES];
    uint32_t process_count;
    uint32_t next_pid;
    process_t* current_process;
    uint32_t kernel_process_pid;
    bool initialized;
} process_manager_t;

static process_manager_t proc_mgr;

// =============================================================================
// PROCESS MANAGER INITIALIZATION
// =============================================================================

void process_init(void) {
    if (proc_mgr.initialized) {
        return;
    }
    
    console_puts("PROC: Initializing Process Manager...\n");
    
    // Initialize process manager
    memset(&proc_mgr, 0, sizeof(process_manager_t));
    
    // Initialize process array
    for (int i = 0; i < MAX_PROCESSES; i++) {
        proc_mgr.processes[i] = NULL;
    }
    
    // Set next PID
    proc_mgr.next_pid = 1;
    
    proc_mgr.initialized = true;
    console_puts("PROC: Process Manager initialized\n");
}

// =============================================================================
// PROCESS CREATION AND DESTRUCTION
// =============================================================================

process_t* process_create(const char* name, uint32_t ppid) {
    if (!proc_mgr.initialized) {
        return NULL;
    }
    
    // Find free slot
    int slot = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (proc_mgr.processes[i] == NULL) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        console_puts("PROC: ERROR - No free process slots\n");
        return NULL;
    }
    
    // Allocate process structure
    process_t* proc = (process_t*)kmalloc(sizeof(process_t));
    if (proc == NULL) {
        console_puts("PROC: ERROR - Failed to allocate process structure\n");
        return NULL;
    }
    
    // Initialize process
    memset(proc, 0, sizeof(process_t));
    
    proc->pid = proc_mgr.next_pid++;
    proc->ppid = ppid;
    strncpy(proc->name, name, MAX_PROCESS_NAME - 1);
    proc->name[MAX_PROCESS_NAME - 1] = '\0';
    proc->state = PROCESS_STATE_READY;
    proc->priority = 0; // Normal priority
    proc->start_time = timer_get_ticks();
    proc->cpu_time = 0;
    proc->memory_usage = 0;
    proc->terminated = false;
    proc->exit_code = 0;
    proc->signal_mask = 0;
    proc->pending_signals = 0;
    proc->fd_count = 0;
    
    // Initialize file descriptors
    for (int i = 0; i < 32; i++) {
        proc->file_descriptors[i] = 0;
    }
    
    // Create TVM for process
    proc->tvm = tvm_create();
    if (proc->tvm == NULL) {
        console_puts("PROC: ERROR - Failed to create TVM\n");
        kfree(proc);
        return NULL;
    }
    
    // Allocate memory for process
    proc->stack_ptr = (uint32_t)kmalloc(8192); // 8KB stack
    if (proc->stack_ptr == 0) {
        console_puts("PROC: ERROR - Failed to allocate stack\n");
        tvm_destroy(proc->tvm);
        kfree(proc);
        return NULL;
    }
    
    proc->heap_ptr = (uint32_t)kmalloc(4096); // 4KB heap
    if (proc->heap_ptr == 0) {
        console_puts("PROC: ERROR - Failed to allocate heap\n");
        kfree((void*)proc->stack_ptr);
        tvm_destroy(proc->tvm);
        kfree(proc);
        return NULL;
    }
    
    // Update memory usage
    proc->memory_usage = 8192 + 4096 + sizeof(process_t);
    
    // Add to process list
    proc_mgr.processes[slot] = proc;
    proc_mgr.process_count++;
    
    console_puts("PROC: Created process ");
    console_puts(proc->name);
    console_puts(" (PID: ");
    printf("%u", proc->pid);
    console_puts(")\n");
    
    return proc;
}

void process_destroy(process_t* proc) {
    if (proc == NULL) {
        return;
    }
    
    console_puts("PROC: Destroying process ");
    console_puts(proc->name);
    console_puts(" (PID: ");
    printf("%u", proc->pid);
    console_puts(")\n");
    
    // Free process memory
    if (proc->stack_ptr != 0) {
        kfree((void*)proc->stack_ptr);
    }
    
    if (proc->heap_ptr != 0) {
        kfree((void*)proc->heap_ptr);
    }
    
    if (proc->code_start != 0) {
        kfree((void*)proc->code_start);
    }
    
    if (proc->data_start != 0) {
        kfree((void*)proc->data_start);
    }
    
    if (proc->bss_start != 0) {
        kfree((void*)proc->bss_start);
    }
    
    // Free command line arguments
    if (proc->argv != NULL) {
        for (uint32_t i = 0; i < proc->argc; i++) {
            if (proc->argv[i] != NULL) {
                kfree(proc->argv[i]);
            }
        }
        kfree(proc->argv);
    }
    
    // Free environment variables
    if (proc->envp != NULL) {
        for (uint32_t i = 0; i < proc->envc; i++) {
            if (proc->envp[i] != NULL) {
                kfree(proc->envp[i]);
            }
        }
        kfree(proc->envp);
    }
    
    // Destroy TVM
    if (proc->tvm != NULL) {
        tvm_destroy(proc->tvm);
    }
    
    // Remove from process list
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (proc_mgr.processes[i] == proc) {
            proc_mgr.processes[i] = NULL;
            proc_mgr.process_count--;
            break;
        }
    }
    
    // Free process structure
    kfree(proc);
}

// =============================================================================
// PROCESS STATE MANAGEMENT
// =============================================================================

void process_set_state(process_t* proc, process_state_t state) {
    if (proc == NULL) {
        return;
    }
    
    process_state_t old_state = proc->state;
    proc->state = state;
    
    console_puts("PROC: Process ");
    console_puts(proc->name);
    console_puts(" state changed from ");
    printf("%d", old_state);
    console_puts(" to ");
    printf("%d", state);
    console_puts("\n");
}

process_state_t process_get_state(process_t* proc) {
    if (proc == NULL) {
        return PROCESS_STATE_BLOCKED;
    }
    
    return proc->state;
}

void process_set_priority(process_t* proc, int32_t priority) {
    if (proc == NULL) {
        return;
    }
    
    // Clamp priority to ternary range
    if (priority < -1) priority = -1;
    if (priority > 1) priority = 1;
    
    proc->priority = priority;
    
    console_puts("PROC: Process ");
    console_puts(proc->name);
    console_puts(" priority set to ");
    printf("%d", priority);
    console_puts("\n");
}

int32_t process_get_priority(process_t* proc) {
    if (proc == NULL) {
        return 0;
    }
    
    return proc->priority;
}

// =============================================================================
// PROCESS QUERY FUNCTIONS
// =============================================================================

process_t* process_find_by_pid(uint32_t pid) {
    if (!proc_mgr.initialized) {
        return NULL;
    }
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (proc_mgr.processes[i] != NULL && proc_mgr.processes[i]->pid == pid) {
            return proc_mgr.processes[i];
        }
    }
    
    return NULL;
}

process_t* process_get_current(void) {
    return proc_mgr.current_process;
}

void process_set_current(process_t* proc) {
    proc_mgr.current_process = proc;
}

uint32_t process_get_count(void) {
    return proc_mgr.process_count;
}

uint32_t process_get_next_pid(void) {
    return proc_mgr.next_pid;
}

// =============================================================================
// PROCESS EXECUTION
// =============================================================================

bool process_load_program(process_t* proc, const char* filename) {
    if (proc == NULL || filename == NULL) {
        return false;
    }
    
    console_puts("PROC: Loading program ");
    console_puts(filename);
    console_puts(" into process ");
    console_puts(proc->name);
    console_puts("\n");
    
    // This is a simplified implementation
    // In a real system, we would load the executable file
    
    // Allocate code section
    proc->code_start = (uint32_t)kmalloc(4096);
    if (proc->code_start == 0) {
        console_puts("PROC: ERROR - Failed to allocate code section\n");
        return false;
    }
    
    proc->code_size = 4096;
    
    // Allocate data section
    proc->data_start = (uint32_t)kmalloc(1024);
    if (proc->data_start == 0) {
        console_puts("PROC: ERROR - Failed to allocate data section\n");
        kfree((void*)proc->code_start);
        return false;
    }
    
    proc->data_size = 1024;
    
    // Allocate BSS section
    proc->bss_start = (uint32_t)kmalloc(512);
    if (proc->bss_start == 0) {
        console_puts("PROC: ERROR - Failed to allocate BSS section\n");
        kfree((void*)proc->code_start);
        kfree((void*)proc->data_start);
        return false;
    }
    
    proc->bss_size = 512;
    
    // Update memory usage
    proc->memory_usage += proc->code_size + proc->data_size + proc->bss_size;
    
    // Load program into TVM
    if (!tvm_load_program(proc->tvm, (void*)proc->code_start, proc->code_size)) {
        console_puts("PROC: ERROR - Failed to load program into TVM\n");
        kfree((void*)proc->code_start);
        kfree((void*)proc->data_start);
        kfree((void*)proc->bss_start);
        return false;
    }
    
    console_puts("PROC: Program loaded successfully\n");
    return true;
}

bool process_execute(process_t* proc) {
    if (proc == NULL) {
        return false;
    }
    
    if (proc->state != PROCESS_STATE_READY) {
        console_puts("PROC: ERROR - Process not ready for execution\n");
        return false;
    }
    
    console_puts("PROC: Executing process ");
    console_puts(proc->name);
    console_puts("\n");
    
    // Set process as running
    process_set_state(proc, PROCESS_STATE_RUNNING);
    
    // Execute in TVM
    trit_t result = tvm_execute(proc->tvm);
    
    // Update CPU time
    proc->cpu_time += tvm_get_instructions_executed(proc->tvm);
    
    // Check execution result
    if (result == TERNARY_NEGATIVE) {
        console_puts("PROC: Process execution failed\n");
        process_set_state(proc, PROCESS_STATE_BLOCKED);
        return false;
    }
    
    console_puts("PROC: Process execution completed\n");
    return true;
}

// =============================================================================
// PROCESS TERMINATION
// =============================================================================

void process_terminate(process_t* proc, uint32_t exit_code) {
    if (proc == NULL) {
        return;
    }
    
    console_puts("PROC: Terminating process ");
    console_puts(proc->name);
    console_puts(" (PID: ");
    printf("%u", proc->pid);
    console_puts(") with exit code ");
    printf("%u", exit_code);
    console_puts("\n");
    
    proc->exit_code = exit_code;
    proc->terminated = true;
    process_set_state(proc, PROCESS_STATE_BLOCKED);
    
    // Notify parent process
    process_t* parent = process_find_by_pid(proc->ppid);
    if (parent != NULL) {
        // Send signal to parent
        process_send_signal(parent, SIGCHLD);
    }
}

bool process_is_terminated(process_t* proc) {
    if (proc == NULL) {
        return true;
    }
    
    return proc->terminated;
}

uint32_t process_get_exit_code(process_t* proc) {
    if (proc == NULL) {
        return 0;
    }
    
    return proc->exit_code;
}

// =============================================================================
// PROCESS SIGNALS
// =============================================================================

void process_send_signal(process_t* proc, uint32_t signal) {
    if (proc == NULL) {
        return;
    }
    
    // Check if signal is masked
    if (proc->signal_mask & (1 << signal)) {
        return;
    }
    
    // Set pending signal
    proc->pending_signals |= (1 << signal);
    
    console_puts("PROC: Signal ");
    printf("%u", signal);
    console_puts(" sent to process ");
    console_puts(proc->name);
    console_puts("\n");
}

void process_handle_signals(process_t* proc) {
    if (proc == NULL) {
        return;
    }
    
    // Handle pending signals
    for (uint32_t i = 0; i < 32; i++) {
        if (proc->pending_signals & (1 << i)) {
            // Clear pending signal
            proc->pending_signals &= ~(1 << i);
            
            // Handle signal
            process_handle_signal(proc, i);
        }
    }
}

void process_handle_signal(process_t* proc, uint32_t signal) {
    if (proc == NULL) {
        return;
    }
    
    console_puts("PROC: Handling signal ");
    printf("%u", signal);
    console_puts(" for process ");
    console_puts(proc->name);
    console_puts("\n");
    
    switch (signal) {
        case SIGTERM:
        case SIGKILL:
            process_terminate(proc, 128 + signal);
            break;
        case SIGCHLD:
            // Child process terminated
            break;
        default:
            // Default signal handling
            break;
    }
}

// =============================================================================
// PROCESS DEBUG FUNCTIONS
// =============================================================================

void process_print_info(process_t* proc) {
    if (proc == NULL) {
        console_puts("PROC: NULL process\n");
        return;
    }
    
    console_puts("PROC: Process Information:\n");
    console_puts("  PID: ");
    printf("%u", proc->pid);
    console_puts("\n");
    console_puts("  PPID: ");
    printf("%u", proc->ppid);
    console_puts("\n");
    console_puts("  Name: ");
    console_puts(proc->name);
    console_puts("\n");
    console_puts("  State: ");
    printf("%d", proc->state);
    console_puts("\n");
    console_puts("  Priority: ");
    printf("%d", proc->priority);
    console_puts("\n");
    console_puts("  Start time: ");
    printf("%u", proc->start_time);
    console_puts("\n");
    console_puts("  CPU time: ");
    printf("%u", proc->cpu_time);
    console_puts("\n");
    console_puts("  Memory usage: ");
    printf("%u", proc->memory_usage);
    console_puts(" bytes\n");
    console_puts("  Terminated: ");
    console_puts(proc->terminated ? "Yes" : "No");
    console_puts("\n");
    console_puts("  Exit code: ");
    printf("%u", proc->exit_code);
    console_puts("\n");
}

void process_print_all(void) {
    if (!proc_mgr.initialized) {
        console_puts("PROC: Process manager not initialized\n");
        return;
    }
    
    console_puts("PROC: All Processes:\n");
    console_puts("  Total processes: ");
    printf("%u", proc_mgr.process_count);
    console_puts("\n");
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (proc_mgr.processes[i] != NULL) {
            process_print_info(proc_mgr.processes[i]);
            console_puts("\n");
        }
    }
}

bool process_is_initialized(void) {
    return proc_mgr.initialized;
}
