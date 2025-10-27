# TEROS System Call Reference

**Version:** 0.1  
**Date:** 2025-01-27

## Overview

This document provides a complete reference for all system calls available in TEROS.

## Invocation

**Method:** `int $0x80` software interrupt

**Registers:**
- `%rax`: System call number
- `%rdi`, `%rsi`, `%rdx`, `%r10`, `%r8`, `%r9`: Arguments 0-5
- `%rax`: Return value (positive/zero on success, negative on error)

## Process Management

### exit (1)

Terminate the calling process.

**Prototype:**
```c
void exit(int status);
```

**Arguments:**
- `status`: Exit code

**Returns:** Never returns

**Example:**
```c
exit(0);  // Successful termination
```

---

### fork (2)

Create a child process.

**Prototype:**
```c
pid_t fork(void);
```

**Returns:**
- Parent: Child PID
- Child: 0
- Error: Negative

**Example:**
```c
pid_t pid = fork();
if (pid == 0) {
    // Child process
} else if (pid > 0) {
    // Parent process
}
```

---

### exec (3)

Replace current process with new program.

**Prototype:**
```c
int exec(const char* path, char* const argv[], char* const envp[]);
```

**Arguments:**
- `path`: Path to executable
- `argv`: Argument vector (currently unused)
- `envp`: Environment vector (currently unused)

**Returns:**
- Success: Does not return
- Error: Negative

**Example:**
```c
exec("/bin/sh", NULL, NULL);
```

---

### wait (4)

Wait for child process to terminate.

**Prototype:**
```c
pid_t wait(pid_t pid);
```

**Arguments:**
- `pid`: Process ID to wait for (-1 for any child)

**Returns:**
- Success: Exit code of child
- Error: Negative

---

### getpid (20)

Get current process ID.

**Prototype:**
```c
pid_t getpid(void);
```

**Returns:** Current PID

---

### getppid (21)

Get parent process ID.

**Prototype:**
```c
pid_t getppid(void);
```

**Returns:** Parent PID

---

### kill (62)

Send signal to process.

**Prototype:**
```c
int kill(pid_t pid, int sig);
```

**Arguments:**
- `pid`: Target process ID
- `sig`: Signal number

**Returns:**
- Success: 0
- Error: Negative

**Signals:**
- `SIGTERM (15)`: Terminate
- `SIGKILL (9)`: Force kill

---

## Memory Management

### brk (45)

Change data segment size.

**Prototype:**
```c
void* brk(void* addr);
```

**Arguments:**
- `addr`: New heap end address

**Returns:** New heap pointer

---

### mmap (9)

Map memory region.

**Prototype:**
```c
void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
```

**Arguments:**
- `addr`: Preferred address (0 for kernel choice)
- `length`: Size to map
- `prot`: Protection flags
- `flags`: Mapping flags
- `fd`: File descriptor (-1 for anonymous)
- `offset`: File offset

**Returns:**
- Success: Mapped address
- Error: Negative

---

### munmap (11)

Unmap memory region.

**Prototype:**
```c
int munmap(void* addr, size_t length);
```

**Arguments:**
- `addr`: Address to unmap
- `length`: Size to unmap

**Returns:**
- Success: 0
- Error: Negative

---

## File Operations

### open (5)

Open file.

**Prototype:**
```c
int open(const char* pathname, int flags, mode_t mode);
```

**Arguments:**
- `pathname`: File path
- `flags`: Open flags (O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, etc.)
- `mode`: Permissions if creating

**Returns:**
- Success: File descriptor (>= 0)
- Error: Negative

**Flags:**
- `O_RDONLY (0x0000)`: Read only
- `O_WRONLY (0x0001)`: Write only
- `O_RDWR (0x0002)`: Read/write
- `O_CREAT (0x0040)`: Create if not exists

---

### close (6)

Close file descriptor.

**Prototype:**
```c
int close(int fd);
```

**Arguments:**
- `fd`: File descriptor

**Returns:**
- Success: 0
- Error: Negative

---

### read (0)

Read from file descriptor.

**Prototype:**
```c
ssize_t read(int fd, void* buf, size_t count);
```

**Arguments:**
- `fd`: File descriptor
- `buf`: Buffer to read into
- `count`: Number of bytes to read

**Returns:**
- Success: Number of bytes read
- EOF: 0
- Error: Negative

---

### write (1)

Write to file descriptor.

**Prototype:**
```c
ssize_t write(int fd, const void* buf, size_t count);
```

**Arguments:**
- `fd`: File descriptor
- `buf`: Data to write
- `count`: Number of bytes

**Returns:**
- Success: Number of bytes written
- Error: Negative

**Special FDs:**
- `0`: stdin
- `1`: stdout
- `2`: stderr

---

### lseek (19)

Reposition file offset.

**Prototype:**
```c
off_t lseek(int fd, off_t offset, int whence);
```

**Arguments:**
- `fd`: File descriptor
- `offset`: Offset value
- `whence`: Origin (SEEK_SET, SEEK_CUR, SEEK_END)

**Returns:**
- Success: New offset
- Error: Negative

---

### stat (107)

Get file status.

**Prototype:**
```c
int stat(const char* pathname, struct stat* statbuf);
```

**Arguments:**
- `pathname`: File path
- `statbuf`: Buffer for file info

**Returns:**
- Success: 0
- Error: Negative

---

## Directory Operations

### opendir (256)

Open directory for reading.

**Prototype:**
```c
DIR* opendir(const char* name);
```

**Arguments:**
- `name`: Directory path

**Returns:**
- Success: Directory handle
- Error: Negative

---

### readdir (257)

Read directory entry.

**Prototype:**
```c
struct dirent* readdir(DIR* dirp);
```

**Arguments:**
- `dirp`: Directory handle

**Returns:**
- Success: Pointer to entry
- End: NULL

---

### closedir (258)

Close directory.

**Prototype:**
```c
int closedir(DIR* dirp);
```

**Arguments:**
- `dirp`: Directory handle

**Returns:**
- Success: 0
- Error: Negative

---

### mkdir (39)

Create directory.

**Prototype:**
```c
int mkdir(const char* pathname, mode_t mode);
```

**Arguments:**
- `pathname`: Directory path
- `mode`: Permissions

**Returns:**
- Success: 0
- Error: Negative

---

### rmdir (40)

Remove directory.

**Prototype:**
```c
int rmdir(const char* pathname);
```

**Arguments:**
- `pathname`: Directory path

**Returns:**
- Success: 0
- Error: Negative

---

## Signal Operations

### signal (48)

Register signal handler.

**Prototype:**
```c
sighandler_t signal(int signum, sighandler_t handler);
```

**Arguments:**
- `signum`: Signal number
- `handler`: Handler function

**Returns:**
- Success: Previous handler
- Error: Negative

---

### sigaction (67)

Advanced signal handling.

**Prototype:**
```c
int sigaction(int signum, const struct sigaction* act, struct sigaction* oldact);
```

**Arguments:**
- `signum`: Signal number
- `act`: New action
- `oldact`: Previous action (output)

**Returns:**
- Success: 0
- Error: Negative

---

## IPC Operations

### pipe (42)

Create pipe.

**Prototype:**
```c
int pipe(int pipefd[2]);
```

**Arguments:**
- `pipefd`: Array to receive read/write FDs

**Returns:**
- Success: 0 (pipefd[0] = read, pipefd[1] = write)
- Error: Negative

**Example:**
```c
int fd[2];
pipe(fd);
write(fd[1], "Hello", 5);
read(fd[0], buf, 5);
```

---

### shmget (29)

Get shared memory segment.

**Prototype:**
```c
int shmget(key_t key, size_t size, int shmflg);
```

**Arguments:**
- `key`: Shared memory key
- `size`: Segment size
- `shmflg`: Flags

**Returns:**
- Success: Shared memory ID
- Error: Negative

---

### shmat (30)

Attach shared memory.

**Prototype:**
```c
void* shmat(int shmid, const void* shmaddr, int shmflg);
```

**Arguments:**
- `shmid`: Shared memory ID
- `shmaddr`: Preferred address (0 for kernel choice)
- `shmflg`: Flags

**Returns:**
- Success: Mapped address
- Error: Negative

---

### shmdt (67)

Detach shared memory.

**Prototype:**
```c
int shmdt(const void* shmaddr);
```

**Arguments:**
- `shmaddr`: Address to detach

**Returns:**
- Success: 0
- Error: Negative

---

## Lambda³ Operations

### lambda_reduce (100)

Perform lambda calculus reduction.

**Prototype:**
```c
int lambda_reduce(lambda_expr_t* expr, int steps);
```

**Arguments:**
- `expr`: Lambda expression
- `steps`: Maximum reduction steps

**Returns:**
- Success: Number of steps performed
- Error: Negative

---

### lambda_typecheck (101)

Type check lambda expression.

**Prototype:**
```c
int lambda_typecheck(lambda_expr_t* expr, lambda_type_t* type);
```

**Arguments:**
- `expr`: Lambda expression
- `type`: Expected type

**Returns:**
- Success: 1 (type correct)
- Failure: 0
- Error: Negative

---

### lambda_eval (102)

Evaluate lambda expression.

**Prototype:**
```c
int lambda_eval(lambda_expr_t* expr, lambda_env_t* env, int steps);
```

**Arguments:**
- `expr`: Expression to evaluate
- `env`: Environment
- `steps`: Maximum steps

**Returns:**
- Success: Result value
- Error: Negative

---

### lambda_parse (103)

Parse lambda expression from string.

**Prototype:**
```c
int lambda_parse(const char* input, lambda_expr_t* output);
```

---

### lambda_compile (104)

Compile lambda expression to bytecode.

**Prototype:**
```c
int lambda_compile(lambda_expr_t* expr, void* output);
```

---

### lambda_optimize (105)

Optimize lambda expression.

**Prototype:**
```c
int lambda_optimize(lambda_expr_t* expr, lambda_expr_t* output);
```

---

### lambda_prove (106)

Prove theorem using lambda calculus.

**Prototype:**
```c
int lambda_prove(lambda_expr_t* expr, lambda_theorem_t* theorem, lambda_proof_t* proof);
```

---

### lambda_verify (107)

Verify proof.

**Prototype:**
```c
int lambda_verify(lambda_proof_t* proof, lambda_theorem_t* theorem);
```

---

## Error Codes

System calls use ternary return values:

- `TERNARY_POSITIVE (+1)`: Success
- `TERNARY_ZERO (0)`: Special case (e.g., EOF)
- `TERNARY_NEGATIVE (-1)`: Error

Traditional errno not yet implemented.

## Summary Table

| Number | Name | Category |
|--------|------|----------|
| 0 | read | File |
| 1 | write | File |
| 2 | fork | Process |
| 3 | exec | Process |
| 4 | wait | Process |
| 5 | open | File |
| 6 | close | File |
| 9 | mmap | Memory |
| 11 | munmap | Memory |
| 19 | lseek | File |
| 20 | getpid | Process |
| 21 | getppid | Process |
| 29 | shmget | IPC |
| 30 | shmat | IPC |
| 39 | mkdir | Directory |
| 40 | rmdir | Directory |
| 42 | pipe | IPC |
| 45 | brk | Memory |
| 48 | signal | Signal |
| 62 | kill | Signal |
| 67 | sigaction / shmdt | Signal / IPC |
| 100-107 | lambda_* | Lambda³ |
| 107 | stat | File |
| 256-258 | *dir | Directory |

**Total Implemented:** 30+ syscalls

## See Also

- [ABI Specification](ABI.md)
- [Build Guide](BUILD.md)
- [T3-ISA](T3-ISA.md)

---

**Maintained by:** TEROS Development Team

