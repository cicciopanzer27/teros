/**
 * @file vfs.c
 * @brief Virtual File System (VFS) Implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "vfs.h"
#include "console.h"
#include "kmalloc.h"
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// VFS IMPLEMENTATION
// =============================================================================

#define VFS_MAX_FILESYSTEMS 16
#define VFS_MAX_MOUNTS 32
#define VFS_MAX_FILE_DESCRIPTORS 1024
#define VFS_MAX_PATH_LENGTH 512

// Inode structure
typedef struct {
    uint32_t inode_num;
    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
    uint64_t size;
    uint64_t atime;  // Access time
    uint64_t mtime;  // Modification time
    uint64_t ctime;  // Creation time
    void* fs_data;   // Filesystem-specific data
} vfs_inode_t;

// VFS operations
typedef struct {
    int (*mount)(vfs_fs_t* fs, const char* source, const char* target);
    int (*umount)(vfs_fs_t* fs);
    vfs_inode_t* (*lookup)(vfs_fs_t* fs, const char* path);
    int (*mkdir)(vfs_fs_t* fs, const char* path, uint32_t mode);
    int (*rmdir)(vfs_fs_t* fs, const char* path);
    int (*create)(vfs_fs_t* fs, const char* path, uint32_t mode);
    int (*unlink)(vfs_fs_t* fs, const char* path);
    ssize_t (*read)(vfs_file_t* file, void* buf, size_t count);
    ssize_t (*write)(vfs_file_t* file, const void* buf, size_t count);
    off_t (*lseek)(vfs_file_t* file, off_t offset, int whence);
    int (*readdir)(vfs_dir_t* dir, pajm_t* entry);
} vfs_ops_t;

// VFS filesystem structure
typedef struct vfs_filesystem {
    const char* name;
    vfs_ops_t* ops;
    bool initialized;
    void* fs_data;
    struct vfs_filesystem* next;
} vfs_fs_t;

// VFS mount point
typedef struct {
    const char* path;
    vfs_fs_t* fs;
    vfs_inode_t* root;
    bool mounted;
} vfs_mount_t;

// VFS file descriptor
typedef struct {
    vfs_file_t* file;
    uint32_t flags;
    uint32_t refcount;
    bool used;
} vfs_fd_t;

// VFS state
typedef struct {
    vfs_fs_t* filesystems[VFS_MAX_FILESYSTEMS];
    uint32_t filesystem_count;
    vfs_mount_t mounts[VFS_MAX_MOUNTS];
    uint32_t mount_count;
    vfs_fd_t descriptors[VFS_MAX_FILE_DESCRIPTORS];
    uint32_t descriptor_count;
    vfs_mount_t* root_mount;
    bool initialized;
} vfs_state_t;

static vfs_state_t vfs_state;

// =============================================================================
// VFS INITIALIZATION
// =============================================================================

void vfs_init(void) {
    if (vfs_state.initialized) {
        return;
    }
    
    console_puts("VFS: Initializing Virtual File System...\n");
    
    // Initialize state
    for (int i = 0; i < VFS_MAX_FILESYSTEMS; i++) {
        vfs_state.filesystems[i] = NULL;
    }
    
    for (int i = 0; i < VFS_MAX_MOUNTS; i++) {
        vfs_state.mounts[i].path = NULL;
        vfs_state.mounts[i].fs = NULL;
        vfs_state.mounts[i].root = NULL;
        vfs_state.mounts[i].mounted = false;
    }
    
    for (int i = 0; i < VFS_MAX_FILE_DESCRIPTORS; i++) {
        vfs_state.descriptors[i].file = NULL;
        vfs_state.descriptors[i].flags = 0;
        vfs_state.descriptors[i].refcount = 0;
        vfs_state.descriptors[i].used = false;
    }
    
    vfs_state.filesystem_count = 0;
    vfs_state.mount_count = 0;
    vfs_state.descriptor_count = 0;
    vfs_state.root_mount = NULL;
    
    vfs_state.initialized = true;
    console_puts("VFS: Virtual File System initialized\n");
}

// =============================================================================
// FILESYSTEM REGISTRATION
// =============================================================================

int vfs_register_filesystem(const char* name, vfs_ops_t* ops) {
    if (!vfs_state.initialized || name == NULL || ops == NULL) {
        return -1;
    }
    
    if (vfs_state.filesystem_count >= VFS_MAX_FILESYSTEMS) {
        console_puts("VFS: ERROR - Maximum filesystems reached\n");
        return -1;
    }
    
    // Allocate filesystem structure
    vfs_fs_t* fs = (vfs_fs_t*)kmalloc(sizeof(vfs_fs_t));
    if (fs == NULL) {
        console_puts("VFS: ERROR - Failed to allocate filesystem structure\n");
        return -1;
    }
    
    fs->name = name;
    fs->ops = ops;
    fs->initialized = true;
    fs->fs_data = NULL;
    fs->next = NULL;
    
    // Add to filesystem list
    vfs_state.filesystems[vfs_state.filesystem_count++] = fs;
    
    console_puts("VFS: Registered filesystem ");
    console_puts(name);
    console_puts("\n");
    
    return 0;
}

vfs_fs_t* vfs_find_filesystem(const char* name) {
    if (!vfs_state.initialized || name == NULL) {
        return NULL;
    }
    
    for (uint32_t i = 0; i < vfs_state.filesystem_count; i++) {
        if (strcmp(vfs_state.filesystems[i]->name, name) == 0) {
            return vfs_state.filesystems[i];
        }
    }
    
    return NULL;
}

// =============================================================================
// MOUNT OPERATIONS
// =============================================================================

int vfs_mount(const char* source, const char* target, const char* fs_type, uint32_t flags) {
    if (!vfs_state.initialized || source == NULL || target == NULL || fs_type == NULL) {
        return -1;
    }
    
    if (vfs_state.mount_count >= VFS_MAX_MOUNTS) {
        console_puts("VFS: ERROR - Maximum mounts reached\n");
        return -1;
    }
    
    // Find filesystem
    vfs_fs_t* fs = vfs_find_filesystem(fs_type);
    if (fs == NULL) {
        console_puts("VFS: ERROR - Filesystem not found\n");
        return -1;
    }
    
    // Find free mount slot
    int mount_index = -1;
    for (int i = 0; i < VFS_MAX_MOUNTS; i++) {
        if (!vfs_state.mounts[i].mounted) {
            mount_index = i;
            break;
        }
    }
    
    if (mount_index < 0) {
        console_puts("VFS: ERROR - No free mount slots\n");
        return -1;
    }
    
    // Mount filesystem
    vfs_mount_t* mount = &vfs_state.mounts[mount_index];
    mount->path = target;
    mount->fs = fs;
    mount->root = NULL;
    mount->mounted = true;
    
    // Call filesystem mount operation
    if (fs->ops->mount != NULL) {
        int result = fs->ops->mount(fs, source, target);
        if (result < 0) {
            mount->mounted = false;
            return result;
        }
    }
    
    vfs_state.mount_count++;
    
    // Set as root mount if mounting at /
    if (strcmp(target, "/") == 0) {
        vfs_state.root_mount = mount;
    }
    
    console_puts("VFS: Mounted ");
    console_puts(fs_type);
    console_puts(" at ");
    console_puts(target);
    console_puts("\n");
    
    return 0;
}

int vfs_umount(const char* target) {
    if (!vfs_state.initialized || target == NULL) {
        return -1;
    }
    
    for (int i = 0; i < VFS_MAX_MOUNTS; i++) {
        vfs_mount_t* mount = &vfs_state.mounts[i];
        if (mount->mounted && strcmp(mount->path, target) == 0) {
            // Call filesystem umount operation
            if (mount->fs->ops->umount != NULL) {
                int result = mount->fs->ops->umount(mount->fs);
                if (result < 0) {
                    return result;
                }
            }
            
            mount->mounted = false;
            vfs_state.mount_count--;
            
            console_puts("VFS: Unmounted ");
            console_puts(target);
            console_puts("\n");
            
            return 0;
        }
    }
    
    console_puts("VFS: ERROR - Mount point not found\n");
    return -1;
}

// =============================================================================
// PATH OPERATIONS
// =============================================================================

vfs_mount_t* vfs_find_mount(const char* path) {
    if (!vfs_state.initialized || path == NULL) {
        return NULL;
    }
    
    // Find the longest matching mount point
    vfs_mount_t* best_match = NULL;
    size_t best_len = 0;
    
    for (int i = 0; i < VFS_MAX_MOUNTS; i++) {
        vfs_mount_t* mount = &vfs_state.mounts[i];
        if (mount->mounted && mount->path != NULL) {
            size_t len = strlen(mount->path);
            if (strncmp(path, mount->path, len) == 0 && len > best_len) {
                best_match = mount;
                best_len = len;
            }
        }
    }
    
    // Default to root mount
    if (best_match == NULL) {
        best_match = vfs_state.root_mount;
    }
    
    return best_match;
}

vfs_inode_t* vfs_lookup(const char* path) {
    if (!vfs_state.initialized || path == NULL) {
        return NULL;
    }
    
    // Find mount point
    vfs_mount_t* mount = vfs_find_mount(path);
    if (mount == NULL || mount->fs == NULL) {
        console_etings("VFS: ERROR - No mount point found\n");
        return NULL;
    }
    
    // Call filesystem lookup
    if (mount->fs->ops->lookup == NULL) {
        console_puts("出自: ERROR - Lookup not supported\n");
        return NULL;
    }
    
    return mount->fs->ops->lookup(mount->fs, path);
}

// =============================================================================
// FILE OPERATIONS
// =============================================================================

int vfs_open(const char* path, uint32_t flags, vfs_file_t** file) {
    if (!vfs_state.initialized || path == NULL || file == NULL) {
        return -1;
    }
    
    // Lookup inode
    vfs_inode_t* inode = vfs_lookup(path);
    if (inode == NULL) {
        // Try to create file if O_CREAT is set
        if (flags & O_CREAT) {
            vfs_mount_t* mount = vfs_find_mount(path);
            if (mount != NULL && mount->fs != NULL && mount->fs->ops->create != NULL) {
                int result = mount->fs->ops->create(mount->fs, path, 0644);
                if (result < 0) {
                    return result;
                }
                
                inode = vfs_lookup(path);
                if (inode == NULL) {
                    return -1;
                }
            }
        } else {
            return -1;
        }
    }
    
    // Create file descriptor
    vfs_file_t* f = (vfs_file_t*)kmalloc(sizeof(vfs_file_t));
    if (f == NULL) {
        return -1;
    }
    
    f->inode = inode;
    f->pos = 0;
    f->flags = flags;
    
    *file = f;
    return 0;
}

int vfs_close(vfs_file_t* file) {
    if (file == NULL) {
        return -1;
    }
    
    kfree(file);
    return 0;
}

ssize_t vfs_read(vfs_file_t* file, void* buf, size_t count) {
    if (file == NULL || buf == NULL) {
        return -1;
    }
    
    // Find filesystem
    vfs_mount_t* mount = vfs_find_mount("/"); // Simplified
    if (mount == NULL || mount->fs == NULL || mount->fs->ops->read == NULL) {
        return -1;
    }
    
    return mount->fs->ops->read(file, buf, count);
}

ssize_t vfs_write(vfs_file_t* file, const void* buf, size_t count) {
    if (file == NULL || buf == NULL) {
        return -1;
    }
    
    // Find filesystem
    vfs_mount_t* mount = vfs_find_mount("/"); // Simplified
    if (mount == NULL || mount->fs == NULL || mount->fs->ops->write == NULL) {
        return -1;
    }
    
    return mount->fs->ops->write(file, buf, count);
}

// =============================================================================
// HELPERS
// =============================================================================

static int strcmp(const char* a, const char* b) {
    while (*a && *b && *a == *b) {
        a++;
        b++;
    }
    return *a - *b;
}

static int strncmp(const char* a, const char* b, size_t n) {
    while (n-- > 0 && *a && *b && *a == *b) {
        a++;
        b++;
    }
    return n == 0 ? 0 : (*a - *b);
}

static size_t strlen(const char* s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

bool vfs_is_initialized(void) {
    return vfs_state.initialized;
}

