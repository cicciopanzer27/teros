/**
 * @file ternary_compiler.h
 * @brief Ternary compiler header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_COMPILER_H
#define TERNARY_COMPILER_H

#include "trit.h"
#include "t3_isa.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY COMPILER STRUCTURE
// =============================================================================

typedef struct {
    t3_instruction_t* instructions;
    size_t instruction_count;
    size_t instruction_capacity;
    bool error;
    char* error_message;
} ternary_compiler_t;

// =============================================================================
// TERNARY COMPILER CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary compiler
 * @return A new ternary compiler, or NULL on failure
 */
ternary_compiler_t* ternary_compiler_create(void);

/**
 * @brief Destroy a ternary compiler
 * @param compiler The compiler to destroy
 */
void ternary_compiler_destroy(ternary_compiler_t* compiler);

// =============================================================================
// TERNARY COMPILER INSTRUCTION MANAGEMENT
// =============================================================================

/**
 * @brief Add an instruction to the compiler
 * @param compiler The compiler instance
 * @param instruction The instruction to add
 * @return true on success, false on failure
 */
bool ternary_compiler_add_instruction(ternary_compiler_t* compiler, t3_instruction_t* instruction);

/**
 * @brief Get an instruction by index
 * @param compiler The compiler instance
 * @param index The instruction index
 * @return The instruction, or NULL on failure
 */
t3_instruction_t* ternary_compiler_get_instruction(ternary_compiler_t* compiler, size_t index);

/**
 * @brief Get the number of instructions
 * @param compiler The compiler instance
 * @return The number of instructions
 */
size_t ternary_compiler_get_instruction_count(ternary_compiler_t* compiler);

// =============================================================================
// TERNARY COMPILER PARSING
// =============================================================================

/**
 * @brief Parse a line of source code
 * @param compiler The compiler instance
 * @param line The line to parse
 * @return true on success, false on failure
 */
bool ternary_compiler_parse_line(ternary_compiler_t* compiler, const char* line);

/**
 * @brief Parse an instruction from a line
 * @param compiler The compiler instance
 * @param line The line to parse
 * @param instruction The instruction to fill
 * @return true on success, false on failure
 */
bool ternary_compiler_parse_instruction(ternary_compiler_t* compiler, const char* line, t3_instruction_t* instruction);

/**
 * @brief Parse an opcode string
 * @param opcode_str The opcode string
 * @return The opcode value, or 0xFF on failure
 */
uint8_t ternary_compiler_parse_opcode(const char* opcode_str);

// =============================================================================
// TERNARY COMPILER CODE GENERATION
// =============================================================================

/**
 * @brief Generate code from source
 * @param compiler The compiler instance
 * @param source_code The source code to compile
 * @return true on success, false on failure
 */
bool ternary_compiler_generate_code(ternary_compiler_t* compiler, const char* source_code);

// =============================================================================
// TERNARY COMPILER ERROR HANDLING
// =============================================================================

/**
 * @brief Check if compiler has error
 * @param compiler The compiler instance
 * @return true if error, false otherwise
 */
bool ternary_compiler_has_error(ternary_compiler_t* compiler);

/**
 * @brief Get compiler error message
 * @param compiler The compiler instance
 * @return The error message, or NULL if no error
 */
const char* ternary_compiler_get_error_message(ternary_compiler_t* compiler);

/**
 * @brief Set compiler error
 * @param compiler The compiler instance
 * @param message The error message
 */
void ternary_compiler_set_error(ternary_compiler_t* compiler, const char* message);

// =============================================================================
// TERNARY COMPILER UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print all instructions
 * @param compiler The compiler instance
 */
void ternary_compiler_print_instructions(ternary_compiler_t* compiler);

/**
 * @brief Debug print compiler information
 * @param compiler The compiler instance
 */
void ternary_compiler_debug(ternary_compiler_t* compiler);

#endif // TERNARY_COMPILER_H
