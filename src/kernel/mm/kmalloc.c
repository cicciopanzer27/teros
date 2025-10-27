/**
 * @file kmalloc.c
 * @brief Kernel Heap Allocator with Slab Allocator
 * @author TEROS Development Team
 * @date 2025
 */

#include "kmalloc.h"
#include "pmm.h"
#include "console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// Forward declarations
static void kmalloc_init_caches(void);

// =============================================================================
// SLAB ALLOCATOR IMPLEMENTATION
// =============================================================================

#define KMALLOC_MIN_SIZE 8
#define KMALLOC_MAX_SIZE 4096
#define KMALLOC_SLAB_SIZE 4096
#define KMALLOC_CACHE_COUNT 12

// Slab structure
typedef struct slab {
    struct slab* next;
    struct slab* prev;
    void* start;
    void* free;
    uint32_t free_count;
    uint32_t total_count;
    uint32_t size;
    bool allocated;
} slab_t;

// Cache structure
typedef struct {
    uint32_t size;
    uint32_t slab_count;
    slab_t* free_slabs;
    slab_t* partial_slabs;
    slab_t* full_slabs;
    uint32_t total_allocations;
    uint32_t total_deallocations;
    uint32_t total_slabs;
} kmalloc_cache_t;

typedef struct {
    kmalloc_cache_t caches[KMALLOC_CACHE_COUNT];
    uint32_t total_allocations;
    uint32_t total_deallocations;
    uint32_t total_bytes_allocated;
    uint32_t total_bytes_freed;
    uintptr_t heap_start;
    uintptr_t heap_end;
    size_t heap_size;
    bool initialized;
} kmalloc_state_t;

static kmalloc_state_t kmalloc_state;

// =============================================================================
// KMALLOC INITIALIZATION
// =============================================================================

void kmalloc_init(void) {
    if (kmalloc_state.initialized) {
        return;
    }
    
    console_puts("KMALLOC: Initializing kernel heap allocator...\n");
    
    // Initialize state
    memset(&kmalloc_state, 0, sizeof(kmalloc_state_t));
    
    // Set heap boundaries
    kmalloc_state.heap_start = 0x2000000;  // 32MB
    kmalloc_state.heap_end = 0x4000000;    // 64MB
    kmalloc_state.heap_size = kmalloc_state.heap_end - kmalloc_state.heap_start;
    
    console_puts("KMALLOC: Heap range: 0x");
    printf("%x", kmalloc_state.heap_start);
    console_puts(" - 0x");
    printf("%x", kmalloc_state.heap_end);
    console_puts("\n");
    
    // Initialize caches
    kmalloc_init_caches();
    
    kmalloc_state.initialized = true;
    console_puts("KMALLOC: Initialization complete\n");
}

static void kmalloc_init_caches(void) {
    console_puts("KMALLOC: Initializing caches...\n");
    
    // Initialize caches for different sizes (must match KMALLOC_CACHE_COUNT)
    uint32_t sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384};
    
    for (int i = 0; i < KMALLOC_CACHE_COUNT && i < 12; i++) {
        kmalloc_cache_t* cache = &kmalloc_state.caches[i];
        
        cache->size = sizes[i];
        cache->slab_count = 0;
        cache->free_slabs = NULL;
        cache->partial_slabs = NULL;
        cache->full_slabs = NULL;
        cache->total_allocations = 0;
        cache->total_deallocations = 0;
        cache->total_slabs = 0;
    }
    
    console_puts("KMALLOC: Caches initialized\n");
}

// =============================================================================
// SLAB OPERATIONS
// =============================================================================

slab_t* kmalloc_alloc_slab(uint32_t size) {
    // Allocate physical page for slab
    void* page = pmm_alloc_page();
    if (page == NULL) {
        return NULL;
    }
    
    // Create slab structure
    slab_t* slab = (slab_t*)kmalloc_state.heap_start;
    kmalloc_state.heap_start += sizeof(slab_t);
    
    if (kmalloc_state.heap_start >= kmalloc_state.heap_end) {
        pmm_free_page(page);
        return NULL;
    }
    
    // Initialize slab
    slab->next = NULL;
    slab->prev = NULL;
    slab->start = page;
    slab->free = page;
    slab->free_count = KMALLOC_SLAB_SIZE / size;
    slab->total_count = slab->free_count;
    slab->size = size;
    slab->allocated = true;
    
    return slab;
}

void kmalloc_free_slab(slab_t* slab) {
    if (slab == NULL) {
        return;
    }
    
    // Free physical page
    pmm_free_page(slab->start);
    
    // Clear slab structure
    memset(slab, 0, sizeof(slab_t));
}

void kmalloc_add_slab_to_list(slab_t** list, slab_t* slab) {
    if (slab == NULL) {
        return;
    }
    
    slab->next = *list;
    slab->prev = NULL;
    
    if (*list != NULL) {
        (*list)->prev = slab;
    }
    
    *list = slab;
}

void kmalloc_remove_slab_from_list(slab_t** list, slab_t* slab) {
    if (slab == NULL || *list == NULL) {
        return;
    }
    
    if (slab->prev != NULL) {
        slab->prev->next = slab->next;
    } else {
        *list = slab->next;
    }
    
    if (slab->next != NULL) {
        slab->next->prev = slab->prev;
    }
}

// =============================================================================
// CACHE OPERATIONS
// =============================================================================

kmalloc_cache_t* kmalloc_get_cache(uint32_t size) {
    // Find appropriate cache
    for (int i = 0; i < KMALLOC_CACHE_COUNT; i++) {
        if (kmalloc_state.caches[i].size >= size) {
            return &kmalloc_state.caches[i];
        }
    }
    
    // Use largest cache for oversized requests
    return &kmalloc_state.caches[KMALLOC_CACHE_COUNT - 1];
}

slab_t* kmalloc_get_slab(kmalloc_cache_t* cache) {
    // Try to get from free slabs first
    if (cache->free_slabs != NULL) {
        slab_t* slab = cache->free_slabs;
        kmalloc_remove_slab_from_list(&cache->free_slabs, slab);
        kmalloc_add_slab_to_list(&cache->partial_slabs, slab);
        return slab;
    }
    
    // Try to get from partial slabs
    if (cache->partial_slabs != NULL) {
        return cache->partial_slabs;
    }
    
    // Allocate new slab
    slab_t* slab = kmalloc_alloc_slab(cache->size);
    if (slab != NULL) {
        cache->total_slabs++;
        kmalloc_add_slab_to_list(&cache->partial_slabs, slab);
    }
    
    return slab;
}

void kmalloc_put_slab(kmalloc_cache_t* cache, slab_t* slab) {
    if (slab == NULL) {
        return;
    }
    
    // Remove from current list
    kmalloc_remove_slab_from_list(&cache->partial_slabs, slab);
    kmalloc_remove_slab_from_list(&cache->full_slabs, slab);
    
    // Add to appropriate list
    if (slab->free_count == slab->total_count) {
        // Free slab
        kmalloc_add_slab_to_list(&cache->free_slabs, slab);
    } else if (slab->free_count == 0) {
        // Full slab
        kmalloc_add_slab_to_list(&cache->full_slabs, slab);
    } else {
        // Partial slab
        kmalloc_add_slab_to_list(&cache->partial_slabs, slab);
    }
}

// =============================================================================
// KMALLOC PUBLIC API
// =============================================================================

void* kmalloc(uint32_t size) {
    if (!kmalloc_state.initialized || size == 0) {
        return NULL;
    }
    
    // Get appropriate cache
    kmalloc_cache_t* cache = kmalloc_get_cache(size);
    
    // Get slab
    slab_t* slab = kmalloc_get_slab(cache);
    if (slab == NULL) {
        console_puts("KMALLOC: ERROR - No memory available\n");
        return NULL;
    }
    
    // Allocate from slab
    void* ptr = slab->free;
    slab->free = (void*)((uintptr_t)slab->free + cache->size);
    slab->free_count--;
    
    // Update statistics
    cache->total_allocations++;
    kmalloc_state.total_allocations++;
    kmalloc_state.total_bytes_allocated += cache->size;
    
    // Move slab to appropriate list
    if (slab->free_count == 0) {
        kmalloc_remove_slab_from_list(&cache->partial_slabs, slab);
        kmalloc_add_slab_to_list(&cache->full_slabs, slab);
    }
    
    return ptr;
}

void kfree(void* ptr) {
    if (!kmalloc_state.initialized || ptr == NULL) {
        return;
    }
    
    // Find which slab this pointer belongs to
    slab_t* slab = NULL;
    kmalloc_cache_t* cache = NULL;
    
    // This is a simplified implementation
    // In a real system, we would need to track which slab each pointer belongs to
    for (int i = 0; i < KMALLOC_CACHE_COUNT; i++) {
        cache = &kmalloc_state.caches[i];
        
        // Check if pointer is within heap range
        if ((uintptr_t)ptr >= (uintptr_t)kmalloc_state.heap_start &&
            (uintptr_t)ptr < (uintptr_t)kmalloc_state.heap_end) {
            
            // Find slab (simplified)
            slab = (slab_t*)((uintptr_t)ptr & ~(KMALLOC_SLAB_SIZE - 1));
            break;
        }
    }
    
    if (slab == NULL || cache == NULL) {
        console_puts("KMALLOC: ERROR - Invalid pointer to free\n");
        return;
    }
    
    // Free from slab
    slab->free_count++;
    
    // Update statistics
    cache->total_deallocations++;
    kmalloc_state.total_deallocations++;
    kmalloc_state.total_bytes_freed += cache->size;
    
    // Move slab to appropriate list
    if (slab->free_count == slab->total_count) {
        kmalloc_remove_slab_from_list(&cache->partial_slabs, slab);
        kmalloc_remove_slab_from_list(&cache->full_slabs, slab);
        kmalloc_add_slab_to_list(&cache->free_slabs, slab);
    } else if (slab->free_count == 1) {
        kmalloc_remove_slab_from_list(&cache->full_slabs, slab);
        kmalloc_add_slab_to_list(&cache->partial_slabs, slab);
    }
}

void* krealloc(void* ptr, uint32_t new_size) {
    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }
    
    if (ptr == NULL) {
        return kmalloc(new_size);
    }
    
    // Allocate new memory
    void* new_ptr = kmalloc(new_size);
    if (new_ptr == NULL) {
        return NULL;
    }
    
    // Copy old data (simplified - we don't know the old size)
    memcpy(new_ptr, ptr, new_size);
    
    // Free old memory
    kfree(ptr);
    
    return new_ptr;
}

void* kcalloc(uint32_t num, uint32_t size) {
    uint32_t total_size = num * size;
    void* ptr = kmalloc(total_size);
    
    if (ptr != NULL) {
        memset(ptr, 0, total_size);
    }
    
    return ptr;
}

// =============================================================================
// KMALLOC QUERY FUNCTIONS
// =============================================================================

uint32_t kmalloc_get_total_allocations(void) {
    return kmalloc_state.total_allocations;
}

uint32_t kmalloc_get_total_deallocations(void) {
    return kmalloc_state.total_deallocations;
}

uint32_t kmalloc_get_total_bytes_allocated(void) {
    return kmalloc_state.total_bytes_allocated;
}

uint32_t kmalloc_get_total_bytes_freed(void) {
    return kmalloc_state.total_bytes_freed;
}

uint32_t kmalloc_get_heap_start(void) {
    return kmalloc_state.heap_start;
}

uint32_t kmalloc_get_heap_end(void) {
    return kmalloc_state.heap_end;
}

uint32_t kmalloc_get_heap_size(void) {
    return kmalloc_state.heap_size;
}

bool kmalloc_is_initialized(void) {
    return kmalloc_state.initialized;
}

// =============================================================================
// KMALLOC DEBUG FUNCTIONS
// =============================================================================

void kmalloc_print_statistics(void) {
    if (!kmalloc_state.initialized) {
        console_puts("KMALLOC: Not initialized\n");
        return;
    }
    
    console_puts("KMALLOC Statistics:\n");
    console_puts("  Total allocations: [count]\n");
    console_puts("  Total deallocations: [count]\n");
    console_puts("  Total bytes allocated: [bytes]\n");
    console_puts("  Total bytes freed: [bytes]\n");
    console_puts("  Heap start: 0x[addr]\n");
    console_puts("  Heap end: 0x[addr]\n");
    console_puts("  Heap size: [size] bytes\n");
    
    console_puts("  Cache statistics:\n");
    for (int i = 0; i < KMALLOC_CACHE_COUNT; i++) {
        kmalloc_cache_t* cache = &kmalloc_state.caches[i];
        if (cache->total_slabs > 0) {
            console_puts("    Size [size]: [slabs] slabs, [allocs] allocs, [frees] frees\n");
        }
    }
}

void kmalloc_print_cache_info(uint32_t size) {
    if (!kmalloc_state.initialized) {
        console_puts("KMALLOC: Not initialized\n");
        return;
    }
    
    kmalloc_cache_t* cache = kmalloc_get_cache(size);
    if (cache == NULL) {
        console_puts("KMALLOC: No cache found for size [size]\n");
        return;
    }
    
    console_puts("KMALLOC Cache Info for size [size]:\n");
    console_puts("  Free slabs: [count]\n");
    console_puts("  Partial slabs: [count]\n");
    console_puts("  Full slabs: [count]\n");
    console_puts("  Total slabs: [count]\n");
    console_puts("  Total allocations: [count]\n");
    console_puts("  Total deallocations: [count]\n");
}
