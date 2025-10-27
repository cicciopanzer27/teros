/**
 * @file stdarg.c
 * @brief Variable argument list handling
 * @note Adapted from musl libc (MIT License)
 * @author TEROS Development Team
 * @date 2025
 */

#include "stdarg.h"

#define va_list __builtin_va_list
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
#define va_copy __builtin_va_copy

// Implementation using GCC builtins
// These are compiler intrinsics, no runtime code needed

/**
 * @file stdarg.h
 * @brief Variable argument list header
 * @note Adapted from musl libc (MIT License)
 */

#ifndef STDARG_H
#define STDARG_H

typedef __builtin_va_list va_list;

#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_end(ap) __builtin_va_end(ap)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_copy(dest, src) __builtin_va_copy(dest, src)

#endif // STDARG_H

