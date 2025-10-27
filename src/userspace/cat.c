/**
 * @file cat.c
 * @brief Cat utility - Concatenate and print files
 * @author TEROS Development Team
 * @date 2025
 */

#include <stdint.h>

#define SYS_EXIT 1
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_READ 0
#define SYS_WRITE 1

// File flags
#define O_RDONLY 0x0000

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

static int open(const char* filename, int flags) {
    return (int)syscall3(SYS_OPEN, (uint32_t)filename, (uint32_t)flags, 0);
}

static int close(int fd) {
    return (int)syscall1(SYS_CLOSE, (uint32_t)fd);
}

static int read(int fd, char* buf, int count) {
    return (int)syscall3(SYS_READ, (uint32_t)fd, (uint32_t)buf, (uint32_t)count);
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
    if (argc < 2) {
        print("Usage: cat <file>\n");
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        
        if (fd < 0) {
            print("cat: ");
            print(argv[i]);
            print(": No such file or directory\n");
            continue;
        }
        
        // Read and print file contents
        char buffer[256];
        int n;
        
        while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
            write(1, buffer, n);
        }
        
        close(fd);
    }
    
    return 0;
}

