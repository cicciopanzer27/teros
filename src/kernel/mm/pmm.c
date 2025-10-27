/**
 * @file pmm.c
 * @brief Physical Memory Manager (PMM) with Buddy Allocator
 * @author TEROS Development Team
 * @date 2025
 */

#include "pmm.h"
#include "console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// BUDDY ALLOCATOR IMPLEMENTATION
// =============================================================================

#define PMM_MAX_ORDER 20        // Maximum order (1MB blocks)
#define PMM_MIN_ORDER 0         // Minimum order (4KB blocks)
#define PMM_PAGE_SIZE 4096      // 4KB pages
#define PMM_BUDDY_ARRAY_SIZE (PMM_MAX_ORDER + 1)

// Buddy allocator structure
typedef struct buddy_block {
    struct buddy_block* next;
    struct buddy_block* prev;
    uint32_t order;
    bool allocated;
    uint32_t physical_address;
} buddy_block_t;

typedef struct buddy_list {
    buddy_block_t* head;
    buddy_block_t* tail;
    uint32_t count;
} buddy_list_t;

typedef struct {
    uint32_t total_pages;
    uint32_t free_pages;
    uint32_t allocated_pages;
    uint32_t reserved_pages;
    uint32_t memory_start;
    uint32_t memory_end;
    buddy_list_t free_lists[PMM_BUDDY_ARRAY_SIZE];
    buddy_block_t* block_array;
    uint32_t block_count;
    uint8_t* page_bitmap;
    uint32_t bitmap_size;
} pmm_state_t;

static pmm_state_t pmm_state;
static bool pmm_initialized = false;

// =============================================================================
// PMM INITIALIZATION
// =============================================================================

void pmm_init(void) {
    if (pmm_initialized) {
        return;
    }
    
    console_puts("PMM: Initializing Physical Memory Manager...\n");
    
    // Initialize state
    memset(&pmm_state, 0, sizeof(pmm_state_t));
    
    // Set memory boundaries (simplified - assume 128MB available)
    pmm_state.memory_start = 0x100000;  // 1MB (skip first 1MB)
    pmm_state.memory_end = 0x8000000;   // 128MB
    pmm_state.total_pages = (pmm_state.memory_end - pmm_state.memory_start) / PMM_PAGE_SIZE;
    
    console_puts("PMM: Memory range: 0x");
    printf("%x", pmm_state.memory_start);
    console_puts(" - 0x");
    printf("%x", pmm_state.memory_end);
    console_puts("\n");
    
    console_puts("PMM: Total pages: ");
    printf("%u", pmm_state.total_pages);
    console_puts("\n");
    
    // Initialize free lists
    for (int i = 0; i < PMM_BUDDY_ARRAY_SIZE; i++) {
        pmm_state.free_lists[i].head = NULL;
        pmm_state.free_lists[i].tail = NULL;
        pmm_state.free_lists[i].count = 0;
    }
    
    // Allocate block array
    pmm_state.block_count = pmm_state.total_pages;
    pmm_state.block_array = (buddy_block_t*)kmalloc(pmm_state.block_count * sizeof(buddy_block_t));
    if (pmm_state.block_array == NULL) {
        console_puts("PMM: ERROR - Failed to allocate block array\n");
        return;
    }
    
    // Initialize blocks
    for (uint32_t i = 0; i < pmm_state.block_count; i++) {
        pmm_state.block_array[i].next = NULL;
        pmm_state.block_array[i].prev = NULL;
        pmm_state.block_array[i].order = 0;
        pmm_state.block_array[i].allocated = false;
        pmm_state.block_array[i].physical_address = pmm_state.memory_start + (i * PMM_PAGE_SIZE);
    }
    
    // Allocate page bitmap
    pmm_state.bitmap_size = (pmm_state.total_pages + 7) / 8;
    pmm_state.page_bitmap = (uint8_t*)kmalloc(pmm_state.bitmap_size);
    if (pmm_state.page_bitmap == NULL) {
        console_puts("PMM: ERROR - Failed to allocate page bitmap\n");
        return;
    }
    
    // Clear bitmap (all pages free initially)
    memset(pmm_state.page_bitmap, 0, pmm_state.bitmap_size);
    
    // Initialize buddy allocator
    pmm_init_buddy_allocator();
    
    pmm_initialized = true;
    console_puts("PMM: Initialization complete\n");
}

void pmm_init_buddy_allocator(void) {
    console_puts("PMM: Initializing buddy allocator...\n");
    
    // Start with all pages as order-0 blocks
    uint32_t pages_per_block = 1;
    uint32_t current_order = 0;
    uint32_t remaining_pages = pmm_state.total_pages;
    
    while (remaining_pages > 0 && current_order <= PMM_MAX_ORDER) {
        uint32_t blocks_this_order = remaining_pages / pages_per_block;
        
        for (uint32_t i = 0; i < blocks_this_order; i++) {
            uint32_t block_index = i * pages_per_block;
            buddy_block_t* block = &pmm_state.block_array[block_index];
            
            block->order = current_order;
            block->allocated = false;
            block->physical_address = pmm_state.memory_start + (block_index * PMM_PAGE_SIZE);
            
            // Add to free list
            pmm_add_to_free_list(block, current_order);
        }
        
        remaining_pages -= blocks_this_order * pages_per_block;
        pages_per_block *= 2;
        current_order++;
    }
    
    pmm_state.free_pages = pmm_state.total_pages;
    console_puts("PMM: Buddy allocator initialized\n");
}

// =============================================================================
// BUDDY ALLOCATOR HELPERS
// =============================================================================

void pmm_add_to_free_list(buddy_block_t* block, uint32_t order) {
    if (order >= PMM_BUDDY_ARRAY_SIZE) {
        return;
    }
    
    buddy_list_t* list = &pmm_state.free_lists[order];
    
    block->next = NULL;
    block->prev = list->tail;
    
    if (list->tail != NULL) {
        list->tail->next = block;
    } else {
        list->head = block;
    }
    
    list->tail = block;
    list->count++;
}

buddy_block_t* pmm_remove_from_free_list(uint32_t order) {
    if (order >= PMM_BUDDY_ARRAY_SIZE) {
        return NULL;
    }
    
    buddy_list_t* list = &pmm_state.free_lists[order];
    
    if (list->head == NULL) {
        return NULL;
    }
    
    buddy_block_t* block = list->head;
    list->head = block->next;
    
    if (list->head != NULL) {
        list->head->prev = NULL;
    } else {
        list->tail = NULL;
    }
    
    list->count--;
    return block;
}

uint32_t pmm_get_buddy_address(uint32_t address, uint32_t order) {
    uint32_t block_size = PMM_PAGE_SIZE * (1 << order);
    return address ^ block_size;
}

buddy_block_t* pmm_find_block_by_address(uint32_t address) {
    uint32_t page_index = (address - pmm_state.memory_start) / PMM_PAGE_SIZE;
    if (page_index >= pmm_state.block_count) {
        return NULL;
    }
    
    return &pmm_state.block_array[page_index];
}

void pmm_split_block(buddy_block_t* block, uint32_t target_order) {
    while (block->order > target_order) {
        uint32_t current_order = block->order;
        uint32_t buddy_address = pmm_get_buddy_address(block->physical_address, current_order - 1);
        
        // Create buddy block
        buddy_block_t* buddy = pmm_find_block_by_address(buddy_address);
        if (buddy != NULL) {
            buddy->order = current_order - 1;
            buddy->allocated = false;
            buddy->physical_address = buddy_address;
            
            // Add buddy to free list
            pmm_add_to_free_list(buddy, current_order - 1);
        }
        
        // Reduce current block order
        block->order = current_order - 1;
    }
}

void pmm_merge_buddies(buddy_block_t* block) {
    while (block->order < PMM_MAX_ORDER) {
        uint32_t buddy_address = pmm_get_buddy_address(block->physical_address, block->order);
        buddy_block_t* buddy = pmm_find_block_by_address(buddy_address);
        
        if (buddy == NULL || buddy->allocated || buddy->order != block->order) {
            break;
        }
        
        // Remove buddy from free list
        buddy_list_t* list = &pmm_state.free_lists[block->order];
        
        if (buddy->prev != NULL) {
            buddy->prev->next = buddy->next;
        } else {
            list->head = buddy->next;
        }
        
        if (buddy->next != NULL) {
            buddy->next->prev = buddy->prev;
        } else {
            list->tail = buddy->prev;
        }
        
        list->count--;
        
        // Merge blocks
        if (block->physical_address > buddy_address) {
            block = buddy;
        }
        
        block->order++;
    }
    
    // Add merged block to free list
    pmm_add_to_free_list(block, block->order);
}

// =============================================================================
// PMM PUBLIC API
// =============================================================================

void* pmm_alloc_pages(uint32_t num_pages) {
    if (!pmm_initialized || num_pages == 0) {
        return NULL;
    }
    
    // Calculate required order
    uint32_t required_order = 0;
    uint32_t pages = 1;
    while (pages < num_pages && required_order < PMM_MAX_ORDER) {
        pages *= 2;
        required_order++;
    }
    
    // Find suitable block
    buddy_block_t* block = NULL;
    uint32_t search_order = required_order;
    
    while (search_order <= PMM_MAX_ORDER) {
        block = pmm_remove_from_free_list(search_order);
        if (block != NULL) {
            break;
        }
        search_order++;
    }
    
    if (block == NULL) {
        console_puts("PMM: ERROR - No free pages available\n");
        return NULL;
    }
    
    // Split block if necessary
    if (block->order > required_order) {
        pmm_split_block(block, required_order);
    }
    
    // Mark as allocated
    block->allocated = true;
    
    // Update page bitmap
    uint32_t start_page = (block->physical_address - pmm_state.memory_start) / PMM_PAGE_SIZE;
    uint32_t pages_to_mark = 1 << required_order;
    
    for (uint32_t i = 0; i < pages_to_mark; i++) {
        uint32_t page_index = start_page + i;
        uint32_t byte_index = page_index / 8;
        uint32_t bit_index = page_index % 8;
        
        if (byte_index < pmm_state.bitmap_size) {
            pmm_state.page_bitmap[byte_index] |= (1 << bit_index);
        }
    }
    
    // Update statistics
    pmm_state.allocated_pages += pages_to_mark;
    pmm_state.free_pages -= pages_to_mark;
    
    return (void*)block->physical_address;
}

void pmm_free_pages(void* address, uint32_t num_pages) {
    if (!pmm_initialized || address == NULL || num_pages == 0) {
        return;
    }
    
    uint32_t physical_address = (uint32_t)address;
    
    // Find the block
    buddy_block_t* block = pmm_find_block_by_address(physical_address);
    if (block == NULL || !block->allocated) {
        console_puts("PMM: ERROR - Invalid address to free\n");
        return;
    }
    
    // Calculate order
    uint32_t order = 0;
    uint32_t pages = 1;
    while (pages < num_pages && order < PMM_MAX_ORDER) {
        pages *= 2;
        order++;
    }
    
    // Update page bitmap
    uint32_t start_page = (physical_address - pmm_state.memory_start) / PMM_PAGE_SIZE;
    uint32_t pages_to_clear = 1 << order;
    
    for (uint32_t i = 0; i < pages_to_clear; i++) {
        uint32_t page_index = start_page + i;
        uint32_t byte_index = page_index / 8;
        uint32_t bit_index = page_index % 8;
        
        if (byte_index < pmm_state.bitmap_size) {
            pmm_state.page_bitmap[byte_index] &= ~(1 << bit_index);
        }
    }
    
    // Mark as free
    block->allocated = false;
    block->order = order;
    
    // Merge with buddies
    pmm_merge_buddies(block);
    
    // Update statistics
    pmm_state.allocated_pages -= pages_to_clear;
    pmm_state.free_pages += pages_to_clear;
}

void* pmm_alloc_page(void) {
    return pmm_alloc_pages(1);
}

void pmm_free_page(void* address) {
    pmm_free_pages(address, 1);
}

// =============================================================================
// PMM QUERY FUNCTIONS
// =============================================================================

uint32_t pmm_get_total_pages(void) {
    return pmm_state.total_pages;
}

uint32_t pmm_get_free_pages(void) {
    return pmm_state.free_pages;
}

uint32_t pmm_get_allocated_pages(void) {
    return pmm_state.allocated_pages;
}

uint32_t pmm_get_reserved_pages(void) {
    return pmm_state.reserved_pages;
}

uint32_t pmm_get_memory_start(void) {
    return pmm_state.memory_start;
}

uint32_t pmm_get_memory_end(void) {
    return pmm_state.memory_end;
}

bool pmm_is_page_allocated(uint32_t physical_address) {
    uint32_t page_index = (physical_address - pmm_state.memory_start) / PMM_PAGE_SIZE;
    if (page_index >= pmm_state.total_pages) {
        return false;
    }
    
    uint32_t byte_index = page_index / 8;
    uint32_t bit_index = page_index % 8;
    
    if (byte_index >= pmm_state.bitmap_size) {
        return false;
    }
    
    return (pmm_state.page_bitmap[byte_index] & (1 << bit_index)) != 0;
}

// =============================================================================
// PMM DEBUG FUNCTIONS
// =============================================================================

void pmm_print_statistics(void) {
    if (!pmm_initialized) {
        console_puts("PMM: Not initialized\n");
        return;
    }
    
    console_puts("PMM Statistics:\n");
    console_puts("  Total pages: ");
    printf("%u", pmm_state.total_pages);
    console_puts("\n");
    console_puts("  Free pages: ");
    printf("%u", pmm_state.free_pages);
    console_puts("\n");
    console_puts("  Allocated pages: ");
    printf("%u", pmm_state.allocated_pages);
    console_puts("\n");
    console_puts("  Reserved pages: ");
    printf("%u", pmm_state.reserved_pages);
    console_puts("\n");
    
    console_puts("  Free lists:\n");
    for (int i = 0; i < PMM_BUDDY_ARRAY_SIZE; i++) {
        if (pmm_state.free_lists[i].count > 0) {
            console_puts("    Order ");
            printf("%d", i);
            console_puts(": ");
            printf("%u", pmm_state.free_lists[i].count);
            console_puts(" blocks\n");
        }
    }
}

void pmm_print_memory_map(void) {
    if (!pmm_initialized) {
        console_puts("PMM: Not initialized\n");
        return;
    }
    
    console_puts("PMM Memory Map:\n");
    console_puts("  Start: 0x");
    printf("%x", pmm_state.memory_start);
    console_puts("\n");
    console_puts("  End: 0x");
    printf("%x", pmm_state.memory_end);
    console_puts("\n");
    console_puts("  Size: ");
    printf("%u", pmm_state.memory_end - pmm_state.memory_start);
    console_puts(" bytes\n");
}

bool pmm_is_initialized(void) {
    return pmm_initialized;
}
