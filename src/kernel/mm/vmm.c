/**
 * @file vmm.c
 * @brief Virtual Memory Manager implementation
 */

#include "vmm.h"
#include "pmm.h"
#include <stddef.h>

// Global VMM state
static vmm_state_t vmm_state = {0};

// Helper function to get page directory index from virtual address
static inline uint32_t vmm_get_pd_index(uint32_t virt_addr) {
    return (virt_addr >> 22) & 0x3FF;  // Bits 31-22
}

// Helper function to get page table index from virtual address
static inline uint32_t vmm_get_pt_index(uint32_t virt_addr) {
    return (virt_addr >> 12) & 0x3FF;  // Bits 21-12
}

// Helper function to get page offset from virtual address
static inline uint32_t vmm_get_offset(uint32_t virt_addr) {
    return virt_addr & 0xFFF;  // Bits 11-0
}

void vmm_init(void) {
    // Allocate page directory using PMM
    uint32_t pd_phys = pmm_alloc_page();
    if (pd_phys == 0) {
        return;  // Failed to allocate
    }
    
    // Map page directory to virtual address
    vmm_state.current_directory = (page_directory_t*)(pd_phys + VMM_KERNEL_BASE);
    
    // Clear page directory
    for (uint32_t i = 0; i < 1024; i++) {
        page_directory_entry_t* entry = &vmm_state.current_directory->entries[i];
        entry->present = 0;
        entry->frame = 0;
        entry->write = 0;
        entry->user = 0;
    }
    
    // Map first 4MB identity mapped (for kernel)
    for (uint32_t i = 0; i < 1024; i++) {
        uint32_t virt_addr = i * VMM_PAGE_SIZE;
        uint32_t phys_addr = virt_addr;
        
        // Create simple 4MB page identity mapping
        vmm_state.current_directory->entries[i].present = 1;
        vmm_state.current_directory->entries[i].write = 1;
        vmm_state.current_directory->entries[i].frame = phys_addr >> 12;
        vmm_state.current_directory->entries[i].user = 0;
    }
    
    vmm_state.initialized = true;
    vmm_state.kernel_pages = 1024;
    vmm_state.user_pages = 0;
}

bool vmm_map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags) {
    if (!vmm_state.initialized) {
        return false;
    }
    
    uint32_t pd_index = vmm_get_pd_index(virt_addr);
    uint32_t pt_index = vmm_get_pt_index(virt_addr);
    
    page_directory_entry_t* pd_entry = &vmm_state.current_directory->entries[pd_index];
    
    // If page table doesn't exist, create it
    if (!pd_entry->present) {
        uint32_t pt_phys = pmm_alloc_page();
        if (pt_phys == 0) {
            return false;
        }
        
        pd_entry->present = 1;
        pd_entry->write = 1;
        pd_entry->user = flags & VMM_USER ? 1 : 0;
        pd_entry->frame = pt_phys >> 12;
        
        // Clear new page table
        page_table_t* pt = (page_table_t*)(pt_phys + VMM_KERNEL_BASE);
        for (uint32_t i = 0; i < 1024; i++) {
            pt->entries[i].present = 0;
        }
    }
    
    // Get page table
    uint32_t pt_phys = pd_entry->frame << 12;
    page_table_t* pt = (page_table_t*)(pt_phys + VMM_KERNEL_BASE);
    
    // Set page table entry
    page_table_entry_t* entry = &pt->entries[pt_index];
    entry->present = 1;
    entry->write = flags & VMM_WRITE ? 1 : 0;
    entry->user = flags & VMM_USER ? 1 : 0;
    entry->frame = phys_addr >> 12;
    
    // Flush TLB
    vmm_flush_tlb();
    
    if (flags & VMM_USER) {
        vmm_state.user_pages++;
    } else {
        vmm_state.kernel_pages++;
    }
    
    return true;
}

bool vmm_unmap_page(uint32_t virt_addr) {
    if (!vmm_state.initialized) {
        return false;
    }
    
    uint32_t pd_index = vmm_get_pd_index(virt_addr);
    uint32_t pt_index = vmm_get_pt_index(virt_addr);
    
    page_directory_entry_t* pd_entry = &vmm_state.current_directory->entries[pd_index];
    
    if (!pd_entry->present) {
        return false;
    }
    
    // Get page table
    uint32_t pt_phys = pd_entry->frame << 12;
    page_table_t* pt = (page_table_t*)(pt_phys + VMM_KERNEL_BASE);
    
    // Clear page table entry
    page_table_entry_t* entry = &pt->entries[pt_index];
    if (entry->present) {
        entry->present = 0;
        entry->frame = 0;
        
        // Flush TLB
        vmm_flush_tlb();
        
        if (entry->user) {
            vmm_state.user_pages--;
        } else {
            vmm_state.kernel_pages--;
        }
        
        return true;
    }
    
    return false;
}

uint32_t vmm_get_physical(uint32_t virt_addr) {
    if (!vmm_state.initialized) {
        return 0;
    }
    
    uint32_t pd_index = vmm_get_pd_index(virt_addr);
    uint32_t pt_index = vmm_get_pt_index(virt_addr);
    
    page_directory_entry_t* pd_entry = &vmm_state.current_directory->entries[pd_index];
    
    if (!pd_entry->present) {
        return 0;
    }
    
    // Get page table
    uint32_t pt_phys = pd_entry->frame << 12;
    page_table_t* pt = (page_table_t*)(pt_phys + VMM_KERNEL_BASE);
    
    // Get page table entry
    page_table_entry_t* entry = &pt->entries[pt_index];
    
    if (!entry->present) {
        return 0;
    }
    
    return (entry->frame << 12) | vmm_get_offset(virt_addr);
}

bool vmm_is_mapped(uint32_t virt_addr) {
    return vmm_get_physical(virt_addr) != 0;
}

void vmm_flush_tlb(void) {
    asm volatile("mov %%cr3, %%eax; mov %%eax, %%cr3" ::: "eax");
}

vmm_state_t* vmm_get_state(void) {
    return &vmm_state;
}

bool vmm_map_pages(uint32_t virt_addr, uint32_t phys_addr, uint32_t count, uint32_t flags) {
    for (uint32_t i = 0; i < count; i++) {
        if (!vmm_map_page(virt_addr + i * VMM_PAGE_SIZE, 
                          phys_addr + i * VMM_PAGE_SIZE, 
                          flags)) {
            // Failed, unmap what we mapped
            for (uint32_t j = 0; j < i; j++) {
                vmm_unmap_page(virt_addr + j * VMM_PAGE_SIZE);
            }
            return false;
        }
    }
    return true;
}

bool vmm_unmap_pages(uint32_t virt_addr, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        vmm_unmap_page(virt_addr + i * VMM_PAGE_SIZE);
    }
    return true;
}

void vmm_handle_page_fault(uint32_t error_code, uint32_t fault_addr) {
    // TODO: Implement page fault handling
    // For now, just halt
    while(1);
}

void vmm_enable_paging(void) {
    if (!vmm_state.initialized) {
        vmm_init();
    }
    
    // Load page directory into CR3
    uint32_t pd_phys = ((uint32_t)vmm_state.current_directory) - VMM_KERNEL_BASE;
    asm volatile("mov %0, %%cr3" :: "r"(pd_phys));
    
    // Enable paging in CR0
    asm volatile(
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"
        "mov %%eax, %%cr0\n"
        ::: "eax"
    );
}

void vmm_disable_paging(void) {
    // Disable paging in CR0
    asm volatile(
        "mov %%cr0, %%eax\n"
        "and $0x7FFFFFFF, %%eax\n"
        "mov %%eax, %%cr0\n"
        ::: "eax"
    );
}

void vmm_switch_directory(page_directory_t* directory) {
    vmm_state.current_directory = directory;
    uint32_t pd_phys = ((uint32_t)directory) - VMM_KERNEL_BASE;
    asm volatile("mov %0, %%cr3" :: "r"(pd_phys));
}

void vmm_print_stats(void) {
    // TODO: Implement with console output
}

