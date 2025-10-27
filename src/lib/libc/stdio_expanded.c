/**
 * @file stdio_expanded.c
 * @brief Extended Standard I/O Implementation
 * @note Adapted from musl libc (MIT License)
 * @author TEROS Development Team  
 * @date 2025
 */

#include "stdio.h"
#include <stdarg.h>
#include <limits.h>

// Extended printf implementation
static int format_integer(char* buffer, size_t size, int value, int base) {
    if (size == 0) return 0;
    
    bool negative = value < 0;
    if (negative) value = -value;
    
    size_t idx = 0;
    
    // Handle zero case
    if (value == 0) {
        if (idx < size - 1) {
            buffer[idx++] = '0';
        }
    } else {
        // Convert to string (reverse order)
        while (value > 0 && idx < size - 1) {
            int digit = value % base;
            buffer[idx++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
            value /= base;
        }
    }
    
    // Add negative sign
    if (negative && idx < size - 1) {
        buffer[idx++] = '-';
    }
    
    // Add null terminator
    buffer[idx] = '\0';
    
    // Reverse string
    for (size_t i = 0; i < idx / 2; i++) {
        char tmp = buffer[i];
        buffer[i] = buffer[idx - 1 - i];
        buffer[idx - 1 - i] = tmp;
    }
    
    return idx;
}

int vsnprintf(char* buffer, size_t size, const char* format, va_list args) {
    if (buffer == NULL || format == NULL || size == 0) {
        return -1;
    }
    
    size_t buffer_pos = 0;
    size_t format_pos = 0;
    
    while (format[format_pos] != '\0' && buffer_pos < size - 1) {
        if (format[format_pos] == '%') {
            format_pos++;
            
            // Handle format specifiers
            switch (format[format_pos]) {
                case 'd':
                case 'i': {
                    int value = va_arg(args, int);
                    char num_buf[32];
                    int num_len = format_integer(num_buf, sizeof(num_buf), value, 10);
                    
                    for (int i = 0; i < num_len && buffer_pos < size - 1; i++) {
                        buffer[buffer_pos++] = num_buf[i];
                    }
                    break;
                }
                case 's': {
                    const char* str = va_arg(args, const char*);
                    if (str == NULL) str = "(null)";
                    
                    while (*str != '\0' && buffer_pos < size - 1) {
                        buffer[buffer_pos++] = *str++;
                    }
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    if (buffer_pos < size - 1) {
                        buffer[buffer_pos++] = c;
                    }
                    break;
                }
                case 'x':
                case 'X': {
                    unsigned int value = va_arg(args, unsigned int);
                    char num_buf[32];
                    int num_len = format_integer(num_buf, sizeof(num_buf), (int)value, 16);
                    
                    for (int i = 0; i < num_len && buffer_pos < size - 1; i++) {
                        buffer[buffer_pos++] = num_buf[i];
                    }
                    break;
                }
                case '%': {
                    if (buffer_pos < size - 1) {
                        buffer[buffer_pos++] = '%';
                    }
                    break;
                }
                default:
                    if (buffer_pos < size - 1) {
                        buffer[buffer_pos++] = format[format_pos];
                    }
                    break;
            }
            format_pos++;
        } else {
            buffer[buffer_pos++] = format[format_pos++];
        }
    }
    
    buffer[buffer_pos] = '\0';
    return (int)buffer_pos;
}

int sprintf(char* buffer, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, SIZE_MAX, format, args);
    va_end(args);
    return result;
}

int snprintf(char* buffer, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, size, format, args);
    va_end(args);
    return result;
}

int vsprintf(char* buffer, const char* format, va_list args) {
    return vsnprintf(buffer, SIZE_MAX, format, args);
}

int sscanf(const char* str, const char* format, ...) {
    // Simplified implementation
    (void)str;
    (void)format;
    // TODO: Implement full scanf
    return 0;
}

