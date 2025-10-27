/**
 * @file errno.c
 * @brief Error code implementation
 * @note Adapted from musl libc (MIT License)
 * @author TEROS Development Team
 * @date 2025
 */

#include "errno.h"

static int __errno_value = 0;

int* __errno_location(void) {
    return &__errno_value;
}

