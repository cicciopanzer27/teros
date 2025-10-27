/**
 * @file ternary_interpreter.h
 * @brief Ternary interpreter header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_INTERPRETER_H
#define TERNARY_INTERPRETER_H

#include "trit.h"
#include "t3_isa.h"
#include "tvm.h"
#include "ternary_alu.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY INTERPRETER STRUCTURE
// =============================================================================

typedef struct {
    ternary_alu_t* alu;
    tvm_t* vm;
    bool running;
    bool halted;
    bool error;
} ternary_interpreter_t;

// =============================================================================
// TERNARY INTERPRETER CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary interpreter
 * @return A new ternary interpreter, or NULL on failure
 */
ternary_interpreter_t* ternary_interpreter_create(void);

/**
 * @brief Destroy a ternary interpreter
 * @param interpreter The interpreter to destroy
 */
void ternary_interpreter_destroy(ternary_interpreter_t* interpreter);

// =============================================================================
// TERNARY INTERPRETER EXECUTION
// =============================================================================

/**
 * @brief Execute a single instruction
 * @param interpreter The interpreter instance
 * @param instruction The instruction to execute
 * @return Result of execution
 */
trit_t ternary_interpreter_execute_instruction(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);

// =============================================================================
// TERNARY INTERPRETER DATA MOVEMENT INSTRUCTIONS
// =============================================================================

trit_t ternary_interpreter_execute_load(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);
trit_t ternary_interpreter_execute_store(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);

// =============================================================================
// TERNARY INTERPRETER ARITHMETIC INSTRUCTIONS
// =============================================================================

trit_t ternary_interpreter_execute_add(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);
trit_t ternary_interpreter_execute_sub(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);
trit_t ternary_interpreter_execute_mul(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);
trit_t ternary_interpreter_execute_div(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);

// =============================================================================
// TERNARY INTERPRETER LOGIC INSTRUCTIONS
// =============================================================================

trit_t ternary_interpreter_execute_and(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);
trit_t ternary_interpreter_execute_or(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);
trit_t ternary_interpreter_execute_not(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);
trit_t ternary_interpreter_execute_xor(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);

// =============================================================================
// TERNARY INTERPRETER COMPARISON INSTRUCTIONS
// =============================================================================

trit_t ternary_interpreter_execute_cmp(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);

// =============================================================================
// TERNARY INTERPRETER CONTROL FLOW INSTRUCTIONS
// =============================================================================

trit_t ternary_interpreter_execute_jmp(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);
trit_t ternary_interpreter_execute_jz(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);
trit_t ternary_interpreter_execute_jnz(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);

// =============================================================================
// TERNARY INTERPRETER STACK INSTRUCTIONS
// =============================================================================

trit_t ternary_interpreter_execute_call(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);
trit_t ternary_interpreter_execute_ret(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);
trit_t ternary_interpreter_execute_push(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);
trit_t ternary_interpreter_execute_pop(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);

// =============================================================================
// TERNARY INTERPRETER SYSTEM INSTRUCTIONS
// =============================================================================

trit_t ternary_interpreter_execute_halt(ternary_interpreter_t* interpreter, t3_instruction_t* instruction);

// =============================================================================
// TERNARY INTERPRETER STATUS OPERATIONS
// =============================================================================

/**
 * @brief Check if interpreter is running
 * @param interpreter The interpreter instance
 * @return true if running, false otherwise
 */
bool ternary_interpreter_is_running(ternary_interpreter_t* interpreter);

/**
 * @brief Check if interpreter is halted
 * @param interpreter The interpreter instance
 * @return true if halted, false otherwise
 */
bool ternary_interpreter_is_halted(ternary_interpreter_t* interpreter);

/**
 * @brief Check if interpreter has error
 * @param interpreter The interpreter instance
 * @return true if error, false otherwise
 */
bool ternary_interpreter_has_error(ternary_interpreter_t* interpreter);

// =============================================================================
// TERNARY INTERPRETER UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print interpreter status
 * @param interpreter The interpreter instance
 */
void ternary_interpreter_print_status(ternary_interpreter_t* interpreter);

/**
 * @brief Debug print interpreter information
 * @param interpreter The interpreter instance
 */
void ternary_interpreter_debug(ternary_interpreter_t* interpreter);

#endif // TERNARY_INTERPRETER_H
