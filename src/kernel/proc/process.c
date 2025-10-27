/**
 * @file process.c
 * @brief Process Control Block implementation
 */

#include "process.h"
#include "../mm/kmalloc.h"
#include "../mm/vmm.h"
#include <stddef.h>

process_t* current_process = NULL;
process_t* ready_queue = NULL;
static process_t* blocked_queue = NULL;
static pid_t next_pid = 1;

static void pcb_init(process_t* pcb, pid_t pid, pid_t ppid) {
    pcb->pid = pid;
    pcb->ppid = ppid;
    pcb->state = PROCESS_STATE_NEW;
    pcb->priority = PRIORITY_NORMAL;
    pcb->parent = NULL;
    pcb->children = NULL;
    pcb->sibling = NULL;
    pcb->next = NULL;
    pcb->prev = NULL;
    
    // Zero context
    process_context_t* ctx = &pcb->context;
    ctx->r0 = ctx->r1 = ctx->r2 = ctx->r3 = 0;
    ctx->r4 = ctx->r5 = ctx->r6 = ctx->r7 = 0;
    ctx->pc = ctx->sp = ctx->fp = ctx->lr = 0;
    ctx->cr = ctx->acc = ctx->tmp = ctx->zero = 0;
    ctx->flags = 0;
    
    pcb->kernel_stack = 0;
    pcb->user_stack = 0;
    pcb->heap_base = 0;
    pcb->heap_size = 0;
    pcb->code_base = 0;
    pcb->code_size = 0;
    pcb->page_directory = NULL;
    pcb->allocated_pages = 0;
    pcb->fd_count = 0;
    pcb->ticks_total = 0;
    pcb->ticks_recent = 0;
    pcb->created_time = 0;
    pcb->sleep_until = 0;
    pcb->uid = 0;
    pcb->gid = 0;
    pcb->capabilities = 0;
    pcb->context_switches = 0;
    pcb->syscalls_count = 0;
}

void process_init(void) {
    current_process = NULL;
    ready_queue = NULL;
    blocked_queue = NULL;
    next_pid = 1;
}

process_t* process_create(uint32_t code_address, uint32_t code_size, int32_t priority) {
    // Allocate PCB
    process_t* pcb = (process_t*)kmalloc(sizeof(process_t));
    if (pcb == NULL) {
        return NULL;
    }
    
    // Initialize PCB
    pcb_init(pcb, next_pid++, current_process ? current_process->pid : 0);
    
    // Set code and priority
    pcb->code_base = code_address;
    pcb->code_size = code_size;
    pcb->priority = priority;
    
    // Allocate kernel stack
    pcb->kernel_stack = (uint32_t)kmalloc(8192); // 8KB kernel stack
    if (pcb->kernel_stack == 0) {
        kfree(pcb);
        return NULL;
    }
    
    // Allocate user stack
    pcb->user_stack = (uint32_t)kmalloc(4096); // 4KB user stack
    if (pcb->user_stack == 0) {
        kfree((void*)pcb->kernel_stack);
        kfree(pcb);
        return NULL;
    }
    
    // Set initial SP
    pcb->context.sp = pcb->user_stack + 4096;
    pcb->context.pc = code_address;
    
    // Add to ready queue
    process_add_to_queue(pcb);
    pcb->state = PROCESS_STATE_READY;
    
    return pcb;
}

void process_terminate(process_t* process) {
    if (process == NULL) {
        return;
    }
    
    process->state = PROCESS_STATE_ZOMBIE;
    process_remove_from_queue(process);
    
    // TODO: Free resources, send signals to children, etc.
}

void process_destroy(process_t* process) {
    if (process == NULL) {
        return;
    }
    
    // Free stacks
    if (process->kernel_stack) {
        kfree((void*)process->kernel_stack);
    }
    if (process->user_stack) {
        kfree((void*)process->user_stack);
    }
    
    // Free PCB
    kfree(process);
}

process_t* process_get_current(void) {
    return current_process;
}

process_t* process_get_by_pid(pid_t pid) {
    // Search ready queue
    for (process_t* p = ready_queue; p != NULL; p = p->next) {
        if (p->pid == pid) return p;
    }
    
    // Search blocked queue
    for (process_t* p = blocked_queue; p != NULL; p = p->next) {
        if (p->pid == pid) return p;
    }
    
    // Check current process
    if (current_process && current_process->pid == pid) {
        return current_process;
    }
    
    return NULL;
}

void process_switch(process_t* new_process) {
    if (current_process == new_process) {
        return;
    }
    
    process_t* old_process = current_process;
    current_process = new_process;
    
    if (old_process) {
        old_process->state = PROCESS_STATE_READY;
        old_process->context_switches++;
        process_add_to_queue(old_process);
    }
    
    if (new_process) {
        new_process->state = PROCESS_STATE_RUNNING;
        new_process->context_switches++;
        process_remove_from_queue(new_process);
        
        // TODO: Actually switch context (assembly code needed)
    }
}

void process_block(void) {
    if (current_process == NULL) {
        return;
    }
    
    current_process->state = PROCESS_STATE_BLOCKED;
    process_remove_from_queue(current_process);
    
    // Add to blocked queue
    if (blocked_queue == NULL) {
        blocked_queue = current_process;
        current_process->next = NULL;
        current_process->prev = NULL;
    } else {
        current_process->next = blocked_queue;
        current_process->prev = NULL;
        blocked_queue->prev = current_process;
        blocked_queue = current_process;
    }
    
    // TODO: Trigger scheduler
}

void process_unblock(process_t* process) {
    if (process == NULL || process->state != PROCESS_STATE_BLOCKED) {
        return;
    }
    
    process_remove_from_queue(process);
    process->state = PROCESS_STATE_READY;
    process_add_to_queue(process);
}

void process_sleep(uint32_t ticks) {
    // TODO: Implement sleep with timer integration
}

void process_wakeup(void) {
    // TODO: Wake up processes that have slept enough
}

void process_set_priority(process_t* process, int32_t priority) {
    if (process != NULL) {
        process->priority = priority;
    }
}

int32_t process_get_state(process_t* process) {
    return process ? process->state : -1;
}

void process_add_to_queue(process_t* process) {
    if (process == NULL) {
        return;
    }
    
    // Add to ready queue based on priority
    process->next = NULL;
    process->prev = NULL;
    
    if (ready_queue == NULL) {
        ready_queue = process;
        return;
    }
    
    // Find correct position based on priority
    process_t* current = ready_queue;
    while (current != NULL) {
        if (process->priority > current->priority) {
            // Insert before current
            process->next = current;
            process->prev = current->prev;
            if (current->prev) {
                current->prev->next = process;
            } else {
                ready_queue = process;
            }
            current->prev = process;
            return;
        }
        
        if (current->next == NULL) {
            // Insert at end
            current->next = process;
            process->prev = current;
            return;
        }
        
        current = current->next;
    }
}

void process_remove_from_queue(process_t* process) {
    if (process == NULL) {
        return;
    }
    
    // Remove from whichever queue it's in
    if (process->prev) {
        process->prev->next = process->next;
    } else if (ready_queue == process) {
        ready_queue = process->next;
    } else if (blocked_queue == process) {
        blocked_queue = process->next;
    }
    
    if (process->next) {
        process->next->prev = process->prev;
    }
    
    process->next = NULL;
    process->prev = NULL;
}

void process_print_info(process_t* process) {
    // TODO: Print process information
}

void process_print_all(void) {
    // TODO: Print all processes
}

