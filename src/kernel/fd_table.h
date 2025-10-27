/**
 * @file fd_table.h
 * @brief File Descriptor Table Management
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef FD_TABLE_H
#define FD_TABLE_H

#include <stdint.h>
#include <stdbool.h>
#include "kernel/fs/vfs.h"

#define MAX_FILE_DESCRIPTORS 256
#define FD_UNUSED 0xFFFFFFFF

// File descriptor entry
typedef struct {
    vfs_file_t* file;
    uint32_t flags;
    uint32_t ref_count;
    bool is_open;
} fd_entry_t;

// Function prototypes
int fd_table_init(void);
int fd_table_alloc(vfs_file_t* file, uint32_t flags);
int fd_table_close(int fd);
vfs_file_t* fd_table_get(int fd);
int fd_table_dup(int oldfd);
int fd_table_dup2(int oldfd, int newfd);
int fd_table_get_ref_count(int fd);
void fd_table_cleanup(void);

#endif // FD_TABLE_H

