/**
 * @file vfs.h
 * @brief Virtual File System Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// File flags
#define O_RDONLY 0x0000
#define O_WRONLY 0x0001
#define O_RDWR   0x0002
#define O_CREAT  0x0040
#define O_TRUNC  0x0200
#define O_APPEND 0x0400

// Seek whence
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

// Directory entry
typedef struct {
    uint32_t inode;
    uint8_t type;
    char name[256];
} vfs_dirent_t;

// File structure
typedef struct {
    void* inode;
    size_t pos;
    uint32_t flags;
} vfs_file_t;

// Directory structure
typedef struct {
    void* inode;
    size_t pos;
} vfs_dir_t;

// Forward declarations
typedef struct vfs_filesystem vfs_fs_t;
typedef struct vfs_inode vfs_inode_t;

// =============================================================================
// VFS INITIALIZATION
// =============================================================================

/**
 * @brief Initialize VFS
 */
void vfs_init(void);

/**
 * @brief Register filesystem
 * @param name Filesystem name
 * @param ops Filesystem operations
 * @return 0 on success, -1 on failure
 */
int vfs_register_filesystem(const char* name, void* ops);

/**
 * @brief Find filesystem by name
 * @param name Filesystem name
 * @return Filesystem structure or NULL
 */
vfs_fs_t* vfs_find_filesystem(const char* name);

// =============================================================================
// MOUNT OPERATIONS
// =============================================================================

/**
 * @brief Mount filesystem
 * @param source Source device
 * @param target Mount point
 * @param fs_type Filesystem type
 * @param flags Mount flags
 * @return 0 on success, -1 on failure
 */
int vfs_mount(const char* source, const char* target, const char* fs_type, uint32_t flags);

/**
 * @brief Unmount filesystem
 * @param target Mount point
 * @return 0 on success, -1 on failure
 */
int vfs_umount(const char* target);

// =============================================================================
// PATH OPERATIONS
// =============================================================================

/**
 * @brief Lookup inode by path
 * @param path File path
 * @return Inode or NULL
 */
vfs_inode_t* vfs_lookup(const char* path);

// =============================================================================
// FILE OPERATIONS
// =============================================================================

/**
 * @brief Open file
 * @param path File path
 * @param flags Open flags
 * @param file Output file handle
 * @return 0 on success, -1 on failure
 */
int vfs_open(const char* path, uint32_t flags, vfs_file_t** file);

/**
 * @brief Close file
 * @param file File handle
 * @return 0 on success, -1 on failure
 */
int vfs_close(vfs_file_t* file);

/**
 * @brief Read from file
 * @param file File handle
 * @param buf Buffer
 * @param count Count to read
 * @return Bytes read or -1 on error
 */
ssize_t vfs_read(vfs_file_t* file, void* buf, size_t count);

/**
 * @brief Write to file
 * @param file File handle
 * @param buf Buffer
 * @param count Count to write
 * @return Bytes written or -1 on error
 */
ssize_t vfs_write(vfs_file_t* file, const void* buf, size_t count);

/**
 * @brief Check if VFS is initialized
 * @return true if initialized
 */
bool vfs_is_initialized(void);

#endif // VFS_H

