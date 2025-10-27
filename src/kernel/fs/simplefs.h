/**
 * @file simplefs.h
 * @brief SimpleFS Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef SIMPLEFS_H
#define SIMPLEFS_H

#include <stdint.h>
#include <stdbool.h>

// Forward declarations
typedef struct simplefs_superblock simplefs_superblock_t;

// =============================================================================
// SIMPLEFS FUNCTIONS
// =============================================================================

/**
 * @brief Initialize SimpleFS
 * @param device_fd Device file descriptor
 */
void simplefs_init(uint32_t device_fd);

/**
 * @brief Allocate a block
 * @return Block number or 0 on failure
 */
uint32_t simplefs_alloc_block(void);

/**
 * @brief Free a block
 * @param block_num Block number
 */
void simplefs_free_block(uint32_t block_num);

/**
 * @brief Allocate an inode
 * @return Inode number or 0 on failure
 */
uint32_t simplefs_alloc_inode(void);

/**
 * @brief Free an inode
 * @param inode_num Inode number
 */
void simplefs_free_inode(uint32_t inode_num);

/**
 * @brief Read a block from device
 * @param block_num Block number
 * @param buffer Buffer to read into
 * @return true on success
 */
bool simplefs_read_block(uint32_t block_num, void* buffer);

/**
 * @brief Write a block to device
 * @param block_num Block number
 * @param buffer Buffer to write from
 * @return true on success
 */
bool simplefs_write_block(uint32_t block_num, const void* buffer);

/**
 * @brief Check if SimpleFS is initialized
 * @return true if initialized
 */
bool simplefs_is_initialized(void);

/**
 * @brief Get superblock
 * @return Superblock pointer
 */
simplefs_superblock_t* simplefs_get_superblock(void);

#endif // SIMPLEFS_H

