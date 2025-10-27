/**
 * @file trap.c
 * @brief Trap and Signal Handling
 * @author TEROS Development Team
 * @date 2025
 */

#include "trap.h"
#include "process.h"
#include "scheduler.h"
#include "console.h"
#include "kmalloc.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_SIGNALS 64
#define MAX_SIGNAL_HANDLERS_PER_PROCESS 32

typedef struct signal_handler {
    void (*handler)(int sig);
    bool pending;
} signal_handler_t;

typedef struct signal_state {
    signal_handler_t handlers[MAX_SIGNALS];
    sigset_t pending_signals;
    sigset_t masked_signals;
    sigset_t handled_signals;
} signal_state_t;

static signal_state_t process_signals[MAX_PROCESSES];
static bool trap_system_initialized = false;

int trap_init(void) {
    if (trap_system_initialized) {
        return 0;
    }
    
    console_puts("TRAP: Initializing trap and signal system...\n");
    
    // Initialize signal states for all processes
    for (int i = 0; i < MAX_PROCESSES; i++) {
        memset(&process_signals[i], 0, sizeof(signal_state_t));
    }
    
    trap_system_initialized = true;
    console_puts("TRAP: Initialized\n");
    
    return 0;
}

int trap_register_handler(pid_t pid, int sig, void (*handler)(int)) {
    if (pid < 0 || pid >= MAX_PROCESSES) {
        console_puts("TRAP: ERROR - Invalid PID\n");
        return -1;
    }
    
    if (sig < 0 || sig >= MAX_SIGNALS) {
        console_puts("TRAP: ERROR - Invalid signal number\n");
        return -1;
    }
    
    if (handler == NULL) {
        console_puts("TRAP: ERROR - Null signal handler\n");
        return -1;
    }
    
    process_signals[pid].handlers[sig].handler = handler;
    process_signals[pid].handlers[sig].pending = false;
    
    return 0;
}

int trap_send_signal(pid_t pid, int sig) {
    if (pid < 0 || pid >= MAX_PROCESSES) {
        console_puts("TRAP: ERROR - Invalid PID\n");
        return -1;
    }
    
    if (sig < 0 || sig >= MAX_SIGNALS) {
        console_puts("TRAP: ERROR - Invalid signal number\n");
        return -1;
    }
    
    // Mark signal as pending
    process_signals[pid].pending_signals |= (1ULL << sig);
    process_signals[pid].handlers[sig].pending = true;
    
    return 0;
}

int trap_handle_pending_signals(pid_t pid) {
    if (pid < 0 || pid >= MAX_PROCESSES) {
        return -1;
    }
    
    signal_state_t* sigstate = &process_signals[pid];
    
    // Check for pending signals
    for (int i = 0; i < MAX_SIGNALS; i++) {
        if (sigstate->handlers[i].pending) {
            if (sigstate->handlers[i].handler != NULL) {
                // Call signal handler
                sigstate->handlers[i].handler(i);
                sigstate->handlers[i].pending = false;
                sigstate->handled_signals |= (1ULL << i);
            }
        }
    }
    
    return 0;
}

int trap_mask_signal(pid_t pid, int sig) {
    if (pid < 0 || pid >= MAX_PROCESSES) {
        return -1;
    }
    
    if (sig < 0 || sig >= MAX_SIGNALS) {
        return -1;
    }
    
    process_signals[pid].masked_signals |= (1ULL << sig);
    
    return 0;
}

int trap_unmask_signal(pid_t pid, int sig) {
    if (pid < 0 || pid >= MAX_PROCESSES) {
        return -1;
    }
    
    if (sig < 0 || sig >= MAX_SIGNALS) {
        return -1;
    }
    
    process_signals[pid].masked_signals &= ~(1ULL << sig);
    
    return 0;
}

/**
 * @file trap.h
 * @brief Trap and Signal Handling Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TRAP_H
#define TRAP_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_SIGNALS 64

typedef uint64_t sigset_t;
typedef int pid_t;

// Standard signals
#define SIGKILL 9
#define SIGTERM 15
#define SIGINT 2
#define SIGSEGV 11
#define SIGFPE 8
#define SIGILL 4
#define SIGTRAP 5

int trap_init(void);
int trap_register_handler(pid_t pid, int sig, void (*handler)(int));
int trap_send_signal(pid_t pid, int sig);
int trap_handle_pending_signals(pid_t pid);
int trap_mask_signal(pid_t pid, int sig);
int trap_unmask_signal(pid_t pid, int sig);

#endif // TRAP_H

