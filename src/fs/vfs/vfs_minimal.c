/**
 * @file vfs_minimal.c
 * @brief Minimal VFS implementation for 2-week sprint
 * Ultra-simplified file system - just enough to work
 */

#include "vfs.h"
#include "../kernel/mm/kmalloc.h"
#include <stddef.h>

#define MAX_FILES 64
#define MAX_NAME 32

// Ultra-simple file structure
typedef struct {
    char name[MAX_NAME];
    void* data;
    size_t size;
    bool in_use;
} simple_file_t;

static simple_file_t filesystem[MAX_FILES];
static bool vfs_initialized = false;

// VFS operations
void vfs_init(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        filesystem[i].in_use = false;
        filesystem[i].name[0] = '\0';
        filesystem[i].data = NULL;
        filesystem[i].size = 0;
    }
    vfs_initialized = true;
}

int vfs_open(const char* path, int flags) {
    if (!vfs_initialized) {
        vfs_init();
    }
    
    // Find free slot
    for (int i = 0; i < MAX_FILES; i++) {
        if (!filesystem[i].in_use) {
            // Create new file
            int j = 0;
            for (; path[j] && j < MAX_NAME - 1; j++) {
                filesystem[i].name[j] = path[j];
            }
            filesystem[i].name[j] = '\0';
            
            filesystem[i].data = kmalloc(4096); // 4KB buffer
            filesystem[i].size = 0;
            filesystem[i].in_use = true;
            return i; // Return FD
        }
    }
    return -1; // No free slots
}

int vfs_read(int fd, void* buffer, size_t count) {
    if (fd < 0 || fd >= MAX_FILES || !filesystem[fd].in_use) {
        return -1;
    }
    
    size_t to_read = count < filesystem[fd].size ? count : filesystem[fd].size;
    
    // Simple memcpy
    char* src = (char*)filesystem[fd].data;
    char* dst = (char*)buffer;
    for (size_t i = 0; i < to_read; i++) {
        dst[i] = src[i];
    }
    return to_read;
}

int vfs_write(int fd, const void* buffer, size_t count) {
    if (fd < 0 || fd >= MAX_FILES || !filesystem[fd].in_use) {
        return -1;
    }
    
    char* dst = (char*)filesystem[fd].data;
    const char* src = (const char*)buffer;
    
    for (size_t i = 0; i < count; i++) {
        dst[i] = src[i];
    }
    
    filesystem[fd].size = count;
    return count;
}

int vfs_close(int fd) {
    if (fd < 0 || fd >= MAX_FILES || !filesystem[fd].in_use) {
        return -1;
    }
    
    kfree(filesystem[fd].data);
    filesystem[fd].in_use = false;
    filesystem[fd].size = 0;
    filesystem[fd].name[0] = '\0';
    
    return 0;
}

