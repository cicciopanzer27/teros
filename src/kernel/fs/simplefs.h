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
#include <stddef.h>

// File types
#define SIMPLEFS_TYPE_FILE 1
#define SIMPLEFS_TYPE_DIR 2
#define SIMPLEFS_TYPE_SYMLINK 3

// File permissions
#define SIMPLEFS_PERM_READ 0x4
#define SIMPLEFS_PERM_WRITE 0x2
#define SIMPLEFS_PERM_EXEC 0x1

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

/**
 * @brief Create a new file
 * @param path File path
 * @param mode File mode
 * @return Inode number or 0 on failure
 */
uint32_t simplefs_create_file(const char* path, uint32_t mode);

/**
 * @brief Create a new directory
 * @param path Directory path
 * @param mode Directory mode
 * @return Inode number or 0 on failure
 */
uint32_t simplefs_create_directory(const char* path, uint32_t mode);

/**
 * @brief Read file inode
 * @param inode_num Inode number
 * @return Inode pointer or NULL on failure
 */
simplefs_inode_t* simplefs_read_inode(uint32_t inode_num);

/**
 * @brief Write file inode
 * @param inode Inode structure
 * @return true on success
 */
bool simplefs_write_inode(simplefs_inode_t* inode);

/**
 * @brief Read directory entry
 * @param dir_inode Directory inode
 * @param index Entry index
 * @return Directory entry or NULL
 */
simplefs_dirent_t* simplefs_read_dirent(uint32_t dir_inode, uint32_t index);

/**
 * @brief Add directory entry
 * @param dir_inode Directory inode
 * @param name Entry name
 * @param inode_num Target inode
 * @param type Entry type
 * @return true on success
 */
bool simplefs_add_dirent(uint32_t dir_inode, const char* name, uint32_t inode_num, uint8_t type);

/**
 * @brief Remove directory entry
 * @param dir_inode Directory inode
 * @param name Entry name
 * @return true on success
 */
bool simplefs_remove_dirent(uint32_t dir_inode, const char* name);

/**
 * @brief Find file by path
 * @param path File path
 * @return Inode number or 0 on failure
 */
uint32_t simplefs_find_file(const char* path);

/**
 * @brief Read file data
 * @param inode_num File inode
 * @param offset Byte offset
 * @param buffer Output buffer
 * @param size Number of bytes to read
 * @return Number of bytes read
 */
size_t simplefs_read_file(uint32_t inode_num, uint32_t offset, void* buffer, size_t size);

/**
 * @brief Write file data
 * @param inode_num File inode
 * @param offset Byte offset
 * @param buffer Input buffer
 * @param size Number of bytes to write
 * @return Number of bytes written
 */
size_t simplefs_write_file(uint32_t inode_num, uint32_t offset, const void* buffer, size_t size);

/**
 * @brief Delete file
 * @param path File path
 * @return true on success
 */
bool simplefs_delete_file(const char* path);

/**
 * @brief Delete directory
 * @param path Directory path
 * @return true on success
 */
bool simplefs_delete_directory(const char* path);

#endif // SIMPLEFS_H

