/**
 * @file vmm.h
 * @brief Virtual Memory Manager for TEROS
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Page size (4KB)
#define VMM_PAGE_SIZE 4096

// Virtual address space layout
#define VMM_KERNEL_BASE  0xC0000000  // 3GB
#define VMM_KERNEL_STACK 0xC0400000  // Kernel stack
#define VMM_USER_BASE    0x40000000  // 1GB (userspace)
#define VMM_USER_STACK   0xBFFFE000  // User stack

// Page directory and table entries
#define VMM_PRESENT    0x1
#define VMM_WRITE      0x2
#define VMM_USER       0x4
#define VMM_PWT        0x8    // Page Write Through
#define VMM_PCD        0x10   // Page Cache Disabled
#define VMM_ACCESSED   0x20
#define VMM_DIRTY      0x40
#define VMM_PS         0x80   // Page Size
#define VMM_G          0x100  // Global

// Page fault error codes
#define VMM_ERR_PRESENT   0x1   // Page was present
#define VMM_ERR_WRITE     0x2   // Write operation
#define VMM_ERR_USER      0x4   // User mode access
#define VMM_ERR_RESERVED  0x8   // Reserved bit violation
#define VMM_ERR_INSTR     0x10  // Instruction fetch

// =============================================================================
// VMM STRUCTURES
// =============================================================================

// Page table entry
typedef struct {
    uint32_t present : 1;
    uint32_t write : 1;
    uint32_t user : 1;
    uint32_t pwt : 1;
    uint32_t pcd : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t page_size : 1;
    uint32_t global : 1;
    uint32_t available : 3;
    uint32_t frame : 20;
} page_table_entry_t;

// Page directory entry (same as page table entry for simplicity)
typedef page_table_entry_t page_directory_entry_t;

// Page table structure
typedef struct {
    page_table_entry_t entries[1024];  // 1024 entries per table
} page_table_t;

// Page directory structure
typedef struct {
    page_directory_entry_t entries[1024];  // 1024 entries per directory
} page_directory_t;

// Virtual memory region
typedef struct vmm_region {
    uint32_t virtual_addr;
    uint32_t physical_addr;
    uint32_t size;
    uint32_t flags;
    bool is_mapped;
    struct vmm_region* next;
} vmm_region_t;

// VMM state
typedef struct {
    page_directory_t* current_directory;
    bool initialized;
    uint32_t allocated_regions;
    uint32_t kernel_pages;
    uint32_t user_pages;
} vmm_state_t;

// =============================================================================
// VIRTUAL MEMORY MANAGER API
// =============================================================================

/**
 * @brief Initialize Virtual Memory Manager
 */
void vmm_init(void);

/**
 * @brief Enable paging
 */
void vmm_enable_paging(void);

/**
 * @brief Disable paging
 */
void vmm_disable_paging(void);

/**
 * @brief Map physical page to virtual address
 * @param virt_addr Virtual address
 * @param phys_addr Physical address
 * @param flags Page flags
 * @return true on success, false on failure
 */
bool vmm_map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags);

/**
 * @brief Unmap virtual page
 * @param virt_addr Virtual address
 * @return true on success, false on failure
 */
bool vmm_unmap_page(uint32_t virt_addr);

/**
 * @brief Map multiple contiguous pages
 * @param virt_addr Virtual address
 * @param phys_addr Physical address
 * @param count Number of pages
 * @param flags Page flags
 * @return true on success, false on failure
 */
bool vmm_map_pages(uint32_t virt_addr, uint32_t phys_addr, uint32_t count, uint32_t flags);

/**
 * @brief Unmap multiple contiguous pages
 * @param virt_addr Virtual address
 * @param count Number of pages
 * @return true on success, false on failure
 */
bool vmm_unmap_pages(uint32_t virt_addr, uint32_t count);

/**
 * @brief Get physical address from virtual address
 * @param virt_addr Virtual address
 * @return Physical address or 0 if not mapped
 */
uint32_t vmm_get_physical(uint32_t virt_addr);

/**
 * @brief Check if virtual address is mapped
 * @param virt_addr Virtual address
 * @return true if mapped, false otherwise
 */
bool vmm_is_mapped(uint32_t virt_addr);

/**
 * @brief Switch to new page directory
 * @param directory New page directory
 */
void vmm_switch_directory(page_directory_t* directory);

/**
 * @brief Flush TLB (Translation Lookaside Buffer)
 */
void vmm_flush_tlb(void);

/**
 * @brief Handle page fault
 * @param error_code Error code from CPU
 * @param fault_addr Faulting address
 */
void vmm_handle_page_fault(uint32_t error_code, uint32_t fault_addr);

/**
 * @brief Get VMM state
 * @return Pointer to VMM state structure
 */
vmm_state_t* vmm_get_state(void);

/**
 * @brief Print VMM statistics
 */
void vmm_print_stats(void);

#endif // VMM_H

