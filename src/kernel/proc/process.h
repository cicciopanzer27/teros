/**
 * @file process.h
 * @brief Process Control Block (PCB) Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// PROCESS CONSTANTS
// =============================================================================

#define MAX_PROCESSES 1024
#define MAX_PROCESS_NAME 64
#define MAX_PROCESS_ARGS 16
#define MAX_PROCESS_ENV 32

// Process states (ternary)
typedef enum {
    PROCESS_STATE_RUNNING = 1,    // Positive (1)
    PROCESS_STATE_READY = 0,       // Zero (0)
    PROCESS_STATE_BLOCKED = -1    // Negative (-1)
} process_state_t;

// Process structure (forward declaration)
typedef struct process process_t;

// =============================================================================
// PROCESS INITIALIZATION
// =============================================================================

/**
 * @brief Initialize Process Manager
 */
void process_init(void);

// =============================================================================
// PROCESS CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new process
 * @param name Process name
 * @param ppid Parent process ID
 * @return Pointer to created process, or NULL on failure
 */
process_t* process_create(const char* name, uint32_t ppid);

/**
 * @brief Destroy a process
 * @param proc Process to destroy
 */
void process_destroy(process_t* proc);

// =============================================================================
// PROCESS STATE MANAGEMENT
// =============================================================================

/**
 * @brief Set process state
 * @param proc Process
 * @param state New state
 */
void process_set_state(process_t* proc, process_state_t state);

/**
 * @brief Get process state
 * @param proc Process
 * @return Process state
 */
process_state_t process_get_state(process_t* proc);

/**
 * @brief Set process priority
 * @param proc Process
 * @param priority Priority (-1, 0, 1)
 */
void process_set_priority(process_t* proc, int32_t priority);

/**
 * @brief Get process priority
 * @param proc Process
 * @return Process priority
 */
int32_t process_get_priority(process_t* proc);

// =============================================================================
// PROCESS QUERY FUNCTIONS
// =============================================================================

/**
 * @brief Find process by PID
 * @param pid Process ID
 * @return Pointer to process, or NULL if not found
 */
process_t* process_find_by_pid(uint32_t pid);

/**
 * @brief Get current process
 * @return Pointer to current process
 */
process_t* process_get_current(void);

/**
 * @brief Set current process
 * @param proc Process to set as current
 */
void process_set_current(process_t* proc);

/**
 * @brief Get total process count
 * @return Number of processes
 */
uint32_t process_get_count(void);

/**
 * @brief Get next PID
 * @return Next PID
 */
uint32_t process_get_next_pid(void);

// =============================================================================
// PROCESS EXECUTION
// =============================================================================

/**
 * @brief Load program into process
 * @param proc Process
 * @param filename Program filename
 * @return true if successful
 */
bool process_load_program(process_t* proc, const char* filename);

/**
 * @brief Execute process
 * @param proc Process
 * @return true if successful
 */
bool process_execute(process_t* proc);

// =============================================================================
// PROCESS TERMINATION
// =============================================================================

/**
 * @brief Terminate process
 * @param proc Process
 * @param exit_code Exit code
 */
void process_terminate(process_t* proc, uint32_t exit_code);

/**
 * @brief Check if process is terminated
 * @param proc Process
 * @return true if terminated
 */
bool process_is_terminated(process_t* proc);

/**
 * @brief Get process exit code
 * @param proc Process
 * @return Exit code
 */
uint32_t process_get_exit_code(process_t* proc);

// =============================================================================
// PROCESS SIGNALS
// =============================================================================

/**
 * @brief Send signal to process
 * @param proc Process
 * @param signal Signal number
 */
void process_send_signal(process_t* proc, uint32_t signal);

/**
 * @brief Handle pending signals
 * @param proc Process
 */
void process_handle_signals(process_t* proc);

/**
 * @brief Handle specific signal
 * @param proc Process
 * @param signal Signal number
 */
void process_handle_signal(process_t* proc, uint32_t signal);

// =============================================================================
// PROCESS DEBUG FUNCTIONS
// =============================================================================

/**
 * @brief Print process information
 * @param proc Process
 */
void process_print_info(process_t* proc);

/**
 * @brief Print all processes
 */
void process_print_all(void);

/**
 * @brief Check if process manager is initialized
 * @return true if initialized
 */
bool process_is_initialized(void);

#endif // PROCESS_H
