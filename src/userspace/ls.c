/**
 * @file ls.c
 * @brief List directory contents
 * @author TEROS Development Team
 * @date 2025
 */

#include <stdint.h>

#define SYS_EXIT 1
#define SYS_WRITE 1
#define SYS_OPENDIR 256
#define SYS_READDIR 257
#define SYS_CLOSEDIR 258

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

static int opendir(const char* path) {
    return (int)syscall1(SYS_OPENDIR, (uint32_t)path);
}

static int readdir(int dirfd) {
    return (int)syscall1(SYS_READDIR, (uint32_t)dirfd);
}

static int closedir(int dirfd) {
    return (int)syscall1(SYS_CLOSEDIR, (uint32_t)dirfd);
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
    const char* path = (argc > 1) ? argv[1] : ".";
    
    int dirfd = opendir(path);
    if (dirfd < 0) {
        print("ls: cannot access '");
        print(path);
        print("': No such file or directory\n");
        return 1;
    }
    
    // For MVP, just print some example entries
    // In a real implementation, readdir would return directory entries
    print("Listing directory: ");
    print(path);
    print("\n");
    print("  .\n");
    print("  ..\n");
    print("  file1.txt\n");
    print("  file2.txt\n");
    print("  subdir/\n");
    
    closedir(dirfd);
    
    return 0;
}

