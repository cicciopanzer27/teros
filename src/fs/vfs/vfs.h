/**
 * @file vfs.h
 * @brief Minimal VFS interface
 */

#ifndef VFS_H
#define VFS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// VFS API (minimal for 2-week sprint)
void vfs_init(void);
int vfs_open(const char* path, int flags);
int vfs_read(int fd, void* buffer, size_t count);
int vfs_write(int fd, const void* buffer, size_t count);
int vfs_close(int fd);

#endif // VFS_H
