/**
 * @file vmm.h
 * @brief Virtual Memory Manager (VMM) Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// VMM CONSTANTS
// =============================================================================

#define VMM_PAGE_SIZE 4096
#define VMM_PAGE_SHIFT 12
#define VMM_PAGE_MASK 0xFFF

// Page table entry flags
#define VMM_PTE_PRESENT 0x001
#define VMM_PTE_WRITABLE 0x002
#define VMM_PTE_USER 0x004
#define VMM_PTE_PWT 0x008
#define VMM_PTE_PCD 0x010
#define VMM_PTE_ACCESSED 0x020
#define VMM_PTE_DIRTY 0x040
#define VMM_PTE_SIZE 0x080
#define VMM_PTE_GLOBAL 0x100
#define VMM_PTE_AVAIL 0xE00

// =============================================================================
// VMM INITIALIZATION
// =============================================================================

/**
 * @brief Initialize Virtual Memory Manager
 */
void vmm_init(void);

/**
 * @brief Initialize kernel mapping
 */
void vmm_init_kernel_mapping(void);

/**
 * @brief Initialize TLB
 */
void vmm_init_tlb(void);

// =============================================================================
// PAGE MAPPING FUNCTIONS
// =============================================================================

/**
 * @brief Map a virtual page to a physical page
 * @param virtual_address Virtual address
 * @param physical_address Physical address
 * @param flags Page flags
 * @return true if successful
 */
bool vmm_map_page(uint32_t virtual_address, uint32_t physical_address, uint32_t flags);

/**
 * @brief Unmap a virtual page
 * @param virtual_address Virtual address
 * @return true if successful
 */
bool vmm_unmap_page(uint32_t virtual_address);

/**
 * @brief Check if a page is mapped
 * @param virtual_address Virtual address
 * @return true if mapped
 */
bool vmm_is_page_mapped(uint32_t virtual_address);

/**
 * @brief Get physical address for virtual address
 * @param virtual_address Virtual address
 * @return Physical address, or 0 if not mapped
 */
uint32_t vmm_get_physical_address(uint32_t virtual_address);

// =============================================================================
// TLB OPERATIONS
// =============================================================================

/**
 * @brief Invalidate TLB entry
 * @param virtual_address Virtual address
 */
void vmm_invalidate_tlb_entry(uint32_t virtual_address);

/**
 * @brief Invalidate entire TLB
 */
void vmm_invalidate_tlb(void);

/**
 * @brief TLB lookup
 * @param virtual_address Virtual address
 * @param physical_address Output physical address
 * @return true if found in TLB
 */
bool vmm_tlb_lookup(uint32_t virtual_address, uint32_t* physical_address);

/**
 * @brief Insert TLB entry
 * @param virtual_address Virtual address
 * @param physical_address Physical address
 */
void vmm_tlb_insert(uint32_t virtual_address, uint32_t physical_address);

// =============================================================================
// PAGE FAULT HANDLING
// =============================================================================

/**
 * @brief Handle page fault
 * @param virtual_address Virtual address that caused fault
 * @param error_code Page fault error code
 * @return true if fault was resolved
 */
bool vmm_handle_page_fault(uint32_t virtual_address, uint32_t error_code);

// =============================================================================
// VMM QUERY FUNCTIONS
// =============================================================================

/**
 * @brief Get total number of pages
 * @return Total pages
 */
uint32_t vmm_get_total_pages(void);

/**
 * @brief Get number of mapped pages
 * @return Mapped pages
 */
uint32_t vmm_get_mapped_pages(void);

/**
 * @brief Get number of unmapped pages
 * @return Unmapped pages
 */
uint32_t vmm_get_unmapped_pages(void);

/**
 * @brief Get number of kernel pages
 * @return Kernel pages
 */
uint32_t vmm_get_kernel_pages(void);

/**
 * @brief Get number of user pages
 * @return User pages
 */
uint32_t vmm_get_user_pages(void);

/**
 * @brief Get number of TLB entries
 * @return TLB entries
 */
uint32_t vmm_get_tlb_entries(void);

/**
 * @brief Get number of TLB hits
 * @return TLB hits
 */
uint32_t vmm_get_tlb_hits(void);

/**
 * @brief Get number of TLB misses
 * @return TLB misses
 */
uint32_t vmm_get_tlb_misses(void);

/**
 * @brief Get number of page faults
 * @return Page faults
 */
uint32_t vmm_get_page_faults(void);

/**
 * @brief Get number of resolved page faults
 * @return Resolved page faults
 */
uint32_t vmm_get_page_faults_resolved(void);

/**
 * @brief Check if VMM is initialized
 * @return true if initialized
 */
bool vmm_is_initialized(void);

// =============================================================================
// VMM DEBUG FUNCTIONS
// =============================================================================

/**
 * @brief Print VMM statistics
 */
void vmm_print_statistics(void);

/**
 * @brief Print page table for virtual address
 * @param virtual_address Virtual address
 */
void vmm_print_page_table(uint32_t virtual_address);

#endif // VMM_H
