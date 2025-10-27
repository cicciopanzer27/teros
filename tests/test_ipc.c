/**
 * @file test_ipc.c
 * @brief Tests for IPC System
 * @author TEROS Development Team
 * @date 2025
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "kernel/ipc.h"

void test_pipe_basic(void) {
    printf("Testing pipe operations...\n");
    
    int pipefd[2];
    int result = pipe_open(pipefd);
    assert(result == 0);
    
    const char* test_data = "Hello, TEROS!";
    size_t data_len = strlen(test_data);
    
    ssize_t written = pipe_write(pipefd[1], test_data, data_len);
    assert(written == data_len);
    
    char buffer[128];
    ssize_t read_len = pipe_read(pipefd[0], buffer, sizeof(buffer));
    assert(read_len == data_len);
    
    buffer[read_len] = '\0';
    assert(strcmp(buffer, test_data) == 0);
    
    pipe_close(pipefd[0]);
    pipe_close(pipefd[1]);
    
    printf("PASS: Pipe operations\n");
}

void test_semaphore_basic(void) {
    printf("Testing semaphore operations...\n");
    
    int sem_id = sem_open("test_sem", 0, 0644, 1);
    assert(sem_id > 0);
    
    int result = sem_wait(sem_id);
    assert(result == 0);
    
    result = sem_post(sem_id);
    assert(result == 0);
    
    sem_close(sem_id);
    
    printf("PASS: Semaphore operations\n");
}

int main(void) {
    printf("=== IPC Tests ===\n");
    
    // Initialize IPC system
    ipc_init();
    
    test_pipe_basic();
    test_semaphore_basic();
    
    printf("\n=== All Tests Passed ===\n");
    return 0;
}

