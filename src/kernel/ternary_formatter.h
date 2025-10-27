/**
 * @file ternary_formatter.h
 * @brief Ternary code formatter header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_FORMATTER_H
#define TERNARY_FORMATTER_H

#include "trit.h"
#include "t3_isa.h"
#include "ternary_compiler.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY FORMATTER STRUCTURE
// =============================================================================

typedef struct {
    ternary_compiler_t* compiler;
    int indent_level;
    int indent_size;
    int line_length;
    bool error;
    char* error_message;
} ternary_formatter_t;

// =============================================================================
// TERNARY FORMATTER CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary formatter
 * @return A new ternary formatter, or NULL on failure
 */
ternary_formatter_t* ternary_formatter_create(void);

/**
 * @brief Destroy a ternary formatter
 * @param formatter The formatter to destroy
 */
void ternary_formatter_destroy(ternary_formatter_t* formatter);

// =============================================================================
// TERNARY FORMATTER CONFIGURATION
// =============================================================================

/**
 * @brief Set indent size
 * @param formatter The formatter instance
 * @param indent_size The indent size
 */
void ternary_formatter_set_indent_size(ternary_formatter_t* formatter, int indent_size);

/**
 * @brief Set line length
 * @param formatter The formatter instance
 * @param line_length The line length
 */
void ternary_formatter_set_line_length(ternary_formatter_t* formatter, int line_length);

/**
 * @brief Get indent size
 * @param formatter The formatter instance
 * @return The indent size
 */
int ternary_formatter_get_indent_size(ternary_formatter_t* formatter);

/**
 * @brief Get line length
 * @param formatter The formatter instance
 * @return The line length
 */
int ternary_formatter_get_line_length(ternary_formatter_t* formatter);

// =============================================================================
// TERNARY FORMATTER CODE FORMATTING
// =============================================================================

/**
 * @brief Format source code
 * @param formatter The formatter instance
 * @param source_code The source code to format
 * @return The formatted code (must be freed by caller)
 */
char* ternary_formatter_format_code(ternary_formatter_t* formatter, const char* source_code);

/**
 * @brief Format compiled instructions
 * @param formatter The formatter instance
 * @return The formatted code (must be freed by caller)
 */
char* ternary_formatter_format_instructions(ternary_formatter_t* formatter);

/**
 * @brief Format a single instruction
 * @param formatter The formatter instance
 * @param instruction The instruction to format
 * @param index The instruction index
 * @return The formatted instruction (must be freed by caller)
 */
char* ternary_formatter_format_instruction(ternary_formatter_t* formatter, t3_instruction_t* instruction, size_t index);

// =============================================================================
// TERNARY FORMATTER INDENTATION
// =============================================================================

/**
 * @brief Increase indent level
 * @param formatter The formatter instance
 */
void ternary_formatter_increase_indent(ternary_formatter_t* formatter);

/**
 * @brief Decrease indent level
 * @param formatter The formatter instance
 */
void ternary_formatter_decrease_indent(ternary_formatter_t* formatter);

/**
 * @brief Reset indent level
 * @param formatter The formatter instance
 */
void ternary_formatter_reset_indent(ternary_formatter_t* formatter);

/**
 * @brief Get current indent level
 * @param formatter The formatter instance
 * @return The current indent level
 */
int ternary_formatter_get_indent_level(ternary_formatter_t* formatter);

// =============================================================================
// TERNARY FORMATTER LINE BREAKING
// =============================================================================

/**
 * @brief Check if line should be broken
 * @param formatter The formatter instance
 * @param line The line to check
 * @return true if line should be broken, false otherwise
 */
bool ternary_formatter_should_break_line(ternary_formatter_t* formatter, const char* line);

/**
 * @brief Break a line at appropriate points
 * @param formatter The formatter instance
 * @param line The line to break
 * @return The broken line (must be freed by caller)
 */
char* ternary_formatter_break_line(ternary_formatter_t* formatter, const char* line);

// =============================================================================
// TERNARY FORMATTER COMMENT HANDLING
// =============================================================================

/**
 * @brief Add a comment to a line
 * @param formatter The formatter instance
 * @param line The line to add comment to
 * @param comment The comment to add
 * @return The line with comment (must be freed by caller)
 */
char* ternary_formatter_add_comment(ternary_formatter_t* formatter, const char* line, const char* comment);

// =============================================================================
// TERNARY FORMATTER ERROR HANDLING
// =============================================================================

/**
 * @brief Check if formatter has error
 * @param formatter The formatter instance
 * @return true if error, false otherwise
 */
bool ternary_formatter_has_error(ternary_formatter_t* formatter);

/**
 * @brief Get formatter error message
 * @param formatter The formatter instance
 * @return The error message, or NULL if no error
 */
const char* ternary_formatter_get_error_message(ternary_formatter_t* formatter);

/**
 * @brief Set formatter error
 * @param formatter The formatter instance
 * @param message The error message
 */
void ternary_formatter_set_error(ternary_formatter_t* formatter, const char* message);

// =============================================================================
// TERNARY FORMATTER UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print formatter configuration
 * @param formatter The formatter instance
 */
void ternary_formatter_print_config(ternary_formatter_t* formatter);

/**
 * @brief Debug print formatter information
 * @param formatter The formatter instance
 */
void ternary_formatter_debug(ternary_formatter_t* formatter);

#endif // TERNARY_FORMATTER_H
