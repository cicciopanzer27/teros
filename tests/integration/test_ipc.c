/**
 * @file test_ipc.c
 * @brief End-to-end integration tests for IPC system
 */

#include <stdint.h>
#include <stdbool.h>
#include "ipc.h"
#include "console.h"

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT(condition, msg) \
    do { \
        tests_run++; \
        if (condition) { \
            tests_passed++; \
            console_puts("✓ PASS: "); \
            console_puts(msg); \
            console_puts("\n"); \
        } else { \
            tests_failed++; \
            console_puts("✗ FAIL: "); \
            console_puts(msg); \
            console_puts("\n"); \
        } \
    } while(0)

// Signal handler for testing
static void test_signal_handler(int sig) {
    (void)sig;
}

// Test 1: Signal registration
void test_signal_registration(void) {
    int result = signal_register(SIGUSR1, test_signal_handler);
    ASSERT(result == 0, "signal_register successful");
    
    signal_handler_t handler = signal_get_handler(SIGUSR1);
    ASSERT(handler != NULL, "signal handler retrieved");
}

// Test 2: Signal masking
void test_signal_masking(void) {
    uint32_t original_mask = signal_get_mask();
    
    int result = signal_mask(SIGUSR1);
    ASSERT(result == 0, "signal_mask successful");
    
    uint32_t new_mask = signal_get_mask();
    ASSERT(new_mask != original_mask, "signal mask changed");
    
    result = signal_unmask(SIGUSR1);
    ASSERT(result == 0, "signal_unmask successful");
}

// Test 3: Shared memory
void test_shared_memory(void) {
    int shm_id = shm_open("/test_shm", 0, 0);
    ASSERT(shm_id >= 0, "shm_open successful");
    
    if (shm_id >= 0) {
        void* addr = shm_map(shm_id, 4096);
        ASSERT(addr != NULL, "shm_map successful");
        
        if (addr) {
            shared_memory_t* shm = shm_get(shm_id);
            ASSERT(shm != NULL, "shm_get successful");
        }
    }
}

// Test 4: Semaphore operations
void test_semaphores(void) {
    int sem_id = sem_open("/test_sem", 0, 0, 1);
    ASSERT(sem_id >= 0, "sem_open successful");
    
    if (sem_id >= 0) {
        int result = sem_wait(sem_id);
        ASSERT(result == 0, "sem_wait successful");
        
        result = sem_post(sem_id);
        ASSERT(result == 0, "sem_post successful");
        
        result = sem_close(sem_id);
        ASSERT(result == 0, "sem_close successful");
    }
}

// Test 5: Message queues
void test_message_queues(void) {
    int mq_id = mq_open("/test_mq", 0, 0, 10, 128);
    ASSERT(mq_id >= 0, "mq_open successful");
    
    if (mq_id >= 0) {
        uint8_t test_data[] = "Test message";
        int result = mq_send(mq_id, test_data, sizeof(test_data), MSG_PRIO_NORMAL);
        ASSERT(result == 0, "mq_send successful");
        
        uint8_t recv_buffer[128];
        message_priority_t priority;
        ssize_t received = mq_receive(mq_id, recv_buffer, sizeof(recv_buffer), &priority);
        ASSERT(received > 0, "mq_receive successful");
        
        mq_close(mq_id);
    }
}

// Test 6: Pipes
void test_pipes(void) {
    int pipefd[2];
    int result = pipe_open(pipefd);
    ASSERT(result == 0, "pipe_open successful");
    
    if (result == 0) {
        uint8_t test_data[] = "Test pipe data";
        ssize_t written = pipe_write(pipefd[1], test_data, sizeof(test_data));
        ASSERT(written > 0, "pipe_write successful");
        
        uint8_t recv_buffer[128];
        ssize_t read_bytes = pipe_read(pipefd[0], recv_buffer, sizeof(recv_buffer));
        ASSERT(read_bytes > 0, "pipe_read successful");
        
        pipe_close(pipefd[0]);
        pipe_close(pipefd[1]);
    }
}

int main(void) {
    console_puts("\n=== IPC Integration Tests ===\n\n");
    
    test_signal_registration();
    test_signal_masking();
    test_shared_memory();
    test_semaphores();
    test_message_queues();
    test_pipes();
    
    console_puts("\n=== Test Summary ===\n");
    console_puts("Total:   ");
    printf("%d", tests_run);
    console_puts("\nPassed:  ");
    printf("%d", tests_passed);
    console_puts("\nFailed:  ");
    printf("%d", tests_failed);
    console_puts("\n");
    
    return (tests_failed > 0) ? 1 : 0;
}

