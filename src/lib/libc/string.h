/**
 * @file string.h
 * @brief String operations for TEROS libc
 * @author CodeLlama AI + TEROS Team
 * @date 2025
 */

#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t strlen(const char* str);
size_t strnlen(const char* str, size_t maxlen);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
const char* strchr(const char* s, int c);
const char* strrchr(const char* s, int c);

#endif // STRING_H

