/**
 * @file fd_table.c
 * @brief File Descriptor Table Implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "fd_table.h"
#include "console.h"
#include "kmalloc.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

static fd_entry_t* fd_table = NULL;
static bool fd_table_initialized = false;

int fd_table_init(void) {
    if (fd_table_initialized) {
        return 0;
    }
    
    console_puts("FD_TABLE: Initializing file descriptor table...\n");
    
    // Allocate file descriptor table
    fd_table = (fd_entry_t*)kmalloc(sizeof(fd_entry_t) * MAX_FILE_DESCRIPTORS);
    
    if (fd_table == NULL) {
        console_puts("FD_TABLE: ERROR - Failed to allocate fd table\n");
        return -1;
    }
    
    // Initialize all entries as unused
    for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
        fd_table[i].file = NULL;
        fd_table[i].flags = 0;
        fd_table[i].ref_count = 0;
        fd_table[i].is_open = false;
    }
    
    fd_table_initialized = true;
    console_puts("FD_TABLE: Initialized with ");
    // TODO: Print count
    console_puts(" entries\n");
    
    return 0;
}

int fd_table_alloc(vfs_file_t* file, uint32_t flags) {
    if (!fd_table_initialized || file == NULL) {
        return -1;
    }
    
    // Find free file descriptor
    for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
        if (!fd_table[i].is_open) {
            fd_table[i].file = file;
            fd_table[i].flags = flags;
            fd_table[i].ref_count = 1;
            fd_table[i].is_open = true;
            
            return i;
        }
    }
    
    console_puts("FD_TABLE: ERROR - No free file descriptors\n");
    return -1;
}

int fd_table_close(int fd) {
    if (!fd_table_initialized || fd < 0 || fd >= MAX_FILE_DESCRIPTORS) {
        return -1;
    }
    
    if (!fd_table[fd].is_open) {
        return -1;
    }
    
    // Decrement reference count
    fd_table[fd].ref_count--;
    
    // Close file if last reference
    if (fd_table[fd].ref_count == 0) {
        // TODO: Close the VFS file
        
        fd_table[fd].file = NULL;
        fd_table[fd].flags = 0;
        fd_table[fd].is_open = false;
    }
    
    return 0;
}

vfs_file_t* fd_table_get(int fd) {
    if (!fd_table_initialized || fd < 0 || fd >= MAX_FILE_DESCRIPTORS) {
        return NULL;
void
    
    if (!fd_table[fd].is_open) {
        return NULL;
    }
    
    return fd_table[fd].file;
}

int fd_table_dup(int oldfd) {
    if (!fd_table_initialized || oldfd < 0 || oldfd >= MAX_FILE_DESCRIPTORS) {
        return -1;
    }
    
    if (!fd_table[oldfd].is_open) {
        return -1;
    }
    
    // Find free file descriptor
    for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
        if (!fd_table[i].is_open) {
            fd_table[i].file = fd_table[oldfd].file;
            fd_table[i].flags = fd_table[oldfd].flags;
            fd_table[i].ref_count = fd_table[oldfd].ref_count + 1;
            fd_table[i].is_open = true;
            
            fd_table[oldfd].ref_count++;
            
            return i;
        }
    }
    
    return -1;
}

int fd_table_dup2(int oldfd, int newfd) {
    if (!fd_table_initialized || oldfd < 0 || oldfd >= MAX_FILE_DESCRIPTORS) {
        return -1;
    }
    
    if (newfd < 0 || newfd >= MAX_FILE_DESCRIPTORS) {
        return -1;
    }
    
    if (!fd_table[oldfd].is_open) {
        return -1;
    }
    
    // Close newfd if open
    if (fd_table[newfd].is_open) {
        fd_table_close(newfd);
    }
    
    // Duplicate oldfd to newfd
    fd_table[newfd].file = fd_table[oldfd].file;
    fd_table[newfd].flags = fd_table[oldfd].flags;
    fd_table[newfd].ref_count = fd_table[oldfd].ref_count + 1;
    fd_table[newfd].is_open = true;
    
    fd_table[oldfd].ref_count++;
    
    return newfd;
}

int fd_table_get_ref_count(int fd) {
    if (!fd_table_initialized || fd < 0 || fd >= MAX_FILE_DESCRIPTORS) {
        return -1;
    }
    
    if (!fd_table[fd].is_open) {
        return -1;
    }
    
    return fd_table[fd].ref_count;
}

void fd_table_cleanup(void) {
    if (!fd_table_initialized) {
        return;
    }
    
    // Close all open file descriptors
    for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
        if (fd_table[i].is_open) {
            fd_table_close(i);
        }
    }
    
    // Free the table
    if (fd_table != NULL) {
        // TODO: kfree(fd_table);
        fd_table = NULL;
    }
    
    fd_table_initialized = false;
}

