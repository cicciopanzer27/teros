/**
 * @file ternary_validator.h
 * @brief Ternary code validator header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_VALIDATOR_H
#define TERNARY_VALIDATOR_H

#include "trit.h"
#include "t3_isa.h"
#include "ternary_compiler.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY VALIDATOR STRUCTURE
// =============================================================================

typedef struct {
    ternary_compiler_t* compiler;
    bool error;
    char* error_message;
    size_t warning_count;
    size_t error_count;
} ternary_validator_t;

// =============================================================================
// TERNARY VALIDATOR CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary validator
 * @return A new ternary validator, or NULL on failure
 */
ternary_validator_t* ternary_validator_create(void);

/**
 * @brief Destroy a ternary validator
 * @param validator The validator to destroy
 */
void ternary_validator_destroy(ternary_validator_t* validator);

// =============================================================================
// TERNARY VALIDATOR CODE VALIDATION
// =============================================================================

/**
 * @brief Validate source code
 * @param validator The validator instance
 * @param source_code The source code to validate
 * @return true on success, false on failure
 */
bool ternary_validator_validate_code(ternary_validator_t* validator, const char* source_code);

/**
 * @brief Validate compiled instructions
 * @param validator The validator instance
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_validator_validate_instructions(ternary_validator_t* validator, size_t instruction_count);

// =============================================================================
// TERNARY VALIDATOR INSTRUCTION VALIDATION
// =============================================================================

/**
 * @brief Validate a single instruction
 * @param validator The validator instance
 * @param instruction The instruction to validate
 * @param index The instruction index
 * @return true on success, false on failure
 */
bool ternary_validator_validate_instruction(ternary_validator_t* validator, t3_instruction_t* instruction, size_t index);

/**
 * @brief Validate an operand
 * @param validator The validator instance
 * @param operand The operand to validate
 * @param name The operand name
 * @param index The instruction index
 * @return true on success, false on failure
 */
bool ternary_validator_validate_operand(ternary_validator_t* validator, uint8_t operand, const char* name, size_t index);

/**
 * @brief Validate an immediate value
 * @param validator The validator instance
 * @param immediate The immediate value to validate
 * @param index The instruction index
 * @return true on success, false on failure
 */
bool ternary_validator_validate_immediate(ternary_validator_t* validator, int16_t immediate, size_t index);

// =============================================================================
// TERNARY VALIDATOR SEMANTIC VALIDATION
// =============================================================================

/**
 * @brief Validate code semantics
 * @param validator The validator instance
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_validator_validate_semantics(ternary_validator_t* validator, size_t instruction_count);

/**
 * @brief Check for unreachable code
 * @param validator The validator instance
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_validator_check_unreachable_code(ternary_validator_t* validator, size_t instruction_count);

/**
 * @brief Check for infinite loops
 * @param validator The validator instance
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_validator_check_infinite_loops(ternary_validator_t* validator, size_t instruction_count);

/**
 * @brief Check for stack overflow
 * @param validator The validator instance
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_validator_check_stack_overflow(ternary_validator_t* validator, size_t instruction_count);

// =============================================================================
// TERNARY VALIDATOR METRICS
// =============================================================================

/**
 * @brief Get warning count
 * @param validator The validator instance
 * @return The warning count
 */
size_t ternary_validator_get_warning_count(ternary_validator_t* validator);

/**
 * @brief Get error count
 * @param validator The validator instance
 * @return The error count
 */
size_t ternary_validator_get_error_count(ternary_validator_t* validator);

/**
 * @brief Check if validator has warnings
 * @param validator The validator instance
 * @return true if warnings, false otherwise
 */
bool ternary_validator_has_warnings(ternary_validator_t* validator);

/**
 * @brief Check if validator has errors
 * @param validator The validator instance
 * @return true if errors, false otherwise
 */
bool ternary_validator_has_errors(ternary_validator_t* validator);

// =============================================================================
// TERNARY VALIDATOR ERROR HANDLING
// =============================================================================

/**
 * @brief Check if validator has error
 * @param validator The validator instance
 * @return true if error, false otherwise
 */
bool ternary_validator_has_error(ternary_validator_t* validator);

/**
 * @brief Get validator error message
 * @param validator The validator instance
 * @return The error message, or NULL if no error
 */
const char* ternary_validator_get_error_message(ternary_validator_t* validator);

/**
 * @brief Set validator error
 * @param validator The validator instance
 * @param message The error message
 */
void ternary_validator_set_error(ternary_validator_t* validator, const char* message);

// =============================================================================
// TERNARY VALIDATOR UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print validation results
 * @param validator The validator instance
 */
void ternary_validator_print_validation(ternary_validator_t* validator);

/**
 * @brief Debug print validator information
 * @param validator The validator instance
 */
void ternary_validator_debug(ternary_validator_t* validator);

#endif // TERNARY_VALIDATOR_H
