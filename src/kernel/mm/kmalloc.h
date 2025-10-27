/**
 * @file kmalloc.h
 * @brief Kernel Heap Allocator Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef KMALLOC_H
#define KMALLOC_H

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// KMALLOC CONSTANTS
// =============================================================================

#define KMALLOC_MIN_SIZE 8
#define KMALLOC_MAX_SIZE 4096
#define KMALLOC_SLAB_SIZE 4096

// =============================================================================
// KMALLOC INITIALIZATION
// =============================================================================

/**
 * @brief Initialize kernel heap allocator
 */
void kmalloc_init(void);

// =============================================================================
// KMALLOC ALLOCATION FUNCTIONS
// =============================================================================

/**
 * @brief Allocate memory from kernel heap
 * @param size Size in bytes
 * @return Pointer to allocated memory, or NULL on failure
 */
void* kmalloc(uint32_t size);

/**
 * @brief Free memory allocated with kmalloc
 * @param ptr Pointer to memory to free
 */
void kfree(void* ptr);

/**
 * @brief Reallocate memory
 * @param ptr Pointer to existing memory
 * @param new_size New size in bytes
 * @return Pointer to reallocated memory, or NULL on failure
 */
void* krealloc(void* ptr, uint32_t new_size);

/**
 * @brief Allocate and zero-initialize memory
 * @param num Number of elements
 * @param size Size of each element
 * @return Pointer to allocated memory, or NULL on failure
 */
void* kcalloc(uint32_t num, uint32_t size);

// =============================================================================
// KMALLOC QUERY FUNCTIONS
// =============================================================================

/**
 * @brief Get total number of allocations
 * @return Total allocations
 */
uint32_t kmalloc_get_total_allocations(void);

/**
 * @brief Get total number of deallocations
 * @return Total deallocations
 */
uint32_t kmalloc_get_total_deallocations(void);

/**
 * @brief Get total bytes allocated
 * @return Total bytes allocated
 */
uint32_t kmalloc_get_total_bytes_allocated(void);

/**
 * @brief Get total bytes freed
 * @return Total bytes freed
 */
uint32_t kmalloc_get_total_bytes_freed(void);

/**
 * @brief Get heap start address
 * @return Heap start address
 */
uint32_t kmalloc_get_heap_start(void);

/**
 * @brief Get heap end address
 * @return Heap end address
 */
uint32_t kmalloc_get_heap_end(void);

/**
 * @brief Get heap size
 * @return Heap size in bytes
 */
uint32_t kmalloc_get_heap_size(void);

/**
 * @brief Check if kmalloc is initialized
 * @return true if initialized
 */
bool kmalloc_is_initialized(void);

// =============================================================================
// KMALLOC DEBUG FUNCTIONS
// =============================================================================

/**
 * @brief Print kmalloc statistics
 */
void kmalloc_print_statistics(void);

/**
 * @brief Print cache information for specific size
 * @param size Size to get cache info for
 */
void kmalloc_print_cache_info(uint32_t size);

#endif // KMALLOC_H
