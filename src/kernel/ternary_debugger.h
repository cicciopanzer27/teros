/**
 * @file ternary_debugger.h
 * @brief Ternary debugger header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_DEBUGGER_H
#define TERNARY_DEBUGGER_H

#include "trit.h"
#include "t3_isa.h"
#include "tvm.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY DEBUGGER STRUCTURE
// =============================================================================

typedef struct {
    tvm_t* vm;
    int* breakpoints;
    size_t breakpoint_count;
    size_t breakpoint_capacity;
    bool running;
    bool stepping;
    bool error;
    char* error_message;
} ternary_debugger_t;

// =============================================================================
// TERNARY DEBUGGER CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary debugger
 * @return A new ternary debugger, or NULL on failure
 */
ternary_debugger_t* ternary_debugger_create(void);

/**
 * @brief Destroy a ternary debugger
 * @param debugger The debugger to destroy
 */
void ternary_debugger_destroy(ternary_debugger_t* debugger);

// =============================================================================
// TERNARY DEBUGGER VM MANAGEMENT
// =============================================================================

/**
 * @brief Attach a VM to the debugger
 * @param debugger The debugger instance
 * @param vm The VM to attach
 * @return true on success, false on failure
 */
bool ternary_debugger_attach_vm(ternary_debugger_t* debugger, tvm_t* vm);

/**
 * @brief Detach the VM from the debugger
 * @param debugger The debugger instance
 */
void ternary_debugger_detach_vm(ternary_debugger_t* debugger);

/**
 * @brief Get the attached VM
 * @param debugger The debugger instance
 * @return The attached VM, or NULL if none
 */
tvm_t* ternary_debugger_get_vm(ternary_debugger_t* debugger);

// =============================================================================
// TERNARY DEBUGGER EXECUTION CONTROL
// =============================================================================

/**
 * @brief Start debugging
 * @param debugger The debugger instance
 * @return true on success, false on failure
 */
bool ternary_debugger_start(ternary_debugger_t* debugger);

/**
 * @brief Stop debugging
 * @param debugger The debugger instance
 */
void ternary_debugger_stop(ternary_debugger_t* debugger);

/**
 * @brief Step one instruction
 * @param debugger The debugger instance
 * @return true on success, false on failure
 */
bool ternary_debugger_step(ternary_debugger_t* debugger);

/**
 * @brief Continue execution
 * @param debugger The debugger instance
 * @return true on success, false on failure
 */
bool ternary_debugger_continue(ternary_debugger_t* debugger);

// =============================================================================
// TERNARY DEBUGGER BREAKPOINT MANAGEMENT
// =============================================================================

/**
 * @brief Set a breakpoint at the specified address
 * @param debugger The debugger instance
 * @param address The address to set breakpoint at
 * @return true on success, false on failure
 */
bool ternary_debugger_set_breakpoint(ternary_debugger_t* debugger, int address);

/**
 * @brief Clear a breakpoint at the specified address
 * @param debugger The debugger instance
 * @param address The address to clear breakpoint at
 * @return true on success, false on failure
 */
bool ternary_debugger_clear_breakpoint(ternary_debugger_t* debugger, int address);

/**
 * @brief Check if there's a breakpoint at the specified address
 * @param debugger The debugger instance
 * @param address The address to check
 * @return true if breakpoint exists, false otherwise
 */
bool ternary_debugger_is_breakpoint(ternary_debugger_t* debugger, int address);

/**
 * @brief Clear all breakpoints
 * @param debugger The debugger instance
 */
void ternary_debugger_clear_all_breakpoints(ternary_debugger_t* debugger);

// =============================================================================
// TERNARY DEBUGGER REGISTER OPERATIONS
// =============================================================================

/**
 * @brief Get a register value
 * @param debugger The debugger instance
 * @param register_index The register index
 * @return The register value
 */
trit_t ternary_debugger_get_register(ternary_debugger_t* debugger, int register_index);

/**
 * @brief Set a register value
 * @param debugger The debugger instance
 * @param register_index The register index
 * @param value The value to set
 */
void ternary_debugger_set_register(ternary_debugger_t* debugger, int register_index, trit_t value);

/**
 * @brief Print all registers
 * @param debugger The debugger instance
 */
void ternary_debugger_print_registers(ternary_debugger_t* debugger);

// =============================================================================
// TERNARY DEBUGGER MEMORY OPERATIONS
// =============================================================================

/**
 * @brief Read a value from memory
 * @param debugger The debugger instance
 * @param address The memory address
 * @return The value at the address
 */
trit_t ternary_debugger_read_memory(ternary_debugger_t* debugger, size_t address);

/**
 * @brief Write a value to memory
 * @param debugger The debugger instance
 * @param address The memory address
 * @param value The value to write
 */
void ternary_debugger_write_memory(ternary_debugger_t* debugger, size_t address, trit_t value);

/**
 * @brief Print memory contents
 * @param debugger The debugger instance
 * @param start_address Starting address
 * @param count Number of words to print
 */
void ternary_debugger_print_memory(ternary_debugger_t* debugger, size_t start_address, size_t count);

// =============================================================================
// TERNARY DEBUGGER STATUS OPERATIONS
// =============================================================================

/**
 * @brief Check if debugger is running
 * @param debugger The debugger instance
 * @return true if running, false otherwise
 */
bool ternary_debugger_is_running(ternary_debugger_t* debugger);

/**
 * @brief Check if debugger is stepping
 * @param debugger The debugger instance
 * @return true if stepping, false otherwise
 */
bool ternary_debugger_is_stepping(ternary_debugger_t* debugger);

/**
 * @brief Check if debugger has error
 * @param debugger The debugger instance
 * @return true if error, false otherwise
 */
bool ternary_debugger_has_error(ternary_debugger_t* debugger);

/**
 * @brief Get program counter
 * @param debugger The debugger instance
 * @return The program counter value
 */
int ternary_debugger_get_pc(ternary_debugger_t* debugger);

/**
 * @brief Set program counter
 * @param debugger The debugger instance
 * @param pc The program counter value
 */
void ternary_debugger_set_pc(ternary_debugger_t* debugger, int pc);

// =============================================================================
// TERNARY DEBUGGER ERROR HANDLING
// =============================================================================

/**
 * @brief Get debugger error message
 * @param debugger The debugger instance
 * @return The error message, or NULL if no error
 */
const char* ternary_debugger_get_error_message(ternary_debugger_t* debugger);

/**
 * @brief Set debugger error
 * @param debugger The debugger instance
 * @param message The error message
 */
void ternary_debugger_set_error(ternary_debugger_t* debugger, const char* message);

// =============================================================================
// TERNARY DEBUGGER UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print debugger status
 * @param debugger The debugger instance
 */
void ternary_debugger_print_status(ternary_debugger_t* debugger);

/**
 * @brief Debug print debugger information
 * @param debugger The debugger instance
 */
void ternary_debugger_debug(ternary_debugger_t* debugger);

#endif // TERNARY_DEBUGGER_H
