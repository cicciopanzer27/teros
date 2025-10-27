/**
 * @file scheduler.h
 * @brief Ternary Scheduler Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

// Forward declaration
typedef struct process process_t;

// =============================================================================
// SCHEDULER CONSTANTS
// =============================================================================

#define SCHEDULER_MAX_PROCESSES 1024
#define SCHEDULER_TIME_SLICE 10    // 10ms time slice
#define SCHEDULER_PRIORITY_LEVELS 3

// =============================================================================
// SCHEDULER INITIALIZATION
// =============================================================================

/**
 * @brief Initialize scheduler
 */
void scheduler_init(void);

// =============================================================================
// SCHEDULER CORE FUNCTIONS
// =============================================================================

/**
 * @brief Add process to scheduler
 * @param proc Process to add
 */
void scheduler_add_process(process_t* proc);

/**
 * @brief Remove process from scheduler
 * @param proc Process to remove
 */
void scheduler_remove_process(process_t* proc);

/**
 * @brief Get next process to run
 * @return Next process, or NULL if none
 */
process_t* scheduler_get_next_process(void);

/**
 * @brief Schedule next process
 */
void scheduler_schedule(void);

/**
 * @brief Perform context switch
 * @param from Process to switch from
 * @param to Process to switch to
 */
void scheduler_context_switch(process_t* from, process_t* to);

/**
 * @brief Save process context
 * @param proc Process
 */
void scheduler_save_context(process_t* proc);

/**
 * @brief Restore process context
 * @param proc Process
 */
void scheduler_restore_context(process_t* proc);

// =============================================================================
// SCHEDULER TIMING
// =============================================================================

/**
 * @brief Scheduler tick (called by timer interrupt)
 */
void scheduler_tick(void);

/**
 * @brief Preempt current process
 */
void scheduler_preempt(void);

/**
 * @brief Yield current process
 */
void scheduler_yield(void);

// =============================================================================
// SCHEDULER CONTROL
// =============================================================================

/**
 * @brief Start scheduler
 */
void scheduler_start(void);

/**
 * @brief Stop scheduler
 */
void scheduler_stop(void);

/**
 * @brief Check if scheduler is running
 * @return true if running
 */
bool scheduler_is_running(void);

/**
 * @brief Check if scheduler has work
 * @return true if has work
 */
bool scheduler_has_work(void);

/**
 * @brief Set current process
 * @param proc Process
 */
void scheduler_set_current_process(process_t* proc);

// =============================================================================
// SCHEDULER QUERY FUNCTIONS
// =============================================================================

/**
 * @brief Get total context switches
 * @return Total switches
 */
uint32_t scheduler_get_total_switches(void);

/**
 * @brief Get total preemptions
 * @return Total preemptions
 */
uint32_t scheduler_get_total_preemptions(void);

/**
 * @brief Get total yields
 * @return Total yields
 */
uint32_t scheduler_get_total_yields(void);

/**
 * @brief Get scheduler ticks
 * @return Scheduler ticks
 */
uint32_t scheduler_get_scheduler_ticks(void);

/**
 * @brief Get time slice
 * @return Time slice in ms
 */
uint32_t scheduler_get_time_slice(void);

/**
 * @brief Set time slice
 * @param time_slice Time slice in ms
 */
void scheduler_set_time_slice(uint32_t time_slice);

/**
 * @brief Get current time slice
 * @return Current time slice in ms
 */
uint32_t scheduler_get_current_time_slice(void);

// =============================================================================
// SCHEDULER DEBUG FUNCTIONS
// =============================================================================

/**
 * @brief Print scheduler statistics
 */
void scheduler_print_statistics(void);

/**
 * @brief Print priority queue
 * @param priority Priority level (-1, 0, 1)
 */
void scheduler_print_queue(int32_t priority);

/**
 * @brief Check if scheduler is initialized
 * @return true if initialized
 */
bool scheduler_is_initialized(void);

#endif // SCHEDULER_H
