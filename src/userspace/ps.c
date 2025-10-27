/**
 * @file ps.c
 * @brief Process status utility - List running processes
 * @author TEROS Development Team
 * @date 2025
 */

#include <stdint.h>

#define SYS_EXIT 1
#define SYS_WRITE 1
#define SYS_GETPID 20

static inline int64_t syscall1(uint32_t num, uint32_t arg1) {
    int64_t ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(arg1));
    return ret;
}

static inline int64_t syscall3(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    int64_t ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3));
    return ret;
}

static int write(int fd, const char* buf, int count) {
    return (int)syscall3(SYS_WRITE, (uint32_t)fd, (uint32_t)buf, (uint32_t)count);
}

static int getpid(void) {
    return (int)syscall1(SYS_GETPID, 0);
}

static void exit(int code) {
    syscall1(SYS_EXIT, (uint32_t)code);
    while(1);
}

static int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

static void print(const char* str) {
    write(1, str, strlen(str));
}

static void print_int(int n) {
    if (n == 0) {
        print("0");
        return;
    }
    
    if (n < 0) {
        print("-");
        n = -n;
    }
    
    char buf[12];
    int i = 0;
    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    
    for (int j = i - 1; j >= 0; j--) {
        write(1, &buf[j], 1);
    }
}

int main(void) {
    print("  PID  STATE    NAME\n");
    print("----------------------------\n");
    
    // For MVP, show static process list
    // In a real implementation, we'd have a syscall to enumerate processes
    print("    1  Running  init\n");
    print("    2  Running  sh\n");
    print("    ");
    print_int(getpid());
    print("  Running  ps\n");
    
    return 0;
}

