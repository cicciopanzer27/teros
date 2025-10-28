/**
 * @file ipc.c
 * @brief Inter-Process Communication (IPC) Implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ipc.h"
#include "console.h"
#include "kmalloc.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define MAX_PIPES 128
#define MAX_SIGNALS 32
#define MAX_SHARED_MEMORY 64
#define MAX_SEMAPHORES 64
#define PIPE_BUFFER_SIZE 4096

// =============================================================================
// PIPES
// =============================================================================

static pipe_t pipes[MAX_PIPES];
static int pipe_count = 0;
static uint32_t next_pipe_id = 1;

static void pipe_init(void) {
    for (int i = 0; i < MAX_PIPES; i++) {
        pipes[i].id = 0;
        pipes[i].buffer = NULL;
        pipes[i].buffer_size = 0;
        pipes[i].read_pos = 0;
        pipes[i].write_pos = 0;
        pipes[i].ref_count = 0;
        pipes[i].is_open = false;
    }
}

int pipe_open(int pipefd[2]) {
    if (pipefd == NULL) {
        return -1;
    }
    
    // Find free pipe slot
    int idx = -1;
    for (int i = 0; i < MAX_PIPES; i++) {
        if (!pipes[i].is_open) {
            idx = i;
            break;
        }
    }
    
    if (idx == -1) {
        console_puts("IPC: ERROR - No free pipe slots\n");
        return -1;
    }
    
    // Allocate pipe
    pipes[idx].id = next_pipe_id++;
    pipes[idx].buffer = (char*)kmalloc(PIPE_BUFFER_SIZE);
    
    if (pipes[idx].buffer == NULL) {
        console_puts("IPC: ERROR - Failed to allocate pipe buffer\n");
        return -1;
    }
    
    pipes[idx].buffer_size = PIPE_BUFFER_SIZE;
    pipes[idx].read_pos = 0;
    pipes[idx].write_pos = 0;
    pipes[idx].ref_count = 2;  // Both ends
    pipes[idx].is_open = true;
    
    pipe_count++;
    
    // Return file descriptors (simplified)
    pipefd[0] = pipes[idx].id;
    pipefd[1] = pipes[idx].id;
    
    return 0;
}

pipe_t* pipe_get(int pipe_id) {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipes[i].id == (uint32_t)pipe_id && pipes[i].is_open) {
            return &pipes[i];
        }
    }
    return NULL;
}

int pipe_close(int fd) {
    pipe_t* pipe = pipe_get(fd);
    if (pipe == NULL) {
        return -1;
    }
    
    pipe->ref_count--;
    
    if (pipe->ref_count == 0) {
        // Close pipe
        if (pipe->buffer != NULL) {
            kfree(pipe->buffer);
            pipe->buffer = NULL;
        }
        pipe->is_open = false;
        pipe_count--;
    }
    
    return 0;
}

ssize_t pipe_read(int fd, void* buf, size_t count) {
    pipe_t* pipe = pipe_get(fd);
    if (pipe == NULL || buf == NULL || count == 0) {
        return -1;
    }
    
    if (pipe->read_pos >= pipe->write_pos) {
        // Buffer is empty
        return 0;
    }
    
    size_t available = pipe->write_pos - pipe->read_pos;
    size_t to_read = (count < available) ? count : available;
    
    memcpy(buf, pipe->buffer + pipe->read_pos, to_read);
    pipe->read_pos += to_read;
    
    // Wrap around if buffer is empty
    if (pipe->read_pos == pipe->write_pos) {
        pipe->read_pos = 0;
        pipe->write_pos = 0;
    }
    
    return to_read;
}

ssize_t pipe_write(int fd, const void* buf, size_t count) {
    pipe_t* pipe = pipe_get(fd);
    if (pipe == NULL || buf == NULL || count == 0) {
        return -1;
    }
    
    size_t available = pipe->buffer_size - (pipe->write_pos - pipe->read_pos);
    size_t to_write = (count < available) ? count : available;
    
    if (to_write == 0) {
        return 0;  // Buffer full
    }
    
    memcpy(pipe->buffer + pipe->write_pos, buf, to_write);
    pipe->write_pos += to_write;
    
    return to_write;
}

// =============================================================================
// SIGNALS
// =============================================================================

static signal_handler_t signal_handlers[SIGMAX];
static bool signal_registered[SIGMAX];
static uint32_t signal_mask_value = 0;  // Bitmask for blocked signals
static uint32_t pending_signals = 0;    // Pending signals
static signal_delivery_state_t signal_states[SIGMAX];  // Ternary delivery states

static void signal_init(void) {
    for (int i = 0; i < SIGMAX; i++) {
        signal_handlers[i] = NULL;
        signal_registered[i] = false;
        signal_states[i] = SIGNAL_PENDING;  // Start with pending state
    }
    signal_mask_value = 0;
    pending_signals = 0;
}

int signal_register(int sig, signal_handler_t handler) {
    if (sig < 0 || sig >= SIGMAX) {
        return -1;
    }
    
    signal_handlers[sig] = handler;
    signal_registered[sig] = true;
    
    return 0;
}

signal_handler_t signal_get_handler(int sig) {
    if (sig < 0 || sig >= SIGMAX || !signal_registered[sig]) {
        return NULL;
    }
    
    return signal_handlers[sig];
}

int signal_send(int pid, int sig) {
    if (sig < 0 || sig >= SIGMAX) {
        return -1;
    }
    
    // Send signal to process
    console_puts("IPC: Sending signal ");
    
    // Print signal number
    char sig_str[12];
    int sig_len = 0;
    int temp_sig = sig;
    if (temp_sig == 0) {
        sig_str[sig_len++] = '0';
    } else {
        char digits[12];
        int digit_count = 0;
        while (temp_sig > 0) {
            digits[digit_count++] = '0' + (temp_sig % 10);
            temp_sig /= 10;
        }
        for (int i = digit_count - 1; i >= 0; i--) {
            sig_str[sig_len++] = digits[i];
        }
    }
    sig_str[sig_len] = '\0';
    console_puts(sig_str);
    
    console_puts(" to PID ");
    
    // Print PID
    char pid_str[12];
    int pid_len = 0;
    int temp_pid = pid;
    if (temp_pid == 0) {
        pid_str[pid_len++] = '0';
    } else {
        char digits[12];
        int digit_count = 0;
        while (temp_pid > 0) {
            digits[digit_count++] = '0' + (temp_pid % 10);
            temp_pid /= 10;
        }
        for (int i = digit_count - 1; i >= 0; i--) {
            pid_str[pid_len++] = digits[i];
        }
    }
    pid_str[pid_len] = '\0';
    console_puts(pid_str);
    console_puts("\n");
    
    return signal_dispatch(pid, sig);
}

int signal_dispatch(int pid, int sig) {
    (void)pid;  // Unused for MVP
    
    // Check if signal is masked (blocked)
    if ((signal_mask_value & (1U << sig)) != 0) {
        // Signal is blocked, mark as pending
        pending_signals |= (1U << sig);
        signal_states[sig] = SIGNAL_BLOCKED;
        return 0;
    }
    
    // Signal can be delivered
    signal_states[sig] = SIGNAL_DELIVERED;
    pending_signals &= ~(1U << sig);
    
    signal_handler_t handler = signal_get_handler(sig);
    
    if (handler != NULL) {
        handler(sig);
        return 0;
    }
    
    // Default action
    return 0;
}

int signal_set_mask(uint32_t mask) {
    signal_mask_value = mask;
    return 0;
}

uint32_t signal_get_mask(void) {
    return signal_mask_value;
}

int signal_unmask(int sig) {
    if (sig < 0 || sig >= SIGMAX) {
        return -1;
    }
    
    signal_mask_value &= ~(1U << sig);
    
    // If signal was pending, deliver it now
    if ((pending_signals & (1U << sig)) != 0) {
        pending_signals &= ~(1U << sig);
        return signal_dispatch(0, sig);  // 0 = current process
    }
    
    return 0;
}

int signal_mask(int sig) {
    if (sig < 0 || sig >= SIGMAX) {
        return -1;
    }
    
    signal_mask_value |= (1U << sig);
    return 0;
}

// =============================================================================
// SHARED MEMORY
// =============================================================================

static shared_memory_t shm_blocks[MAX_SHARED_MEMORY];
static int shm_count = 0;
static uint32_t next_shm_id = 1;

static void shm_init(void) {
    for (int i = 0; i < MAX_SHARED_MEMORY; i++) {
        shm_blocks[i].id = 0;
        shm_blocks[i].addr = NULL;
        shm_blocks[i].size = 0;
        shm_blocks[i].ref_count = 0;
        shm_blocks[i].is_valid = false;
        shm_blocks[i].copy_on_write = true;  // Enable COW by default
        shm_blocks[i].write_count = 0;
    }
}

int shm_open(const char* name, int oflag, uint32_t mode) {
    // Named shared memory implementation (simplified for MVP)
    // In a full implementation, this would maintain a name->id mapping
    (void)name;   // Suppress unused warning for MVP
    (void)oflag;  // Suppress unused warning for MVP
    (void)mode;   // Suppress unused warning for MVP
    
    // Find free slot
    int idx = -1;
    for (int i = 0; i < MAX_SHARED_MEMORY; i++) {
        if (!shm_blocks[i].is_valid) {
            idx = i;
            break;
        }
    }
    
    if (idx == -1) {
        return -1;
    }
    
    shm_blocks[idx].id = next_shm_id++;
    shm_blocks[idx].ref_count = 1;
    shm_blocks[idx].is_valid = true;
    
    shm_count++;
    
    return shm_blocks[idx].id;
}

shared_memory_t* shm_get(int shm_id) {
    for (int i = 0; i < MAX_SHARED_MEMORY; i++) {
        if (shm_blocks[i].id == (uint32_t)shm_id && shm_blocks[i].is_valid) {
            return &shm_blocks[i];
        }
    }
    return NULL;
}

// COW handler for shared memory (called on write fault)
static void* shm_cow_copy(shared_memory_t* shm, size_t size) {
    if (shm == NULL || !shm->copy_on_write) {
        return shm->addr;
    }
    
    // Allocate new copy
    void* new_addr = kmalloc(size);
    if (new_addr == NULL) {
        return NULL;
    }
    
    // Copy original data
    if (shm->addr != NULL) {
        memcpy(new_addr, shm->addr, size);
    }
    
    shm->write_count++;
    
    // Disable COW after first write
    // In real implementation, would handle multiple readers
    shm->addr = new_addr;
    
    return new_addr;
}

void* shm_map(int shm_id, size_t size) {
    shared_memory_t* shm = shm_get(shm_id);
    if (shm == NULL || size == 0) {
        return NULL;
    }
    
    // If first mapping, allocate memory
    if (shm->addr == NULL) {
        void* addr = kmalloc(size);
        if (addr == NULL) {
            return NULL;
        }
        
        shm->addr = addr;
        shm->size = size;
    } else {
        // Handle COW - create copy if needed
        shm->addr = shm_cow_copy(shm, size);
        if (shm->addr == NULL) {
            return NULL;
        }
    }
    
    shm->ref_count++;
    
    return shm->addr;
}

int shm_unmap(void* addr, size_t size) {
    (void)size;  // Not used in simplified version
    for (int i = 0; i < MAX_SHARED_MEMORY; i++) {
        if (shm_blocks[i].addr == addr && shm_blocks[i].is_valid) {
            shm_blocks[i].ref_count--;
            
            if (shm_blocks[i].ref_count == 0) {
                kfree(shm_blocks[i].addr);
                shm_blocks[i].addr = NULL;
                shm_blocks[i].is_valid = false;
                shm_count--;
            }
            
            return 0;
        }
    }
    
    return -1;
}

int shm_unlink(const char* name) {
    // Unlink named shared memory (simplified for MVP)
    // In a full implementation, this would remove the name->id mapping
    // and mark the segment for deletion when all references are closed
    (void)name;  // Suppress unused warning for MVP
    
    // For MVP, just return success
    // Real implementation would:
    // 1. Find shm segment by name
    // 2. Mark as unlinked (no new mappings allowed)
    // 3. Delete when ref_count reaches 0
    return 0;
}

// =============================================================================
// SEMAPHORES
// =============================================================================

static semaphore_t semaphores[MAX_SEMAPHORES];
static int sem_count = 0;
static uint32_t next_sem_id = 1;

static void sem_init(void) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        semaphores[i].id = 0;
        semaphores[i].value = 0;
        semaphores[i].max_value = 0;
        semaphores[i].ref_count = 0;
        semaphores[i].is_valid = false;
        semaphores[i].deadlock_detected = false;
        semaphores[i].wait_count = 0;
    }
}

int sem_open(const char* name, int oflag, uint32_t mode, uint32_t value) {
    (void)name;   // For MVP, name not used
    (void)oflag;  // For MVP, oflag not used
    (void)mode;   // For MVP, mode not used
    // Find free slot
    int idx = -1;
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (!semaphores[i].is_valid) {
            idx = i;
            break;
        }
    }
    
    if (idx == -1) {
        return -1;
    }
    
    semaphores[idx].id = next_sem_id++;
    semaphores[idx].value = value;
    semaphores[idx].max_value = value;
    semaphores[idx].ref_count = 1;
    semaphores[idx].is_valid = true;
    
    sem_count++;
    
    return semaphores[idx].id;
}

semaphore_t* sem_get(int sem_id) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i].id == (uint32_t)sem_id && semaphores[i].is_valid) {
            return &semaphores[i];
        }
    }
    return NULL;
}

// Deadlock detection using ternary logic
// Ternary deadlock states: -1 (deadlock detected), 0 (checking), +1 (no deadlock)
static int sem_check_deadlock(semaphore_t* sem) {
    if (sem == NULL) {
        return -1;
    }
    
    // Check for deadlock conditions
    // Basic check: if all semaphores are at 0 and all have waiting processes
    if (sem->value <= 0 && sem->wait_count > 0) {
        // Could be deadlock - check all semaphores
        int total_waiting = 0;
        for (int i = 0; i < MAX_SEMAPHORES; i++) {
            if (semaphores[i].is_valid) {
                total_waiting += semaphores[i].wait_count;
            }
        }
        
        // If all semaphores have waiters and are at 0, it's a deadlock
        if (total_waiting > 0 && sem_count == total_waiting) {
            sem->deadlock_detected = true;
            return -1;  // Deadlock!
        }
    }
    
    return 1;  // No deadlock
}

int sem_wait(int sem_id) {
    semaphore_t* sem = sem_get(sem_id);
    if (sem == NULL) {
        return -1;
    }
    
    // Increment wait count
    sem->wait_count++;
    
    // Wait for semaphore
    // Note: This is a spinlock implementation for MVP
    // A full implementation would integrate with the scheduler to block/wake processes
    int iterations = 0;
    while (sem->value <= 0) {
        // Check for deadlock periodically
        if ((iterations % 1000) == 0) {
            int deadlock = sem_check_deadlock(sem);
            if (deadlock < 0) {
                sem->wait_count--;
                console_puts("IPC: Semaphore deadlock detected!\n");
                return -1;
            }
        }
        
        // Busy wait (yield CPU)
        asm volatile("pause");
        iterations++;
    }
    
    sem->wait_count--;
    sem->value--;
    return 0;
}

int sem_post(int sem_id) {
    semaphore_t* sem = sem_get(sem_id);
    if (sem == NULL) {
        return -1;
    }
    
    if (sem->value < sem->max_value) {
        sem->value++;
    }
    
    return 0;
}

int sem_trywait(int sem_id) {
    semaphore_t* sem = sem_get(sem_id);
    if (sem == NULL || sem->value <= 0) {
        return -1;
    }
    
    sem->value--;
    return 0;
}

int sem_close(int sem_id) {
    semaphore_t* sem = sem_get(sem_id);
    if (sem == NULL) {
        return -1;
    }
    
    sem->ref_count--;
    
    if (sem->ref_count == 0) {
        sem->is_valid = false;
        sem_count--;
    }
    
    return 0;
}

// =============================================================================
// IPC INITIALIZATION
// =============================================================================

// =============================================================================
// MESSAGE QUEUES
// =============================================================================

#define MAX_MESSAGE_QUEUES 32

static message_queue_t message_queues[MAX_MESSAGE_QUEUES];
static int mq_count = 0;
static uint32_t next_mq_id = 1;

static void mq_init(void) {
    for (int i = 0; i < MAX_MESSAGE_QUEUES; i++) {
        message_queues[i].id = 0;
        message_queues[i].messages = NULL;
        message_queues[i].msg_count = 0;
        message_queues[i].max_messages = 0;
        message_queues[i].max_message_size = 0;
        message_queues[i].is_valid = false;
    }
}

int mq_open(const char* name, int oflag, uint32_t mode, uint32_t max_msgs, size_t max_msg_size) {
    (void)name; (void)oflag; (void)mode;
    
    int idx = -1;
    for (int i = 0; i < MAX_MESSAGE_QUEUES; i++) {
        if (!message_queues[i].is_valid) {
            idx = i;
            break;
        }
    }
    
    if (idx == -1) return -1;
    
    message_queues[idx].id = next_mq_id++;
    message_queues[idx].max_messages = max_msgs;
    message_queues[idx].max_message_size = max_msg_size;
    message_queues[idx].msg_count = 0;
    message_queues[idx].messages = NULL;
    message_queues[idx].is_valid = true;
    
    mq_count++;
    return message_queues[idx].id;
}

int mq_send(int mq_id, const void* buf, size_t size, message_priority_t priority) {
    message_queue_t* mq = mq_get(mq_id);
    if (mq == NULL || buf == NULL || size == 0) return -1;
    
    if (size > mq->max_message_size) return -1;
    if (mq->msg_count >= mq->max_messages) return -1;
    
    message_queue_entry_t* entry = (message_queue_entry_t*)kmalloc(sizeof(message_queue_entry_t));
    if (entry == NULL) return -1;
    
    entry->data = kmalloc(size);
    if (entry->data == NULL) return -1;
    
    memcpy(entry->data, buf, size);
    entry->size = size;
    entry->priority = priority;
    entry->next = NULL;
    
    message_queue_entry_t** p = &mq->messages;
    while (*p != NULL && (*p)->priority <= priority) {
        p = &(*p)->next;
    }
    entry->next = *p;
    *p = entry;
    mq->msg_count++;
    
    return 0;
}

ssize_t mq_receive(int mq_id, void* buf, size_t max_size, message_priority_t* priority) {
    message_queue_t* mq = mq_get(mq_id);
    if (mq == NULL || buf == NULL || mq->messages == NULL) return -1;
    
    message_queue_entry_t* entry = mq->messages;
    mq->messages = entry->next;
    
    size_t copy_size = (entry->size > max_size) ? max_size : entry->size;
    memcpy(buf, entry->data, copy_size);
    
    if (priority) *priority = entry->priority;
    
    mq->msg_count--;
    return copy_size;
}

int mq_close(int mq_id) {
    for (int i = 0; i < MAX_MESSAGE_QUEUES; i++) {
        if (message_queues[i].id == (uint32_t)mq_id && message_queues[i].is_valid) {
            message_queues[i].is_valid = false;
            mq_count--;
            return 0;
        }
    }
    return -1;
}

message_queue_t* mq_get(int mq_id) {
    for (int i = 0; i < MAX_MESSAGE_QUEUES; i++) {
        if (message_queues[i].id == (uint32_t)mq_id && message_queues[i].is_valid) {
            return &message_queues[i];
        }
    }
    return NULL;
}

int ipc_init(void) {
    console_puts("IPC: Initializing IPC system...\n");
    
    pipe_init();
    signal_init();
    shm_init();
    sem_init();
    mq_init();
    
    console_puts("IPC: Initialized\n");
    
    return 0;
}

