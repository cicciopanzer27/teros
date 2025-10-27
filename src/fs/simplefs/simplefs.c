/**
 * @file simplefs.c
 * @brief SimpleFS implementation
 */

#include "simplefs.h"
#include <stddef.h>

static simplefs_superblock_t sb;
static bool mounted = false;

int simplefs_format(uint32_t device_start, uint32_t device_size) {
    // Initialize superblock
    sb.magic = SIMPLEFS_MAGIC;
    sb.version = 1;
    sb.block_size = SIMPLEFS_BLOCK_SIZE;
    sb.block_count = device_size / SIMPLEFS_BLOCK_SIZE;
    sb.inode_count = sb.block_count / 16;  // 1 inode per 16 blocks
    sb.free_blocks = sb.block_count;
    sb.free_inodes = sb.inode_count;
    
    // TODO: Initialize bitmap and inode table
    mounted = true;
    return 0;
}

int simplefs_mount(uint32_t device_start) {
    // TODO: Read superblock and verify
    // For now, just set flag
    mounted = true;
    return 0;
}

int simplefs_umount(void) {
    mounted = false;
    return 0;
}

int simplefs_get_superblock(simplefs_superblock_t* out_sb) {
    if (out_sb == NULL) return -1;
    if (!mounted) return -1;
    
    *out_sb = sb;
    return 0;
}

