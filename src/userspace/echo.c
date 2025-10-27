/**
 * @file echo.c  
 * @brief Echo utility - Print arguments to stdout
 * @author TEROS Development Team
 * @date 2025
 */

#include <stdint.h>

#define SYS_EXIT 1
#define SYS_WRITE 1

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

int main(int argc, char* argv[]) {
    // Print all arguments separated by spaces
    for (int i = 1; i < argc; i++) {
        print(argv[i]);
        if (i < argc - 1) {
            print(" ");
        }
    }
    print("\n");
    
    return 0;
}

