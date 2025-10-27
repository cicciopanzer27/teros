/**
 * @file stdio.h
 * @brief Standard I/O Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#define EOF (-1)
#define BUFSIZ 4096
#define MAXFILES 256

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef struct FILE FILE;

// Standard streams
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

// Standard file descriptors
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

int stdio_init(void);
FILE* fopen(const char* filename, const char* mode);
int fclose(FILE* stream);
size_t fread(void* ptr, size_t size, size_t count, FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t count, FILE* stream);
int fflush(FILE* stream);
int feof(FILE* stream);
int ferror(FILE* stream);

int printf(const char* format, ...);
int fprintf(FILE* stream, const char* format, ...);
int sprintf(char* buffer, const char* format, ...);
int snprintf(char* buffer, size_t size, const char* format, ...);

#endif // STDIO_H

