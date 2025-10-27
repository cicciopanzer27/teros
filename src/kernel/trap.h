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

// Signal set type
typedef uint64_t sigset_t;

// Process ID type
typedef int pid_t;

/**
 * @brief Initialize trap/signal handling system
 * @return 0 on success, -1 on error
 */
int trap_init(void);

/**
 * @brief Register signal handler for a process
 * @param pid Process ID
 * @param sig Signal number
 * @param handler Signal handler function
 * @return 0 on success, -1 on error
 */
int trap_register_handler(pid_t pid, int sig, void (*handler)(int));

/**
 * @brief Send signal to a process
 * @param pid Process ID
 * @param sig Signal number
 * @return 0 on success, -1 on error
 */
int trap_send_signal(pid_t pid, int sig);

/**
 * @brief Handle pending signals for a process
 * @param pid Process ID
 * @return Number of signals handled
 */
int trap_handle_pending_signals(pid_t pid);

/**
 * @brief Mask a signal for a process
 * @param pid Process ID
 * @param sig Signal number
 * @return 0 on success, -1 on error
 */
int trap_mask_signal(pid_t pid, int sig);

/**
 * @brief Unmask a signal for a process
 * @param pid Process ID
 * @param sig Signal number
 * @return 0 on success, -1 on error
 */
int trap_unmask_signal(pid_t pid, int sig);

#endif // TRAP_H

