/**
 * @file ipc.h
 * @brief Inter-Process Communication (IPC) Interface
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef IPC_H
#define IPC_H

#include <stdint.h>
#include <stddef.h>

// =============================================================================
// PIPES
// =============================================================================

typedef struct pipe {
    uint32_t id;
    char* buffer;
    size_t buffer_size;
    size_t read_pos;
    size_t write_pos;
    uint32_t ref_count;
    bool is_open;
} pipe_t;

int pipe_open(int pipefd[2]);
int pipe_close(int fd);
ssize_t pipe_read(int fd, void* buf, size_t count);
ssize_t pipe_write(int fd, const void* buf, size_t count);
pipe_t* pipe_get(int pipe_id);

// =============================================================================
// SIGNALS
// =============================================================================

typedef enum {
    SIGHUP    = 1,
    SIGINT    = 2,
    SIGQUIT   = 3,
    SIGILL    = 4,
    SIGTRAP   = 5,
    SIGABRT   = 6,
    SIGBUS    = 7,
    SIGFPE    = 8,
    SIGKILL   = 9,
    SIGUSR1   = 10,
    SIGSEGV   = 11,
    SIGUSR2   = 12,
    SIGPIPE   = 13,
    SIGALRM   = 14,
    SIGTERM   = 15,
    SIGCHLD   = 16,
    SIGCONT   = 17,
    SIGSTOP   = 18,
    SIGTSTP   = 19,
    SIGTTIN   = 20,
    SIGTTOU   = 21,
    SIGURG    = 22,
    SIGXCPU   = 24,
    SIGXFSZ   = 25,
    SIGVTALRM = 26,
    SIGPROF   = 27,
    SIGWINCH  = 28,
    SIGIO     = 29,
    SIGPWR    = 30,
    SIGSYS    = 31,
    SIGMAX    = 32
} signal_t;

typedef void (*signal_handler_t)(int sig);

int signal_register(int sig, signal_handler_t handler);
int signal_send(int pid, int sig);
int signal_dispatch(int pid, int sig);
signal_handler_t signal_get_handler(int sig);

// =============================================================================
// SHARED MEMORY
// =============================================================================

typedef struct shared_memory {
    uint32_t id;
    void* addr;
    size_t size;
    uint32_t ref_count;
    bool is_valid;
} shared_memory_t;

int shm_open(const char* name, int oflag, uint32_t mode);
int shm_unlink(const char* name);
void* shm_map(int shm_id, size_t size);
int shm_unmap(void* addr, size_t size);
shared_memory_t* shm_get(int shm_id);

// =============================================================================
// SEMAPHORES
// =============================================================================

typedef struct semaphore {
    uint32_t id;
    int32_t value;
    int32_t max_value;
    uint32_t ref_count;
    bool is_valid;
} semaphore_t;

int sem_open(const char* name, int oflag, uint32_t mode, uint32_t value);
int sem_close(int sem_id);
int sem_wait(int sem_id);
int sem_post(int sem_id);
int sem_trywait(int sem_id);
semaphore_t* sem_get(int sem_id);

// =============================================================================
// IPC INITIALIZATION
// =============================================================================

int ipc_init(void);

#endif // IPC_H

