/**
 * @file ternary_linter.h
 * @brief Ternary code linter header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_LINTER_H
#define TERNARY_LINTER_H

#include "trit.h"
#include "t3_isa.h"
#include "ternary_compiler.h"
#include "ternary_validator.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY LINTER STRUCTURE
// =============================================================================

typedef struct {
    ternary_compiler_t* compiler;
    ternary_validator_t* validator;
    bool error;
    char* error_message;
    size_t warning_count;
    size_t error_count;
} ternary_linter_t;

// =============================================================================
// TERNARY LINTER CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary linter
 * @return A new ternary linter, or NULL on failure
 */
ternary_linter_t* ternary_linter_create(void);

/**
 * @brief Destroy a ternary linter
 * @param linter The linter to destroy
 */
void ternary_linter_destroy(ternary_linter_t* linter);

// =============================================================================
// TERNARY LINTER CODE LINTING
// =============================================================================

/**
 * @brief Lint source code
 * @param linter The linter instance
 * @param source_code The source code to lint
 * @return true on success, false on failure
 */
bool ternary_linter_lint_code(ternary_linter_t* linter, const char* source_code);

/**
 * @brief Lint a single instruction
 * @param linter The linter instance
 * @param instruction The instruction to lint
 * @param index The instruction index
 */
void ternary_linter_lint_instruction(ternary_linter_t* linter, t3_instruction_t* instruction, size_t index);

// =============================================================================
// TERNARY LINTER STYLE CHECKS
// =============================================================================

/**
 * @brief Check code style
 * @param linter The linter instance
 * @param source_code The source code to check
 * @return true on success, false on failure
 */
bool ternary_linter_check_style(ternary_linter_t* linter, const char* source_code);

/**
 * @brief Check naming conventions
 * @param linter The linter instance
 * @param source_code The source code to check
 * @return true on success, false on failure
 */
bool ternary_linter_check_naming_conventions(ternary_linter_t* linter, const char* source_code);

/**
 * @brief Check indentation
 * @param linter The linter instance
 * @param source_code The source code to check
 * @return true on success, false on failure
 */
bool ternary_linter_check_indentation(ternary_linter_t* linter, const char* source_code);

/**
 * @brief Check line length
 * @param linter The linter instance
 * @param source_code The source code to check
 * @return true on success, false on failure
 */
bool ternary_linter_check_line_length(ternary_linter_t* linter, const char* source_code);

// =============================================================================
// TERNARY LINTER PERFORMANCE CHECKS
// =============================================================================

/**
 * @brief Check code performance
 * @param linter The linter instance
 * @param source_code The source code to check
 * @return true on success, false on failure
 */
bool ternary_linter_check_performance(ternary_linter_t* linter, const char* source_code);

/**
 * @brief Check for inefficient operations
 * @param linter The linter instance
 * @param source_code The source code to check
 * @return true on success, false on failure
 */
bool ternary_linter_check_inefficient_operations(ternary_linter_t* linter, const char* source_code);

/**
 * @brief Check memory usage
 * @param linter The linter instance
 * @param source_code The source code to check
 * @return true on success, false on failure
 */
bool ternary_linter_check_memory_usage(ternary_linter_t* linter, const char* source_code);

// =============================================================================
// TERNARY LINTER SECURITY CHECKS
// =============================================================================

/**
 * @brief Check code security
 * @param linter The linter instance
 * @param source_code The source code to check
 * @return true on success, false on failure
 */
bool ternary_linter_check_security(ternary_linter_t* linter, const char* source_code);

/**
 * @brief Check for buffer overflows
 * @param linter The linter instance
 * @param source_code The source code to check
 * @return true on success, false on failure
 */
bool ternary_linter_check_buffer_overflows(ternary_linter_t* linter, const char* source_code);

/**
 * @brief Check for integer overflows
 * @param linter The linter instance
 * @param source_code The source code to check
 * @return true on success, false on failure
 */
bool ternary_linter_check_integer_overflows(ternary_linter_t* linter, const char* source_code);

// =============================================================================
// TERNARY LINTER METRICS
// =============================================================================

/**
 * @brief Get warning count
 * @param linter The linter instance
 * @return The warning count
 */
size_t ternary_linter_get_warning_count(ternary_linter_t* linter);

/**
 * @brief Get error count
 * @param linter The linter instance
 * @return The error count
 */
size_t ternary_linter_get_error_count(ternary_linter_t* linter);

/**
 * @brief Check if linter has warnings
 * @param linter The linter instance
 * @return true if warnings, false otherwise
 */
bool ternary_linter_has_warnings(ternary_linter_t* linter);

/**
 * @brief Check if linter has errors
 * @param linter The linter instance
 * @return true if errors, false otherwise
 */
bool ternary_linter_has_errors(ternary_linter_t* linter);

// =============================================================================
// TERNARY LINTER ERROR HANDLING
// =============================================================================

/**
 * @brief Check if linter has error
 * @param linter The linter instance
 * @return true if error, false otherwise
 */
bool ternary_linter_has_error(ternary_linter_t* linter);

/**
 * @brief Get linter error message
 * @param linter The linter instance
 * @return The error message, or NULL if no error
 */
const char* ternary_linter_get_error_message(ternary_linter_t* linter);

/**
 * @brief Set linter error
 * @param linter The linter instance
 * @param message The error message
 */
void ternary_linter_set_error(ternary_linter_t* linter, const char* message);

// =============================================================================
// TERNARY LINTER UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print linting results
 * @param linter The linter instance
 */
void ternary_linter_print_linting(ternary_linter_t* linter);

/**
 * @brief Debug print linter information
 * @param linter The linter instance
 */
void ternary_linter_debug(ternary_linter_t* linter);

#endif // TERNARY_LINTER_H
