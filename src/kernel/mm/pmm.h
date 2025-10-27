/**
 * @file pmm.h
 * @brief Physical Memory Manager for TEROS Dashboard
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Memory page size (4KB standard)
#define PMM_PAGE_SIZE    4096
#define PMM_BITS_PER_INT 32

// Memory regions
#define PMM_REGION_KERNEL_START 0x00100000  // 1MB
#define PMM_REGION_KERNEL_END   0x01000000  // 16MB
#define PMM_REGION_USER_START   0x01000000  // 16MB
#define PMM_REGION_USER_END     0xFFFFFFFF  // 4GB

// =============================================================================
// MEMORY ALLOCATION STRUCTURES
// =============================================================================

// Page frame structure
typedef struct pmm_page {
    struct pmm_page* next;
    uint32_t frame_number;
    bool allocated;
    uint32_t reference_count;
} pmm_page_t;

// Memory region descriptor
typedef struct {
    uint64_t base_address;
    uint64_t length;
    uint32_t type;           // 1=available, 2=reserved, etc.
    uint32_t acpi_attribute;
} mem_region_t;

// PMM state structure
typedef struct {
    uint32_t total_frames;
    uint32_t free_frames;
    uint32_t used_frames;
    uint32_t bitmap_size;
    uint32_t* bitmap;
    mem_region_t* regions;
    uint32_t region_count;
    bool initialized;
} pmm_state_t;

// =============================================================================
// PHYSICAL MEMORY MANAGER API
// =============================================================================

/**
 * @brief Initialize Physical Memory Manager
 * @param mem_upper Upper memory limit in KB
 */
void pmm_init(uint32_t mem_upper);

/**
 * @brief Allocate a page frame
 * @return Physical address of allocated page, or 0 on failure
 */
uint32_t pmm_alloc_page(void);

/**
 * @brief Free a page frame
 * @param phys_addr Physical address to free
 */
void pmm_free_page(uint32_t phys_addr);

/**
 * @brief Allocate multiple contiguous page frames
 * @param count Number of pages to allocate
 * @return Physical address of first allocated page, or 0 on failure
 */
uint32_t pmm_alloc_pages(uint32_t count);

/**
 * @brief Free multiple contiguous page frames
 * @param phys_addr Physical address of first page
 * @param count Number of pages to free
 */
void pmm_free_pages(uint32_t phys_addr, uint32_t count);

/**
 * @brief Mark page as used (don't allocate)
 * @param phys_addr Physical address to mark
 */
void pmm_mark_used(uint32_t phys_addr);

/**
 * @brief Mark page as free (available for allocation)
 * @param phys_addr Physical address to mark
 */
void pmm_mark_free(uint32_t phys_addr);

/**
 * @brief Check if page is allocated
 * @param phys_addr Physical address to check
 * @return true if allocated, false otherwise
 */
bool pmm_is_allocated(uint32_t phys_addr);

// =============================================================================
// MEMORY STATISTICS
// =============================================================================

/**
 * @brief Get total available frames
 * @return Total number of frames
 */
uint32_t pmm_get_total_frames(void);

/**
 * @brief Get number of free frames
 * @return Number of free frames
 */
uint32_t pmm_get_free_frames(void);

/**
 * @brief Get number of used frames
 * @return Number of used frames
 */
uint32_t pmm_get_used_frames(void);

/**
 * @brief Get memory usage percentage
 * @return Usage percentage (0-100)
 */
uint32_t pmm_get_usage_percent(void);

/**
 * @brief Print PMM statistics
 */
void pmm_print_stats(void);

/**
 * @brief Get PMM state
 * @return Pointer to PMM state structure
 */
pmm_state_t* pmm_get_state(void);

#endif // PMM_H

