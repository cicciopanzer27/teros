/**
 * @file memory.h
 * @brief Memory operations for TEROS libc
 * @author CodeLlama AI + TEROS Team
 * @date 2025
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>

void* memset(void* dest, int value, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
void* memmove(void* dest, const void* src, size_t count);
int memcmp(const void* ptr1, const void* ptr2, size_t count);
void* memchr(const void* ptr, int value, size_t count);

#endif // MEMORY_H

