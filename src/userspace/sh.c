/**
 * @file sh.c
 * @brief TEROS Shell - Minimal REPL with Built-in Commands
 * @author TEROS Development Team
 * @date 2025
 */

#include <stdint.h>
#include <stdbool.h>

// System call numbers
#define SYS_EXIT 1
#define SYS_FORK 2
#define SYS_EXEC 3
#define SYS_WAIT 4
#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_GETPID 20
#define SYS_KILL 62

// Simplified syscall interface
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

// Userspace wrappers
static int write(int fd, const char* buf, int count) {
    return (int)syscall3(SYS_WRITE, (uint32_t)fd, (uint32_t)buf, (uint32_t)count);
}

static int read(int fd, char* buf, int count) {
    return (int)syscall3(SYS_READ, (uint32_t)fd, (uint32_t)buf, (uint32_t)count);
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
    while(1);
}

static int wait(int pid) {
    return (int)syscall1(SYS_WAIT, (uint32_t)pid);
}

static int kill(int pid, int sig) {
    return (int)syscall2(SYS_KILL, (uint32_t)pid, (uint32_t)sig);
}

// String functions
static int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static int strncmp(const char* s1, const char* s2, int n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

static void* memset(void* s, int c, int n) {
    unsigned char* p = s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

// Print functions
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
    
    // Reverse
    for (int j = i - 1; j >= 0; j--) {
        write(1, &buf[j], 1);
    }
}

// Command buffer
#define CMD_BUFFER_SIZE 256
static char cmd_buffer[CMD_BUFFER_SIZE];
static int cmd_pos = 0;

/**
 * @brief Read a line from stdin
 */
static int readline(char* buffer, int max_len) {
    int pos = 0;
    
    while (pos < max_len - 1) {
        char c;
        int n = read(0, &c, 1);  // stdin
        
        if (n <= 0) break;
        
        if (c == '\n' || c == '\r') {
            buffer[pos] = '\0';
            print("\n");
            return pos;
        } else if (c == '\b' || c == 127) {  // Backspace
            if (pos > 0) {
                pos--;
                print("\b \b");  // Erase character
            }
        } else if (c >= 32 && c < 127) {  // Printable characters
            buffer[pos++] = c;
            write(1, &c, 1);  // Echo
        }
    }
    
    buffer[pos] = '\0';
    return pos;
}

/**
 * @brief Built-in command: help
 */
static void cmd_help(void) {
    print("TEROS Shell - Built-in Commands:\n");
    print("  help        - Show this help message\n");
    print("  exit        - Exit shell\n");
    print("  echo <args> - Print arguments\n");
    print("  pid         - Show current PID\n");
    print("  clear       - Clear screen\n");
    print("  ls          - List directory (external)\n");
    print("  cat <file>  - Display file (external)\n");
    print("  ps          - List processes (external)\n");
    print("  kill <pid>  - Terminate process (external)\n");
}

/**
 * @brief Built-in command: echo
 */
static void cmd_echo(const char* args) {
    print(args);
    print("\n");
}

/**
 * @brief Built-in command: pid
 */
static void cmd_pid(void) {
    print("Current PID: ");
    print_int(getpid());
    print("\n");
}

/**
 * @brief Built-in command: clear
 */
static void cmd_clear(void) {
    // ANSI escape code to clear screen
    print("\033[2J\033[H");
}

/**
 * @brief Parse and execute command
 */
static void execute_command(const char* cmd) {
    // Skip leading whitespace
    while (*cmd == ' ') cmd++;
    
    // Empty command
    if (*cmd == '\0') return;
    
    // Built-in commands
    if (strcmp(cmd, "help") == 0) {
        cmd_help();
        return;
    }
    
    if (strcmp(cmd, "exit") == 0) {
        print("Exiting shell...\n");
        exit(0);
    }
    
    if (strncmp(cmd, "echo ", 5) == 0) {
        cmd_echo(cmd + 5);
        return;
    }
    
    if (strcmp(cmd, "echo") == 0) {
        print("\n");
        return;
    }
    
    if (strcmp(cmd, "pid") == 0) {
        cmd_pid();
        return;
    }
    
    if (strcmp(cmd, "clear") == 0) {
        cmd_clear();
        return;
    }
    
    // External command - fork and exec
    int pid = fork();
    
    if (pid == 0) {
        // Child process: exec command
        char path[64];
        strcpy(path, "/bin/");
        
        // Extract command name (first word)
        int i = 0;
        while (cmd[i] && cmd[i] != ' ') {
            path[5 + i] = cmd[i];
            i++;
        }
        path[5 + i] = '\0';
        
        int result = exec(path);
        if (result < 0) {
            print("sh: command not found: ");
            print(cmd);
            print("\n");
        }
        exit(1);
    } else if (pid > 0) {
        // Parent process: wait for child
        wait(pid);
    } else {
        print("sh: fork failed\n");
    }
}

/**
 * @brief Shell main loop (REPL)
 */
static void shell_loop(void) {
    while (1) {
        // Print prompt
        print("teros> ");
        
        // Read command
        memset(cmd_buffer, 0, CMD_BUFFER_SIZE);
        int len = readline(cmd_buffer, CMD_BUFFER_SIZE);
        
        if (len <= 0) continue;
        
        // Execute command
        execute_command(cmd_buffer);
    }
}

/**
 * @brief Shell entry point
 */
int main(void) {
    print("\n");
    print("=========================================\n");
    print("   TEROS Shell v0.1\n");
    print("   Type 'help' for available commands\n");
    print("=========================================\n");
    print("\n");
    
    // Enter REPL
    shell_loop();
    
    return 0;
}

