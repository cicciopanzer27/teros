/**
 * @file syscall.c
 * @brief System call implementation
 */

#include "syscall.h"
#include "process.h"
#include "../mm/kmalloc.h"

// Syscall handlers
typedef long (*syscall_handler_t)(void*);

static syscall_handler_t syscall_handlers[256];

void syscall_init(void) {
    // Clear all handlers
    for (int i = 0; i < 256; i++) {
        syscall_handlers[i] = NULL;
    }
}

long syscall_invoke(uint64_t sysnum, syscall_args_t* args) {
    if (sysnum >= 256) {
        return -1; // Invalid syscall
    }
    
    if (syscall_handlers[sysnum] == NULL) {
        return -1; // Not implemented
    }
    
    return syscall_handlers[sysnum](args);
}

char* syscall_lambda_reduce(const char* expression, int max_steps) {
    // TODO: Implement Lambda³ reduction via syscall
    return NULL;
}

