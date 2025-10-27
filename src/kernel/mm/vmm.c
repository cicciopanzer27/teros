/**
 * @file vmm.c
 * @brief Virtual Memory Manager (VMM) with Ternary Page Tables
 * @author TEROS Development Team
 * @date 2025
 */

#include "vmm.h"
#include "pmm.h"
#include "kmalloc.h"
#include "console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// TERNARY PAGE TABLE IMPLEMENTATION
// =============================================================================

#define VMM_PAGE_SIZE 4096
#define VMM_PAGE_SHIFT 12
#define VMM_PAGE_MASK 0xFFF

// Ternary page table entry flags
#define VMM_PTE_PRESENT 0x001    // Page present
#define VMM_PTE_WRITABLE 0x002   // Page writable
#define VMM_PTE_USER 0x004       // User accessible
#define VMM_PTE_PWT 0x008        // Page write-through
#define VMM_PTE_PCD 0x010        // Page cache disabled
#define VMM_PTE_ACCESSED 0x020  // Page accessed
#define VMM_PTE_DIRTY 0x040      // Page dirty
#define VMM_PTE_SIZE 0x080       // Page size (2MB/4MB)
#define VMM_PTE_GLOBAL 0x100     // Global page
#define VMM_PTE_AVAIL 0xE00      // Available bits

// Ternary page table structure
typedef struct {
    uint32_t entries[1024];     // 1024 entries per level
    uint32_t level;            // Level (0=PT, 1=PD, 2=PDPT, 3=PML4)
    bool allocated;            // Whether this table is allocated
} ternary_page_table_t;

typedef struct {
    uint32_t total_pages;
    uint32_t mapped_pages;
    uint32_t unmapped_pages;
    uint32_t kernel_pages;
    uint32_t user_pages;
    ternary_page_table_t* page_tables;
    uint32_t page_table_count;
    uint32_t tlb_entries;
    uint32_t tlb_hits;
    uint32_t tlb_misses;
    uint32_t page_faults;
    uint32_t page_faults_resolved;
} vmm_state_t;

static vmm_state_t vmm_state;
static bool vmm_initialized = false;

// =============================================================================
// VMM INITIALIZATION
// =============================================================================

void vmm_init(void) {
    if (vmm_initialized) {
        return;
    }
    
    console_puts("VMM: Initializing Virtual Memory Manager...\n");
    
    // Initialize state
    memset(&vmm_state, 0, sizeof(vmm_state_t));
    
    // Initialize page tables
    vmm_state.page_table_count = 1024; // Start with 1024 page tables
    vmm_state.page_tables = (ternary_page_table_t*)kmalloc(vmm_state.page_table_count * sizeof(ternary_page_table_t));
    if (vmm_state.page_tables == NULL) {
        console_puts("VMM: ERROR - Failed to allocate page tables\n");
        return;
    }
    
    // Initialize all page tables
    for (uint32_t i = 0; i < vmm_state.page_table_count; i++) {
        memset(&vmm_state.page_tables[i], 0, sizeof(ternary_page_table_t));
        vmm_state.page_tables[i].level = 0;
        vmm_state.page_tables[i].allocated = false;
    }
    
    // Initialize kernel page tables
    vmm_init_kernel_mapping();
    
    // Initialize TLB
    vmm_init_tlb();
    
    vmm_initialized = true;
    console_puts("VMM: Initialization complete\n");
}

void vmm_init_kernel_mapping(void) {
    console_puts("VMM: Initializing kernel mapping...\n");
    
    // Map kernel space (0xC0000000 - 0xFFFFFFFF)
    uint32_t kernel_start = 0xC0000000;
    uint32_t kernel_end = 0xFFFFFFFF;
    
    for (uint32_t virt = kernel_start; virt < kernel_end; virt += VMM_PAGE_SIZE) {
        uint32_t phys = virt - kernel_start; // Identity mapping for kernel
        vmm_map_page(virt, phys, VMM_PTE_PRESENT | VMM_PTE_WRITABLE);
    }
    
    console_puts("VMM: Kernel mapping complete\n");
}

void vmm_init_tlb(void) {
    console_puts("VMM: Initializing TLB...\n");
    
    // Initialize TLB statistics
    vmm_state.tlb_entries = 0;
    vmm_state.tlb_hits = 0;
    vmm_state.tlb_misses = 0;
    
    console_puts("VMM: TLB initialized\n");
}

// =============================================================================
// PAGE TABLE OPERATIONS
// =============================================================================

ternary_page_table_t* vmm_alloc_page_table(uint32_t level) {
    // Find free page table
    for (uint32_t i = 0; i < vmm_state.page_table_count; i++) {
        if (!vmm_state.page_tables[i].allocated) {
            ternary_page_table_t* table = &vmm_state.page_tables[i];
            memset(table, 0, sizeof(ternary_page_table_t));
            table->level = level;
            table->allocated = true;
            return table;
        }
    }
    
    // No free page table found
    console_puts("VMM: ERROR - No free page tables available\n");
    return NULL;
}

void vmm_free_page_table(ternary_page_table_t* table) {
    if (table == NULL) {
        return;
    }
    
    // Clear all entries
    memset(table->entries, 0, sizeof(table->entries));
    table->allocated = false;
    table->level = 0;
}

uint32_t vmm_get_page_table_index(uintptr_t virtual_address, uint32_t level) {
    switch (level) {
        case 0: return (virtual_address >> 12) & 0x3FF;  // PT
        case 1: return (virtual_address >> 22) & 0x3FF;  // PD
        case 2: return (virtual_address >> 32) & 0x3FF;  // PDPT
        case 3: return (virtual_address >> 42) & 0x3FF;  // PML4
        default: return 0;
    }
}

ternary_page_table_t* vmm_get_page_table(uint32_t virtual_address, uint32_t level) {
    // This is a simplified implementation
    // In a real system, we would walk the page table hierarchy
    uint32_t index = vmm_get_page_table_index(virtual_address, level);
    
    if (index < vmm_state.page_table_count) {
        return &vmm_state.page_tables[index];
    }
    
    return NULL;
}

// =============================================================================
// PAGE MAPPING FUNCTIONS
// =============================================================================

bool vmm_map_page(uint32_t virtual_address, uint32_t physical_address, uint32_t flags) {
    if (!vmm_initialized) {
        return false;
    }
    
    // Get page table for this virtual address
    ternary_page_table_t* table = vmm_get_page_table(virtual_address, 0);
    if (table == NULL) {
        table = vmm_alloc_page_table(0);
        if (table == NULL) {
            return false;
        }
    }
    
    // Calculate page table index
    uint32_t index = vmm_get_page_table_index(virtual_address, 0);
    
    // Create page table entry
    uint32_t pte = physical_address | flags;
    table->entries[index] = pte;
    
    // Update statistics
    vmm_state.mapped_pages++;
    
    // Invalidate TLB entry
    vmm_invalidate_tlb_entry(virtual_address);
    
    return true;
}

bool vmm_unmap_page(uint32_t virtual_address) {
    if (!vmm_initialized) {
        return false;
    }
    
    // Get page table for this virtual address
    ternary_page_table_t* table = vmm_get_page_table(virtual_address, 0);
    if (table == NULL) {
        return false;
    }
    
    // Calculate page table index
    uint32_t index = vmm_get_page_table_index(virtual_address, 0);
    
    // Clear page table entry
    table->entries[index] = 0;
    
    // Update statistics
    vmm_state.mapped_pages--;
    vmm_state.unmapped_pages++;
    
    // Invalidate TLB entry
    vmm_invalidate_tlb_entry(virtual_address);
    
    return true;
}

bool vmm_is_page_mapped(uint32_t virtual_address) {
    if (!vmm_initialized) {
        return false;
    }
    
    // Get page table for this virtual address
    ternary_page_table_t* table = vmm_get_page_table(virtual_address, 0);
    if (table == NULL) {
        return false;
    }
    
    // Calculate page table index
    uint32_t index = vmm_get_page_table_index(virtual_address, 0);
    
    // Check if page is present
    return (table->entries[index] & VMM_PTE_PRESENT) != 0;
}

uint32_t vmm_get_physical_address(uint32_t virtual_address) {
    if (!vmm_initialized) {
        return 0;
    }
    
    // Get page table for this virtual address
    ternary_page_table_t* table = vmm_get_page_table(virtual_address, 0);
    if (table == NULL) {
        return 0;
    }
    
    // Calculate page table index
    uint32_t index = vmm_get_page_table_index(virtual_address, 0);
    
    // Get page table entry
    uint32_t pte = table->entries[index];
    
    if ((pte & VMM_PTE_PRESENT) == 0) {
        return 0;
    }
    
    // Return physical address
    return pte & ~VMM_PAGE_MASK;
}

// =============================================================================
// TLB OPERATIONS
// =============================================================================

void vmm_invalidate_tlb_entry(uint32_t virtual_address) {
    (void)virtual_address;
    // Invalidate TLB entry for virtual address
    // This is a simplified implementation
    vmm_state.tlb_entries--;
}

void vmm_invalidate_tlb(void) {
    // Invalidate entire TLB
    vmm_state.tlb_entries = 0;
}

bool vmm_tlb_lookup(uint32_t virtual_address, uint32_t* physical_address) {
    (void)virtual_address;
    (void)physical_address;
    // TLB lookup
    // This is a simplified implementation
    vmm_state.tlb_misses++;
    return false;
}

void vmm_tlb_insert(uint32_t virtual_address, uint32_t physical_address) {
    (void)virtual_address;
    (void)physical_address;
    // Insert TLB entry
    vmm_state.tlb_entries++;
    vmm_state.tlb_hits++;
}

// =============================================================================
// PAGE FAULT HANDLING
// =============================================================================

bool vmm_handle_page_fault(uint32_t virtual_address, uint32_t error_code) {
    if (!vmm_initialized) {
        return false;
    }
    
    vmm_state.page_faults++;
    
    // Check if page is already mapped
    if (vmm_is_page_mapped(virtual_address)) {
        // Page is mapped but fault occurred
        // This might be a permission fault
        console_puts("VMM: Page fault on mapped page\n");
        return false;
    }
    
    // Allocate physical page
    void* physical_page = pmm_alloc_page();
    if (physical_page == NULL) {
        console_puts("VMM: ERROR - No physical memory available\n");
        return false;
    }
    
    // Determine flags based on error code
    uint32_t flags = VMM_PTE_PRESENT;
    if (error_code & 0x2) { // Write fault
        flags |= VMM_PTE_WRITABLE;
    }
    if (error_code & 0x4) { // User fault
        flags |= VMM_PTE_USER;
    }
    
    // Map the page
    if (vmm_map_page(virtual_address, (uintptr_t)physical_page, flags)) {
        vmm_state.page_faults_resolved++;
        return true;
    }
    
    // Free the physical page if mapping failed
    pmm_free_page(physical_page);
    return false;
}

// =============================================================================
// VMM QUERY FUNCTIONS
// =============================================================================

uint32_t vmm_get_total_pages(void) {
    return vmm_state.total_pages;
}

uint32_t vmm_get_mapped_pages(void) {
    return vmm_state.mapped_pages;
}

uint32_t vmm_get_unmapped_pages(void) {
    return vmm_state.unmapped_pages;
}

uint32_t vmm_get_kernel_pages(void) {
    return vmm_state.kernel_pages;
}

uint32_t vmm_get_user_pages(void) {
    return vmm_state.user_pages;
}

uint32_t vmm_get_tlb_entries(void) {
    return vmm_state.tlb_entries;
}

uint32_t vmm_get_tlb_hits(void) {
    return vmm_state.tlb_hits;
}

uint32_t vmm_get_tlb_misses(void) {
    return vmm_state.tlb_misses;
}

uint32_t vmm_get_page_faults(void) {
    return vmm_state.page_faults;
}

uint32_t vmm_get_page_faults_resolved(void) {
    return vmm_state.page_faults_resolved;
}

bool vmm_is_initialized(void) {
    return vmm_initialized;
}

// =============================================================================
// VMM DEBUG FUNCTIONS
// =============================================================================

void vmm_print_statistics(void) {
    if (!vmm_initialized) {
        console_puts("VMM: Not initialized\n");
        return;
    }
    
    console_puts("VMM Statistics:\n");
    console_puts("  Total pages: ");
    printf("%u", vmm_state.total_pages);
    console_puts("\n");
    console_puts("  Mapped pages: ");
    printf("%u", vmm_state.mapped_pages);
    console_puts("\n");
    console_puts("  Unmapped pages: ");
    printf("%u", vmm_state.unmapped_pages);
    console_puts("\n");
    console_puts("  Kernel pages: ");
    printf("%u", vmm_state.kernel_pages);
    console_puts("\n");
    console_puts("  User pages: ");
    printf("%u", vmm_state.user_pages);
    console_puts("\n");
    console_puts("  TLB entries: ");
    printf("%u", vmm_state.tlb_entries);
    console_puts("\n");
    console_puts("  TLB hits: ");
    printf("%u", vmm_state.tlb_hits);
    console_puts("\n");
    console_puts("  TLB misses: ");
    printf("%u", vmm_state.tlb_misses);
    console_puts("\n");
    console_puts("  Page faults: ");
    printf("%u", vmm_state.page_faults);
    console_puts("\n");
    console_puts("  Page faults resolved: ");
    printf("%u", vmm_state.page_faults_resolved);
    console_puts("\n");
}

void vmm_print_page_table(uint32_t virtual_address) {
    if (!vmm_initialized) {
        console_puts("VMM: Not initialized\n");
        return;
    }
    
    console_puts("VMM Page Table for address 0x");
    printf("%x", virtual_address);
    console_puts(":\n");
    
    // Get page table for this virtual address
    ternary_page_table_t* table = vmm_get_page_table(virtual_address, 0);
    if (table == NULL) {
        console_puts("  No page table found\n");
        return;
    }
    
    // Calculate page table index
    uint32_t index = vmm_get_page_table_index(virtual_address, 0);
    
    // Get page table entry
    uint32_t pte = table->entries[index];
    
    console_puts("  Entry: 0x");
    printf("%x", pte);
    console_puts("\n");
    console_puts("  Present: ");
    console_puts((pte & VMM_PTE_PRESENT) ? "Yes" : "No");
    console_puts("\n");
    console_puts("  Writable: ");
    console_puts((pte & VMM_PTE_WRITABLE) ? "Yes" : "No");
    console_puts("\n");
    console_puts("  User: ");
    console_puts((pte & VMM_PTE_USER) ? "Yes" : "No");
    console_puts("\n");
    
    if (pte & VMM_PTE_PRESENT) {
        uint32_t phys_addr = pte & ~VMM_PAGE_MASK;
        console_puts("  Physical address: 0x");
        printf("%x", phys_addr);
        console_puts("\n");
    }
}
