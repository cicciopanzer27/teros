/**
 * @file kmalloc.h
 * @brief Kernel heap allocator for TEROS
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef KMALLOC_H
#define KMALLOC_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Memory block structure
typedef struct mem_block {
    struct mem_block* next;
    struct mem_block* prev;
    size_t size;
    bool free;
} mem_block_t;

// Heap statistics
typedef struct {
    size_t total_size;
    size_t used_size;
    size_t free_size;
    uint32_t allocated_blocks;
    uint32_t free_blocks;
    uint32_t allocation_count;
    uint32_t deallocation_count;
} heap_stats_t;

/**
 * @brief Initialize heap allocator
 * @param heap_start Start address of heap
 * @param heap_size Size of heap in bytes
 */
void kmalloc_init(uint32_t heap_start, size_t heap_size);

/**
 * @brief Allocate memory
 * @param size Size in bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 */
void* kmalloc(size_t size);

/**
 * @brief Allocate aligned memory
 * @param size Size in bytes to allocate
 * @param align Alignment requirement
 * @return Pointer to allocated memory, or NULL on failure
 */
void* kmalloc_aligned(size_t size, size_t align);

/**
 * @brief Free memory
 * @param ptr Pointer to memory to free
 */
void kfree(void* ptr);

/**
 * @brief Reallocate memory
 * @param ptr Pointer to old memory
 * @param size New size
 * @return Pointer to reallocated memory, or NULL on failure
 */
void* krealloc(void* ptr, size_t size);

/**
 * @brief Allocate and zero memory
 * @param nmemb Number of elements
 * @param size Size of each element
 * @return Pointer to allocated memory, or NULL on failure
 */
void* kcalloc(size_t nmemb, size_t size);

/**
 * @brief Check for memory leaks
 * @return true if leaks detected, false otherwise
 */
bool kmalloc_check_leaks(void);

/**
 * @brief Get heap statistics
 * @return Pointer to heap statistics structure
 */
heap_stats_t* kmalloc_get_stats(void);

/**
 * @brief Print heap statistics
 */
void kmalloc_print_stats(void);

/**
 * @brief Compact heap (merge adjacent free blocks)
 */
void kmalloc_compact(void);

#endif // KMALLOC_H

