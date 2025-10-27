/**
 * @file ternary_memory.h
 * @brief Ternary memory management header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_MEMORY_H
#define TERNARY_MEMORY_H

#include "trit.h"
#include "trit_array.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY MEMORY STRUCTURE
// =============================================================================

typedef struct {
    trit_t* data;
    size_t size;
    size_t used;
    bool read_only;
} ternary_memory_t;

// =============================================================================
// TERNARY MEMORY CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary memory instance
 * @param size Size of the memory in trits
 * @return A new ternary memory instance, or NULL on failure
 */
ternary_memory_t* ternary_memory_create(size_t size);

/**
 * @brief Destroy a ternary memory instance
 * @param memory The memory instance to destroy
 */
void ternary_memory_destroy(ternary_memory_t* memory);

// =============================================================================
// TERNARY MEMORY ACCESS OPERATIONS
// =============================================================================

/**
 * @brief Read a trit from memory
 * @param memory The memory instance
 * @param address Memory address
 * @return The trit at the address
 */
trit_t ternary_memory_read(ternary_memory_t* memory, size_t address);

/**
 * @brief Write a trit to memory
 * @param memory The memory instance
 * @param address Memory address
 * @param value The trit to write
 */
void ternary_memory_write(ternary_memory_t* memory, size_t address, trit_t value);

/**
 * @brief Check if an address is valid
 * @param memory The memory instance
 * @param address Memory address
 * @return true if valid, false otherwise
 */
bool ternary_memory_is_valid_address(ternary_memory_t* memory, size_t address);

// =============================================================================
// TERNARY MEMORY BULK OPERATIONS
// =============================================================================

/**
 * @brief Read a block of trits from memory
 * @param memory The memory instance
 * @param address Starting address
 * @param buffer Buffer to store the trits
 * @param count Number of trits to read
 * @return true on success, false on failure
 */
bool ternary_memory_read_block(ternary_memory_t* memory, size_t address, trit_t* buffer, size_t count);

/**
 * @brief Write a block of trits to memory
 * @param memory The memory instance
 * @param address Starting address
 * @param buffer Buffer containing the trits
 * @param count Number of trits to write
 * @return true on success, false on failure
 */
bool ternary_memory_write_block(ternary_memory_t* memory, size_t address, const trit_t* buffer, size_t count);

// =============================================================================
// TERNARY MEMORY ARRAY OPERATIONS
// =============================================================================

/**
 * @brief Read a trit array from memory
 * @param memory The memory instance
 * @param address Starting address
 * @param count Number of trits to read
 * @return A new trit array, or NULL on failure
 */
trit_array_t* ternary_memory_read_array(ternary_memory_t* memory, size_t address, size_t count);

/**
 * @brief Write a trit array to memory
 * @param memory The memory instance
 * @param address Starting address
 * @param array The trit array to write
 * @return true on success, false on failure
 */
bool ternary_memory_write_array(ternary_memory_t* memory, size_t address, trit_array_t* array);

// =============================================================================
// TERNARY MEMORY SEARCH OPERATIONS
// =============================================================================

/**
 * @brief Find a trit in memory
 * @param memory The memory instance
 * @param value The trit to find
 * @param start_address Starting address for search
 * @return Address of the trit, or SIZE_MAX if not found
 */
size_t ternary_memory_find(ternary_memory_t* memory, trit_t value, size_t start_address);

/**
 * @brief Find a trit array pattern in memory
 * @param memory The memory instance
 * @param pattern The pattern to find
 * @param start_address Starting address for search
 * @return Address of the pattern, or SIZE_MAX if not found
 */
size_t ternary_memory_find_array(ternary_memory_t* memory, trit_array_t* pattern, size_t start_address);

// =============================================================================
// TERNARY MEMORY COPY OPERATIONS
// =============================================================================

/**
 * @brief Copy data between memory instances
 * @param source Source memory instance
 * @param dest Destination memory instance
 * @param source_address Source address
 * @param dest_address Destination address
 * @param count Number of trits to copy
 * @return true on success, false on failure
 */
bool ternary_memory_copy(ternary_memory_t* source, ternary_memory_t* dest, size_t source_address, size_t dest_address, size_t count);

/**
 * @brief Move data within a memory instance
 * @param memory The memory instance
 * @param source_address Source address
 * @param dest_address Destination address
 * @param count Number of trits to move
 * @return true on success, false on failure
 */
bool ternary_memory_move(ternary_memory_t* memory, size_t source_address, size_t dest_address, size_t count);

// =============================================================================
// TERNARY MEMORY FILL OPERATIONS
// =============================================================================

/**
 * @brief Fill memory with a trit value
 * @param memory The memory instance
 * @param value The trit value to fill with
 * @param start_address Starting address
 * @param count Number of trits to fill
 */
void ternary_memory_fill(ternary_memory_t* memory, trit_t value, size_t start_address, size_t count);

/**
 * @brief Clear memory (set to neutral)
 * @param memory The memory instance
 * @param start_address Starting address
 * @param count Number of trits to clear
 */
void ternary_memory_clear(ternary_memory_t* memory, size_t start_address, size_t count);

// =============================================================================
// TERNARY MEMORY STATUS OPERATIONS
// =============================================================================

/**
 * @brief Get total memory size
 * @param memory The memory instance
 * @return Total size in trits
 */
size_t ternary_memory_size(ternary_memory_t* memory);

/**
 * @brief Get used memory size
 * @param memory The memory instance
 * @return Used size in trits
 */
size_t ternary_memory_used(ternary_memory_t* memory);

/**
 * @brief Get free memory size
 * @param memory The memory instance
 * @return Free size in trits
 */
size_t ternary_memory_free(ternary_memory_t* memory);

/**
 * @brief Check if memory is read-only
 * @param memory The memory instance
 * @return true if read-only, false otherwise
 */
bool ternary_memory_is_read_only(ternary_memory_t* memory);

/**
 * @brief Set memory read-only flag
 * @param memory The memory instance
 * @param read_only Read-only flag
 */
void ternary_memory_set_read_only(ternary_memory_t* memory, bool read_only);

// =============================================================================
// TERNARY MEMORY UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print memory contents
 * @param memory The memory instance
 * @param start_address Starting address
 * @param count Number of trits to print
 */
void ternary_memory_print(ternary_memory_t* memory, size_t start_address, size_t count);

/**
 * @brief Debug print memory information
 * @param memory The memory instance
 */
void ternary_memory_debug(ternary_memory_t* memory);

// =============================================================================
// TERNARY MEMORY ALLOCATION OPERATIONS
// =============================================================================

/**
 * @brief Allocate memory block
 * @param memory The memory instance
 * @param size Size of the block to allocate
 * @return Address of allocated block, or SIZE_MAX on failure
 */
size_t ternary_memory_allocate(ternary_memory_t* memory, size_t size);

/**
 * @brief Deallocate memory block
 * @param memory The memory instance
 * @param address Address of the block to deallocate
 * @param size Size of the block to deallocate
 */
void ternary_memory_deallocate(ternary_memory_t* memory, size_t address, size_t size);

#endif // TERNARY_MEMORY_H
