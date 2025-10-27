/**
 * @file ternary_simulator.h
 * @brief Ternary simulator header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_SIMULATOR_H
#define TERNARY_SIMULATOR_H

#include "trit.h"
#include "t3_isa.h"
#include "tvm.h"
#include "ternary_debugger.h"
#include "ternary_profiler.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY SIMULATOR STRUCTURE
// =============================================================================

typedef struct {
    tvm_t* vm;
    ternary_debugger_t* debugger;
    ternary_profiler_t* profiler;
    bool running;
    bool stepping;
    bool error;
    char* error_message;
} ternary_simulator_t;

// =============================================================================
// TERNARY SIMULATOR CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary simulator
 * @return A new ternary simulator, or NULL on failure
 */
ternary_simulator_t* ternary_simulator_create(void);

/**
 * @brief Destroy a ternary simulator
 * @param simulator The simulator to destroy
 */
void ternary_simulator_destroy(ternary_simulator_t* simulator);

// =============================================================================
// TERNARY SIMULATOR EXECUTION CONTROL
// =============================================================================

/**
 * @brief Start the simulator
 * @param simulator The simulator instance
 * @return true on success, false on failure
 */
bool ternary_simulator_start(ternary_simulator_t* simulator);

/**
 * @brief Stop the simulator
 * @param simulator The simulator instance
 */
void ternary_simulator_stop(ternary_simulator_t* simulator);

/**
 * @brief Step one instruction
 * @param simulator The simulator instance
 * @return true on success, false on failure
 */
bool ternary_simulator_step(ternary_simulator_t* simulator);

/**
 * @brief Continue execution
 * @param simulator The simulator instance
 * @return true on success, false on failure
 */
bool ternary_simulator_continue(ternary_simulator_t* simulator);

// =============================================================================
// TERNARY SIMULATOR PROGRAM LOADING
// =============================================================================

/**
 * @brief Load a program into the simulator
 * @param simulator The simulator instance
 * @param program The program instructions
 * @param program_size The number of instructions
 * @return true on success, false on failure
 */
bool ternary_simulator_load_program(ternary_simulator_t* simulator, t3_instruction_t* program, size_t program_size);

/**
 * @brief Load binary data into the simulator
 * @param simulator The simulator instance
 * @param binary_data The binary data
 * @param data_size The size of the binary data
 * @return true on success, false on failure
 */
bool ternary_simulator_load_binary(ternary_simulator_t* simulator, const uint8_t* binary_data, size_t data_size);

// =============================================================================
// TERNARY SIMULATOR BREAKPOINT MANAGEMENT
// =============================================================================

/**
 * @brief Set a breakpoint at the specified address
 * @param simulator The simulator instance
 * @param address The address to set breakpoint at
 * @return true on success, false on failure
 */
bool ternary_simulator_set_breakpoint(ternary_simulator_t* simulator, int address);

/**
 * @brief Clear a breakpoint at the specified address
 * @param simulator The simulator instance
 * @param address The address to clear breakpoint at
 * @return true on success, false on failure
 */
bool ternary_simulator_clear_breakpoint(ternary_simulator_t* simulator, int address);

/**
 * @brief Check if there's a breakpoint at the specified address
 * @param simulator The simulator instance
 * @param address The address to check
 * @return true if breakpoint exists, false otherwise
 */
bool ternary_simulator_is_breakpoint(ternary_simulator_t* simulator, int address);

/**
 * @brief Clear all breakpoints
 * @param simulator The simulator instance
 */
void ternary_simulator_clear_all_breakpoints(ternary_simulator_t* simulator);

// =============================================================================
// TERNARY SIMULATOR REGISTER OPERATIONS
// =============================================================================

/**
 * @brief Get a register value
 * @param simulator The simulator instance
 * @param register_index The register index
 * @return The register value
 */
trit_t ternary_simulator_get_register(ternary_simulator_t* simulator, int register_index);

/**
 * @brief Set a register value
 * @param simulator The simulator instance
 * @param register_index The register index
 * @param value The value to set
 */
void ternary_simulator_set_register(ternary_simulator_t* simulator, int register_index, trit_t value);

/**
 * @brief Print all registers
 * @param simulator The simulator instance
 */
void ternary_simulator_print_registers(ternary_simulator_t* simulator);

// =============================================================================
// TERNARY SIMULATOR MEMORY OPERATIONS
// =============================================================================

/**
 * @brief Read a value from memory
 * @param simulator The simulator instance
 * @param address The memory address
 * @return The value at the address
 */
trit_t ternary_simulator_read_memory(ternary_simulator_t* simulator, size_t address);

/**
 * @brief Write a value to memory
 * @param simulator The simulator instance
 * @param address The memory address
 * @param value The value to write
 */
void ternary_simulator_write_memory(ternary_simulator_t* simulator, size_t address, trit_t value);

/**
 * @brief Print memory contents
 * @param simulator The simulator instance
 * @param start_address Starting address
 * @param count Number of words to print
 */
void ternary_simulator_print_memory(ternary_simulator_t* simulator, size_t start_address, size_t count);

// =============================================================================
// TERNARY SIMULATOR STATUS OPERATIONS
// =============================================================================

/**
 * @brief Check if simulator is running
 * @param simulator The simulator instance
 * @return true if running, false otherwise
 */
bool ternary_simulator_is_running(ternary_simulator_t* simulator);

/**
 * @brief Check if simulator is stepping
 * @param simulator The simulator instance
 * @return true if stepping, false otherwise
 */
bool ternary_simulator_is_stepping(ternary_simulator_t* simulator);

/**
 * @brief Check if simulator has error
 * @param simulator The simulator instance
 * @return true if error, false otherwise
 */
bool ternary_simulator_has_error(ternary_simulator_t* simulator);

/**
 * @brief Get program counter
 * @param simulator The simulator instance
 * @return The program counter value
 */
int ternary_simulator_get_pc(ternary_simulator_t* simulator);

/**
 * @brief Set program counter
 * @param simulator The simulator instance
 * @param pc The program counter value
 */
void ternary_simulator_set_pc(ternary_simulator_t* simulator, int pc);

// =============================================================================
// TERNARY SIMULATOR PROFILING
// =============================================================================

/**
 * @brief Get execution time in seconds
 * @param simulator The simulator instance
 * @return The execution time
 */
uint64_t ternary_simulator_get_execution_time(ternary_simulator_t* simulator);

/**
 * @brief Get instructions per second
 * @param simulator The simulator instance
 * @return The instructions per second
 */
uint64_t ternary_simulator_get_instructions_per_second(ternary_simulator_t* simulator);

/**
 * @brief Print profiling summary
 * @param simulator The simulator instance
 */
void ternary_simulator_print_profiling_summary(ternary_simulator_t* simulator);

// =============================================================================
// TERNARY SIMULATOR ERROR HANDLING
// =============================================================================

/**
 * @brief Get simulator error message
 * @param simulator The simulator instance
 * @return The error message, or NULL if no error
 */
const char* ternary_simulator_get_error_message(ternary_simulator_t* simulator);

/**
 * @brief Set simulator error
 * @param simulator The simulator instance
 * @param message The error message
 */
void ternary_simulator_set_error(ternary_simulator_t* simulator, const char* message);

// =============================================================================
// TERNARY SIMULATOR UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print simulator status
 * @param simulator The simulator instance
 */
void ternary_simulator_print_status(ternary_simulator_t* simulator);

/**
 * @brief Debug print simulator information
 * @param simulator The simulator instance
 */
void ternary_simulator_debug(ternary_simulator_t* simulator);

#endif // TERNARY_SIMULATOR_H
