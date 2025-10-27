/**
 * @file pmm.h
 * @brief Physical Memory Manager (PMM) Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// PMM CONSTANTS
// =============================================================================

#define PMM_PAGE_SIZE 4096      // 4KB pages
#define PMM_MAX_ORDER 20        // Maximum order (1MB blocks)
#define PMM_MIN_ORDER 0         // Minimum order (4KB blocks)

// =============================================================================
// PMM INITIALIZATION
// =============================================================================

/**
 * @brief Initialize Physical Memory Manager
 */
void pmm_init(void);

// =============================================================================
// PMM ALLOCATION FUNCTIONS
// =============================================================================

/**
 * @brief Allocate a single page
 * @return Physical address of allocated page, or NULL on failure
 */
void* pmm_alloc_page(void);

/**
 * @brief Allocate multiple pages
 * @param num_pages Number of pages to allocate
 * @return Physical address of allocated pages, or NULL on failure
 */
void* pmm_alloc_pages(uint32_t num_pages);

/**
 * @brief Free a single page
 * @param address Physical address of page to free
 */
void pmm_free_page(void* address);

/**
 * @brief Free multiple pages
 * @param address Physical address of pages to free
 * @param num_pages Number of pages to free
 */
void pmm_free_pages(void* address, uint32_t num_pages);

// =============================================================================
// PMM QUERY FUNCTIONS
// =============================================================================

/**
 * @brief Get total number of pages
 * @return Total pages
 */
uint32_t pmm_get_total_pages(void);

/**
 * @brief Get number of free pages
 * @return Free pages
 */
uint32_t pmm_get_free_pages(void);

/**
 * @brief Get number of allocated pages
 * @return Allocated pages
 */
uint32_t pmm_get_allocated_pages(void);

/**
 * @brief Get number of reserved pages
 * @return Reserved pages
 */
uint32_t pmm_get_reserved_pages(void);

/**
 * @brief Get memory start address
 * @return Memory start address
 */
uint32_t pmm_get_memory_start(void);

/**
 * @brief Get memory end address
 * @return Memory end address
 */
uint32_t pmm_get_memory_end(void);

/**
 * @brief Check if a page is allocated
 * @param physical_address Physical address to check
 * @return true if page is allocated
 */
bool pmm_is_page_allocated(uint32_t physical_address);

/**
 * @brief Check if PMM is initialized
 * @return true if initialized
 */
bool pmm_is_initialized(void);

// =============================================================================
// PMM DEBUG FUNCTIONS
// =============================================================================

/**
 * @brief Print PMM statistics
 */
void pmm_print_statistics(void);

/**
 * @brief Print memory map
 */
void pmm_print_memory_map(void);

#endif // PMM_H
