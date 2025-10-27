/**
 * @file pmm.c
 * @brief Physical Memory Manager implementation
 */

#include "pmm.h"
#include <stddef.h>

// PMM global state
static pmm_state_t pmm_state = {0};

// Helper macros
#define ADDR_TO_FRAME(addr) ((addr) / PMM_PAGE_SIZE)
#define FRAME_TO_ADDR(frame) ((frame) * PMM_PAGE_SIZE)

// Bitmap functions
static inline void pmm_set_bit(uint32_t bit) {
    pmm_state.bitmap[bit / PMM_BITS_PER_INT] |= (1 << (bit % PMM_BITS_PER_INT));
}

static inline void pmm_clear_bit(uint32_t bit) {
    pmm_state.bitmap[bit / PMM_BITS_PER_INT] &= ~(1 << (bit % PMM_BITS_PER_INT));
}

static inline bool pmm_test_bit(uint32_t bit) {
    return (pmm_state.bitmap[bit / PMM_BITS_PER_INT] & (1 << (bit % PMM_BITS_PER_INT))) != 0;
}

void pmm_init(uint32_t mem_upper) {
    // Calculate total frames (mem_upper is in KB)
    uint32_t total_mem_kb = mem_upper;
    uint32_t total_mem_bytes = total_mem_kb * 1024;
    pmm_state.total_frames = total_mem_bytes / PMM_PAGE_SIZE;
    
    // Calculate bitmap size
    pmm_state.bitmap_size = (pmm_state.total_frames + PMM_BITS_PER_INT - 1) / PMM_BITS_PER_INT;
    
    // Allocate bitmap (using first available pages)
    pmm_state.bitmap = (uint32_t*)0x100000; // Simple hardcoded location for now
    
    // Initialize bitmap (all pages free by default)
    for (uint32_t i = 0; i < pmm_state.bitmap_size; i++) {
        pmm_state.bitmap[i] = 0;
    }
    
    // Mark pages 0-1023 as used (0-4MB used for kernel and bitmap)
    for (uint32_t i = 0; i < 1024; i++) {
        pmm_set_bit(i);
    }
    
    // Mark bitmap pages as used
    for (uint32_t i = 0; i < pmm_state.bitmap_size; i++) {
        pmm_set_bit(i);
    }
    
    // Initialize region list
    pmm_state.regions = NULL;
    pmm_state.region_count = 0;
    
    // Calculate free frames
    pmm_state.free_frames = pmm_state.total_frames - 1024 - pmm_state.bitmap_size;
    pmm_state.used_frames = 1024 + pmm_state.bitmap_size;
    
    pmm_state.initialized = true;
}

uint32_t pmm_alloc_page(void) {
    if (!pmm_state.initialized) {
        return 0;
    }
    
    // Search for free frame
    for (uint32_t i = 0; i < pmm_state.total_frames; i++) {
        if (!pmm_test_bit(i)) {
            // Found free frame, mark as allocated
            pmm_set_bit(i);
            pmm_state.free_frames--;
            pmm_state.used_frames++;
            return FRAME_TO_ADDR(i);
        }
    }
    
    // No free frames available
    return 0;
}

void pmm_free_page(uint32_t phys_addr) {
    if (!pmm_state.initialized || phys_addr == 0) {
        return;
    }
    
    uint32_t frame = ADDR_TO_FRAME(phys_addr);
    
    if (frame >= pmm_state.total_frames) {
        return;
    }
    
    if (pmm_test_bit(frame)) {
        pmm_clear_bit(frame);
        pmm_state.free_frames++;
        pmm_state.used_frames--;
    }
}

uint32_t pmm_alloc_pages(uint32_t count) {
    if (!pmm_state.initialized || count == 0) {
        return 0;
    }
    
    // Find contiguous free frames
    uint32_t consecutive = 0;
    uint32_t start_frame = 0;
    
    for (uint32_t i = 0; i < pmm_state.total_frames; i++) {
        if (!pmm_test_bit(i)) {
            if (consecutive == 0) {
                start_frame = i;
            }
            consecutive++;
            
            if (consecutive == count) {
                // Found enough contiguous frames, allocate them
                for (uint32_t j = 0; j < count; j++) {
                    pmm_set_bit(start_frame + j);
                }
                pmm_state.free_frames -= count;
                pmm_state.used_frames += count;
                return FRAME_TO_ADDR(start_frame);
            }
        } else {
            consecutive = 0;
        }
    }
    
    // Not enough contiguous frames
    return 0;
}

void pmm_free_pages(uint32_t phys_addr, uint32_t count) {
    if (!pmm_state.initialized || phys_addr == 0 || count == 0) {
        return;
    }
    
    uint32_t start_frame = ADDR_TO_FRAME(phys_addr);
    
    if (start_frame + count > pmm_state.total_frames) {
        return;
    }
    
    for (uint32_t i = 0; i < count; i++) {
        pmm_clear_bit(start_frame + i);
        pmm_state.free_frames++;
        pmm_state.used_frames--;
    }
}

void pmm_mark_used(uint32_t phys_addr) {
    uint32_t frame = ADDR_TO_FRAME(phys_addr);
    if (frame < pmm_state.total_frames && !pmm_test_bit(frame)) {
        pmm_set_bit(frame);
        pmm_state.free_frames--;
        pmm_state.used_frames++;
    }
}

void pmm_mark_free(uint32_t phys_addr) {
    uint32_t frame = ADDR_TO_FRAME(phys_addr);
    if (frame < pmm_state.total_frames && pmm_test_bit(frame)) {
        pmm_clear_bit(frame);
        pmm_state.free_frames++;
        pmm_state.used_frames--;
    }
}

bool pmm_is_allocated(uint32_t phys_addr) {
    uint32_t frame = ADDR_TO_FRAME(phys_addr);
    return pmm_test_bit(frame);
}

uint32_t pmm_get_total_frames(void) {
    return pmm_state.total_frames;
}

uint32_t pmm_get_free_frames(void) {
    return pmm_state.free_frames;
}

uint32_t pmm_get_used_frames(void) {
    return pmm_state.used_frames;
}

uint32_t pmm_get_usage_percent(void) {
    return pmm_state.total_frames == 0 ? 0 : 
        (pmm_state.used_frames * 100) / pmm_state.total_frames;
}

pmm_state_t* pmm_get_state(void) {
    return &pmm_state;
}

void pmm_print_stats(void) {
    // TODO: Implement with console output
    // This will be integrated with console driver later
}

