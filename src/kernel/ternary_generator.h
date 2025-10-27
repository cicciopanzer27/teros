/**
 * @file ternary_generator.h
 * @brief Ternary code generator header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_GENERATOR_H
#define TERNARY_GENERATOR_H

#include "trit.h"
#include "t3_isa.h"
#include "ternary_compiler.h"
#include "ternary_optimizer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY GENERATOR STRUCTURE
// =============================================================================

typedef struct {
    ternary_compiler_t* compiler;
    ternary_optimizer_t* optimizer;
    bool error;
    char* error_message;
} ternary_generator_t;

// =============================================================================
// TERNARY GENERATOR CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary generator
 * @return A new ternary generator, or NULL on failure
 */
ternary_generator_t* ternary_generator_create(void);

/**
 * @brief Destroy a ternary generator
 * @param generator The generator to destroy
 */
void ternary_generator_destroy(ternary_generator_t* generator);

// =============================================================================
// TERNARY GENERATOR CODE GENERATION
// =============================================================================

/**
 * @brief Generate code from source
 * @param generator The generator instance
 * @param source_code The source code to generate from
 * @return true on success, false on failure
 */
bool ternary_generator_generate_code(ternary_generator_t* generator, const char* source_code);

// =============================================================================
// TERNARY GENERATOR BINARY GENERATION
// =============================================================================

/**
 * @brief Generate binary data from compiled code
 * @param generator The generator instance
 * @param binary_size Pointer to store the binary size
 * @return The binary data (must be freed by caller)
 */
uint8_t* ternary_generator_generate_binary(ternary_generator_t* generator, size_t* binary_size);

// =============================================================================
// TERNARY GENERATOR ASSEMBLY GENERATION
// =============================================================================

/**
 * @brief Generate assembly code from compiled code
 * @param generator The generator instance
 * @return The assembly code (must be freed by caller)
 */
char* ternary_generator_generate_assembly(ternary_generator_t* generator);

/**
 * @brief Format a single instruction as assembly
 * @param generator The generator instance
 * @param instruction The instruction to format
 * @param index The instruction index
 * @return The formatted instruction (must be freed by caller)
 */
char* ternary_generator_format_instruction(ternary_generator_t* generator, t3_instruction_t* instruction, size_t index);

// =============================================================================
// TERNARY GENERATOR OPTIMIZATION
// =============================================================================

/**
 * @brief Optimize generated code
 * @param generator The generator instance
 * @param level The optimization level
 * @return true on success, false on failure
 */
bool ternary_generator_optimize_code(ternary_generator_t* generator, ternary_optimization_level_t level);

// =============================================================================
// TERNARY GENERATOR METRICS
// =============================================================================

/**
 * @brief Get instruction count
 * @param generator The generator instance
 * @return The instruction count
 */
size_t ternary_generator_get_instruction_count(ternary_generator_t* generator);

/**
 * @brief Get binary size
 * @param generator The generator instance
 * @return The binary size in bytes
 */
size_t ternary_generator_get_binary_size(ternary_generator_t* generator);

// =============================================================================
// TERNARY GENERATOR ERROR HANDLING
// =============================================================================

/**
 * @brief Check if generator has error
 * @param generator The generator instance
 * @return true if error, false otherwise
 */
bool ternary_generator_has_error(ternary_generator_t* generator);

/**
 * @brief Get generator error message
 * @param generator The generator instance
 * @return The error message, or NULL if no error
 */
const char* ternary_generator_get_error_message(ternary_generator_t* generator);

/**
 * @brief Set generator error
 * @param generator The generator instance
 * @param message The error message
 */
void ternary_generator_set_error(ternary_generator_t* generator, const char* message);

// =============================================================================
// TERNARY GENERATOR UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print generation results
 * @param generator The generator instance
 */
void ternary_generator_print_generation(ternary_generator_t* generator);

/**
 * @brief Debug print generator information
 * @param generator The generator instance
 */
void ternary_generator_debug(ternary_generator_t* generator);

#endif // TERNARY_GENERATOR_H
