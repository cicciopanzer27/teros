/**
 * @file syscall.c
 * @brief System Call Implementation with Lambda³ Integration
 * @author TEROS Development Team
 * @date 2025
 */

#include "syscall.h"
#include "process.h"
#include "scheduler.h"
#include "console.h"
#include "kmalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// SYSTEM CALL IMPLEMENTATION
// =============================================================================

#define MAX_SYSCALLS 256
#define MAX_SYSCALL_ARGS 6

// System call handler function pointer
typedef trit_t (*syscall_handler_t)(uint32_t arg0, uint32_t arg1, uint32_t arg2, 
                                   uint32_t arg3, uint32_t arg4, uint32_t arg5);

// System call table entry
typedef struct {
    syscall_handler_t handler;
    const char* name;
    uint32_t arg_count;
    bool privileged;
} syscall_entry_t;

typedef struct {
    syscall_entry_t syscalls[MAX_SYSCALLS];
    uint32_t syscall_count;
    uint32_t total_syscalls;
    uint32_t failed_syscalls;
    bool initialized;
} syscall_state_t;

static syscall_state_t syscall_state;

// =============================================================================
// SYSTEM CALL INITIALIZATION
// =============================================================================

void syscall_init(void) {
    if (syscall_state.initialized) {
        return;
    }
    
    console_puts("SYSCALL: Initializing System Call Handler...\n");
    
    // Initialize syscall state
    memset(&syscall_state, 0, sizeof(syscall_state_t));
    
    // Initialize syscall table
    for (int i = 0; i < MAX_SYSCALLS; i++) {
        syscall_state.syscalls[i].handler = NULL;
        syscall_state.syscalls[i].name = "UNKNOWN";
        syscall_state.syscalls[i].arg_count = 0;
        syscall_state.syscalls[i].privileged = false;
    }
    
    // Register system calls
    syscall_register_handlers();
    
    syscall_state.initialized = true;
    console_puts("SYSCALL: System Call Handler initialized\n");
}

void syscall_register_handlers(void) {
    console_puts("SYSCALL: Registering system call handlers...\n");
    
    // Process management syscalls
    syscall_register(SYS_EXIT, syscall_exit, "exit", 1, false);
    syscall_register(SYS_FORK, syscall_fork, "fork", 0, false);
    syscall_register(SYS_EXEC, syscall_exec, "exec", 3, false);
    syscall_register(SYS_WAIT, syscall_wait, "wait", 1, false);
    syscall_register(SYS_GETPID, syscall_getpid, "getpid", 0, false);
    syscall_register(SYS_GETPPID, syscall_getppid, "getppid", 0, false);
    
    // Memory management syscalls
    syscall_register(SYS_MMAP, syscall_mmap, "mmap", 6, false);
    syscall_register(SYS_MUNMAP, syscall_munmap, "munmap", 2, false);
    syscall_register(SYS_BRK, syscall_brk, "brk", 1, false);
    
    // File system syscalls
    syscall_register(SYS_OPEN, syscall_open, "open", 3, false);
    syscall_register(SYS_CLOSE, syscall_close, "close", 1, false);
    syscall_register(SYS_READ, syscall_read, "read", 3, false);
    syscall_register(SYS_WRITE, syscall_write, "write", 3, false);
    syscall_register(SYS_LSEEK, syscall_lseek, "lseek", 3, false);
    syscall_register(SYS_STAT, syscall_stat, "stat", 2, false);
    
    // Directory syscalls
    syscall_register(SYS_OPENDIR, syscall_opendir, "opendir", 1, false);
    syscall_register(SYS_READDIR, syscall_readdir, "readdir", 1, false);
    syscall_register(SYS_CLOSEDIR, syscall_closedir, "closedir", 1, false);
    syscall_register(SYS_MKDIR, syscall_mkdir, "mkdir", 2, false);
    syscall_register(SYS_RMDIR, syscall_rmdir, "rmdir", 1, false);
    
    // Signal syscalls
    syscall_register(SYS_KILL, syscall_kill, "kill", 2, false);
    syscall_register(SYS_SIGNAL, syscall_signal, "signal", 2, false);
    syscall_register(SYS_SIGACTION, syscall_sigaction, "sigaction", 3, false);
    
    // IPC syscalls
    syscall_register(SYS_PIPE, syscall_pipe, "pipe", 1, false);
    syscall_register(SYS_SHMGET, syscall_shmget, "shmget", 3, false);
    syscall_register(SYS_SHMAT, syscall_shmat, "shmat", 3, false);
    syscall_register(SYS_SHMDT, syscall_shmdt, "shmdt", 1, false);
    
    // Lambda³ syscalls
    syscall_register(SYS_LAMBDA_REDUCE, syscall_lambda_reduce, "lambda_reduce", 2, false);
    syscall_register(SYS_LAMBDA_TYPECHECK, syscall_lambda_typecheck, "lambda_typecheck", 2, false);
    syscall_register(SYS_LAMBDA_EVAL, syscall_lambda_eval, "lambda_eval", 3, false);
    syscall_register(SYS_LAMBDA_PARSE, syscall_lambda_parse, "lambda_parse", 2, false);
    syscall_register(SYS_LAMBDA_COMPILE, syscall_lambda_compile, "lambda_compile", 2, false);
    syscall_register(SYS_LAMBDA_OPTIMIZE, syscall_lambda_optimize, "lambda_optimize", 2, false);
    syscall_register(SYS_LAMBDA_PROVE, syscall_lambda_prove, "lambda_prove", 3, false);
    syscall_register(SYS_LAMBDA_VERIFY, syscall_lambda_verify, "lambda_verify", 2, false);
    
    console_puts("SYSCALL: System call handlers registered\n");
}

bool syscall_register(uint32_t syscall_num, syscall_handler_t handler, 
                     const char* name, uint32_t arg_count, bool privileged) {
    if (syscall_num >= MAX_SYSCALLS) {
        console_puts("SYSCALL: ERROR - Invalid syscall number\n");
        return false;
    }
    
    syscall_entry_t* entry = &syscall_state.syscalls[syscall_num];
    entry->handler = handler;
    entry->name = name;
    entry->arg_count = arg_count;
    entry->privileged = privileged;
    
    syscall_state.syscall_count++;
    
    console_puts("SYSCALL: Registered ");
    console_puts(name);
    console_puts(" (");
    printf("%u", syscall_num);
    console_puts(")\n");
    
    return true;
}

// =============================================================================
// SYSTEM CALL DISPATCHER
// =============================================================================

trit_t syscall_dispatch(uint32_t syscall_num, uint32_t arg0, uint32_t arg1, 
                        uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    if (!syscall_state.initialized) {
        console_puts("SYSCALL: ERROR - System call handler not initialized\n");
        return TERNARY_NEGATIVE;
    }
    
    if (syscall_num >= MAX_SYSCALLS) {
        console_puts("SYSCALL: ERROR - Invalid syscall number\n");
        syscall_state.failed_syscalls++;
        return TERNARY_NEGATIVE;
    }
    
    syscall_entry_t* entry = &syscall_state.syscalls[syscall_num];
    
    if (entry->handler == NULL) {
        console_puts("SYSCALL: ERROR - Unimplemented syscall\n");
        syscall_state.failed_syscalls++;
        return TERNARY_NEGATIVE;
    }
    
    // Check privilege level
    if (entry->privileged) {
        process_t* current = process_get_current();
        if (current == NULL || current->pid != 0) {
            console_puts("SYSCALL: ERROR - Privileged syscall from user process\n");
            syscall_state.failed_syscalls++;
            return TERNARY_NEGATIVE;
        }
    }
    
    // Update statistics
    syscall_state.total_syscalls++;
    
    // Call handler
    trit_t result = entry->handler(arg0, arg1, arg2, arg3, arg4, arg5);
    
    console_puts("SYSCALL: ");
    console_puts(entry->name);
    console_puts(" called, result: ");
    printf("%d", result);
    console_puts("\n");
    
    return result;
}

// =============================================================================
// PROCESS MANAGEMENT SYSCALLS
// =============================================================================

trit_t syscall_exit(uint32_t exit_code, uint32_t arg1, uint32_t arg2, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    process_t* current = process_get_current();
    if (current == NULL) {
        return TERNARY_NEGATIVE;
    }
    
    console_puts("SYSCALL: Process ");
    console_puts(current->name);
    console_puts(" exiting with code ");
    printf("%u", exit_code);
    console_puts("\n");
    
    // Terminate process
    process_terminate(current, exit_code);
    
    // Remove from scheduler
    scheduler_remove_process(current);
    
    // Schedule next process
    scheduler_schedule();
    
    return TERNARY_POSITIVE;
}

trit_t syscall_fork(uint32_t arg0, uint32_t arg1, uint32_t arg2, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    process_t* current = process_get_current();
    if (current == NULL) {
        return TERNARY_NEGATIVE;
    }
    
    console_puts("SYSCALL: Forking process ");
    console_puts(current->name);
    console_puts("\n");
    
    // Create child process
    process_t* child = process_create(current->name, current->pid);
    if (child == NULL) {
        console_puts("SYSCALL: ERROR - Failed to create child process\n");
        return TERNARY_NEGATIVE;
    }
    
    // Copy parent's memory (simplified)
    // In a real system, we would implement copy-on-write
    
    // Add child to scheduler
    scheduler_add_process(child);
    
    // Return child PID to parent, 0 to child
    if (current->pid == 0) {
        return TERNARY_POSITIVE; // Child process
    } else {
        return (trit_t)child->pid; // Parent process
    }
}

trit_t syscall_exec(uint32_t filename, uint32_t argv, uint32_t envp, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    process_t* current = process_get_current();
    if (current == NULL) {
        return TERNARY_NEGATIVE;
    }
    
    console_puts("SYSCALL: Executing program\n");
    
    // Load new program
    if (!process_load_program(current, (const char*)filename)) {
        console_puts("SYSCALL: ERROR - Failed to load program\n");
        return TERNARY_NEGATIVE;
    }
    
    // Execute program
    if (!process_execute(current)) {
        console_puts("SYSCALL: ERROR - Failed to execute program\n");
        return TERNARY_NEGATIVE;
    }
    
    return TERNARY_POSITIVE;
}

trit_t syscall_wait(uint32_t pid, uint32_t arg1, uint32_t arg2, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    process_t* current = process_get_current();
    if (current == NULL) {
        return TERNARY_NEGATIVE;
    }
    
    console_puts("SYSCALL: Waiting for process ");
    printf("%u", pid);
    console_puts("\n");
    
    // Find child process
    process_t* child = process_find_by_pid(pid);
    if (child == NULL) {
        console_puts("SYSCALL: ERROR - Child process not found\n");
        return TERNARY_NEGATIVE;
    }
    
    // Wait for child to terminate
    while (!process_is_terminated(child)) {
        // Yield CPU
        scheduler_yield();
    }
    
    // Get exit code
    uint32_t exit_code = process_get_exit_code(child);
    
    // Destroy child process
    process_destroy(child);
    
    return (trit_t)exit_code;
}

trit_t syscall_getpid(uint32_t arg0, uint32_t arg1, uint32_t arg2, 
                      uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    process_t* current = process_get_current();
    if (current == NULL) {
        return TERNARY_NEGATIVE;
    }
    
    return (trit_t)current->pid;
}

trit_t syscall_getppid(uint32_t arg0, uint32_t arg1, uint32_t arg2, 
                       uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    process_t* current = process_get_current();
    if (current == NULL) {
        return TERNARY_NEGATIVE;
    }
    
    return (trit_t)current->ppid;
}

// =============================================================================
// MEMORY MANAGEMENT SYSCALLS
// =============================================================================

trit_t syscall_mmap(uint32_t addr, uint32_t length, uint32_t prot, 
                    uint32_t flags, uint32_t fd, uint32_t offset) {
    process_t* current = process_get_current();
    if (current == NULL) {
        return TERNARY_NEGATIVE;
    }
    
    console_puts("SYSCALL: mmap called\n");
    
    // Allocate memory
    void* ptr = kmalloc(length);
    if (ptr == NULL) {
        console_puts("SYSCALL: ERROR - Failed to allocate memory\n");
        return TERNARY_NEGATIVE;
    }
    
    // Update process memory usage
    current->memory_usage += length;
    
    return (trit_t)(uintptr_t)ptr;
}

trit_t syscall_munmap(uint32_t addr, uint32_t length, uint32_t arg2, 
                      uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    process_t* current = process_get_current();
    if (current == NULL) {
        return TERNARY_NEGATIVE;
    }
    
    console_puts("SYSCALL: munmap called\n");
    
    // Free memory
    kfree((void*)addr);
    
    // Update process memory usage
    current->memory_usage -= length;
    
    return TERNARY_POSITIVE;
}

trit_t syscall_brk(uint32_t brk, uint32_t arg1, uint32_t arg2, 
                   uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    process_t* current = process_get_current();
    if (current == NULL) {
        return TERNARY_NEGATIVE;
    }
    
    console_puts("SYSCALL: brk called\n");
    
    // Set new heap pointer
    current->heap_ptr = brk;
    
    return TERNARY_POSITIVE;
}

// =============================================================================
// FILE SYSTEM SYSCALLS
// =============================================================================

trit_t syscall_open(uint32_t filename, uint32_t flags, uint32_t mode, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    process_t* current = process_get_current();
    if (current == NULL) {
        return TERNARY_NEGATIVE;
    }
    
    console_puts("SYSCALL: open called\n");
    
    // Find free file descriptor
    int fd = -1;
    for (int i = 0; i < 32; i++) {
        if (current->file_descriptors[i] == 0) {
            fd = i;
            break;
        }
    }
    
    if (fd == -1) {
        console_puts("SYSCALL: ERROR - No free file descriptors\n");
        return TERNARY_NEGATIVE;
    }
    
    // Allocate file descriptor
    current->file_descriptors[fd] = 1; // Simplified
    current->fd_count++;
    
    return (trit_t)fd;
}

trit_t syscall_close(uint32_t fd, uint32_t arg1, uint32_t arg2, 
                     uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    process_t* current = process_get_current();
    if (current == NULL) {
        return TERNARY_NEGATIVE;
    }
    
    console_puts("SYSCALL: close called\n");
    
    if (fd >= 32 || current->file_descriptors[fd] == 0) {
        console_puts("SYSCALL: ERROR - Invalid file descriptor\n");
        return TERNARY_NEGATIVE;
    }
    
    // Close file descriptor
    current->file_descriptors[fd] = 0;
    current->fd_count--;
    
    return TERNARY_POSITIVE;
}

trit_t syscall_read(uint32_t fd, uint32_t buf, uint32_t count, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    process_t* current = process_get_current();
    if (current == NULL) {
        return TERNARY_NEGATIVE;
    }
    
    console_puts("SYSCALL: read called\n");
    
    if (fd >= 32 || current->file_descriptors[fd] == 0) {
        console_puts("SYSCALL: ERROR - Invalid file descriptor\n");
        return TERNARY_NEGATIVE;
    }
    
    // Simplified read implementation
    // In a real system, we would read from the actual file
    
    return (trit_t)count;
}

trit_t syscall_write(uint32_t fd, uint32_t buf, uint32_t count, 
                     uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    process_t* current = process_get_current();
    if (current == NULL) {
        return TERNARY_NEGATIVE;
    }
    
    console_puts("SYSCALL: write called\n");
    
    if (fd >= 32 || current->file_descriptors[fd] == 0) {
        console_puts("SYSCALL: ERROR - Invalid file descriptor\n");
        return TERNARY_NEGATIVE;
    }
    
    // Write to console for stdout/stderr
    if (fd == 1 || fd == 2) {
        console_puts((const char*)buf);
    }
    
    return (trit_t)count;
}

// =============================================================================
// LAMBDA³ SYSCALLS
// =============================================================================

trit_t syscall_lambda_reduce(uint32_t expr, uint32_t steps, uint32_t arg2, 
                            uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    console_puts("SYSCALL: Lambda³ reduce called\n");
    
    // Lambda³ reduction implementation
    // This will be implemented with the Lambda³ engine
    
    return TERNARY_POSITIVE;
}

trit_t syscall_lambda_typecheck(uint32_t expr, uint32_t type, uint32_t arg2, 
                               uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    console_puts("SYSCALL: Lambda³ typecheck called\n");
    
    // Lambda³ type checking implementation
    // This will be implemented with the Lambda³ engine
    
    return TERNARY_POSITIVE;
}

trit_t syscall_lambda_eval(uint32_t expr, uint32_t env, uint32_t steps, 
                          uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    console_puts("SYSCALL: Lambda³ eval called\n");
    
    // Lambda³ evaluation implementation
    // This will be implemented with the Lambda³ engine
    
    return TERNARY_POSITIVE;
}

trit_t syscall_lambda_parse(uint32_t input, uint32_t output, uint32_t arg2, 
                           uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    console_puts("SYSCALL: Lambda³ parse called\n");
    
    // Lambda³ parsing implementation
    // This will be implemented with the Lambda³ engine
    
    return TERNARY_POSITIVE;
}

trit_t syscall_lambda_compile(uint32_t expr, uint32_t output, uint32_t arg2, 
                             uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    console_puts("SYSCALL: Lambda³ compile called\n");
    
    // Lambda³ compilation implementation
    // This will be implemented with the Lambda³ engine
    
    return TERNARY_POSITIVE;
}

trit_t syscall_lambda_optimize(uint32_t expr, uint32_t output, uint32_t arg2, 
                              uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    console_puts("SYSCALL: Lambda³ optimize called\n");
    
    // Lambda³ optimization implementation
    // This will be implemented with the Lambda³ engine
    
    return TERNARY_POSITIVE;
}

trit_t syscall_lambda_prove(uint32_t expr, uint32_t theorem, uint32_t proof, 
                           uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    console_puts("SYSCALL: Lambda³ prove called\n");
    
    // Lambda³ proof implementation
    // This will be implemented with the Lambda³ engine
    
    return TERNARY_POSITIVE;
}

trit_t syscall_lambda_verify(uint32_t proof, uint32_t theorem, uint32_t arg2, 
                            uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    console_puts("SYSCALL: Lambda³ verify called\n");
    
    // Lambda³ verification implementation
    // This will be implemented with the Lambda³ engine
    
    return TERNARY_POSITIVE;
}

// =============================================================================
// SYSCALL QUERY FUNCTIONS
// =============================================================================

uint32_t syscall_get_total_syscalls(void) {
    return syscall_state.total_syscalls;
}

uint32_t syscall_get_failed_syscalls(void) {
    return syscall_state.failed_syscalls;
}

uint32_t syscall_get_syscall_count(void) {
    return syscall_state.syscall_count;
}

bool syscall_is_initialized(void) {
    return syscall_state.initialized;
}

// =============================================================================
// SYSCALL DEBUG FUNCTIONS
// =============================================================================

void syscall_print_statistics(void) {
    if (!syscall_state.initialized) {
        console_puts("SYSCALL: Not initialized\n");
        return;
    }
    
    console_puts("SYSCALL: Statistics:\n");
    console_puts("  Total syscalls: ");
    printf("%u", syscall_state.total_syscalls);
    console_puts("\n");
    console_puts("  Failed syscalls: ");
    printf("%u", syscall_state.failed_syscalls);
    console_puts("\n");
    console_puts("  Registered syscalls: ");
    printf("%u", syscall_state.syscall_count);
    console_puts("\n");
    
    console_puts("  Registered syscalls:\n");
    for (int i = 0; i < MAX_SYSCALLS; i++) {
        if (syscall_state.syscalls[i].handler != NULL) {
            console_puts("    ");
            printf("%d", i);
            console_puts(": ");
            console_puts(syscall_state.syscalls[i].name);
            console_puts("\n");
        }
    }
}
