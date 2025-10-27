/**
 * @file process.h
 * @brief Process Control Block (PCB) and process management
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Process states (ternary values)
#define PROCESS_STATE_RUNNING  1
#define PROCESS_STATE_READY    0
#define PROCESS_STATE_BLOCKED  -1
#define PROCESS_STATE_ZOMBIE   -2
#define PROCESS_STATE_NEW      2

// Priority levels (ternary)
#define PRIORITY_LOW    -1
#define PRIORITY_NORMAL 0
#define PRIORITY_HIGH   1

// Process ID
typedef uint32_t pid_t;

// =============================================================================
// PROCESS CONTROL BLOCK STRUCTURE
// =============================================================================

// Register context for TVM
typedef struct {
    uint32_t r0, r1, r2, r3, r4, r5, r6, r7;  // General purpose registers
    uint32_t pc;   // Program counter
    uint32_t sp;   // Stack pointer
    uint32_t fp;   // Frame pointer
    uint32_t lr;   // Link register
    uint32_t cr;   // Condition register
    uint32_t acc;  // Accumulator
    uint32_t tmp;  // Temporary
    uint32_t zero; // Zero register
    uint32_t flags; // Flags register
} process_context_t;

// Process Control Block
typedef struct process {
    pid_t pid;
    pid_t ppid;                    // Parent PID
    int32_t state;                 // Process state (ternary)
    int32_t priority;              // Priority (ternary)
    
    // Context
    process_context_t context;
    uint32_t kernel_stack;         // Kernel stack pointer
    uint32_t user_stack;           // User stack pointer
    uint32_t heap_base;            // Heap base address
    uint32_t heap_size;            // Heap size
    uint32_t code_base;            // Code base address
    uint32_t code_size;            // Code size
    
    // Memory management
    void* page_directory;          // Page directory (for VMM)
    uint32_t allocated_pages;      // Number of allocated pages
    
    // File descriptors
    void* file_descriptors[64];    // FD table (simplified)
    uint32_t fd_count;
    
    // IPC
    void* ipc_channels;            // IPC channels
    void* shared_memory;           // Shared memory regions
    
    // Timing
    uint32_t ticks_total;          // Total CPU ticks
    uint32_t ticks_recent;         // Recent CPU ticks
    uint32_t created_time;         // Creation timestamp
    uint32_t sleep_until;          // Wake time for sleeping processes
    
    // Security
    uint32_t uid;                  // User ID
    uint32_t gid;                  // Group ID
    uint32_t capabilities;         // Capabilities bitmap
    
    // Process tree
    struct process* parent;
    struct process* children;
    struct process* sibling;
    
    // Scheduling
    struct process* next;          // Next process in queue
    struct process* prev;          // Previous process in queue
    
    // Stats
    uint32_t context_switches;     // Number of context switches
    uint32_t syscalls_count;       // Number of syscalls made
} process_t;

// =============================================================================
// PROCESS MANAGEMENT API
// =============================================================================

/**
 * @brief Initialize process management system
 */
void process_init(void);

/**
 * @brief Create a new process
 * @param code_address Code start address
 * @param code_size Code size in bytes
 * @param priority Process priority
 * @return Pointer to created process, or NULL on failure
 */
process_t* process_create(uint32_t code_address, uint32_t code_size, int32_t priority);

/**
 * @brief Terminate a process
 * @param process Process to terminate
 * @return Exit code
 */
int32_t process_terminate(process_t* process);

/**
 * @brief Destroy process and free resources
 * @param process Process to destroy
 */
void process_destroy(process_t* process);

/**
 * @brief Get current running process
 * @return Pointer to current process
 */
process_t* process_get_current(void);

/**
 * @brief Get process by PID
 * @param pid Process ID
 * @return Pointer to process, or NULL if not found
 */
process_t* process_get_by_pid(pid_t pid);

/**
 * @brief Switch to a different process
 * @param new_process Process to switch to
 */
void process_switch(process_t* new_process);

/**
 * @brief Block current process
 */
void process_block(void);

/**
 * @brief Unblock a process
 * @param process Process to unblock
 */
void process_unblock(process_t* process);

/**
 * @brief Sleep for specified ticks
 * @param ticks Number of ticks to sleep
 */
void process_sleep(uint32_t ticks);

/**
 * @brief Wake up processes that are ready
 */
void process_wakeup(void);

/**
 * @brief Set process priority
 * @param process Process to modify
 * @param priority New priority
 */
void process_set_priority(process_t* process, int32_t priority);

/**
 * @brief Get process state
 * @param process Process to query
 * @return Process state
 */
int32_t process_get_state(process_t* process);

/**
 * @brief Add process to ready queue
 * @param process Process to add
 */
void process_add_to_queue(process_t* process);

/**
 * @brief Remove process from queue
 * @param process Process to remove
 */
void process_remove_from_queue(process_t* process);

/**
 * @brief Print process information
 * @param process Process to print
 */
void process_print_info(process_t* process);

/**
 * @brief Print all processes
 */
void process_print_all(void);

// Global variables (extern)
extern process_t* current_process;
extern process_t* ready_queue;

#endif // PROCESS_H

