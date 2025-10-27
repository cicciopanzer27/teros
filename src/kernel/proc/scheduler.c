/**
 * @file scheduler.c
 * @brief Ternary Scheduler with Round-Robin and Priority Scheduling
 * @author TEROS Development Team
 * @date 2025
 */

#include "scheduler.h"
#include "process.h"
#include "timer.h"
#include "console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// SCHEDULER IMPLEMENTATION
// =============================================================================

#define SCHEDULER_MAX_PROCESSES 1024
#define SCHEDULER_TIME_SLICE 10    // 10ms time slice
#define SCHEDULER_PRIORITY_LEVELS 3

// Scheduler queue structure
typedef struct {
    process_t* processes[SCHEDULER_MAX_PROCESSES];
    uint32_t count;
    uint32_t head;
    uint32_t tail;
} scheduler_queue_t;

typedef struct {
    scheduler_queue_t ready_queues[SCHEDULER_PRIORITY_LEVELS]; // -1, 0, 1
    process_t* current_process;
    uint32_t time_slice;
    uint32_t current_time_slice;
    uint32_t total_switches;
    uint32_t total_preemptions;
    uint32_t total_yields;
    uint32_t scheduler_ticks;
    bool initialized;
    bool running;
} scheduler_state_t;

static scheduler_state_t scheduler_state;

// =============================================================================
// SCHEDULER INITIALIZATION
// =============================================================================

void scheduler_init(void) {
    if (scheduler_state.initialized) {
        return;
    }
    
    console_puts("SCHED: Initializing Ternary Scheduler...\n");
    
    // Initialize scheduler state
    memset(&scheduler_state, 0, sizeof(scheduler_state_t));
    
    // Initialize ready queues
    for (int i = 0; i < SCHEDULER_PRIORITY_LEVELS; i++) {
        scheduler_queue_t* queue = &scheduler_state.ready_queues[i];
        queue->count = 0;
        queue->head = 0;
        queue->tail = 0;
        
        for (int j = 0; j < SCHEDULER_MAX_PROCESSES; j++) {
            queue->processes[j] = NULL;
        }
    }
    
    scheduler_state.time_slice = SCHEDULER_TIME_SLICE;
    scheduler_state.current_time_slice = 0;
    scheduler_state.total_switches = 0;
    scheduler_state.total_preemptions = 0;
    scheduler_state.total_yields = 0;
    scheduler_state.scheduler_ticks = 0;
    scheduler_state.running = false;
    
    scheduler_state.initialized = true;
    console_puts("SCHED: Scheduler initialized\n");
}

// =============================================================================
// QUEUE OPERATIONS
// =============================================================================

bool scheduler_queue_enqueue(scheduler_queue_t* queue, process_t* proc) {
    if (queue == NULL || proc == NULL) {
        return false;
    }
    
    if (queue->count >= SCHEDULER_MAX_PROCESSES) {
        console_puts("SCHED: ERROR - Queue full\n");
        return false;
    }
    
    queue->processes[queue->tail] = proc;
    queue->tail = (queue->tail + 1) % SCHEDULER_MAX_PROCESSES;
    queue->count++;
    
    return true;
}

process_t* scheduler_queue_dequeue(scheduler_queue_t* queue) {
    if (queue == NULL || queue->count == 0) {
        return NULL;
    }
    
    process_t* proc = queue->processes[queue->head];
    queue->processes[queue->head] = NULL;
    queue->head = (queue->head + 1) % SCHEDULER_MAX_PROCESSES;
    queue->count--;
    
    return proc;
}

bool scheduler_queue_is_empty(scheduler_queue_t* queue) {
    if (queue == NULL) {
        return true;
    }
    
    return queue->count == 0;
}

uint32_t scheduler_queue_count(scheduler_queue_t* queue) {
    if (queue == NULL) {
        return 0;
    }
    
    return queue->count;
}

// =============================================================================
// SCHEDULER CORE FUNCTIONS
// =============================================================================

void scheduler_add_process(process_t* proc) {
    if (!scheduler_state.initialized || proc == NULL) {
        return;
    }
    
    // Determine priority queue index
    int32_t priority = process_get_priority(proc);
    int queue_index = priority + 1; // Convert -1,0,1 to 0,1,2
    
    if (queue_index < 0 || queue_index >= SCHEDULER_PRIORITY_LEVELS) {
        queue_index = 1; // Default to normal priority
    }
    
    scheduler_queue_t* queue = &scheduler_state.ready_queues[queue_index];
    
    if (scheduler_queue_enqueue(queue, proc)) {
        process_set_state(proc, PROCESS_STATE_READY);
        
        console_puts("SCHED: Added process ");
        console_puts(proc->name);
        console_puts(" to priority queue ");
        printf("%d", priority);
        console_puts("\n");
    }
}

void scheduler_remove_process(process_t* proc) {
    if (!scheduler_state.initialized || proc == NULL) {
        return;
    }
    
    // Remove from all queues
    for (int i = 0; i < SCHEDULER_PRIORITY_LEVELS; i++) {
        scheduler_queue_t* queue = &scheduler_state.ready_queues[i];
        
        for (uint32_t j = 0; j < queue->count; j++) {
            uint32_t index = (queue->head + j) % SCHEDULER_MAX_PROCESSES;
            if (queue->processes[index] == proc) {
                // Remove process from queue
                queue->processes[index] = NULL;
                
                // Shift remaining processes
                for (uint32_t k = j; k < queue->count - 1; k++) {
                    uint32_t curr = (queue->head + k) % SCHEDULER_MAX_PROCESSES;
                    uint32_t next = (queue->head + k + 1) % SCHEDULER_MAX_PROCESSES;
                    queue->processes[curr] = queue->processes[next];
                }
                
                queue->count--;
                queue->tail = (queue->tail - 1) % SCHEDULER_MAX_PROCESSES;
                
                console_puts("SCHED: Removed process ");
                console_puts(proc->name);
                console_puts(" from priority queue ");
                printf("%d", i - 1);
                console_puts("\n");
                
                return;
            }
        }
    }
}

process_t* scheduler_get_next_process(void) {
    if (!scheduler_state.initialized) {
        return NULL;
    }
    
    // Check queues in priority order (high to low)
    for (int i = SCHEDULER_PRIORITY_LEVELS - 1; i >= 0; i--) {
        scheduler_queue_t* queue = &scheduler_state.ready_queues[i];
        
        if (!scheduler_queue_is_empty(queue)) {
            process_t* proc = scheduler_queue_dequeue(queue);
            if (proc != NULL) {
                console_puts("SCHED: Selected process ");
                console_puts(proc->name);
                console_puts(" from priority queue ");
                printf("%d", i - 1);
                console_puts("\n");
                return proc;
            }
        }
    }
    
    return NULL;
}

void scheduler_schedule(void) {
    if (!scheduler_state.initialized) {
        return;
    }
    
    // Get next process
    process_t* next_proc = scheduler_get_next_process();
    
    if (next_proc == NULL) {
        // No processes to schedule
        scheduler_state.current_process = NULL;
        return;
    }
    
    // Get current process
    process_t* current_proc = scheduler_state.current_process;
    
    // Check if we need to switch processes
    if (current_proc != next_proc) {
        // Perform context switch
        scheduler_context_switch(current_proc, next_proc);
        
        // Update current process
        scheduler_state.current_process = next_proc;
        process_set_current(next_proc);
        
        // Update statistics
        scheduler_state.total_switches++;
        
        console_puts("SCHED: Context switch to process ");
        console_puts(next_proc->name);
        console_puts("\n");
    }
    
    // Reset time slice
    scheduler_state.current_time_slice = scheduler_state.time_slice;
}

void scheduler_context_switch(process_t* from, process_t* to) {
    if (to == NULL) {
        return;
    }
    
    // Save context of current process
    if (from != NULL) {
        // Save CPU state
        scheduler_save_context(from);
        
        // Add back to ready queue if not blocked
        if (process_get_state(from) == PROCESS_STATE_RUNNING) {
            scheduler_add_process(from);
        }
    }
    
    // Restore context of new process
    scheduler_restore_context(to);
    
    // Set new process as running
    process_set_state(to, PROCESS_STATE_RUNNING);
}

void scheduler_save_context(process_t* proc) {
    if (proc == NULL) {
        return;
    }
    
    // Save TVM state
    // This is a simplified implementation
    // In a real system, we would save all CPU registers
    
    console_puts("SCHED: Saved context for process ");
    console_puts(proc->name);
    console_puts("\n");
}

void scheduler_restore_context(process_t* proc) {
    if (proc == NULL) {
        return;
    }
    
    // Restore TVM state
    // This is a simplified implementation
    // In a real system, we would restore all CPU registers
    
    console_puts("SCHED: Restored context for process ");
    console_puts(proc->name);
    console_puts("\n");
}

// =============================================================================
// SCHEDULER TIMING
// =============================================================================

void scheduler_tick(void) {
    if (!scheduler_state.initialized) {
        return;
    }
    
    scheduler_state.scheduler_ticks++;
    
    // Check if current process should be preempted
    if (scheduler_state.current_process != NULL) {
        scheduler_state.current_time_slice--;
        
        if (scheduler_state.current_time_slice <= 0) {
            // Time slice expired, preempt current process
            scheduler_preempt();
        }
    }
    
    // Schedule next process
    scheduler_schedule();
}

void scheduler_preempt(void) {
    if (!scheduler_state.initialized) {
        return;
    }
    
    process_t* current_proc = scheduler_state.current_process;
    
    if (current_proc != NULL) {
        console_puts("SCHED: Preempting process ");
        console_puts(current_proc->name);
        console_puts("\n");
        
        // Add back to ready queue
        scheduler_add_process(current_proc);
        
        // Update statistics
        scheduler_state.total_preemptions++;
    }
}

void scheduler_yield(void) {
    if (!scheduler_state.initialized) {
        return;
    }
    
    process_t* current_proc = scheduler_state.current_process;
    
    if (current_proc != NULL) {
        console_puts("SCHED: Process ");
        console_puts(current_proc->name);
        console_puts(" yielded\n");
        
        // Add back to ready queue
        scheduler_add_process(current_proc);
        
        // Update statistics
        scheduler_state.total_yields++;
        
        // Schedule next process
        scheduler_schedule();
    }
}

// =============================================================================
// SCHEDULER CONTROL
// =============================================================================

void scheduler_start(void) {
    if (!scheduler_state.initialized) {
        return;
    }
    
    console_puts("SCHED: Starting scheduler\n");
    scheduler_state.running = true;
    
    // Start scheduling
    scheduler_schedule();
}

void scheduler_stop(void) {
    if (!scheduler_state.initialized) {
        return;
    }
    
    console_puts("SCHED: Stopping scheduler\n");
    scheduler_state.running = false;
    
    // Clear current process
    scheduler_state.current_process = NULL;
}

bool scheduler_is_running(void) {
    return scheduler_state.running;
}

bool scheduler_has_work(void) {
    if (!scheduler_state.initialized) {
        return false;
    }
    
    // Check if any queue has processes
    for (int i = 0; i < SCHEDULER_PRIORITY_LEVELS; i++) {
        if (!scheduler_queue_is_empty(&scheduler_state.ready_queues[i])) {
            return true;
        }
    }
    
    return false;
}

void scheduler_set_current_process(process_t* proc) {
    scheduler_state.current_process = proc;
    process_set_current(proc);
}

// =============================================================================
// SCHEDULER QUERY FUNCTIONS
// =============================================================================

uint32_t scheduler_get_total_switches(void) {
    return scheduler_state.total_switches;
}

uint32_t scheduler_get_total_preemptions(void) {
    return scheduler_state.total_preemptions;
}

uint32_t scheduler_get_total_yields(void) {
    return scheduler_state.total_yields;
}

uint32_t scheduler_get_scheduler_ticks(void) {
    return scheduler_state.scheduler_ticks;
}

uint32_t scheduler_get_time_slice(void) {
    return scheduler_state.time_slice;
}

void scheduler_set_time_slice(uint32_t time_slice) {
    scheduler_state.time_slice = time_slice;
}

uint32_t scheduler_get_current_time_slice(void) {
    return scheduler_state.current_time_slice;
}

// =============================================================================
// SCHEDULER DEBUG FUNCTIONS
// =============================================================================

void scheduler_print_statistics(void) {
    if (!scheduler_state.initialized) {
        console_puts("SCHED: Scheduler not initialized\n");
        return;
    }
    
    console_puts("SCHED: Scheduler Statistics:\n");
    console_puts("  Total switches: ");
    printf("%u", scheduler_state.total_switches);
    console_puts("\n");
    console_puts("  Total preemptions: ");
    printf("%u", scheduler_state.total_preemptions);
    console_puts("\n");
    console_puts("  Total yields: ");
    printf("%u", scheduler_state.total_yields);
    console_puts("\n");
    console_puts("  Scheduler ticks: ");
    printf("%u", scheduler_state.scheduler_ticks);
    console_puts("\n");
    console_puts("  Time slice: ");
    printf("%u", scheduler_state.time_slice);
    console_puts(" ms\n");
    console_puts("  Current time slice: ");
    printf("%u", scheduler_state.current_time_slice);
    console_puts(" ms\n");
    console_puts("  Running: ");
    console_puts(scheduler_state.running ? "Yes" : "No");
    console_puts("\n");
    
    console_puts("  Ready queues:\n");
    for (int i = 0; i < SCHEDULER_PRIORITY_LEVELS; i++) {
        int32_t priority = i - 1;
        uint32_t count = scheduler_queue_count(&scheduler_state.ready_queues[i]);
        
        console_puts("    Priority ");
        printf("%d", priority);
        console_puts(": ");
        printf("%u", count);
        console_puts(" processes\n");
    }
}

void scheduler_print_queue(int32_t priority) {
    if (!scheduler_state.initialized) {
        console_puts("SCHED: Scheduler not initialized\n");
        return;
    }
    
    int queue_index = priority + 1;
    if (queue_index < 0 || queue_index >= SCHEDULER_PRIORITY_LEVELS) {
        console_puts("SCHED: Invalid priority\n");
        return;
    }
    
    scheduler_queue_t* queue = &scheduler_state.ready_queues[queue_index];
    
    console_puts("SCHED: Priority ");
    printf("%d", priority);
    console_puts(" queue:\n");
    console_puts("  Count: ");
    printf("%u", queue->count);
    console_puts("\n");
    
    if (queue->count > 0) {
        console_puts("  Processes:\n");
        for (uint32_t i = 0; i < queue->count; i++) {
            uint32_t index = (queue->head + i) % SCHEDULER_MAX_PROCESSES;
            process_t* proc = queue->processes[index];
            
            if (proc != NULL) {
                console_puts("    ");
                console_puts(proc->name);
                console_puts(" (PID: ");
                printf("%u", proc->pid);
                console_puts(")\n");
            }
        }
    }
}

bool scheduler_is_initialized(void) {
    return scheduler_state.initialized;
}
