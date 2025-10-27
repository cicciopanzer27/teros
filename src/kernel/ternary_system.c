/**
 * @file ternary_system.c
 * @brief Ternary system integration implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =============================================================================
// TERNARY SYSTEM IMPLEMENTATION
// =============================================================================

ternary_system_t* ternary_system_create(size_t memory_size) {
    if (memory_size == 0) {
        memory_size = TERNARY_SYSTEM_DEFAULT_MEMORY_SIZE;
    }
    
    ternary_system_t* system = malloc(sizeof(ternary_system_t));
    if (system == NULL) return NULL;
    
    system->vm = tvm_create(memory_size);
    if (system->vm == NULL) {
        free(system);
        return NULL;
    }
    
    system->memory = ternary_memory_create(memory_size);
    if (system->memory == NULL) {
        tvm_destroy(system->vm);
        free(system);
        return NULL;
    }
    
    // Initialize registers
    for (int i = 0; i < T3_REGISTER_COUNT; i++) {
        system->registers[i] = trit_create(TERNARY_NEUTRAL);
    }
    
    system->initialized = false;
    system->running = false;
    system->error = false;
    
    return system;
}

void ternary_system_destroy(ternary_system_t* system) {
    if (system != NULL) {
        if (system->vm != NULL) {
            tvm_destroy(system->vm);
        }
        if (system->memory != NULL) {
            ternary_memory_destroy(system->memory);
        }
        free(system);
    }
}

// =============================================================================
// TERNARY SYSTEM INITIALIZATION
// =============================================================================

bool ternary_system_init(ternary_system_t* system) {
    if (system == NULL) return false;
    
    // Initialize VM
    if (!tvm_is_running(system->vm)) {
        tvm_reset(system->vm);
    }
    
    // Initialize memory
    ternary_memory_clear(system->memory, 0, system->memory->size);
    
    // Initialize registers
    for (int i = 0; i < T3_REGISTER_COUNT; i++) {
        system->registers[i] = trit_create(TERNARY_NEUTRAL);
    }
    
    system->initialized = true;
    system->running = false;
    system->error = false;
    
    return true;
}

void ternary_system_shutdown(ternary_system_t* system) {
    if (system == NULL) return;
    
    system->running = false;
    system->initialized = false;
    
    if (system->vm != NULL) {
        tvm_halt(system->vm);
    }
}

// =============================================================================
// TERNARY SYSTEM EXECUTION
// =============================================================================

trit_t ternary_system_run(ternary_system_t* system) {
    if (system == NULL || !system->initialized) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    system->running = true;
    system->error = false;
    
    // Run the VM
    trit_t result = tvm_run(system->vm);
    
    if (!trit_is_valid(result)) {
        system->error = true;
    }
    
    system->running = false;
    return result;
}

void ternary_system_halt(ternary_system_t* system) {
    if (system == NULL) return;
    
    system->running = false;
    
    if (system->vm != NULL) {
        tvm_halt(system->vm);
    }
}

void ternary_system_reset(ternary_system_t* system) {
    if (system == NULL) return;
    
    system->running = false;
    system->error = false;
    
    if (system->vm != NULL) {
        tvm_reset(system->vm);
    }
    
    if (system->memory != NULL) {
        ternary_memory_clear(system->memory, 0, system->memory->size);
    }
    
    // Reset registers
    for (int i = 0; i < T3_REGISTER_COUNT; i++) {
        system->registers[i] = trit_create(TERNARY_NEUTRAL);
    }
}

// =============================================================================
// TERNARY SYSTEM STATUS
// =============================================================================

bool ternary_system_is_initialized(ternary_system_t* system) {
    return system != NULL && system->initialized;
}

bool ternary_system_is_running(ternary_system_t* system) {
    return system != NULL && system->running;
}

bool ternary_system_has_error(ternary_system_t* system) {
    return system != NULL && system->error;
}

// =============================================================================
// TERNARY SYSTEM MEMORY OPERATIONS
// =============================================================================

ternary_memory_t* ternary_system_get_memory(ternary_system_t* system) {
    if (system == NULL) return NULL;
    return system->memory;
}

tvm_t* ternary_system_get_vm(ternary_system_t* system) {
    if (system == NULL) return NULL;
    return system->vm;
}

// =============================================================================
// TERNARY SYSTEM UTILITY FUNCTIONS
// =============================================================================

void ternary_system_print_status(ternary_system_t* system) {
    if (system == NULL) {
        printf("Ternary System Status: NULL\n");
        return;
    }
    
    printf("Ternary System Status:\n");
    printf("  Initialized: %s\n", system->initialized ? "true" : "false");
    printf("  Running: %s\n", system->running ? "true" : "false");
    printf("  Error: %s\n", system->error ? "true" : "false");
    
    if (system->vm != NULL) {
        printf("  VM Status: ");
        tvm_print_status(system->vm);
    }
    
    if (system->memory != NULL) {
        printf("  Memory Size: %zu\n", system->memory->size);
        printf("  Memory Used: %zu\n", system->memory->used);
    }
}

void ternary_system_debug(ternary_system_t* system) {
    if (system == NULL) {
        printf("Ternary System Debug: NULL\n");
        return;
    }
    
    printf("Ternary System Debug:\n");
    printf("  Initialized: %s\n", system->initialized ? "true" : "false");
    printf("  Running: %s\n", system->running ? "true" : "false");
    printf("  Error: %s\n", system->error ? "true" : "false");
    
    if (system->vm != NULL) {
        printf("  VM Debug:\n");
        tvm_debug(system->vm);
    }
    
    if (system->memory != NULL) {
        printf("  Memory Debug:\n");
        ternary_memory_debug(system->memory);
    }
    
    printf("  Registers:\n");
    for (int i = 0; i < T3_REGISTER_COUNT; i++) {
        printf("    R%d: %s\n", i, trit_to_string(system->registers[i]));
    }
}

const char* ternary_system_get_version(void) {
    static char version[32];
    snprintf(version, sizeof(version), "%d.%d.%d", 
             TERNARY_SYSTEM_VERSION_MAJOR,
             TERNARY_SYSTEM_VERSION_MINOR,
             TERNARY_SYSTEM_VERSION_PATCH);
    return version;
}
