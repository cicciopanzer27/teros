/**
 * @file scheduler.c
 * @brief Scheduler implementation
 */

#include "scheduler.h"
#include "process.h"
#include <stddef.h>

static uint32_t scheduler_ticks = 0;
static uint32_t scheduler_switches = 0;

void scheduler_init(void) {
    scheduler_ticks = 0;
    scheduler_switches = 0;
}

process_t* scheduler_next(void) {
    // For now, simple round-robin
    // TODO: Implement priority-based scheduling
    
    // Get next process from ready queue
    return ready_queue;  // Will be defined by process module
}

void scheduler_schedule(void) {
    // TODO: Implement context switch
    process_t* next = scheduler_next();
    if (next != NULL) {
        process_switch(next);
        scheduler_switches++;
    }
}

void scheduler_add(process_t* process) {
    process_add_to_queue(process);
}

void scheduler_remove(process_t* process) {
    process_remove_from_queue(process);
}

void scheduler_yield(void) {
    // Move current process to end of queue
    if (current_process != NULL) {
        process_remove_from_queue(current_process);
        process_add_to_queue(current_process);
    }
    
    // Schedule next process
    scheduler_schedule();
}

void scheduler_tick(void) {
    scheduler_ticks++;
    
    // Check if quantum expired
    if (current_process != NULL) {
        current_process->ticks_total++;
        current_process->ticks_recent++;
        
        if (current_process->ticks_recent >= SCHEDULER_QUANTUM) {
            current_process->ticks_recent = 0;
            scheduler_schedule();
        }
    }
}

void scheduler_print_stats(void) {
    // TODO: Implement with console output
}

