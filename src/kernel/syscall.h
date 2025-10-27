/**
 * @file syscall.h
 * @brief System Call Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include "trit.h"
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// SYSTEM CALL NUMBERS
// =============================================================================

// Process management
#define SYS_EXIT 1
#define SYS_FORK 2
#define SYS_EXEC 3
#define SYS_WAIT 4
#define SYS_GETPID 20
#define SYS_GETPPID 21
#define SYS_KILL 62
#define SYS_SIGNAL 48

// Memory management
#define SYS_MMAP 9
#define SYS_MUNMAP 11
#define SYS_BRK 45

// File operations
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_LSEEK 19
#define SYS_STAT 107

// Directory operations
#define SYS_OPENDIR 256
#define SYS_READDIR 257
#define SYS_CLOSEDIR 258
#define SYS_MKDIR 39
#define SYS_RMDIR 40

// Signal operations
#define SYS_SIGACTION 67

// IPC
#define SYS_PIPE 42
#define SYS_SHMGET 29
#define SYS_SHMAT 30
#define SYS_SHMDT 67

// Lambda³ operations
#define SYS_LAMBDA_REDUCE 100
#define SYS_LAMBDA_TYPECHECK 101
#define SYS_LAMBDA_EVAL 102
#define SYS_LAMBDA_PARSE 103
#define SYS_LAMBDA_COMPILE 104
#define SYS_LAMBDA_OPTIMIZE 105
#define SYS_LAMBDA_PROVE 106
#define SYS_LAMBDA_VERIFY 107

// =============================================================================
// SYSTEM CALL INITIALIZATION
// =============================================================================

/**
 * @brief Initialize system call handler
 */
void syscall_init(void);

/**
 * @brief Register system call handlers
 */
void syscall_register_handlers(void);

/**
 * @brief Register a system call
 * @param syscall_num System call number
 * @param handler Handler function
 * @param name System call name
 * @param arg_count Argument count
 * @param privileged Whether syscall requires privilege
 * @return true if successful
 */
bool syscall_register(uint32_t syscall_num, 
                      trit_t (*handler)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t),
                      const char* name, uint32_t arg_count, bool privileged);

// =============================================================================
// SYSTEM CALL DISPATCHER
// =============================================================================

/**
 * @brief Dispatch system call
 * @param syscall_num System call number
 * @param arg0-arg5 Arguments
 * @return Result
 */
trit_t syscall_dispatch(uint32_t syscall_num, uint32_t arg0, uint32_t arg1, 
                        uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);

// =============================================================================
// PROCESS MANAGEMENT SYSCALLS
// =============================================================================

trit_t syscall_exit(uint32_t exit_code, uint32_t arg1, uint32_t arg2, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_fork(uint32_t arg0, uint32_t arg1, uint32_t arg2, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_exec(uint32_t filename, uint32_t argv, uint32_t envp, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_wait(uint32_t pid, uint32_t arg1, uint32_t arg2, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_getpid(uint32_t arg0, uint32_t arg1, uint32_t arg2, 
                      uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_getppid(uint32_t arg0, uint32_t arg1, uint32_t arg2, 
                       uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_kill(uint32_t pid, uint32_t sig, uint32_t arg2, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_signal(uint32_t sig, uint32_t handler, uint32_t arg2, 
                     uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_sigaction(uint32_t sig, uint32_t act, uint32_t oldact, 
                        uint32_t arg3, uint32_t arg4, uint32_t arg5);

// =============================================================================
// MEMORY MANAGEMENT SYSCALLS
// =============================================================================

trit_t syscall_mmap(uint32_t addr, uint32_t length, uint32_t prot, 
                    uint32_t flags, uint32_t fd, uint32_t offset);
trit_t syscall_munmap(uint32_t addr, uint32_t length, uint32_t arg2, 
                      uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_brk(uint32_t brk, uint32_t arg1, uint32_t arg2, 
                   uint32_t arg3, uint32_t arg4, uint32_t arg5);

// =============================================================================
// FILE SYSTEM SYSCALLS
// =============================================================================

trit_t syscall_open(uint32_t filename, uint32_t flags, uint32_t mode, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_close(uint32_t fd, uint32_t arg1, uint32_t arg2, 
                     uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_read(uint32_t fd, uint32_t buf, uint32_t count, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_write(uint32_t fd, uint32_t buf, uint32_t count, 
                     uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_lseek(uint32_t fd, uint32_t offset, uint32_t whence, 
                     uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_stat(uint32_t path, uint32_t statbuf, uint32_t arg2, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5);

// =============================================================================
// DIRECTORY SYSCALLS
// =============================================================================

trit_t syscall_opendir(uint32_t path, uint32_t arg1, uint32_t arg2, 
                       uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_readdir(uint32_t dirfd, uint32_t arg1, uint32_t arg2, 
                       uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_closedir(uint32_t dirfd, uint32_t arg1, uint32_t arg2, 
                        uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_mkdir(uint32_t path, uint32_t mode, uint32_t arg2, 
                     uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_rmdir(uint32_t path, uint32_t arg1, uint32_t arg2, 
                     uint32_t arg3, uint32_t arg4, uint32_t arg5);

// =============================================================================
// IPC SYSCALLS
// =============================================================================

trit_t syscall_pipe(uint32_t pipefd, uint32_t arg1, uint32_t arg2, 
                    uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_shmget(uint32_t key, uint32_t size, uint32_t shmflg, 
                      uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_shmat(uint32_t shmid, uint32_t shmaddr, uint32_t shmflg, 
                     uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_shmdt(uint32_t shmaddr, uint32_t arg1, uint32_t arg2, 
                     uint32_t arg3, uint32_t arg4, uint32_t arg5);

// =============================================================================
// LAMBDA³ SYSCALLS
// =============================================================================

trit_t syscall_lambda_reduce(uint32_t expr, uint32_t steps, uint32_t arg2, 
                            uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_lambda_typecheck(uint32_t expr, uint32_t type, uint32_t arg2, 
                               uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_lambda_eval(uint32_t expr, uint32_t env, uint32_t steps, 
                          uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_lambda_parse(uint32_t input, uint32_t output, uint32_t arg2, 
                           uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_lambda_compile(uint32_t expr, uint32_t output, uint32_t arg2, 
                             uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_lambda_optimize(uint32_t expr, uint32_t output, uint32_t arg2, 
                              uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_lambda_prove(uint32_t expr, uint32_t theorem, uint32_t proof, 
                           uint32_t arg3, uint32_t arg4, uint32_t arg5);
trit_t syscall_lambda_verify(uint32_t proof, uint32_t theorem, uint32_t arg2, 
                            uint32_t arg3, uint32_t arg4, uint32_t arg5);

// =============================================================================
// SYSCALL QUERY FUNCTIONS
// =============================================================================

/**
 * @brief Get total syscalls
 * @return Total syscalls
 */
uint32_t syscall_get_total_syscalls(void);

/**
 * @brief Get failed syscalls
 * @return Failed syscalls
 */
uint32_t syscall_get_failed_syscalls(void);

/**
 * @brief Get syscall count
 * @return Syscall count
 */
uint32_t syscall_get_syscall_count(void);

/**
 * @brief Check if syscall handler is initialized
 * @return true if initialized
 */
bool syscall_is_initialized(void);

// =============================================================================
// SYSCALL DEBUG FUNCTIONS
// =============================================================================

/**
 * @brief Print syscall statistics
 */
void syscall_print_statistics(void);

#endif // SYSCALL_H
