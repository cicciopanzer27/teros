/**
 * @file ternary_memory.c
 * @brief Ternary memory management implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_memory.h"
#include "trit.h"
#include "trit_array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TERNARY MEMORY IMPLEMENTATION
// =============================================================================

ternary_memory_t* ternary_memory_create(size_t size) {
    if (size == 0) return NULL;
    
    ternary_memory_t* memory = malloc(sizeof(ternary_memory_t));
    if (memory == NULL) return NULL;
    
    memory->data = malloc(size * sizeof(trit_t));
    if (memory->data == NULL) {
        free(memory);
        return NULL;
    }
    
    memory->size = size;
    memory->used = 0;
    memory->read_only = false;
    
    // Initialize memory to neutral
    for (size_t i = 0; i < size; i++) {
        memory->data[i] = trit_create(TERNARY_NEUTRAL);
    }
    
    return memory;
}

void ternary_memory_destroy(ternary_memory_t* memory) {
    if (memory != NULL) {
        if (memory->data != NULL) {
            free(memory->data);
        }
        free(memory);
    }
}

// =============================================================================
// TERNARY MEMORY ACCESS OPERATIONS
// =============================================================================

trit_t ternary_memory_read(ternary_memory_t* memory, size_t address) {
    if (memory == NULL || memory->data == NULL || address >= memory->size) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return memory->data[address];
}

void ternary_memory_write(ternary_memory_t* memory, size_t address, trit_t value) {
    if (memory == NULL || memory->data == NULL || address >= memory->size || memory->read_only) {
        return;
    }
    
    memory->data[address] = value;
}

bool ternary_memory_is_valid_address(ternary_memory_t* memory, size_t address) {
    return memory != NULL && memory->data != NULL && address < memory->size;
}

// =============================================================================
// TERNARY MEMORY BULK OPERATIONS
// =============================================================================

bool ternary_memory_read_block(ternary_memory_t* memory, size_t address, trit_t* buffer, size_t count) {
    if (memory == NULL || memory->data == NULL || buffer == NULL || count == 0) {
        return false;
    }
    
    if (address + count > memory->size) {
        return false;
    }
    
    for (size_t i = 0; i < count; i++) {
        buffer[i] = memory->data[address + i];
    }
    
    return true;
}

bool ternary_memory_write_block(ternary_memory_t* memory, size_t address, const trit_t* buffer, size_t count) {
    if (memory == NULL || memory->data == NULL || buffer == NULL || count == 0 || memory->read_only) {
        return false;
    }
    
    if (address + count > memory->size) {
        return false;
    }
    
    for (size_t i = 0; i < count; i++) {
        memory->data[address + i] = buffer[i];
    }
    
    return true;
}

// =============================================================================
// TERNARY MEMORY ARRAY OPERATIONS
// =============================================================================

trit_array_t* ternary_memory_read_array(ternary_memory_t* memory, size_t address, size_t count) {
    if (memory == NULL || address + count > memory->size) {
        return NULL;
    }
    
    trit_array_t* array = trit_array_create(count);
    if (array == NULL) return NULL;
    
    for (size_t i = 0; i < count; i++) {
        array->trits[i] = memory->data[address + i];
    }
    
    return array;
}

bool ternary_memory_write_array(ternary_memory_t* memory, size_t address, trit_array_t* array) {
    if (memory == NULL || array == NULL || memory->read_only) {
        return false;
    }
    
    if (address + array->size > memory->size) {
        return false;
    }
    
    for (size_t i = 0; i < array->size; i++) {
        memory->data[address + i] = array->trits[i];
    }
    
    return true;
}

// =============================================================================
// TERNARY MEMORY SEARCH OPERATIONS
// =============================================================================

size_t ternary_memory_find(ternary_memory_t* memory, trit_t value, size_t start_address) {
    if (memory == NULL || memory->data == NULL || start_address >= memory->size) {
        return SIZE_MAX;
    }
    
    for (size_t i = start_address; i < memory->size; i++) {
        if (trit_equal(memory->data[i], value).value == 1) {
            return i;
        }
    }
    
    return SIZE_MAX;
}

size_t ternary_memory_find_array(ternary_memory_t* memory, trit_array_t* pattern, size_t start_address) {
    if (memory == NULL || memory->data == NULL || pattern == NULL || start_address >= memory->size) {
        return SIZE_MAX;
    }
    
    for (size_t i = start_address; i <= memory->size - pattern->size; i++) {
        bool found = true;
        for (size_t j = 0; j < pattern->size; j++) {
            if (trit_equal(memory->data[i + j], pattern->trits[j]).value != 1) {
                found = false;
                break;
            }
        }
        if (found) {
            return i;
        }
    }
    
    return SIZE_MAX;
}

// =============================================================================
// TERNARY MEMORY COPY OPERATIONS
// =============================================================================

bool ternary_memory_copy(ternary_memory_t* source, ternary_memory_t* dest, size_t source_address, size_t dest_address, size_t count) {
    if (source == NULL || dest == NULL || source->data == NULL || dest->data == NULL) {
        return false;
    }
    
    if (source_address + count > source->size || dest_address + count > dest->size) {
        return false;
    }
    
    for (size_t i = 0; i < count; i++) {
        dest->data[dest_address + i] = source->data[source_address + i];
    }
    
    return true;
}

bool ternary_memory_move(ternary_memory_t* memory, size_t source_address, size_t dest_address, size_t count) {
    if (memory == NULL || memory->data == NULL || memory->read_only) {
        return false;
    }
    
    if (source_address + count > memory->size || dest_address + count > memory->size) {
        return false;
    }
    
    // Handle overlapping regions
    if (source_address < dest_address) {
        // Move from end to beginning
        for (int i = count - 1; i >= 0; i--) {
            memory->data[dest_address + i] = memory->data[source_address + i];
        }
    } else {
        // Move from beginning to end
        for (size_t i = 0; i < count; i++) {
            memory->data[dest_address + i] = memory->data[source_address + i];
        }
    }
    
    return true;
}

// =============================================================================
// TERNARY MEMORY FILL OPERATIONS
// =============================================================================

void ternary_memory_fill(ternary_memory_t* memory, trit_t value, size_t start_address, size_t count) {
    if (memory == NULL || memory->data == NULL || memory->read_only) {
        return;
    }
    
    if (start_address + count > memory->size) {
        count = memory->size - start_address;
    }
    
    for (size_t i = 0; i < count; i++) {
        memory->data[start_address + i] = value;
    }
}

void ternary_memory_clear(ternary_memory_t* memory, size_t start_address, size_t count) {
    ternary_memory_fill(memory, trit_create(TERNARY_NEUTRAL), start_address, count);
}

// =============================================================================
// TERNARY MEMORY STATUS OPERATIONS
// =============================================================================

size_t ternary_memory_size(ternary_memory_t* memory) {
    if (memory == NULL) return 0;
    return memory->size;
}

size_t ternary_memory_used(ternary_memory_t* memory) {
    if (memory == NULL) return 0;
    return memory->used;
}

size_t ternary_memory_free(ternary_memory_t* memory) {
    if (memory == NULL) return 0;
    return memory->size - memory->used;
}

bool ternary_memory_is_read_only(ternary_memory_t* memory) {
    return memory != NULL && memory->read_only;
}

void ternary_memory_set_read_only(ternary_memory_t* memory, bool read_only) {
    if (memory != NULL) {
        memory->read_only = read_only;
    }
}

// =============================================================================
// TERNARY MEMORY UTILITY FUNCTIONS
// =============================================================================

void ternary_memory_print(ternary_memory_t* memory, size_t start_address, size_t count) {
    // DEBUG: printf version disabled (requires printf)
    if (memory == NULL || memory->data == NULL) {
        return;
    }
    // Would print memory range [start_address, start_address+count-1]
    (void)start_address;
    (void)count;
}

void ternary_memory_debug(ternary_memory_t* memory) {
    // DEBUG: printf version disabled (requires printf)
    if (memory == NULL) {
        return;
    }
    // Would print size, used, free, read_only, and first 16 words
    (void)memory;
}

// =============================================================================
// TERNARY MEMORY ALLOCATION OPERATIONS
// =============================================================================

size_t ternary_memory_allocate(ternary_memory_t* memory, size_t size) {
    if (memory == NULL || size == 0) {
        return SIZE_MAX;
    }
    
    // Simple allocation - find first free block
    for (size_t i = 0; i <= memory->size - size; i++) {
        bool free = true;
        for (size_t j = 0; j < size; j++) {
            if (!trit_is_neutral(memory->data[i + j])) {
                free = false;
                break;
            }
        }
        if (free) {
            // Mark as allocated
            for (size_t j = 0; j < size; j++) {
                memory->data[i + j] = trit_create(TERNARY_POSITIVE);
            }
            memory->used += size;
            return i;
        }
    }
    
    return SIZE_MAX;
}

void ternary_memory_deallocate(ternary_memory_t* memory, size_t address, size_t size) {
    if (memory == NULL || address >= memory->size || address + size > memory->size) {
        return;
    }
    
    // Mark as free
    for (size_t i = 0; i < size; i++) {
        memory->data[address + i] = trit_create(TERNARY_NEUTRAL);
    }
    
    if (memory->used >= size) {
        memory->used -= size;
    } else {
        memory->used = 0;
    }
}
