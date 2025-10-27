/**
 * @file ternary_validator.c
 * @brief Ternary code validator implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_validator.h"
#include "trit.h"
#include "t3_isa.h"
#include "ternary_compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TERNARY VALIDATOR IMPLEMENTATION
// =============================================================================

ternary_validator_t* ternary_validator_create(void) {
    ternary_validator_t* validator = malloc(sizeof(ternary_validator_t));
    if (validator == NULL) return NULL;
    
    validator->compiler = ternary_compiler_create();
    if (validator->compiler == NULL) {
        free(validator);
        return NULL;
    }
    
    validator->error = false;
    validator->error_message = NULL;
    validator->warning_count = 0;
    validator->error_count = 0;
    
    return validator;
}

void ternary_validator_destroy(ternary_validator_t* validator) {
    if (validator != NULL) {
        if (validator->compiler != NULL) {
            ternary_compiler_destroy(validator->compiler);
        }
        if (validator->error_message != NULL) {
            free(validator->error_message);
        }
        free(validator);
    }
}

// =============================================================================
// TERNARY VALIDATOR CODE VALIDATION
// =============================================================================

bool ternary_validator_validate_code(ternary_validator_t* validator, const char* source_code) {
    if (validator == NULL || source_code == NULL) {
        return false;
    }
    
    validator->error = false;
    validator->warning_count = 0;
    validator->error_count = 0;
    
    // Compile source code
    if (!ternary_compiler_generate_code(validator->compiler, source_code)) {
        validator->error = true;
        validator->error_message = strdup("Compilation failed");
        return false;
    }
    
    // Validate compiled instructions
    size_t instruction_count = ternary_compiler_get_instruction_count(validator->compiler);
    if (instruction_count == 0) {
        validator->error = true;
        validator->error_message = strdup("No instructions generated");
        return false;
    }
    
    // Validate instructions
    if (!ternary_validator_validate_instructions(validator, instruction_count)) {
        return false;
    }
    
    return true;
}

bool ternary_validator_validate_instructions(ternary_validator_t* validator, size_t instruction_count) {
    if (validator == NULL || instruction_count == 0) {
        return false;
    }
    
    // Validate each instruction
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = ternary_compiler_get_instruction(validator->compiler, i);
        if (instruction != NULL) {
            if (!ternary_validator_validate_instruction(validator, instruction, i)) {
                return false;
            }
        }
    }
    
    return true;
}

// =============================================================================
// TERNARY VALIDATOR INSTRUCTION VALIDATION
// =============================================================================

bool ternary_validator_validate_instruction(ternary_validator_t* validator, t3_instruction_t* instruction, size_t index) {
    if (validator == NULL || instruction == NULL) {
        return false;
    }
    
    // Validate opcode
    if (instruction->opcode >= T3_OPCODE_COUNT) {
        validator->error = true;
        validator->error_message = strdup("Invalid opcode");
        validator->error_count++;
        return false;
    }
    
    // Validate operands based on opcode
    switch (instruction->opcode) {
        case T3_OPCODE_LOAD:
        case T3_OPCODE_STORE:
        case T3_OPCODE_NOT:
        case T3_OPCODE_JZ:
        case T3_OPCODE_JNZ:
        case T3_OPCODE_PUSH:
        case T3_OPCODE_POP:
            // 2 operands
            if (!ternary_validator_validate_operand(validator, instruction->operand1, "operand1", index)) {
                return false;
            }
            if (!ternary_validator_validate_operand(validator, instruction->operand2, "operand2", index)) {
                return false;
            }
            break;
            
        case T3_OPCODE_ADD:
        case T3_OPCODE_SUB:
        case T3_OPCODE_MUL:
        case T3_OPCODE_DIV:
        case T3_OPCODE_AND:
        case T3_OPCODE_OR:
        case T3_OPCODE_XOR:
        case T3_OPCODE_CMP:
            // 3 operands
            if (!ternary_validator_validate_operand(validator, instruction->operand1, "operand1", index)) {
                return false;
            }
            if (!ternary_validator_validate_operand(validator, instruction->operand2, "operand2", index)) {
                return false;
            }
            if (!ternary_validator_validate_operand(validator, instruction->operand3, "operand3", index)) {
                return false;
            }
            break;
            
        case T3_OPCODE_JMP:
        case T3_OPCODE_CALL:
            // 1 operand (immediate)
            if (!ternary_validator_validate_immediate(validator, instruction->immediate, index)) {
                return false;
            }
            break;
            
        case T3_OPCODE_RET:
        case T3_OPCODE_HALT:
        case T3_OPCODE_NOP:
            // No operands
            break;
            
        default:
            validator->error = true;
            validator->error_message = strdup("Unknown opcode");
            validator->error_count++;
            return false;
    }
    
    return true;
}

bool ternary_validator_validate_operand(ternary_validator_t* validator, uint8_t operand, const char* name, size_t index) {
    if (validator == NULL) {
        return false;
    }
    
    if (operand >= T3_REGISTER_COUNT) {
        validator->error = true;
        validator->error_message = strdup("Invalid register index");
        validator->error_count++;
        return false;
    }
    
    return true;
}

bool ternary_validator_validate_immediate(ternary_validator_t* validator, int16_t immediate, size_t index) {
    if (validator == NULL) {
        return false;
    }
    
    // Check for reasonable immediate values
    if (immediate < -32768 || immediate > 32767) {
        validator->error = true;
        validator->error_message = strdup("Immediate value out of range");
        validator->error_count++;
        return false;
    }
    
    return true;
}

// =============================================================================
// TERNARY VALIDATOR SEMANTIC VALIDATION
// =============================================================================

bool ternary_validator_validate_semantics(ternary_validator_t* validator, size_t instruction_count) {
    if (validator == NULL || instruction_count == 0) {
        return false;
    }
    
    // Check for unreachable code
    if (!ternary_validator_check_unreachable_code(validator, instruction_count)) {
        return false;
    }
    
    // Check for infinite loops
    if (!ternary_validator_check_infinite_loops(validator, instruction_count)) {
        return false;
    }
    
    // Check for stack overflow
    if (!ternary_validator_check_stack_overflow(validator, instruction_count)) {
        return false;
    }
    
    return true;
}

bool ternary_validator_check_unreachable_code(ternary_validator_t* validator, size_t instruction_count) {
    if (validator == NULL || instruction_count == 0) {
        return false;
    }
    
    // Simple unreachable code detection
    bool* reachable = calloc(instruction_count, sizeof(bool));
    if (reachable == NULL) {
        validator->error = true;
        validator->error_message = strdup("Memory allocation failed");
        return false;
    }
    
    reachable[0] = true;  // First instruction is always reachable
    
    // Mark reachable instructions
    for (size_t i = 0; i < instruction_count; i++) {
        if (reachable[i]) {
            t3_instruction_t* instruction = ternary_compiler_get_instruction(validator->compiler, i);
            if (instruction != NULL && instruction->valid) {
                switch (instruction->opcode) {
                    case T3_OPCODE_JMP:
                    case T3_OPCODE_JZ:
                    case T3_OPCODE_JNZ:
                    case T3_OPCODE_CALL:
                        // Mark target as reachable
                        if (instruction->immediate >= 0 && instruction->immediate < (int)instruction_count) {
                            reachable[instruction->immediate] = true;
                        }
                        break;
                        
                    case T3_OPCODE_RET:
                    case T3_OPCODE_HALT:
                        // Don't mark next instruction as reachable
                        break;
                        
                    default:
                        // Mark next instruction as reachable
                        if (i + 1 < instruction_count) {
                            reachable[i + 1] = true;
                        }
                        break;
                }
            }
        }
    }
    
    // Check for unreachable instructions
    for (size_t i = 0; i < instruction_count; i++) {
        if (!reachable[i]) {
            validator->warning_count++;
            printf("Warning: Instruction %zu is unreachable\n", i);
        }
    }
    
    free(reachable);
    return true;
}

bool ternary_validator_check_infinite_loops(ternary_validator_t* validator, size_t instruction_count) {
    if (validator == NULL || instruction_count == 0) {
        return false;
    }
    
    // Simple infinite loop detection
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = ternary_compiler_get_instruction(validator->compiler, i);
        if (instruction != NULL && instruction->valid) {
            if (instruction->opcode == T3_OPCODE_JMP && instruction->immediate == (int)i) {
                validator->warning_count++;
                printf("Warning: Potential infinite loop at instruction %zu\n", i);
            }
        }
    }
    
    return true;
}

bool ternary_validator_check_stack_overflow(ternary_validator_t* validator, size_t instruction_count) {
    if (validator == NULL || instruction_count == 0) {
        return false;
    }
    
    // Simple stack overflow detection
    int stack_depth = 0;
    int max_stack_depth = 0;
    
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = ternary_compiler_get_instruction(validator->compiler, i);
        if (instruction != NULL && instruction->valid) {
            switch (instruction->opcode) {
                case T3_OPCODE_PUSH:
                case T3_OPCODE_CALL:
                    stack_depth++;
                    if (stack_depth > max_stack_depth) {
                        max_stack_depth = stack_depth;
                    }
                    break;
                    
                case T3_OPCODE_POP:
                case T3_OPCODE_RET:
                    stack_depth--;
                    if (stack_depth < 0) {
                        validator->error = true;
                        validator->error_message = strdup("Stack underflow");
                        validator->error_count++;
                        return false;
                    }
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    if (max_stack_depth > 100) {  // Arbitrary limit
        validator->warning_count++;
        printf("Warning: Deep stack usage detected (depth: %d)\n", max_stack_depth);
    }
    
    return true;
}

// =============================================================================
// TERNARY VALIDATOR METRICS
// =============================================================================

size_t ternary_validator_get_warning_count(ternary_validator_t* validator) {
    if (validator == NULL) return 0;
    return validator->warning_count;
}

size_t ternary_validator_get_error_count(ternary_validator_t* validator) {
    if (validator == NULL) return 0;
    return validator->error_count;
}

bool ternary_validator_has_warnings(ternary_validator_t* validator) {
    return validator != NULL && validator->warning_count > 0;
}

bool ternary_validator_has_errors(ternary_validator_t* validator) {
    return validator != NULL && validator->error_count > 0;
}

// =============================================================================
// TERNARY VALIDATOR ERROR HANDLING
// =============================================================================

bool ternary_validator_has_error(ternary_validator_t* validator) {
    return validator != NULL && validator->error;
}

const char* ternary_validator_get_error_message(ternary_validator_t* validator) {
    if (validator == NULL) return NULL;
    return validator->error_message;
}

void ternary_validator_set_error(ternary_validator_t* validator, const char* message) {
    if (validator == NULL) return;
    
    validator->error = true;
    if (validator->error_message != NULL) {
        free(validator->error_message);
    }
    validator->error_message = message != NULL ? strdup(message) : NULL;
}

// =============================================================================
// TERNARY VALIDATOR UTILITY FUNCTIONS
// =============================================================================

void ternary_validator_print_validation(ternary_validator_t* validator) {
    if (validator == NULL) {
        printf("Ternary Validator Validation: NULL\n");
        return;
    }
    
    printf("Ternary Validator Validation:\n");
    printf("  Warnings: %zu\n", validator->warning_count);
    printf("  Errors: %zu\n", validator->error_count);
    printf("  Valid: %s\n", (validator->error_count == 0) ? "true" : "false");
}

void ternary_validator_debug(ternary_validator_t* validator) {
    if (validator == NULL) {
        printf("Ternary Validator Debug: NULL\n");
        return;
    }
    
    printf("Ternary Validator Debug:\n");
    printf("  Error: %s\n", validator->error ? "true" : "false");
    printf("  Error Message: %s\n", validator->error_message != NULL ? validator->error_message : "None");
    printf("  Warning Count: %zu\n", validator->warning_count);
    printf("  Error Count: %zu\n", validator->error_count);
}
