/**
 * @file scheduler.h
 * @brief Process scheduler for TEROS
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

// Scheduler configuration
#define SCHEDULER_QUANTUM 10  // Time quantum in ticks

/**
 * @brief Initialize scheduler
 */
void scheduler_init(void);

/**
 * @brief Schedule next process to run
 * @return Pointer to next process to run
 */
process_t* scheduler_next(void);

/**
 * @brief Schedule current process and switch to next
 */
void scheduler_schedule(void);

/**
 * @brief Add process to ready queue
 * @param process Process to add
 */
void scheduler_add(process_t* process);

/**
 * @brief Remove process from ready queue
 * @param process Process to remove
 */
void scheduler_remove(process_t* process);

/**
 * @brief Give up CPU voluntarily
 */
void scheduler_yield(void);

/**
 * @brief Trigger scheduler (called by timer interrupt)
 */
void scheduler_tick(void);

/**
 * @brief Print scheduler statistics
 */
void scheduler_print_stats(void);

#endif // SCHEDULER_H

