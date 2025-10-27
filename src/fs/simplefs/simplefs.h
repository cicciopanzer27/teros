/**
 * @file simplefs.h
 * @brief SimpleFS filesystem implementation
 */

#ifndef SIMPLEFS_H
#define SIMPLEFS_H

#include <stdint.h>
#include <stdbool.h>

// Block size
#define SIMPLEFS_BLOCK_SIZE 4096
#define SIMPLEFS_MAGIC 0x53494D50  // "SIMP"

// Superblock structure
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t block_size;
    uint32_t inode_count;
    uint32_t block_count;
    uint32_t free_inodes;
    uint32_t free_blocks;
    uint32_t first_data_block;
    uint32_t bitmap_block;
    uint32_t inode_table_block;
} simplefs_superblock_t;

// Inode structure
typedef struct {
    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
    uint32_t size;
    uint32_t blocks;
    uint32_t links;
    uint32_t atime;
    uint32_t mtime;
    uint32_t ctime;
    uint32_t data[12];  // Direct blocks
} simplefs_inode_t;

// Functions
int simplefs_format(uint32_t device_start, uint32_t device_size);
int simplefs_mount(uint32_t device_start);
int simplefs_umount(void);
int simplefs_get_superblock(simplefs_superblock_t* sb);

#endif // SIMPLEFS_H

