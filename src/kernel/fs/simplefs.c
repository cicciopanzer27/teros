/**
 * @file simplefs.c
 * @brief SimpleFS - Simple File System Implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "simplefs.h"
#include "vfs.h"
#include "console.h"
#include "kmalloc.h"
#include "block_device.h"
#include "ramdisk.h"
#include "timer.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

// =============================================================================
// SIMPLEFS IMPLEMENTATION
// =============================================================================

#define SIMPLEFS_MAGIC 0x53494D50  // "SIMP"
#define SIMPLEFS_VERSION 1
#define SIMPLEFS_BLOCK_SIZE 4096
#define SIMPLEFS_INODES_PER_BLOCK (SIMPLEFS_BLOCK_SIZE / sizeof(simplefs_inode_t))
#define SIMPLEFS_DIR_ENTRIES_PER_BLOCK (SIMPLEFS_BLOCK_SIZE / sizeof(simplefs_dirent_t))
#define SIMPLEFS_MAX_BLOCKS 1024
#define SIMPLEFS_MAX_INODES 256

// Superblock structure
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t block_size;
    uint32_t total_blocks;
    uint32_t total_inodes;
    uint32_t free_blocks;
    uint32_t free_inodes;
    uint32_t inode_bitmap_block;
    uint32_t data_bitmap_block;
    uint32_t inode_table_block;
    uint32_t root_inode;
} simplefs_superblock_t;

// Inode structure
typedef struct {
    uint32_t mode;           // File type and permissions
    uint32_t uid;            // User ID
    uint32_t gid;            // Group ID
    uint32_t size;           // File size in bytes
    uint32_t blocks;         // Number of blocks
    uint64_t atime;          // Access time
    uint64_t mtime;          // Modification time
    uint64_t ctime;          // Creation time
    uint32_t direct_blocks[12];  // Direct block pointers
    uint32_t indirect_block;     // Indirect block pointer
} simplefs_inode_t;

// Directory entry structure
typedef struct {
    uint32_t inode;
    uint8_t name[60];  // Max filename length
    uint8_t type;      // File type
} simplefs_dirent_t;

// SimpleFS state
typedef struct {
    simplefs_superblock_t superblock;
    uint8_t* block_bitmap;
    uint8_t* inode_bitmap;
    bool initialized;
    uint32_t device_fd;
    block_device_t* device;  /* Block device pointer */
} simplefs_state_t;

static simplefs_state_t simplefs_state;

// =============================================================================
// SIMPLEFS INITIALIZATION
// =============================================================================

void simplefs_init(uint32_t device_fd) {
    if (simplefs_state.initialized) {
        return;
    }
    
    console_puts("SIMPLEFS: Initializing SimpleFS...\n");
    
    simplefs_state.device_fd = device_fd;
    
    // Get ramdisk device
    simplefs_state.device = ramdisk_get_device();
    if (simplefs_state.device == NULL) {
        console_puts("SIMPLEFS: ERROR - No block device available\n");
        return;
    }
    
    // Try to read existing superblock from device
    simplefs_superblock_t sb;
    uint32_t sectors_read = block_device_read(simplefs_state.device, 0, &sb, sizeof(sb) / 512 + 1);
    
    if (sectors_read > 0 && sb.magic == SIMPLEFS_MAGIC) {
        // Valid filesystem found, use it
        console_puts("SIMPLEFS: Found existing filesystem\n");
        simplefs_state.superblock = sb;
    } else {
        // Initialize new filesystem
        console_puts("SIMPLEFS: Creating new filesystem\n");
        simplefs_state.superblock.magic = SIMPLEFS_MAGIC;
        simplefs_state.superblock.version = SIMPLEFS_VERSION;
        simplefs_state.superblock.block_size = SIMPLEFS_BLOCK_SIZE;
        simplefs_state.superblock.total_blocks = SIMPLEFS_MAX_BLOCKS;
        simplefs_state.superblock.total_inodes = SIMPLEFS_MAX_INODES;
        simplefs_state.superblock.free_blocks = SIMPLEFS_MAX_BLOCKS - 10; // Reserve for superblock/metadata
        simplefs_state.superblock.free_inodes = SIMPLEFS_MAX_INODES - 1;  // Reserve root inode
        simplefs_state.superblock.inode_bitmap_block = 1;
        simplefs_state.superblock.data_bitmap_block = 2;
        simplefs_state.superblock.inode_table_block = 3;
        simplefs_state.superblock.root_inode = 0;
        
        // Write superblock to device
        block_device_write(simplefs_state.device, 0, &simplefs_state.superblock, sizeof(simplefs_superblock_t) / 512 + 1);
    }
    
    // Allocate bitmaps
    simplefs_state.block_bitmap = (uint8_t*)kmalloc(SIMPLEFS_MAX_BLOCKS / 8);
    simplefs_state.inode_bitmap = (uint8_t*)kmalloc(SIMPLEFS_MAX_INODES / 8);
    
    if (simplefs_state.block_bitmap == NULL || simplefs_state.inode_bitmap == NULL) {
        console_puts("SIMPLEFS: ERROR - Failed to allocate bitmaps\n");
        return;
    }
    
    // Initialize bitmaps (all free initially)
    for (int i = 0; i < SIMPLEFS_MAX_BLOCKS / 8; i++) {
        simplefs_state.block_bitmap[i] = 0xFF;
    }
    
    for (int i = 0; i < SIMPLEFS_MAX_INODES / 8; i++) {
        simplefs_state.inode_bitmap[i] = 0xFF;
    }
    
    // Reserve metadata blocks
    for (int i = 0; i < 10; i++) {
        uint32_t byte = i / 8;
        uint32_t bit = i % 8;
        simplefs_state.block_bitmap[byte] &= ~(1 << bit);
    }
    
    // Reserve root inode
    uint32_t byte = 0 / 8;
    uint32_t bit = 0 % 8;
    simplefs_state.inode_bitmap[byte] &= ~(1 << bit);
    
    simplefs_state.initialized = true;
    
    console_puts("SIMPLEFS: SimpleFS initialized\n");
}

// =============================================================================
// BLOCK ALLOCATION
// =============================================================================

uint32_t simplefs_alloc_block(void) {
    if (!simplefs_state.initialized) {
        return 0;
    }
    
    for (uint32_t i = 0; i < SIMPLEFS_MAX_BLOCKS; i++) {
        uint32_t byte = i / 8;
        uint32_t bit = i % 8;
        
        if (simplefs_state.block_bitmap[byte] & (1 << bit)) {
            // Block is free, allocate it
            simplefs_state.block_bitmap[byte] &= ~(1 << bit);
            simplefs_state.superblock.free_blocks--;
            return i;
        }
    }
    
    // No free blocks
    return 0;
}

void simplefs_free_block(uint32_t block_num) {
    if (!simplefs_state.initialized || block_num >= SIMPLEFS_MAX_BLOCKS) {
        return;
    }
    
    uint32_t byte = block_num / 8;
    uint32_t bit = block_num % 8;
    
    if ((simplefs_state.block_bitmap[byte] & (1 << bit)) == 0) {
        // Block is allocated, free it
        simplefs_state.block_bitmap[byte] |= (1 << bit);
        simplefs_state.superblock.free_blocks++;
    }
}

// =============================================================================
// INODE ALLOCATION
// =============================================================================

uint32_t simplefs_alloc_inode(void) {
    if (!simplefs_state.initialized) {
        return 0;
    }
    
    for (uint32_t i = 0; i < SIMPLEFS_MAX_INODES; i++) {
        uint32_t byte = i / 8;
        uint32_t bit = i % 8;
        
        if (simplefs_state.inode_bitmap[byte] & (1 << bit)) {
            // Inode is free, allocate it
            simplefs_state.inode_bitmap[byte] &= ~(1 << bit);
            simplefs_state.superblock.free_inodes--;
            return i;
        }
    }
    
    // No free inodes
    return 0;
}

void simplefs_free_inode(uint32_t inode_num) {
    if (!simplefs_state.initialized || inode_num >= SIMPLEFS_MAX_INODES) {
        return;
    }
    
    uint32_t byte = inode_num / 8;
    uint32_t bit = inode_num % 8;
    
    if ((simplefs_state.inode_bitmap[byte] & (1 << bit)) == 0) {
        // Inode is allocated, free it
        simplefs_state.inode_bitmap[byte] |= (1 << bit);
        simplefs_state.superblock.free_inodes++;
    }
}

// =============================================================================
// BLOCK I/O
// =============================================================================

bool simplefs_read_block(uint32_t block_num, void* buffer) {
    if (!simplefs_state.initialized || block_num >= SIMPLEFS_MAX_BLOCKS || buffer == NULL) {
        return false;
    }
    
    if (simplefs_state.device == NULL) {
        return false;
    }
    
    // Calculate sector number (assuming 512-byte sectors)
    // SimpleFS blocks are 4096 bytes = 8 sectors
    uint32_t sector = block_num * (SIMPLEFS_BLOCK_SIZE / 512);
    uint32_t sector_count = SIMPLEFS_BLOCK_SIZE / 512;
    
    uint32_t sectors_read = block_device_read(simplefs_state.device, sector, buffer, sector_count);
    
    return sectors_read == sector_count;
}

bool simplefs_write_block(uint32_t block_num, const void* buffer) {
    if (!simplefs_state.initialized || block_num >= SIMPLEFS_MAX_BLOCKS || buffer == NULL) {
        return false;
    }
    
    if (simplefs_state.device == NULL) {
        return false;
    }
    
    // Calculate sector number (assuming 512-byte sectors)
    // SimpleFS blocks are 4096 bytes = 8 sectors
    uint32_t sector = block_num * (SIMPLEFS_BLOCK_SIZE / 512);
    uint32_t sector_count = SIMPLEFS_BLOCK_SIZE / 512;
    
    uint32_t sectors_written = block_device_write(simplefs_state.device, sector, buffer, sector_count);
    
    return sectors_written == sector_count;
}

// =============================================================================
// DIRECTORY OPERATIONS
// =============================================================================

bool simplefs_is_initialized(void) {
    return simplefs_state.initialized;
}

simplefs_superblock_t* simplefs_get_superblock(void) {
    return &simplefs_state.superblock;
}

// =============================================================================
// INODE OPERATIONS
// =============================================================================

simplefs_inode_t* simplefs_read_inode(uint32_t inode_num) {
    if (!simplefs_state.initialized || inode_num >= SIMPLEFS_MAX_INODES) {
        return NULL;
    }

    // Check if inode is allocated
    uint32_t byte = inode_num / 8;
    uint32_t bit = inode_num % 8;
    if ((simplefs_state.inode_bitmap[byte] & (1 << bit)) != 0) {
        return NULL; // Inode not allocated
    }

    // Read inode from inode table
    static simplefs_inode_t inode_buffer;
    uint32_t inode_table_start = simplefs_state.superblock.inode_table_block * SIMPLEFS_BLOCK_SIZE;
    uint32_t inode_offset = inode_num * sizeof(simplefs_inode_t);

    // For simplicity, use a static buffer (in real implementation, read from device)
    memset(&inode_buffer, 0, sizeof(simplefs_inode_t));
    inode_buffer.mode = SIMPLEFS_TYPE_FILE; // Default to file

    return &inode_buffer;
}

bool simplefs_write_inode(simplefs_inode_t* inode) {
    if (!simplefs_state.initialized || inode == NULL) {
        return false;
    }

    // In real implementation, write to device
    // For now, just return success
    return true;
}

// =============================================================================
// DIRECTORY OPERATIONS
// =============================================================================

uint32_t simplefs_create_file(const char* path, uint32_t mode) {
    if (!simplefs_state.initialized || path == NULL) {
        return 0;
    }

    // Allocate inode
    uint32_t inode_num = simplefs_alloc_inode();
    if (inode_num == 0) {
        return 0;
    }

    // Create inode structure
    simplefs_inode_t inode;
    memset(&inode, 0, sizeof(simplefs_inode_t));
    inode.mode = mode | SIMPLEFS_TYPE_FILE;
    inode.uid = 0; // Root user
    inode.gid = 0; // Root group
    inode.size = 0;
    inode.blocks = 0;
    inode.ctime = timer_get_ticks(); // Current time from timer
    inode.mtime = inode.ctime;
    inode.atime = inode.ctime;

    // Write inode
    if (!simplefs_write_inode(&inode)) {
        simplefs_free_inode(inode_num);
        return 0;
    }

    // Add to parent directory
    // Simplified: For MVP, all files go in root directory
    // Full implementation would parse path and traverse directory tree
    // Example: "/dir/file.txt" -> find "dir" inode, add "file.txt" entry

    return inode_num;
}

uint32_t simplefs_create_directory(const char* path, uint32_t mode) {
    if (!simplefs_state.initialized || path == NULL) {
        return 0;
    }

    // Allocate inode
    uint32_t inode_num = simplefs_alloc_inode();
    if (inode_num == 0) {
        return 0;
    }

    // Create inode structure
    simplefs_inode_t inode;
    memset(&inode, 0, sizeof(simplefs_inode_t));
    inode.mode = mode | SIMPLEFS_TYPE_DIR;
    inode.uid = 0; // Root user
    inode.gid = 0; // Root group
    inode.size = 0;
    inode.blocks = 0;
    inode.ctime = timer_get_ticks(); // Current time from timer
    inode.mtime = inode.ctime;
    inode.atime = inode.ctime;

    // Write inode
    if (!simplefs_write_inode(&inode)) {
        simplefs_free_inode(inode_num);
        return 0;
    }

    // Add . and .. directory entries
    // Simplified: For MVP, directory entries handled by VFS layer
    // Full implementation would:
    // 1. Allocate block for directory entries
    // 2. Create "." entry pointing to self (inode_num)
    // 3. Create ".." entry pointing to parent directory
    // 4. Write directory block to disk

    return inode_num;
}

uint32_t simplefs_find_file(const char* path) {
    if (!simplefs_state.initialized || path == NULL) {
        return 0;
    }

    // Simplified path resolution for MVP
    // Full implementation would:
    // 1. Split path into components: "/home/user/file.txt" -> ["home", "user", "file.txt"]
    // 2. Start at root inode
    // 3. For each component:
    //    a. Read directory entries
    //    b. Find matching entry
    //    c. Move to that inode
    // 4. Return final inode number
    
    // For now, support root path only
    if (strcmp(path, "/") == 0) {
        return simplefs_state.superblock.root_inode;
    }

    // Stub: would search through directory entries here
    return 0; // Not found
}

// =============================================================================
// FILE I/O OPERATIONS
// =============================================================================

size_t simplefs_read_file(uint32_t inode_num, uint32_t offset, void* buffer, size_t size) {
    if (!simplefs_state.initialized || buffer == NULL) {
        return 0;
    }

    simplefs_inode_t* inode = simplefs_read_inode(inode_num);
    if (inode == NULL || (inode->mode & SIMPLEFS_TYPE_FILE) == 0) {
        return 0;
    }

    // Check bounds
    if (offset >= inode->size) {
        return 0;
    }

    size_t bytes_to_read = (offset + size > inode->size) ? (inode->size - offset) : size;

    // Read direct blocks
    size_t bytes_read = 0;
    uint32_t current_offset = offset;
    uint32_t block_index = current_offset / SIMPLEFS_BLOCK_SIZE;
    uint32_t block_offset = current_offset % SIMPLEFS_BLOCK_SIZE;

    while (bytes_read < bytes_to_read && block_index < 12) {
        if (inode->direct_blocks[block_index] == 0) {
            break; // No more blocks
        }

        uint32_t block_num = inode->direct_blocks[block_index];
        uint8_t block_buffer[SIMPLEFS_BLOCK_SIZE];

        if (simplefs_read_block(block_num, block_buffer)) {
            size_t block_bytes = SIMPLEFS_BLOCK_SIZE - block_offset;
            if (block_bytes > bytes_to_read - bytes_read) {
                block_bytes = bytes_to_read - bytes_read;
            }

            memcpy((uint8_t*)buffer + bytes_read, block_buffer + block_offset, block_bytes);
            bytes_read += block_bytes;
            current_offset += block_bytes;
            block_index++;
            block_offset = 0;
        } else {
            break;
        }
    }

    // Update access time
    inode->atime = timer_get_ticks(); // Current time from timer
    simplefs_write_inode(inode);

    return bytes_read;
}

size_t simplefs_write_file(uint32_t inode_num, uint32_t offset, const void* buffer, size_t size) {
    if (!simplefs_state.initialized || buffer == NULL) {
        return 0;
    }

    simplefs_inode_t* inode = simplefs_read_inode(inode_num);
    if (inode == NULL || (inode->mode & SIMPLEFS_TYPE_FILE) == 0) {
        return 0;
    }

    // Implement file writing with block allocation
    size_t bytes_written = 0;
    uint32_t current_offset = offset;
    uint32_t block_index = current_offset / SIMPLEFS_BLOCK_SIZE;
    uint32_t block_offset = current_offset % SIMPLEFS_BLOCK_SIZE;

    while (bytes_written < size && block_index < 12) {
        // Allocate block if needed
        if (inode->direct_blocks[block_index] == 0) {
            uint32_t new_block = simplefs_alloc_block();
            if (new_block == 0) {
                break; // No more blocks available
            }
            inode->direct_blocks[block_index] = new_block;
            inode->blocks++;
        }

        uint32_t block_num = inode->direct_blocks[block_index];
        uint8_t block_buffer[SIMPLEFS_BLOCK_SIZE];

        // Read existing block content if we're doing partial write
        if (block_offset != 0 || (size - bytes_written) < SIMPLEFS_BLOCK_SIZE) {
            simplefs_read_block(block_num, block_buffer);
        }

        // Calculate bytes to write in this block
        size_t block_bytes = SIMPLEFS_BLOCK_SIZE - block_offset;
        if (block_bytes > size - bytes_written) {
            block_bytes = size - bytes_written;
        }

        // Copy data to block buffer
        memcpy(block_buffer + block_offset, (const uint8_t*)buffer + bytes_written, block_bytes);

        // Write block back
        if (!simplefs_write_block(block_num, block_buffer)) {
            break;
        }

        bytes_written += block_bytes;
        current_offset += block_bytes;
        block_index++;
        block_offset = 0;
    }

    // Update file size and modification time
    if (offset + bytes_written > inode->size) {
        inode->size = offset + bytes_written;
    }
    inode->mtime = timer_get_ticks(); // Current time from timer
    simplefs_write_inode(inode);

    return bytes_written;
}

// =============================================================================
// FILE DELETION
// =============================================================================

bool simplefs_delete_file(const char* path) {
    if (!simplefs_state.initialized || path == NULL) {
        return false;
    }

    uint32_t inode_num = simplefs_find_file(path);
    if (inode_num == 0) {
        return false;
    }

    simplefs_inode_t* inode = simplefs_read_inode(inode_num);
    if (inode == NULL || (inode->mode & SIMPLEFS_TYPE_FILE) == 0) {
        return false;
    }

    // Free all blocks
    for (int i = 0; i < 12; i++) {
        if (inode->direct_blocks[i] != 0) {
            simplefs_free_block(inode->direct_blocks[i]);
        }
    }
    if (inode->indirect_block != 0) {
        simplefs_free_block(inode->indirect_block);
    }

    // Free inode
    simplefs_free_inode(inode_num);

    // Remove from parent directory
    // TODO: Parse path and remove from parent directory

    return true;
}

bool simplefs_delete_directory(const char* path) {
    if (!simplefs_state.initialized || path == NULL) {
        return false;
    }

    uint32_t inode_num = simplefs_find_file(path);
    if (inode_num == 0) {
        return false;
    }

    simplefs_inode_t* inode = simplefs_read_inode(inode_num);
    if (inode == NULL || (inode->mode & SIMPLEFS_TYPE_DIR) == 0) {
        return false;
    }

    // Check if directory is empty
    // Full implementation would read directory entries and check if only . and .. exist
    // For MVP, assume directory can be deleted

    // Free all data blocks
    for (int i = 0; i < 12; i++) {
        if (inode->direct_blocks[i] != 0) {
            simplefs_free_block(inode->direct_blocks[i]);
        }
    }

    // Free inode
    simplefs_free_inode(inode_num);

    // Remove from parent directory
    // Simplified: For MVP, directory removal handled by VFS layer

    return true;
}

