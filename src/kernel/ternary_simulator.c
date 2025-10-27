/**
 * @file ternary_simulator.c
 * @brief Ternary simulator implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_simulator.h"
#include "trit.h"
#include "t3_isa.h"
#include "tvm.h"
#include "ternary_debugger.h"
#include "ternary_profiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

// =============================================================================
// TERNARY SIMULATOR IMPLEMENTATION
// =============================================================================

ternary_simulator_t* ternary_simulator_create(void) {
    ternary_simulator_t* simulator = malloc(sizeof(ternary_simulator_t));
    if (simulator == NULL) return NULL;
    
    simulator->vm = tvm_create(TVM_DEFAULT_MEMORY_SIZE);
    if (simulator->vm == NULL) {
        free(simulator);
        return NULL;
    }
    
    simulator->debugger = ternary_debugger_create();
    if (simulator->debugger == NULL) {
        tvm_destroy(simulator->vm);
        free(simulator);
        return NULL;
    }
    
    simulator->profiler = ternary_profiler_create();
    if (simulator->profiler == NULL) {
        ternary_debugger_destroy(simulator->debugger);
        tvm_destroy(simulator->vm);
        free(simulator);
        return NULL;
    }
    
    // Attach VM to debugger and profiler
    ternary_debugger_attach_vm(simulator->debugger, simulator->vm);
    ternary_profiler_attach_vm(simulator->profiler, simulator->vm);
    
    simulator->running = false;
    simulator->stepping = false;
    simulator->error = false;
    simulator->error_message = NULL;
    
    return simulator;
}

void ternary_simulator_destroy(ternary_simulator_t* simulator) {
    if (simulator != NULL) {
        if (simulator->debugger != NULL) {
            ternary_debugger_destroy(simulator->debugger);
        }
        if (simulator->profiler != NULL) {
            ternary_profiler_destroy(simulator->profiler);
        }
        if (simulator->vm != NULL) {
            tvm_destroy(simulator->vm);
        }
        if (simulator->error_message != NULL) {
            free(simulator->error_message);
        }
        free(simulator);
    }
}

// =============================================================================
// TERNARY SIMULATOR EXECUTION CONTROL
// =============================================================================

bool ternary_simulator_start(ternary_simulator_t* simulator) {
    if (simulator == NULL) {
        return false;
    }
    
    simulator->running = true;
    simulator->stepping = false;
    simulator->error = false;
    
    // Start debugger and profiler
    if (!ternary_debugger_start(simulator->debugger)) {
        simulator->error = true;
        simulator->error_message = strdup("Failed to start debugger");
        return false;
    }
    
    if (!ternary_profiler_start(simulator->profiler)) {
        simulator->error = true;
        simulator->error_message = strdup("Failed to start profiler");
        return false;
    }
    
    return true;
}

void ternary_simulator_stop(ternary_simulator_t* simulator) {
    if (simulator == NULL) return;
    
    simulator->running = false;
    simulator->stepping = false;
    
    if (simulator->debugger != NULL) {
        ternary_debugger_stop(simulator->debugger);
    }
    
    if (simulator->profiler != NULL) {
        ternary_profiler_stop(simulator->profiler);
    }
}

bool ternary_simulator_step(ternary_simulator_t* simulator) {
    if (simulator == NULL || !simulator->running) {
        return false;
    }
    
    // Step debugger
    if (!ternary_debugger_step(simulator->debugger)) {
        simulator->error = true;
        simulator->error_message = strdup("Debugger step failed");
        return false;
    }
    
    // Check if we hit a breakpoint
    if (ternary_debugger_is_stepping(simulator->debugger)) {
        simulator->stepping = true;
        return true;
    }
    
    // Profile the instruction
    int pc = ternary_debugger_get_pc(simulator->debugger);
    if (pc >= 0) {
        // Get current instruction opcode (simplified)
        uint8_t opcode = 0;  // In a real implementation, we would decode the instruction
        ternary_profiler_profile_instruction(simulator->profiler, opcode);
    }
    
    return true;
}

bool ternary_simulator_continue(ternary_simulator_t* simulator) {
    if (simulator == NULL || !simulator->running) {
        return false;
    }
    
    simulator->stepping = false;
    
    // Continue debugger
    if (!ternary_debugger_continue(simulator->debugger)) {
        simulator->error = true;
        simulator->error_message = strdup("Debugger continue failed");
        return false;
    }
    
    return true;
}

// =============================================================================
// TERNARY SIMULATOR PROGRAM LOADING
// =============================================================================

bool ternary_simulator_load_program(ternary_simulator_t* simulator, t3_instruction_t* program, size_t program_size) {
    if (simulator == NULL || program == NULL || program_size == 0) {
        return false;
    }
    
    // Load program into VM
    if (!tvm_load_program(simulator->vm, program, program_size)) {
        simulator->error = true;
        simulator->error_message = strdup("Failed to load program into VM");
        return false;
    }
    
    return true;
}

bool ternary_simulator_load_binary(ternary_simulator_t* simulator, const uint8_t* binary_data, size_t data_size) {
    if (simulator == NULL || binary_data == NULL || data_size == 0) {
        return false;
    }
    
    // Convert binary data to instructions (simplified)
    size_t instruction_count = data_size / 4;  // Each instruction is 4 bytes
    t3_instruction_t* instructions = malloc(instruction_count * sizeof(t3_instruction_t));
    if (instructions == NULL) {
        simulator->error = true;
        simulator->error_message = strdup("Failed to allocate memory for instructions");
        return false;
    }
    
    for (size_t i = 0; i < instruction_count; i++) {
        instructions[i].opcode = binary_data[i * 4];
        instructions[i].operand1 = binary_data[i * 4 + 1];
        instructions[i].operand2 = binary_data[i * 4 + 2];
        instructions[i].operand3 = binary_data[i * 4 + 3];
        instructions[i].immediate = 0;
        instructions[i].valid = true;
    }
    
    bool success = ternary_simulator_load_program(simulator, instructions, instruction_count);
    free(instructions);
    
    return success;
}

// =============================================================================
// TERNARY SIMULATOR BREAKPOINT MANAGEMENT
// =============================================================================

bool ternary_simulator_set_breakpoint(ternary_simulator_t* simulator, int address) {
    if (simulator == NULL || simulator->debugger == NULL) {
        return false;
    }
    
    return ternary_debugger_set_breakpoint(simulator->debugger, address);
}

bool ternary_simulator_clear_breakpoint(ternary_simulator_t* simulator, int address) {
    if (simulator == NULL || simulator->debugger == NULL) {
        return false;
    }
    
    return ternary_debugger_clear_breakpoint(simulator->debugger, address);
}

bool ternary_simulator_is_breakpoint(ternary_simulator_t* simulator, int address) {
    if (simulator == NULL || simulator->debugger == NULL) {
        return false;
    }
    
    return ternary_debugger_is_breakpoint(simulator->debugger, address);
}

void ternary_simulator_clear_all_breakpoints(ternary_simulator_t* simulator) {
    if (simulator == NULL || simulator->debugger == NULL) {
        return;
    }
    
    ternary_debugger_clear_all_breakpoints(simulator->debugger);
}

// =============================================================================
// TERNARY SIMULATOR REGISTER OPERATIONS
// =============================================================================

trit_t ternary_simulator_get_register(ternary_simulator_t* simulator, int register_index) {
    if (simulator == NULL || simulator->debugger == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return ternary_debugger_get_register(simulator->debugger, register_index);
}

void ternary_simulator_set_register(ternary_simulator_t* simulator, int register_index, trit_t value) {
    if (simulator == NULL || simulator->debugger == NULL) {
        return;
    }
    
    ternary_debugger_set_register(simulator->debugger, register_index, value);
}

void ternary_simulator_print_registers(ternary_simulator_t* simulator) {
    if (simulator == NULL || simulator->debugger == NULL) {
        printf("Simulator Registers: NULL\n");
        return;
    }
    
    ternary_debugger_print_registers(simulator->debugger);
}

// =============================================================================
// TERNARY SIMULATOR MEMORY OPERATIONS
// =============================================================================

trit_t ternary_simulator_read_memory(ternary_simulator_t* simulator, size_t address) {
    if (simulator == NULL || simulator->debugger == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return ternary_debugger_read_memory(simulator->debugger, address);
}

void ternary_simulator_write_memory(ternary_simulator_t* simulator, size_t address, trit_t value) {
    if (simulator == NULL || simulator->debugger == NULL) {
        return;
    }
    
    ternary_debugger_write_memory(simulator->debugger, address, value);
}

void ternary_simulator_print_memory(ternary_simulator_t* simulator, size_t start_address, size_t count) {
    if (simulator == NULL || simulator->debugger == NULL) {
        printf("Simulator Memory: NULL\n");
        return;
    }
    
    ternary_debugger_print_memory(simulator->debugger, start_address, count);
}

// =============================================================================
// TERNARY SIMULATOR STATUS OPERATIONS
// =============================================================================

bool ternary_simulator_is_running(ternary_simulator_t* simulator) {
    return simulator != NULL && simulator->running;
}

bool ternary_simulator_is_stepping(ternary_simulator_t* simulator) {
    return simulator != NULL && simulator->stepping;
}

bool ternary_simulator_has_error(ternary_simulator_t* simulator) {
    return simulator != NULL && simulator->error;
}

int ternary_simulator_get_pc(ternary_simulator_t* simulator) {
    if (simulator == NULL || simulator->debugger == NULL) {
        return -1;
    }
    
    return ternary_debugger_get_pc(simulator->debugger);
}

void ternary_simulator_set_pc(ternary_simulator_t* simulator, int pc) {
    if (simulator == NULL || simulator->debugger == NULL) {
        return;
    }
    
    ternary_debugger_set_pc(simulator->debugger, pc);
}

// =============================================================================
// TERNARY SIMULATOR PROFILING
// =============================================================================

uint64_t ternary_simulator_get_execution_time(ternary_simulator_t* simulator) {
    if (simulator == NULL || simulator->profiler == NULL) {
        return 0;
    }
    
    // TODO: Implement without floating-point (SSE disabled in kernel)
    return 0;  // Stub - was calling ternary_profiler_get_execution_time(simulator->profiler);
}

uint64_t ternary_simulator_get_instructions_per_second(ternary_simulator_t* simulator) {
    if (simulator == NULL || simulator->profiler == NULL) {
        return 0;
    }
    
    // TODO: Implement without floating-point (SSE disabled in kernel)
    return 0;  // Stub - was calling ternary_profiler_get_instructions_per_second(simulator->profiler);
}

void ternary_simulator_print_profiling_summary(ternary_simulator_t* simulator) {
    if (simulator == NULL || simulator->profiler == NULL) {
        printf("Simulator Profiling: NULL\n");
        return;
    }
    
    ternary_profiler_print_summary(simulator->profiler);
}

// =============================================================================
// TERNARY SIMULATOR ERROR HANDLING
// =============================================================================

const char* ternary_simulator_get_error_message(ternary_simulator_t* simulator) {
    if (simulator == NULL) return NULL;
    return simulator->error_message;
}

void ternary_simulator_set_error(ternary_simulator_t* simulator, const char* message) {
    if (simulator == NULL) return;
    
    simulator->error = true;
    if (simulator->error_message != NULL) {
        free(simulator->error_message);
    }
    simulator->error_message = message != NULL ? strdup(message) : NULL;
}

// =============================================================================
// TERNARY SIMULATOR UTILITY FUNCTIONS
// =============================================================================

void ternary_simulator_print_status(ternary_simulator_t* simulator) {
    if (simulator == NULL) {
        printf("Ternary Simulator Status: NULL\n");
        return;
    }
    
    printf("Ternary Simulator Status:\n");
    printf("  Running: %s\n", simulator->running ? "true" : "false");
    printf("  Stepping: %s\n", simulator->stepping ? "true" : "false");
    printf("  Error: %s\n", simulator->error ? "true" : "false");
    printf("  PC: %d\n", ternary_simulator_get_pc(simulator));
    
    if (simulator->debugger != NULL) {
        printf("  Debugger Status:\n");
        ternary_debugger_print_status(simulator->debugger);
    }
    
    if (simulator->profiler != NULL) {
        printf("  Profiler Status:\n");
        ternary_profiler_print_summary(simulator->profiler);
    }
}

void ternary_simulator_debug(ternary_simulator_t* simulator) {
    if (simulator == NULL) {
        printf("Ternary Simulator Debug: NULL\n");
        return;
    }
    
    printf("Ternary Simulator Debug:\n");
    printf("  Running: %s\n", simulator->running ? "true" : "false");
    printf("  Stepping: %s\n", simulator->stepping ? "true" : "false");
    printf("  Error: %s\n", simulator->error ? "true" : "false");
    printf("  Error Message: %s\n", simulator->error_message != NULL ? simulator->error_message : "None");
    printf("  PC: %d\n", ternary_simulator_get_pc(simulator));
    
    if (simulator->debugger != NULL) {
        printf("  Debugger Debug:\n");
        ternary_debugger_debug(simulator->debugger);
    }
    
    if (simulator->profiler != NULL) {
        printf("  Profiler Debug:\n");
        ternary_profiler_debug(simulator->profiler);
    }
}
