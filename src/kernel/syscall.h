/**
 * @file syscall.h
 * @brief System call definitions for TEROS
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

// =============================================================================
// SYSCALL NUMBERS
// =============================================================================

// Basic syscalls (0-99)
#define SYS_EXIT    0
#define SYS_FORK    1
#define SYS_EXEC    2
#define SYS_WAIT    3
#define SYS_GETPID  4
#define SYS_KILL    5
#define SYS_SIGNAL  6
#define SYS_BRK     7
#define SYS_MMAP    8
#define SYS_MUNMAP  9

// File I/O syscalls (10-29)
#define SYS_OPEN    10
#define SYS_READ    11
#define SYS_WRITE   12
#define SYS_CLOSE   13
#define SYS_SEEK    14
#define SYS_STAT    15
#define SYS_UNLINK  16

// File System syscalls (30-49)
#define SYS_MKDIR   30
#define SYS_RMDIR   31
#define SYS_LINK    32
#define SYS_UNLINK  33
#define SYS_SYMLINK 34
#define SYS_READDIR 35

// IPC syscalls (50-79)
#define SYS_PIPE    50
#define SYS_DUP     51
#define SYS_DUP2    52
#define SYS_SHMAT   60
#define SYS_SHMGET  61
#define SYS_SHMCTL  62
#define SYS_SEMGET  70
#define SYS_SEMOP   71
#define SYS_SEMCTL  72

// Network syscalls (80-99)
#define SYS_SOCKET  80
#define SYS_BIND    81
#define SYS_LISTEN  82
#define SYS_ACCEPT  83
#define SYS_CONNECT 84
#define SYS_SEND    85
#define SYS_RECV    86

// Lambda³ syscalls (100-120)
#define SYS_LAMBDA_REDUCE     100  // Lambda calculus reduction
#define SYS_LAMBDA_TYPECHECK  101  // Type checking
#define SYS_LAMBDA_EVAL       102  // Expression evaluation
#define SYS_LAMBDA_PARSE      103  // Parse lambda expression
#define SYS_LAMBDA_INFER      104  // Type inference
#define SYS_LAMBDA_PROOF      105  // Proof assistance
#define SYS_LAMBDA_COMPILE    106  // Compile to TVM bytecode

// System information (120-129)
#define SYS_UNAME     120
#define SYS_GETTIME   121
#define SYS_NANOSLEEP 122

// =============================================================================
// SYSCALL STRUCTURES
// =============================================================================

// Syscall parameter structure
typedef struct {
    uint64_t arg0;
    uint64_t arg1;
    uint64_t arg2;
    uint64_t arg3;
    uint64_t arg4;
    uint64_t arg5;
} syscall_args_t;

// Lambda³ reduction request
typedef struct {
    char* expression;    // Lambda expression string
    size_t expr_len;     // Length of expression
    int max_steps;       // Maximum reduction steps
} lambda_reduce_req_t;

// Lambda³ reduction response
typedef struct {
    char* result;        // Reduced expression
    size_t result_len;   // Length of result
    int steps_taken;     // Number of steps taken
    int status;          // Status code
} lambda_reduce_resp_t;

// =============================================================================
// SYSCALL INTERFACE
// =============================================================================

/**
 * @brief Invoke a system call
 * @param sysnum System call number
 * @param args System call arguments
 * @return Result of system call (or error code)
 */
long syscall_invoke(uint64_t sysnum, syscall_args_t* args);

/**
 * @brief Lambda calculus reduction via syscall
 * @param expression Lambda expression to reduce
 * @param max_steps Maximum reduction steps
 * @return Reduced expression or NULL on error
 */
char* syscall_lambda_reduce(const char* expression, int max_steps);

#endif // SYSCALL_H

