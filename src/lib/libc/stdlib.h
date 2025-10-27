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

