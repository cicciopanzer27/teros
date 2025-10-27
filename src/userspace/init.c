/**
 * @file init.c
 * @brief Init Process (PID 1) for TEROS
 * @author TEROS Development Team
 * @date 2025
 */

#include <stdint.h>
#include <stdbool.h>

// System call numbers (from syscall.h)
#define SYS_EXIT 1
#define SYS_FORK 2
#define SYS_EXEC 3
#define SYS_WAIT 4
#define SYS_WRITE 1
#define SYS_GETPID 20

// Simplified syscall interface for userspace
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

// Userspace wrappers
static int write(int fd, const char* buf, int count) {
    return (int)syscall3(SYS_WRITE, (uint32_t)fd, (uint32_t)buf, (uint32_t)count);
}

static int getpid(void) {
    return (int)syscall1(SYS_GETPID, 0);
}

static int fork(void) {
    return (int)syscall1(SYS_FORK, 0);
}

static int exec(const char* filename) {
    return (int)syscall3(SYS_EXEC, (uint32_t)filename, 0, 0);
}

static void exit(int code) {
    syscall1(SYS_EXIT, (uint32_t)code);
    while(1);  // Should never reach here
}

static int wait(int pid) {
    return (int)syscall1(SYS_WAIT, (uint32_t)pid);
}

// Simple strlen
static int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

// Simple print function
static void print(const char* str) {
    write(1, str, strlen(str));  // stdout
}

/**
 * @brief Init process entry point
 * 
 * Init is the first userspace process (PID 1) and is responsible for:
 * 1. Initializing the userspace environment
 * 2. Spawning the shell
 * 3. Reaping orphaned processes
 */
int main(void) {
    print("TEROS Init Process (PID 1) starting...\n");
    
    // Verify we are PID 1
    int pid = getpid();
    if (pid != 1) {
        print("ERROR: Init must be PID 1!\n");
        exit(1);
    }
    
    print("Init: PID 1 confirmed\n");
    
    // Spawn shell
    print("Init: Spawning shell...\n");
    int shell_pid = fork();
    
    if (shell_pid == 0) {
        // Child process: exec shell
        print("Init: Child process executing shell\n");
        int result = exec("/bin/sh");
        if (result < 0) {
            print("Init: ERROR - Failed to exec shell\n");
            exit(1);
        }
        // Should never reach here
        exit(0);
    } else if (shell_pid < 0) {
        print("Init: ERROR - Failed to fork shell\n");
        exit(1);
    }
    
    // Parent process: reap zombie processes
    print("Init: Shell spawned, entering reaper loop\n");
    
    while (1) {
        // Wait for any child process to terminate
        int status = wait(-1);  // -1 = wait for any child
        
        if (status >= 0) {
            print("Init: Reaped zombie process\n");
            
            // If shell died, respawn it
            if (status == shell_pid) {
                print("Init: Shell terminated, respawning...\n");
                shell_pid = fork();
                
                if (shell_pid == 0) {
                    exec("/bin/sh");
                    exit(1);
                }
            }
        }
        
        // Yield CPU to avoid busy-wait
        // In a real system, we'd block waiting for SIGCHLD
        asm volatile("pause");
    }
    
    // Should never reach here
    return 0;
}

