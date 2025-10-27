/**
 * @file kill.c
 * @brief Kill utility - Send signals to processes
 * @author TEROS Development Team
 * @date 2025
 */

#include <stdint.h>

#define SYS_EXIT 1
#define SYS_WRITE 1
#define SYS_KILL 62

// Signal definitions
#define SIGTERM 15
#define SIGKILL 9

static inline int64_t syscall1(uint32_t num, uint32_t arg1) {
    int64_t ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(arg1));
    return ret;
}

static inline int64_t syscall2(uint32_t num, uint32_t arg1, uint32_t arg2) {
    int64_t ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(arg1), "c"(arg2));
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

static int kill(int pid, int sig) {
    return (int)syscall2(SYS_KILL, (uint32_t)pid, (uint32_t)sig);
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

// Simple atoi implementation
static int atoi(const char* str) {
    int result = 0;
    int sign = 1;
    
    // Skip whitespace
    while (*str == ' ') str++;
    
    // Handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    // Convert digits
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print("Usage: kill [-SIGNAL] <pid>\n");
        print("  -9   SIGKILL (force kill)\n");
        print("  -15  SIGTERM (terminate, default)\n");
        return 1;
    }
    
    int sig = SIGTERM;  // Default signal
    int pid_arg_idx = 1;
    
    // Check for signal argument
    if (argv[1][0] == '-') {
        sig = atoi(argv[1] + 1);
        pid_arg_idx = 2;
        
        if (argc < 3) {
            print("kill: missing PID\n");
            return 1;
        }
    }
    
    int pid = atoi(argv[pid_arg_idx]);
    
    if (pid <= 0) {
        print("kill: invalid PID\n");
        return 1;
    }
    
    int result = kill(pid, sig);
    
    if (result < 0) {
        print("kill: failed to send signal to process\n");
        return 1;
    }
    
    print("Signal sent to process ");
    print(argv[pid_arg_idx]);
    print("\n");
    
    return 0;
}

