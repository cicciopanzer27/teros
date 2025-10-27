/**
 * @file kmalloc.c
 * @brief Kernel heap allocator implementation
 */

#include "kmalloc.h"
#include <stddef.h>

#define HEAP_ALIGN 8
#define MIN_BLOCK_SIZE (sizeof(mem_block_t) + HEAP_ALIGN)

static uint32_t heap_start = 0;
static size_t heap_size = 0;
static mem_block_t* heap_list = NULL;
static heap_stats_t stats = {0};

// Helper to align size
static inline size_t align_size(size_t size) {
    return (size + HEAP_ALIGN - 1) & ~(HEAP_ALIGN - 1);
}

// Helper to get header from data pointer
static inline mem_block_t* get_header(void* ptr) {
    return (mem_block_t*)((uint8_t*)ptr - sizeof(mem_block_t));
}

// Helper to get data pointer from header
static inline void* get_data(mem_block_t* block) {
    return (void*)((uint8_t*)block + sizeof(mem_block_t));
}

// Helper to get next block
static inline mem_block_t* get_next_block(mem_block_t* block) {
    return (mem_block_t*)((uint8_t*)block + sizeof(mem_block_t) + block->size);
}

void kmalloc_init(uint32_t start, size_t size) {
    heap_start = start;
    heap_size = size;
    
    // Initialize first block
    heap_list = (mem_block_t*)heap_start;
    heap_list->next = NULL;
    heap_list->prev = NULL;
    heap_list->size = size - sizeof(mem_block_t);
    heap_list->free = true;
    
    // Initialize statistics
    stats.total_size = size;
    stats.free_size = heap_list->size;
    stats.used_size = 0;
    stats.allocated_blocks = 0;
    stats.free_blocks = 1;
}

void* kmalloc(size_t size) {
    if (size == 0 || heap_list == NULL) {
        return NULL;
    }
    
    size = align_size(size);
    
    // Find a free block large enough
    mem_block_t* current = heap_list;
    while (current != NULL) {
        if (current->free && current->size >= size) {
            // Found suitable block
            
            // Check if we should split the block
            size_t remaining = current->size - size - sizeof(mem_block_t);
            if (remaining >= MIN_BLOCK_SIZE) {
                // Split block
                mem_block_t* new_block = get_next_block(current);
                new_block->size = remaining;
                new_block->free = true;
                new_block->next = current->next;
                new_block->prev = current;
                
                if (current->next) {
                    current->next->prev = new_block;
                }
                
                current->next = new_block;
                current->size = size;
                stats.free_blocks++;
            }
            
            // Mark as allocated
            current->free = false;
            stats.allocated_blocks++;
            stats.free_blocks--;
            stats.used_size += current->size;
            stats.free_size -= current->size;
            stats.allocation_count++;
            
            return get_data(current);
        }
        current = current->next;
    }
    
    return NULL;  // No suitable block found
}

void kfree(void* ptr) {
    if (ptr == NULL || heap_list == NULL) {
        return;
    }
    
    mem_block_t* block = get_header(ptr);
    if (block->free) {
        return;  // Already free
    }
    
    block->free = true;
    stats.allocated_blocks--;
    stats.free_blocks++;
    stats.used_size -= block->size;
    stats.free_size += block->size;
    stats.deallocation_count++;
    
    // Merge with next block if it's free
    if (block->next && block->next->free) {
        block->size += sizeof(mem_block_t) + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
        stats.free_blocks--;
    }
    
    // Merge with previous block if it's free
    if (block->prev && block->prev->free) {
        block->prev->size += sizeof(mem_block_t) + block->size;
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        }
        stats.free_blocks--;
    }
}

void* kmalloc_aligned(size_t size, size_t align) {
    void* ptr = kmalloc(size + align - 1 + sizeof(void*));
    if (ptr == NULL) {
        return NULL;
    }
    
    uint32_t addr = (uint32_t)ptr + sizeof(void*);
    uint32_t aligned_addr = (addr + align - 1) & ~(align - 1);
    
    void** header = (void**)(aligned_addr - sizeof(void*));
    *header = ptr;
    
    return (void*)aligned_addr;
}

void* krealloc(void* ptr, size_t size) {
    if (ptr == NULL) {
        return kmalloc(size);
    }
    
    mem_block_t* block = get_header(ptr);
    if (block->size >= size) {
        return ptr;
    }
    
    void* new_ptr = kmalloc(size);
    if (new_ptr == NULL) {
        return NULL;
    }
    
    // Copy old data (simple byte copy)
    uint8_t* old_bytes = (uint8_t*)ptr;
    uint8_t* new_bytes = (uint8_t*)new_ptr;
    for (size_t i = 0; i < block->size; i++) {
        new_bytes[i] = old_bytes[i];
    }
    
    kfree(ptr);
    return new_ptr;
}

void* kcalloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void* ptr = kmalloc(total);
    if (ptr != NULL) {
        // Zero memory
        uint8_t* bytes = (uint8_t*)ptr;
        for (size_t i = 0; i < total; i++) {
            bytes[i] = 0;
        }
    }
    return ptr;
}

bool kmalloc_check_leaks(void) {
    return stats.allocated_blocks > 0;
}

heap_stats_t* kmalloc_get_stats(void) {
    return &stats;
}

void kmalloc_print_stats(void) {
    // TODO: Implement with console output
}

void kmalloc_compact(void) {
    // Merge all adjacent free blocks
    mem_block_t* current = heap_list;
    while (current != NULL && current->next != NULL) {
        if (current->free && current->next->free) {
            current->size += sizeof(mem_block_t) + current->next->size;
            current->next = current->next->next;
            if (current->next) {
                current->next->prev = current;
            }
            stats.free_blocks--;
        } else {
            current = current->next;
        }
    }
}

