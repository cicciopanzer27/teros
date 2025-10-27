/**
 * @file syscalls.c
 * @brief System call wrappers for TEROS
 * @author TEROS Development Team
 * @date 2025
 */

#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>

// System call numbers
#define SYS_EXIT    0
#define SYS_FORK    1
#define SYS_EXEC    2
#define SYS_WAIT    3
#define SYS_READ    4
#define SYS_WRITE   5
#define SYS_OPEN    6
#define SYS_CLOSE   7
#define SYS_MMAP    8
#define SYS_MUNMAP  9

// Lambda³ system calls
#define SYS_LAMBDA_REDUCE     100
#define SYS_LAMBDA_TYPECHECK  101
#define SYS_LAMBDA_EVAL       102

static inline int syscall0(int num) {
    int ret;
    asm volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num)
        : "memory"
    );
    return ret;
}

static inline int syscall1(int num, int arg1) {
    int ret;
    asm volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "b"(arg1)
        : "memory"
    );
    return ret;
}

static inline int syscall2(int num, int arg1, int arg2) {
    int ret;
    asm volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "b"(arg1), "c"(arg2)
        : "memory"
    );
    return ret;
}

static inline int syscall3(int num, int arg1, int arg2, int arg3) {
    int ret;
    asm volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3)
        : "memory"
    );
    return ret;
}

void _exit(int code) {
    syscall1(SYS_EXIT, code);
    // Should never return, but add infinite loop for safety
    while(1) {}
}

int fork(void) {
    return syscall0(SYS_FORK);
}

int execve(const char* path, char* const argv[], char* const envp[]) {
    (void)path;
    (void)argv;
    (void)envp;
    // Stub implementation
    return -1;
}

int wait(int* status) {
    return syscall1(SYS_WAIT, (uintptr_t)status);
}

ssize_t read(int fd, void* buf, size_t count) {
    return syscall3(SYS_READ, fd, (uintptr_t)buf, count);
}

ssize_t write(int fd, const void* buf, size_t count) {
    return syscall3(SYS_WRITE, fd, (uintptr_t)buf, count);
}

int open(const char* pathname, int flags) {
    return syscall2(SYS_OPEN, (uintptr_t)pathname, flags);
}

int close(int fd) {
    return syscall1(SYS_CLOSE, fd);
}

// Lambda³ system call wrappers
int sys_lambda_reduce(const char* expr, char* result, size_t max_len) {
    (void)expr;
    (void)result;
    (void)max_len;
    // Lambda reduction syscall
    return -1; // Stub implementation
}

int sys_lambda_typecheck(const char* expr, char* type, size_t max_len) {
    (void)expr;
    (void)type;
    (void)max_len;
    // Lambda typecheck syscall
    return -1; // Stub implementation
}

int sys_lambda_eval(const char* expr, char* result, size_t max_len) {
    (void)expr;
    (void)result;
    (void)max_len;
    // Lambda evaluation syscall
    return -1; // Stub implementation
}
