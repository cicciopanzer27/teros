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
        if (pipes[i].id == pipe_id && pipes[i].is_open) {
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
            // TODO: kfree(pipe->buffer);
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

static void signal_init(void) {
    for (int i = 0; i < SIGMAX; i++) {
        signal_handlers[i] = NULL;
        signal_registered[i] = false;
    }
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
    
    // TODO: Send signal to process
    console_puts("IPC: Sending signal ");
    // TODO: Print sig
    console_puts(" to PID ");
    // TODO: Print pid
    console_puts("\n");
    
    return signal_dispatch(pid, sig);
}

int signal_dispatch(int pid, int sig) {
    signal_handler_t handler = signal_get_handler(sig);
    
    if (handler != NULL) {
        handler(sig);
        return 0;
    }
    
    // Default action
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
    }
}

int shm_open(const char* name, int oflag, uint32_t mode) {
    // TODO: Implement named shared memory
    // For now, just return a new shm block
    
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
        if (shm_blocks[i].id == shm_id && shm_blocks[i].is_valid) {
            return &shm_blocks[i];
        }
    }
    return NULL;
}

void* shm_map(int shm_id, size_t size) {
    shared_memory_t* shm = shm_get(shm_id);
    if (shm == NULL || size == 0) {
        return NULL;
    }
    
    // Allocate memory
    void* addr = kmalloc(size);
    if (addr == NULL) {
        return NULL;
    }
    
    shm->addr = addr;
    shm->size = size;
    
    return addr;
}

int shm_unmap(void* addr, size_t size) {
    for (int i = 0; i < MAX_SHARED_MEMORY; i++) {
        if (shm_blocks[i].addr == addr && shm_blocks[i].is_valid) {
            shm_blocks[i].ref_count--;
            
            if (shm_blocks[i].ref_count == 0) {
                // TODO: kfree(shm_blocks[i].addr);
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
    // TODO: Implement
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
    }
}

int sem_open(const char* name, int oflag, uint32_t mode, uint32_t value) {
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
        if (semaphores[i].id == sem_id && semaphores[i].is_valid) {
            return &semaphores[i];
        }
    }
    return NULL;
}

int sem_wait(int sem_id) {
    semaphore_t* sem = sem_get(sem_id);
    if (sem == NULL) {
        return -1;
    }
    
    // Wait for semaphore
    while (sem->value <= 0) {
        // TODO: Sleep/wait
    }
    
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

int ipc_init(void) {
    console_puts("IPC: Initializing IPC system...\n");
    
    pipe_init();
    signal_init();
    shm_init();
    sem_init();
    
    console_puts("IPC: Initialized\n");
    
    return 0;
}

