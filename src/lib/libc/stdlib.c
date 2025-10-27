/**
 * @file stdlib.c
 * @brief Standard Library Implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "stdlib.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Memory management
static bool heap_initialized = false;
static uint8_t heap[HEAP_SIZE];
static size_t heap_used = 0;

void* malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    if (!heap_initialized) {
        memset(heap, 0, HEAP_SIZE);
        heap_initialized = true;
    }
    
    // Simple first-fit allocator
    if (heap_used + size > HEAP_SIZE) {
        return NULL;
    }
    
    void* ptr = heap + heap_used;
    heap_used += size;
    
    return ptr;
}

void free(void* ptr) {
    // Simplified implementation - doesn't actually free
    (void)ptr;
}

void* calloc(size_t num, size_t size) {
    size_t total = num * size;
    void* ptr = malloc(total);
    
    if (ptr != NULL) {
        memset(ptr, 0, total);
    }
    
    return ptr;
}

void* realloc(void* ptr, size_t new_size) {
    if (ptr == NULL) {
        return malloc(new_size);
    }
    
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    
    // Simplified implementation - allocate new block
    void* new_ptr = malloc(new_size);
    if (new_ptr != NULL && ptr != NULL) {
        memcpy(new_ptr, ptr, new_size);  // Simplified
    }
    
    return new_ptr;
}

// String conversion
int atoi(const char* str) {
    if (str == NULL) {
        return 0;
    }
    
    int result = 0;
    int sign = 1;
    int i = 0;
    
    // Skip whitespace
    while (str[i] == ' ' || str[i] == '\t') {
        i++;
    }
    
    // Check for sign
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }
    
    // Convert digits
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    
    return result * sign;
}

long atol(const char* str) {
    if (str == NULL) {
        return 0;
    }
    
    long result = 0;
    int sign = 1;
    int i = 0;
    
    // Skip whitespace
    while (str[i] == ' ' || str[i] == '\t') {
        i++;
    }
    
    // Check for sign
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }
    
    // Convert digits
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    
    return result * sign;
}

// Random number generation
static unsigned long rand_seed = 1;

void srand(unsigned int seed) {
    rand_seed = seed;
}

int rand(void) {
    rand_seed = rand_seed * 1103515245 + 12345;
    return (int)((rand_seed >> 16) & 0x7FFF);
}

// Exit handling
static exit_func_t* exit_funcs[MAX_EXIT_FUNCS];
static int exit_func_count = 0;

int atexit(void (*func)(void)) {
    if (func == NULL || exit_func_count >= MAX_EXIT_FUNCS) {
        return -1;
    }
    
    exit_funcs[exit_func_count] = func;
    exit_func_count++;
    
    return 0;
}

void _exit(int status) {
    // Call exit functions
    for (int i = exit_func_count - 1; i >= 0; i--) {
        if (exit_funcs[i] != NULL) {
            exit_funcs[i]();
        }
    }
    
    // Call syscall to exit
    // syscall(SYS_EXIT, status);
    
    // Should not return
    while (1) {}
}

/**
 * @file stdlib.h
 * @brief Standard Library Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdint.h>

#define HEAP_SIZE (1024 * 1024)  // 1MB heap
#define MAX_EXIT_FUNCS 32

typedef void (*exit_func_t)(void);

void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t new_size);

int atoi(const char* str);
long atol(const char* str);

void srand(unsigned int seed);
int rand(void);

int atexit(void (*func)(void));
void _exit(int status);

#endif // STDLIB_H

