/**
 * @file ternary_emulator.c
 * @brief Ternary emulator implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_emulator.h"
#include "trit.h"
#include "t3_isa.h"
#include "tvm.h"
#include "ternary_simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

// =============================================================================
// TERNARY EMULATOR IMPLEMENTATION
// =============================================================================

ternary_emulator_t* ternary_emulator_create(void) {
    ternary_emulator_t* emulator = malloc(sizeof(ternary_emulator_t));
    if (emulator == NULL) return NULL;
    
    emulator->simulator = ternary_simulator_create();
    if (emulator->simulator == NULL) {
        free(emulator);
        return NULL;
    }
    
    emulator->running = false;
    emulator->error = false;
    emulator->error_message = NULL;
    
    return emulator;
}

void ternary_emulator_destroy(ternary_emulator_t* emulator) {
    if (emulator != NULL) {
        if (emulator->simulator != NULL) {
            ternary_simulator_destroy(emulator->simulator);
        }
        if (emulator->error_message != NULL) {
            free(emulator->error_message);
        }
        free(emulator);
    }
}

// =============================================================================
// TERNARY EMULATOR EXECUTION CONTROL
// =============================================================================

bool ternary_emulator_start(ternary_emulator_t* emulator) {
    if (emulator == NULL) {
        return false;
    }
    
    emulator->running = true;
    emulator->error = false;
    
    // Start simulator
    if (!ternary_simulator_start(emulator->simulator)) {
        emulator->error = true;
        emulator->error_message = strdup("Failed to start simulator");
        return false;
    }
    
    return true;
}

void ternary_emulator_stop(ternary_emulator_t* emulator) {
    if (emulator == NULL) return;
    
    emulator->running = false;
    
    if (emulator->simulator != NULL) {
        ternary_simulator_stop(emulator->simulator);
    }
}

bool ternary_emulator_step(ternary_emulator_t* emulator) {
    if (emulator == NULL || !emulator->running) {
        return false;
    }
    
    // Step simulator
    if (!ternary_simulator_step(emulator->simulator)) {
        emulator->error = true;
        emulator->error_message = strdup("Simulator step failed");
        return false;
    }
    
    return true;
}

bool ternary_emulator_continue(ternary_emulator_t* emulator) {
    if (emulator == NULL || !emulator->running) {
        return false;
    }
    
    // Continue simulator
    if (!ternary_simulator_continue(emulator->simulator)) {
        emulator->error = true;
        emulator->error_message = strdup("Simulator continue failed");
        return false;
    }
    
    return true;
}

// =============================================================================
// TERNARY EMULATOR PROGRAM LOADING
// =============================================================================

bool ternary_emulator_load_program(ternary_emulator_t* emulator, t3_instruction_t* program, size_t program_size) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return false;
    }
    
    return ternary_simulator_load_program(emulator->simulator, program, program_size);
}

bool ternary_emulator_load_binary(ternary_emulator_t* emulator, const uint8_t* binary_data, size_t data_size) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return false;
    }
    
    return ternary_simulator_load_binary(emulator->simulator, binary_data, data_size);
}

// =============================================================================
// TERNARY EMULATOR BREAKPOINT MANAGEMENT
// =============================================================================

bool ternary_emulator_set_breakpoint(ternary_emulator_t* emulator, int address) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return false;
    }
    
    return ternary_simulator_set_breakpoint(emulator->simulator, address);
}

bool ternary_emulator_clear_breakpoint(ternary_emulator_t* emulator, int address) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return false;
    }
    
    return ternary_simulator_clear_breakpoint(emulator->simulator, address);
}

bool ternary_emulator_is_breakpoint(ternary_emulator_t* emulator, int address) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return false;
    }
    
    return ternary_simulator_is_breakpoint(emulator->simulator, address);
}

void ternary_emulator_clear_all_breakpoints(ternary_emulator_t* emulator) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return;
    }
    
    ternary_simulator_clear_all_breakpoints(emulator->simulator);
}

// =============================================================================
// TERNARY EMULATOR REGISTER OPERATIONS
// =============================================================================

trit_t ternary_emulator_get_register(ternary_emulator_t* emulator, int register_index) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return ternary_simulator_get_register(emulator->simulator, register_index);
}

void ternary_emulator_set_register(ternary_emulator_t* emulator, int register_index, trit_t value) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return;
    }
    
    ternary_simulator_set_register(emulator->simulator, register_index, value);
}

void ternary_emulator_print_registers(ternary_emulator_t* emulator) {
    if (emulator == NULL || emulator->simulator == NULL) {
        printf("Emulator Registers: NULL\n");
        return;
    }
    
    ternary_simulator_print_registers(emulator->simulator);
}

// =============================================================================
// TERNARY EMULATOR MEMORY OPERATIONS
// =============================================================================

trit_t ternary_emulator_read_memory(ternary_emulator_t* emulator, size_t address) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return ternary_simulator_read_memory(emulator->simulator, address);
}

void ternary_emulator_write_memory(ternary_emulator_t* emulator, size_t address, trit_t value) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return;
    }
    
    ternary_simulator_write_memory(emulator->simulator, address, value);
}

void ternary_emulator_print_memory(ternary_emulator_t* emulator, size_t start_address, size_t count) {
    if (emulator == NULL || emulator->simulator == NULL) {
        printf("Emulator Memory: NULL\n");
        return;
    }
    
    ternary_simulator_print_memory(emulator->simulator, start_address, count);
}

// =============================================================================
// TERNARY EMULATOR STATUS OPERATIONS
// =============================================================================

bool ternary_emulator_is_running(ternary_emulator_t* emulator) {
    return emulator != NULL && emulator->running;
}

bool ternary_emulator_is_stepping(ternary_emulator_t* emulator) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return false;
    }
    
    return ternary_simulator_is_stepping(emulator->simulator);
}

bool ternary_emulator_has_error(ternary_emulator_t* emulator) {
    return emulator != NULL && emulator->error;
}

int ternary_emulator_get_pc(ternary_emulator_t* emulator) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return -1;
    }
    
    return ternary_simulator_get_pc(emulator->simulator);
}

void ternary_emulator_set_pc(ternary_emulator_t* emulator, int pc) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return;
    }
    
    ternary_simulator_set_pc(emulator->simulator, pc);
}

// =============================================================================
// TERNARY EMULATOR PROFILING
// =============================================================================

uint64_t ternary_emulator_get_execution_time(ternary_emulator_t* emulator) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return 0;
    }
    
    // TODO: Implement without floating point
    // Can't call ternary_simulator_get_execution_time() because it returns double (SSE disabled in kernel)
    return 0;
}

uint64_t ternary_emulator_get_instructions_per_second(ternary_emulator_t* emulator) {
    if (emulator == NULL || emulator->simulator == NULL) {
        return 0;
    }
    
    // TODO: Implement without floating point
    // Can't call ternary_simulator_get_instructions_per_second() because it returns double (SSE disabled in kernel)
    return 0;
}

void ternary_emulator_print_profiling_summary(ternary_emulator_t* emulator) {
    if (emulator == NULL || emulator->simulator == NULL) {
        printf("Emulator Profiling: NULL\n");
        return;
    }
    
    ternary_simulator_print_profiling_summary(emulator->simulator);
}

// =============================================================================
// TERNARY EMULATOR ERROR HANDLING
// =============================================================================

const char* ternary_emulator_get_error_message(ternary_emulator_t* emulator) {
    if (emulator == NULL) return NULL;
    return emulator->error_message;
}

void ternary_emulator_set_error(ternary_emulator_t* emulator, const char* message) {
    if (emulator == NULL) return;
    
    emulator->error = true;
    if (emulator->error_message != NULL) {
        free(emulator->error_message);
    }
    emulator->error_message = message != NULL ? strdup(message) : NULL;
}

// =============================================================================
// TERNARY EMULATOR UTILITY FUNCTIONS
// =============================================================================

void ternary_emulator_print_status(ternary_emulator_t* emulator) {
    if (emulator == NULL) {
        printf("Ternary Emulator Status: NULL\n");
        return;
    }
    
    printf("Ternary Emulator Status:\n");
    printf("  Running: %s\n", emulator->running ? "true" : "false");
    printf("  Error: %s\n", emulator->error ? "true" : "false");
    printf("  PC: %d\n", ternary_emulator_get_pc(emulator));
    
    if (emulator->simulator != NULL) {
        printf("  Simulator Status:\n");
        ternary_simulator_print_status(emulator->simulator);
    }
}

void ternary_emulator_debug(ternary_emulator_t* emulator) {
    if (emulator == NULL) {
        printf("Ternary Emulator Debug: NULL\n");
        return;
    }
    
    printf("Ternary Emulator Debug:\n");
    printf("  Running: %s\n", emulator->running ? "true" : "false");
    printf("  Error: %s\n", emulator->error ? "true" : "false");
    printf("  Error Message: %s\n", emulator->error_message != NULL ? emulator->error_message : "None");
    printf("  PC: %d\n", ternary_emulator_get_pc(emulator));
    
    if (emulator->simulator != NULL) {
        printf("  Simulator Debug:\n");
        ternary_simulator_debug(emulator->simulator);
    }
}
