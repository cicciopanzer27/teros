/**
 * @file ternary_transpiler.h
 * @brief Ternary code transpiler header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_TRANSPILER_H
#define TERNARY_TRANSPILER_H

#include "trit.h"
#include "t3_isa.h"
#include "ternary_compiler.h"
#include "ternary_generator.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// =============================================================================
// TERNARY TRANSPILER TARGET TYPES
// =============================================================================

typedef enum {
    TERNARY_TARGET_C,
    TERNARY_TARGET_PYTHON,
    TERNARY_TARGET_JAVASCRIPT,
    TERNARY_TARGET_RUST,
    TERNARY_TARGET_GO
} ternary_target_t;

// =============================================================================
// TERNARY TRANSPILER STRUCTURE
// =============================================================================

typedef struct {
    ternary_compiler_t* compiler;
    ternary_generator_t* generator;
    bool error;
    char* error_message;
} ternary_transpiler_t;

// =============================================================================
// TERNARY TRANSPILER LIFECYCLE
// =============================================================================

ternary_transpiler_t* ternary_transpiler_create(void);
void ternary_transpiler_destroy(ternary_transpiler_t* transpiler);

// =============================================================================
// TERNARY TRANSPILER CODE TRANSPILATION
// =============================================================================

char* ternary_transpiler_transpile_code(ternary_transpiler_t* transpiler, const char* source_code, ternary_target_t target);

// =============================================================================
// TERNARY TRANSPILER C TARGET
// =============================================================================

char* ternary_transpiler_transpile_to_c(ternary_transpiler_t* transpiler);
char* ternary_transpiler_format_instruction_c(ternary_transpiler_t* transpiler, t3_instruction_t* instruction, size_t index);

// =============================================================================
// TERNARY TRANSPILER PYTHON TARGET
// =============================================================================

char* ternary_transpiler_transpile_to_python(ternary_transpiler_t* transpiler);
char* ternary_transpiler_format_instruction_python(ternary_transpiler_t* transpiler, t3_instruction_t* instruction, size_t index);

// =============================================================================
// TERNARY TRANSPILER JAVASCRIPT TARGET
// =============================================================================

char* ternary_transpiler_transpile_to_javascript(ternary_transpiler_t* transpiler);
char* ternary_transpiler_format_instruction_javascript(ternary_transpiler_t* transpiler, t3_instruction_t* instruction, size_t index);

// =============================================================================
// TERNARY TRANSPILER RUST TARGET
// =============================================================================

char* ternary_transpiler_transpile_to_rust(ternary_transpiler_t* transpiler);
char* ternary_transpiler_format_instruction_rust(ternary_transpiler_t* transpiler, t3_instruction_t* instruction, size_t index);

// =============================================================================
// TERNARY TRANSPILER GO TARGET
// =============================================================================

char* ternary_transpiler_transpile_to_go(ternary_transpiler_t* transpiler);
char* ternary_transpiler_format_instruction_go(ternary_transpiler_t* transpiler, t3_instruction_t* instruction, size_t index);

// =============================================================================
// TERNARY TRANSPILER ERROR HANDLING
// =============================================================================

bool ternary_transpiler_has_error(ternary_transpiler_t* transpiler);
const char* ternary_transpiler_get_error_message(ternary_transpiler_t* transpiler);
void ternary_transpiler_set_error(ternary_transpiler_t* transpiler, const char* message);

// =============================================================================
// TERNARY TRANSPILER UTILITY FUNCTIONS
// =============================================================================

void ternary_transpiler_print_transpilation(ternary_transpiler_t* transpiler);
void ternary_transpiler_debug(ternary_transpiler_t* transpiler);

#endif // TERNARY_TRANSPILER_H
