/**
 * @file vfs.h
 * @brief Virtual File System interface
 */

#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define VFS_MAX_PATH 256
#define VFS_MAX_NAME 64

// File types
#define VFS_TYPE_FILE     0x1
#define VFS_TYPE_DIR      0x2
#define VFS_TYPE_SYMLINK  0x4
#define VFS_TYPE_DEVICE   0x8

// Open flags
#define VFS_O_RDONLY  0x1
#define VFS_O_WRONLY  0x2
#define VFS_O_RDWR    0x3
#define VFS_O_CREAT   0x4
#define VFS_O_TRUNC   0x8
#define VFS_O_APPEND  0x10

// Inode structure
typedef struct {
    uint32_t inode_num;
    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
    uint32_t size;
    uint32_t blocks;
    uint32_t links;
    uint32_t atime;
    uint32_t mtime;
    uint32_t ctime;
    uint32_t data_blocks[12];
} vfs_inode_t;

// File descriptor
typedef struct {
    uint32_t fd;
    uint32_t inode;
    uint32_t offset;
    uint32_t flags;
    bool valid;
} vfs_fd_t;

// Directory entry
typedef struct {
    uint32_t inode;
    char name[VFS_MAX_NAME];
    uint8_t type;
} vfs_dentry_t;

// File operations
typedef struct {
    int (*open)(vfs_inode_t* inode, uint32_t flags);
    int (*read)(vfs_inode_t* inode, void* buffer, size_t count, size_t offset);
    int (*write)(vfs_inode_t* inode, const void* buffer, size_t count, size_t offset);
    int (*close)(vfs_inode_t* inode);
    int (*sync)(vfs_inode_t* inode);
} vfs_file_ops_t;

// Directory operations
typedef struct {
    int (*lookup)(vfs_inode_t* dir, const char* name);
    int (*create)(vfs_inode_t* dir, const char* name, uint32_t mode);
    int (*unlink)(vfs_inode_t* dir, const char* name);
    int (*mkdir)(vfs_inode_t* dir, const char* name, uint32_t mode);
    int (*rmdir)(vfs_inode_t* dir, const char* name);
} vfs_dir_ops_t;

// Filesystem operations
typedef struct {
    vfs_file_ops_t file_ops;
    vfs_dir_ops_t dir_ops;
    int (*mount)(const char* device, const char* mountpoint);
    int (*umount)(const char* mountpoint);
} vfs_fs_ops_t;

// VFS functions
void vfs_init(void);
int vfs_open(const char* path, uint32_t flags);
int vfs_read(int fd, void* buffer, size_t count);
int vfs_write(int fd, const void* buffer, size_t count);
int vfs_close(int fd);
int vfs_mkdir(const char* path, uint32_t mode);
int vfs_rmdir(const char* path);
int vfs_unlink(const char* path);
vfs_inode_t* vfs_lookup(const char* path);

#endif // VFS_H

