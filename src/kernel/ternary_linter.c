/**
 * @file ternary_linter.c
 * @brief Ternary code linter implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_linter.h"
#include "trit.h"
#include "t3_isa.h"
#include "ternary_compiler.h"
#include "ternary_validator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TERNARY LINTER IMPLEMENTATION
// =============================================================================

ternary_linter_t* ternary_linter_create(void) {
    ternary_linter_t* linter = malloc(sizeof(ternary_linter_t));
    if (linter == NULL) return NULL;
    
    linter->compiler = ternary_compiler_create();
    if (linter->compiler == NULL) {
        free(linter);
        return NULL;
    }
    
    linter->validator = ternary_validator_create();
    if (linter->validator == NULL) {
        ternary_compiler_destroy(linter->compiler);
        free(linter);
        return NULL;
    }
    
    linter->error = false;
    linter->error_message = NULL;
    linter->warning_count = 0;
    linter->error_count = 0;
    
    return linter;
}

void ternary_linter_destroy(ternary_linter_t* linter) {
    if (linter != NULL) {
        if (linter->compiler != NULL) {
            ternary_compiler_destroy(linter->compiler);
        }
        if (linter->validator != NULL) {
            ternary_validator_destroy(linter->validator);
        }
        if (linter->error_message != NULL) {
            free(linter->error_message);
        }
        free(linter);
    }
}

// =============================================================================
// TERNARY LINTER CODE LINTING
// =============================================================================

bool ternary_linter_lint_code(ternary_linter_t* linter, const char* source_code) {
    if (linter == NULL || source_code == NULL) {
        return false;
    }
    
    linter->error = false;
    linter->warning_count = 0;
    linter->error_count = 0;
    
    // Compile source code
    if (!ternary_compiler_generate_code(linter->compiler, source_code)) {
        linter->error = true;
        linter->error_message = strdup("Compilation failed");
        return false;
    }
    
    // Validate code
    if (!ternary_validator_validate_code(linter->validator, source_code)) {
        linter->error = true;
        linter->error_message = strdup("Validation failed");
        return false;
    }
    
    // Lint instructions
    size_t instruction_count = ternary_compiler_get_instruction_count(linter->compiler);
    if (instruction_count == 0) {
        linter->error = true;
        linter->error_message = strdup("No instructions generated");
        return false;
    }
    
    // Lint each instruction
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = ternary_compiler_get_instruction(linter->compiler, i);
        if (instruction != NULL) {
            ternary_linter_lint_instruction(linter, instruction, i);
        }
    }
    
    // Update counts from validator
    linter->warning_count += ternary_validator_get_warning_count(linter->validator);
    linter->error_count += ternary_validator_get_error_count(linter->validator);
    
    return true;
}

void ternary_linter_lint_instruction(ternary_linter_t* linter, t3_instruction_t* instruction, size_t index) {
    if (linter == NULL || instruction == NULL) {
        return;
    }
    
    // Check for common issues
    switch (instruction->opcode) {
        case T3_OPCODE_LOAD:
            if (instruction->operand1 == instruction->operand2) {
                linter->warning_count++;
                printf("Warning: Redundant load at instruction %zu\n", index);
            }
            break;
            
        case T3_OPCODE_STORE:
            if (instruction->operand1 == instruction->operand2) {
                linter->warning_count++;
                printf("Warning: Redundant store at instruction %zu\n", index);
            }
            break;
            
        case T3_OPCODE_ADD:
        case T3_OPCODE_SUB:
        case T3_OPCODE_MUL:
        case T3_OPCODE_DIV:
            if (instruction->operand2 == 0 && instruction->operand3 == 0) {
                linter->warning_count++;
                printf("Warning: Operation with zero operands at instruction %zu\n", index);
            }
            break;
            
        case T3_OPCODE_AND:
        case T3_OPCODE_OR:
        case T3_OPCODE_XOR:
            if (instruction->operand2 == instruction->operand3) {
                linter->warning_count++;
                printf("Warning: Operation with same operands at instruction %zu\n", index);
            }
            break;
            
        case T3_OPCODE_JMP:
        case T3_OPCODE_CALL:
            if (instruction->immediate < 0) {
                linter->error_count++;
                printf("Error: Negative jump target at instruction %zu\n", index);
            }
            break;
            
        case T3_OPCODE_JZ:
        case T3_OPCODE_JNZ:
            if (instruction->operand1 >= T3_REGISTER_COUNT) {
                linter->error_count++;
                printf("Error: Invalid register index at instruction %zu\n", index);
            }
            break;
            
        default:
            break;
    }
}

// =============================================================================
// TERNARY LINTER STYLE CHECKS
// =============================================================================

bool ternary_linter_check_style(ternary_linter_t* linter, const char* source_code) {
    if (linter == NULL || source_code == NULL) {
        return false;
    }
    
    // Check for common style issues
    if (!ternary_linter_check_naming_conventions(linter, source_code)) {
        return false;
    }
    
    if (!ternary_linter_check_indentation(linter, source_code)) {
        return false;
    }
    
    if (!ternary_linter_check_line_length(linter, source_code)) {
        return false;
    }
    
    return true;
}

bool ternary_linter_check_naming_conventions(ternary_linter_t* linter, const char* source_code) {
    if (linter == NULL || source_code == NULL) {
        return false;
    }
    
    // Simple naming convention checks
    // In a real implementation, this would be more sophisticated
    
    return true;
}

bool ternary_linter_check_indentation(ternary_linter_t* linter, const char* source_code) {
    if (linter == NULL || source_code == NULL) {
        return false;
    }
    
    // Simple indentation checks
    // In a real implementation, this would be more sophisticated
    
    return true;
}

bool ternary_linter_check_line_length(ternary_linter_t* linter, const char* source_code) {
    if (linter == NULL || source_code == NULL) {
        return false;
    }
    
    // Simple line length checks
    // In a real implementation, this would be more sophisticated
    
    return true;
}

// =============================================================================
// TERNARY LINTER PERFORMANCE CHECKS
// =============================================================================

bool ternary_linter_check_performance(ternary_linter_t* linter, const char* source_code) {
    if (linter == NULL || source_code == NULL) {
        return false;
    }
    
    // Check for performance issues
    if (!ternary_linter_check_inefficient_operations(linter, source_code)) {
        return false;
    }
    
    if (!ternary_linter_check_memory_usage(linter, source_code)) {
        return false;
    }
    
    return true;
}

bool ternary_linter_check_inefficient_operations(ternary_linter_t* linter, const char* source_code) {
    if (linter == NULL || source_code == NULL) {
        return false;
    }
    
    // Check for inefficient operations
    // In a real implementation, this would be more sophisticated
    
    return true;
}

bool ternary_linter_check_memory_usage(ternary_linter_t* linter, const char* source_code) {
    if (linter == NULL || source_code == NULL) {
        return false;
    }
    
    // Check for memory usage issues
    // In a real implementation, this would be more sophisticated
    
    return true;
}

// =============================================================================
// TERNARY LINTER SECURITY CHECKS
// =============================================================================

bool ternary_linter_check_security(ternary_linter_t* linter, const char* source_code) {
    if (linter == NULL || source_code == NULL) {
        return false;
    }
    
    // Check for security issues
    if (!ternary_linter_check_buffer_overflows(linter, source_code)) {
        return false;
    }
    
    if (!ternary_linter_check_integer_overflows(linter, source_code)) {
        return false;
    }
    
    return true;
}

bool ternary_linter_check_buffer_overflows(ternary_linter_t* linter, const char* source_code) {
    if (linter == NULL || source_code == NULL) {
        return false;
    }
    
    // Check for buffer overflow issues
    // In a real implementation, this would be more sophisticated
    
    return true;
}

bool ternary_linter_check_integer_overflows(ternary_linter_t* linter, const char* source_code) {
    if (linter == NULL || source_code == NULL) {
        return false;
    }
    
    // Check for integer overflow issues
    // In a real implementation, this would be more sophisticated
    
    return true;
}

// =============================================================================
// TERNARY LINTER METRICS
// =============================================================================

size_t ternary_linter_get_warning_count(ternary_linter_t* linter) {
    if (linter == NULL) return 0;
    return linter->warning_count;
}

size_t ternary_linter_get_error_count(ternary_linter_t* linter) {
    if (linter == NULL) return 0;
    return linter->error_count;
}

bool ternary_linter_has_warnings(ternary_linter_t* linter) {
    return linter != NULL && linter->warning_count > 0;
}

bool ternary_linter_has_errors(ternary_linter_t* linter) {
    return linter != NULL && linter->error_count > 0;
}

// =============================================================================
// TERNARY LINTER ERROR HANDLING
// =============================================================================

bool ternary_linter_has_error(ternary_linter_t* linter) {
    return linter != NULL && linter->error;
}

const char* ternary_linter_get_error_message(ternary_linter_t* linter) {
    if (linter == NULL) return NULL;
    return linter->error_message;
}

void ternary_linter_set_error(ternary_linter_t* linter, const char* message) {
    if (linter == NULL) return;
    
    linter->error = true;
    if (linter->error_message != NULL) {
        free(linter->error_message);
    }
    linter->error_message = message != NULL ? strdup(message) : NULL;
}

// =============================================================================
// TERNARY LINTER UTILITY FUNCTIONS
// =============================================================================

void ternary_linter_print_linting(ternary_linter_t* linter) {
    if (linter == NULL) {
        printf("Ternary Linter Linting: NULL\n");
        return;
    }
    
    printf("Ternary Linter Linting:\n");
    printf("  Warnings: %zu\n", linter->warning_count);
    printf("  Errors: %zu\n", linter->error_count);
    printf("  Valid: %s\n", (linter->error_count == 0) ? "true" : "false");
}

void ternary_linter_debug(ternary_linter_t* linter) {
    if (linter == NULL) {
        printf("Ternary Linter Debug: NULL\n");
        return;
    }
    
    printf("Ternary Linter Debug:\n");
    printf("  Error: %s\n", linter->error ? "true" : "false");
    printf("  Error Message: %s\n", linter->error_message != NULL ? linter->error_message : "None");
    printf("  Warning Count: %zu\n", linter->warning_count);
    printf("  Error Count: %zu\n", linter->error_count);
}
