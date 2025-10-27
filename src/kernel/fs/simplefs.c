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
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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
    
    // TODO: Read superblock from device
    // For now, initialize with defaults
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
    
    // TODO: Read from device
    // For now, just zero out the buffer
    uint8_t* buf = (uint8_t*)buffer;
    for (int i = 0; i < SIMPLEFS_BLOCK_SIZE; i++) {
        buf[i] = 0;
    }
    
    return true;
}

bool simplefs_write_block(uint32_t block_num, const void* buffer) {
    if (!simplefs_state.initialized || block_num >= SIMPLEFS_MAX_BLOCKS || buffer == NULL) {
        return false;
    }
    
    // TODO: Write to device
    return true;
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

