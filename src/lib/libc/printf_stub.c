/**
 * @file printf_stub.c
 * @brief Stub implementation of printf for TEROS kernel
 * 
 * This is a minimal stub that allows code with printf() debug statements
 * to link successfully. It does nothing - printf calls are no-ops.
 * 
 * For actual formatted output in the kernel, use console_puts() instead.
 */

#include <stdarg.h>

/**
 * Stub printf - does nothing, just returns 0
 * This allows linking of code that uses printf for debug output
 * without requiring a full printf implementation in the kernel.
 */
int printf(const char* format, ...) {
    // Suppress unused parameter warning
    (void)format;
    
    // Consume variadic arguments (required for ABI compliance)
    va_list args;
    va_start(args, format);
    va_end(args);
    
    // Return 0 (no characters printed)
    return 0;
}

/**
 * Stub snprintf - does nothing, just returns 0
 */
int snprintf(char* str, unsigned long size, const char* format, ...) {
    (void)str;
    (void)size;
    (void)format;
    
    va_list args;
    va_start(args, format);
    va_end(args);
    
    return 0;
}

