/**
 * @file ternary_debugger.c
 * @brief Ternary debugger implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_debugger.h"
#include "trit.h"
#include "t3_isa.h"
#include "tvm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TERNARY DEBUGGER IMPLEMENTATION
// =============================================================================

ternary_debugger_t* ternary_debugger_create(void) {
    ternary_debugger_t* debugger = malloc(sizeof(ternary_debugger_t));
    if (debugger == NULL) return NULL;
    
    debugger->vm = NULL;
    debugger->breakpoints = NULL;
    debugger->breakpoint_count = 0;
    debugger->breakpoint_capacity = 0;
    debugger->running = false;
    debugger->stepping = false;
    debugger->error = false;
    debugger->error_message = NULL;
    
    return debugger;
}

void ternary_debugger_destroy(ternary_debugger_t* debugger) {
    if (debugger != NULL) {
        if (debugger->breakpoints != NULL) {
            free(debugger->breakpoints);
        }
        if (debugger->error_message != NULL) {
            free(debugger->error_message);
        }
        free(debugger);
    }
}

// =============================================================================
// TERNARY DEBUGGER VM MANAGEMENT
// =============================================================================

bool ternary_debugger_attach_vm(ternary_debugger_t* debugger, tvm_t* vm) {
    if (debugger == NULL || vm == NULL) {
        return false;
    }
    
    debugger->vm = vm;
    return true;
}

void ternary_debugger_detach_vm(ternary_debugger_t* debugger) {
    if (debugger != NULL) {
        debugger->vm = NULL;
    }
}

tvm_t* ternary_debugger_get_vm(ternary_debugger_t* debugger) {
    if (debugger == NULL) return NULL;
    return debugger->vm;
}

// =============================================================================
// TERNARY DEBUGGER EXECUTION CONTROL
// =============================================================================

bool ternary_debugger_start(ternary_debugger_t* debugger) {
    if (debugger == NULL || debugger->vm == NULL) {
        return false;
    }
    
    debugger->running = true;
    debugger->stepping = false;
    debugger->error = false;
    
    return true;
}

void ternary_debugger_stop(ternary_debugger_t* debugger) {
    if (debugger == NULL) return;
    
    debugger->running = false;
    debugger->stepping = false;
    
    if (debugger->vm != NULL) {
        tvm_halt(debugger->vm);
    }
}

bool ternary_debugger_step(ternary_debugger_t* debugger) {
    if (debugger == NULL || debugger->vm == NULL) {
        return false;
    }
    
    if (!debugger->running) {
        return false;
    }
    
    // Check for breakpoints
    int pc = trit_to_int(tvm_get_register(debugger->vm, T3_REGISTER_PC));
    if (ternary_debugger_is_breakpoint(debugger, pc)) {
        debugger->stepping = true;
        return true;
    }
    
    // Execute one instruction
    // This is a simplified version - in a real implementation,
    // we would need to decode and execute the instruction
    int new_pc = pc + 1;
    tvm_set_register(debugger->vm, T3_REGISTER_PC, trit_create(new_pc));
    
    return true;
}

bool ternary_debugger_continue(ternary_debugger_t* debugger) {
    if (debugger == NULL || debugger->vm == NULL) {
        return false;
    }
    
    if (!debugger->running) {
        return false;
    }
    
    debugger->stepping = false;
    
    // Continue execution until breakpoint or halt
    while (debugger->running && !debugger->stepping) {
        int pc = trit_to_int(tvm_get_register(debugger->vm, T3_REGISTER_PC));
        
        if (ternary_debugger_is_breakpoint(debugger, pc)) {
            debugger->stepping = true;
            break;
        }
        
        // Execute one instruction
        int new_pc = pc + 1;
        tvm_set_register(debugger->vm, T3_REGISTER_PC, trit_create(new_pc));
        
        // Check if VM is halted
        if (tvm_is_halted(debugger->vm)) {
            debugger->running = false;
            break;
        }
    }
    
    return true;
}

// =============================================================================
// TERNARY DEBUGGER BREAKPOINT MANAGEMENT
// =============================================================================

bool ternary_debugger_set_breakpoint(ternary_debugger_t* debugger, int address) {
    if (debugger == NULL || address < 0) {
        return false;
    }
    
    // Check if breakpoint already exists
    if (ternary_debugger_is_breakpoint(debugger, address)) {
        return true;
    }
    
    // Resize breakpoint array if needed
    if (debugger->breakpoint_count >= debugger->breakpoint_capacity) {
        size_t new_capacity = debugger->breakpoint_capacity == 0 ? 16 : debugger->breakpoint_capacity * 2;
        int* new_breakpoints = realloc(debugger->breakpoints, new_capacity * sizeof(int));
        if (new_breakpoints == NULL) {
            return false;
        }
        debugger->breakpoints = new_breakpoints;
        debugger->breakpoint_capacity = new_capacity;
    }
    
    // Add breakpoint
    debugger->breakpoints[debugger->breakpoint_count] = address;
    debugger->breakpoint_count++;
    
    return true;
}

bool ternary_debugger_clear_breakpoint(ternary_debugger_t* debugger, int address) {
    if (debugger == NULL || address < 0) {
        return false;
    }
    
    // Find and remove breakpoint
    for (size_t i = 0; i < debugger->breakpoint_count; i++) {
        if (debugger->breakpoints[i] == address) {
            // Shift remaining breakpoints
            for (size_t j = i; j < debugger->breakpoint_count - 1; j++) {
                debugger->breakpoints[j] = debugger->breakpoints[j + 1];
            }
            debugger->breakpoint_count--;
            return true;
        }
    }
    
    return false;
}

bool ternary_debugger_is_breakpoint(ternary_debugger_t* debugger, int address) {
    if (debugger == NULL || address < 0) {
        return false;
    }
    
    for (size_t i = 0; i < debugger->breakpoint_count; i++) {
        if (debugger->breakpoints[i] == address) {
            return true;
        }
    }
    
    return false;
}

void ternary_debugger_clear_all_breakpoints(ternary_debugger_t* debugger) {
    if (debugger == NULL) return;
    
    debugger->breakpoint_count = 0;
}

// =============================================================================
// TERNARY DEBUGGER REGISTER OPERATIONS
// =============================================================================

trit_t ternary_debugger_get_register(ternary_debugger_t* debugger, int register_index) {
    if (debugger == NULL || debugger->vm == NULL || register_index < 0 || register_index >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return tvm_get_register(debugger->vm, register_index);
}

void ternary_debugger_set_register(ternary_debugger_t* debugger, int register_index, trit_t value) {
    if (debugger == NULL || debugger->vm == NULL || register_index < 0 || register_index >= T3_REGISTER_COUNT) {
        return;
    }
    
    tvm_set_register(debugger->vm, register_index, value);
}

void ternary_debugger_print_registers(ternary_debugger_t* debugger) {
    if (debugger == NULL || debugger->vm == NULL) {
        printf("Debugger Registers: NULL\n");
        return;
    }
    
    printf("Debugger Registers:\n");
    for (int i = 0; i < T3_REGISTER_COUNT; i++) {
        trit_t value = tvm_get_register(debugger->vm, i);
        printf("  R%d: %s\n", i, trit_to_string(value));
    }
}

// =============================================================================
// TERNARY DEBUGGER MEMORY OPERATIONS
// =============================================================================

trit_t ternary_debugger_read_memory(ternary_debugger_t* debugger, size_t address) {
    if (debugger == NULL || debugger->vm == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return tvm_memory_read(debugger->vm, address);
}

void ternary_debugger_write_memory(ternary_debugger_t* debugger, size_t address, trit_t value) {
    if (debugger == NULL || debugger->vm == NULL) {
        return;
    }
    
    tvm_memory_write(debugger->vm, address, value);
}

void ternary_debugger_print_memory(ternary_debugger_t* debugger, size_t start_address, size_t count) {
    if (debugger == NULL || debugger->vm == NULL) {
        printf("Debugger Memory: NULL\n");
        return;
    }
    
    printf("Debugger Memory [%zu-%zu]:\n", start_address, start_address + count - 1);
    for (size_t i = 0; i < count; i++) {
        trit_t value = tvm_memory_read(debugger->vm, start_address + i);
        printf("  [%zu]: %s\n", start_address + i, trit_to_string(value));
    }
}

// =============================================================================
// TERNARY DEBUGGER STATUS OPERATIONS
// =============================================================================

bool ternary_debugger_is_running(ternary_debugger_t* debugger) {
    return debugger != NULL && debugger->running;
}

bool ternary_debugger_is_stepping(ternary_debugger_t* debugger) {
    return debugger != NULL && debugger->stepping;
}

bool ternary_debugger_has_error(ternary_debugger_t* debugger) {
    return debugger != NULL && debugger->error;
}

int ternary_debugger_get_pc(ternary_debugger_t* debugger) {
    if (debugger == NULL || debugger->vm == NULL) {
        return -1;
    }
    
    trit_t pc = tvm_get_register(debugger->vm, T3_REGISTER_PC);
    return trit_to_int(pc);
}

void ternary_debugger_set_pc(ternary_debugger_t* debugger, int pc) {
    if (debugger == NULL || debugger->vm == NULL || pc < 0) {
        return;
    }
    
    tvm_set_register(debugger->vm, T3_REGISTER_PC, trit_create(pc));
}

// =============================================================================
// TERNARY DEBUGGER ERROR HANDLING
// =============================================================================

const char* ternary_debugger_get_error_message(ternary_debugger_t* debugger) {
    if (debugger == NULL) return NULL;
    return debugger->error_message;
}

void ternary_debugger_set_error(ternary_debugger_t* debugger, const char* message) {
    if (debugger == NULL) return;
    
    debugger->error = true;
    if (debugger->error_message != NULL) {
        free(debugger->error_message);
    }
    debugger->error_message = message != NULL ? strdup(message) : NULL;
}

// =============================================================================
// TERNARY DEBUGGER UTILITY FUNCTIONS
// =============================================================================

void ternary_debugger_print_status(ternary_debugger_t* debugger) {
    if (debugger == NULL) {
        printf("Ternary Debugger Status: NULL\n");
        return;
    }
    
    printf("Ternary Debugger Status:\n");
    printf("  Running: %s\n", debugger->running ? "true" : "false");
    printf("  Stepping: %s\n", debugger->stepping ? "true" : "false");
    printf("  Error: %s\n", debugger->error ? "true" : "false");
    printf("  Breakpoints: %zu\n", debugger->breakpoint_count);
    printf("  PC: %d\n", ternary_debugger_get_pc(debugger));
}

void ternary_debugger_debug(ternary_debugger_t* debugger) {
    if (debugger == NULL) {
        printf("Ternary Debugger Debug: NULL\n");
        return;
    }
    
    printf("Ternary Debugger Debug:\n");
    printf("  Running: %s\n", debugger->running ? "true" : "false");
    printf("  Stepping: %s\n", debugger->stepping ? "true" : "false");
    printf("  Error: %s\n", debugger->error ? "true" : "false");
    printf("  Error Message: %s\n", debugger->error_message != NULL ? debugger->error_message : "None");
    printf("  Breakpoint Count: %zu\n", debugger->breakpoint_count);
    printf("  Breakpoint Capacity: %zu\n", debugger->breakpoint_capacity);
    printf("  PC: %d\n", ternary_debugger_get_pc(debugger));
    
    printf("  Breakpoints: ");
    for (size_t i = 0; i < debugger->breakpoint_count; i++) {
        printf("%d ", debugger->breakpoints[i]);
    }
    printf("\n");
}
