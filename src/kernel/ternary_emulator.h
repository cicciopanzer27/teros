/**
 * @file ternary_emulator.h
 * @brief Ternary emulator header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_EMULATOR_H
#define TERNARY_EMULATOR_H

#include "trit.h"
#include "t3_isa.h"
#include "tvm.h"
#include "ternary_simulator.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY EMULATOR STRUCTURE
// =============================================================================

typedef struct {
    ternary_simulator_t* simulator;
    bool running;
    bool error;
    char* error_message;
} ternary_emulator_t;

// =============================================================================
// TERNARY EMULATOR CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary emulator
 * @return A new ternary emulator, or NULL on failure
 */
ternary_emulator_t* ternary_emulator_create(void);

/**
 * @brief Destroy a ternary emulator
 * @param emulator The emulator to destroy
 */
void ternary_emulator_destroy(ternary_emulator_t* emulator);

// =============================================================================
// TERNARY EMULATOR EXECUTION CONTROL
// =============================================================================

/**
 * @brief Start the emulator
 * @param emulator The emulator instance
 * @return true on success, false on failure
 */
bool ternary_emulator_start(ternary_emulator_t* emulator);

/**
 * @brief Stop the emulator
 * @param emulator The emulator instance
 */
void ternary_emulator_stop(ternary_emulator_t* emulator);

/**
 * @brief Step one instruction
 * @param emulator The emulator instance
 * @return true on success, false on failure
 */
bool ternary_emulator_step(ternary_emulator_t* emulator);

/**
 * @brief Continue execution
 * @param emulator The emulator instance
 * @return true on success, false on failure
 */
bool ternary_emulator_continue(ternary_emulator_t* emulator);

// =============================================================================
// TERNARY EMULATOR PROGRAM LOADING
// =============================================================================

/**
 * @brief Load a program into the emulator
 * @param emulator The emulator instance
 * @param program The program instructions
 * @param program_size The number of instructions
 * @return true on success, false on failure
 */
bool ternary_emulator_load_program(ternary_emulator_t* emulator, t3_instruction_t* program, size_t program_size);

/**
 * @brief Load binary data into the emulator
 * @param emulator The emulator instance
 * @param binary_data The binary data
 * @param data_size The size of the binary data
 * @return true on success, false on failure
 */
bool ternary_emulator_load_binary(ternary_emulator_t* emulator, const uint8_t* binary_data, size_t data_size);

// =============================================================================
// TERNARY EMULATOR BREAKPOINT MANAGEMENT
// =============================================================================

/**
 * @brief Set a breakpoint at the specified address
 * @param emulator The emulator instance
 * @param address The address to set breakpoint at
 * @return true on success, false on failure
 */
bool ternary_emulator_set_breakpoint(ternary_emulator_t* emulator, int address);

/**
 * @brief Clear a breakpoint at the specified address
 * @param emulator The emulator instance
 * @param address The address to clear breakpoint at
 * @return true on success, false on failure
 */
bool ternary_emulator_clear_breakpoint(ternary_emulator_t* emulator, int address);

/**
 * @brief Check if there's a breakpoint at the specified address
 * @param emulator The emulator instance
 * @param address The address to check
 * @return true if breakpoint exists, false otherwise
 */
bool ternary_emulator_is_breakpoint(ternary_emulator_t* emulator, int address);

/**
 * @brief Clear all breakpoints
 * @param emulator The emulator instance
 */
void ternary_emulator_clear_all_breakpoints(ternary_emulator_t* emulator);

// =============================================================================
// TERNARY EMULATOR REGISTER OPERATIONS
// =============================================================================

/**
 * @brief Get a register value
 * @param emulator The emulator instance
 * @param register_index The register index
 * @return The register value
 */
trit_t ternary_emulator_get_register(ternary_emulator_t* emulator, int register_index);

/**
 * @brief Set a register value
 * @param emulator The emulator instance
 * @param register_index The register index
 * @param value The value to set
 */
void ternary_emulator_set_register(ternary_emulator_t* emulator, int register_index, trit_t value);

/**
 * @brief Print all registers
 * @param emulator The emulator instance
 */
void ternary_emulator_print_registers(ternary_emulator_t* emulator);

// =============================================================================
// TERNARY EMULATOR MEMORY OPERATIONS
// =============================================================================

/**
 * @brief Read a value from memory
 * @param emulator The emulator instance
 * @param address The memory address
 * @return The value at the address
 */
trit_t ternary_emulator_read_memory(ternary_emulator_t* emulator, size_t address);

/**
 * @brief Write a value to memory
 * @param emulator The emulator instance
 * @param address The memory address
 * @param value The value to write
 */
void ternary_emulator_write_memory(ternary_emulator_t* emulator, size_t address, trit_t value);

/**
 * @brief Print memory contents
 * @param emulator The emulator instance
 * @param start_address Starting address
 * @param count Number of words to print
 */
void ternary_emulator_print_memory(ternary_emulator_t* emulator, size_t start_address, size_t count);

// =============================================================================
// TERNARY EMULATOR STATUS OPERATIONS
// =============================================================================

/**
 * @brief Check if emulator is running
 * @param emulator The emulator instance
 * @return true if running, false otherwise
 */
bool ternary_emulator_is_running(ternary_emulator_t* emulator);

/**
 * @brief Check if emulator is stepping
 * @param emulator The emulator instance
 * @return true if stepping, false otherwise
 */
bool ternary_emulator_is_stepping(ternary_emulator_t* emulator);

/**
 * @brief Check if emulator has error
 * @param emulator The emulator instance
 * @return true if error, false otherwise
 */
bool ternary_emulator_has_error(ternary_emulator_t* emulator);

/**
 * @brief Get program counter
 * @param emulator The emulator instance
 * @return The program counter value
 */
int ternary_emulator_get_pc(ternary_emulator_t* emulator);

/**
 * @brief Set program counter
 * @param emulator The emulator instance
 * @param pc The program counter value
 */
void ternary_emulator_set_pc(ternary_emulator_t* emulator, int pc);

// =============================================================================
// TERNARY EMULATOR PROFILING
// =============================================================================

/**
 * @brief Get execution time in seconds
 * @param emulator The emulator instance
 * @return The execution time
 */
double ternary_emulator_get_execution_time(ternary_emulator_t* emulator);

/**
 * @brief Get instructions per second
 * @param emulator The emulator instance
 * @return The instructions per second
 */
double ternary_emulator_get_instructions_per_second(ternary_emulator_t* emulator);

/**
 * @brief Print profiling summary
 * @param emulator The emulator instance
 */
void ternary_emulator_print_profiling_summary(ternary_emulator_t* emulator);

// =============================================================================
// TERNARY EMULATOR ERROR HANDLING
// =============================================================================

/**
 * @brief Get emulator error message
 * @param emulator The emulator instance
 * @return The error message, or NULL if no error
 */
const char* ternary_emulator_get_error_message(ternary_emulator_t* emulator);

/**
 * @brief Set emulator error
 * @param emulator The emulator instance
 * @param message The error message
 */
void ternary_emulator_set_error(ternary_emulator_t* emulator, const char* message);

// =============================================================================
// TERNARY EMULATOR UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print emulator status
 * @param emulator The emulator instance
 */
void ternary_emulator_print_status(ternary_emulator_t* emulator);

/**
 * @brief Debug print emulator information
 * @param emulator The emulator instance
 */
void ternary_emulator_debug(ternary_emulator_t* emulator);

#endif // TERNARY_EMULATOR_H
