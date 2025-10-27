/**
 * @file ternary_generator.c
 * @brief Ternary code generator implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_generator.h"
#include "trit.h"
#include "t3_isa.h"
#include "ternary_compiler.h"
#include "ternary_optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TERNARY GENERATOR IMPLEMENTATION
// =============================================================================

ternary_generator_t* ternary_generator_create(void) {
    ternary_generator_t* generator = malloc(sizeof(ternary_generator_t));
    if (generator == NULL) return NULL;
    
    generator->compiler = ternary_compiler_create();
    if (generator->compiler == NULL) {
        free(generator);
        return NULL;
    }
    
    generator->optimizer = ternary_optimizer_create();
    if (generator->optimizer == NULL) {
        ternary_compiler_destroy(generator->compiler);
        free(generator);
        return NULL;
    }
    
    generator->error = false;
    generator->error_message = NULL;
    
    return generator;
}

void ternary_generator_destroy(ternary_generator_t* generator) {
    if (generator != NULL) {
        if (generator->compiler != NULL) {
            ternary_compiler_destroy(generator->compiler);
        }
        if (generator->optimizer != NULL) {
            ternary_optimizer_destroy(generator->optimizer);
        }
        if (generator->error_message != NULL) {
            free(generator->error_message);
        }
        free(generator);
    }
}

// =============================================================================
// TERNARY GENERATOR CODE GENERATION
// =============================================================================

bool ternary_generator_generate_code(ternary_generator_t* generator, const char* source_code) {
    if (generator == NULL || source_code == NULL) {
        return false;
    }
    
    generator->error = false;
    
    // Compile source code
    if (!ternary_compiler_generate_code(generator->compiler, source_code)) {
        generator->error = true;
        generator->error_message = strdup("Compilation failed");
        return false;
    }
    
    // Get compiled instructions
    size_t instruction_count = ternary_compiler_get_instruction_count(generator->compiler);
    if (instruction_count == 0) {
        generator->error = true;
        generator->error_message = strdup("No instructions generated");
        return false;
    }
    
    // Optimize instructions
    if (!ternary_optimizer_optimize(generator->optimizer, generator->compiler->instructions, instruction_count)) {
        generator->error = true;
        generator->error_message = strdup("Optimization failed");
        return false;
    }
    
    return true;
}

// =============================================================================
// TERNARY GENERATOR BINARY GENERATION
// =============================================================================

uint8_t* ternary_generator_generate_binary(ternary_generator_t* generator, size_t* binary_size) {
    if (generator == NULL || binary_size == NULL) {
        return NULL;
    }
    
    size_t instruction_count = ternary_compiler_get_instruction_count(generator->compiler);
    if (instruction_count == 0) {
        generator->error = true;
        generator->error_message = strdup("No instructions to generate binary from");
        return NULL;
    }
    
    // Each instruction is 4 bytes
    *binary_size = instruction_count * 4;
    uint8_t* binary = malloc(*binary_size);
    if (binary == NULL) {
        generator->error = true;
        generator->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    // Convert instructions to binary
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = ternary_compiler_get_instruction(generator->compiler, i);
        if (instruction != NULL && instruction->valid) {
            binary[i * 4] = instruction->opcode;
            binary[i * 4 + 1] = instruction->operand1;
            binary[i * 4 + 2] = instruction->operand2;
            binary[i * 4 + 3] = instruction->operand3;
        } else {
            // Fill with zeros for invalid instructions
            binary[i * 4] = 0;
            binary[i * 4 + 1] = 0;
            binary[i * 4 + 2] = 0;
            binary[i * 4 + 3] = 0;
        }
    }
    
    return binary;
}

// =============================================================================
// TERNARY GENERATOR ASSEMBLY GENERATION
// =============================================================================

char* ternary_generator_generate_assembly(ternary_generator_t* generator) {
    if (generator == NULL) {
        return NULL;
    }
    
    size_t instruction_count = ternary_compiler_get_instruction_count(generator->compiler);
    if (instruction_count == 0) {
        return strdup("");
    }
    
    // Calculate total size needed
    size_t total_size = instruction_count * 256;  // Estimate per instruction
    char* result = malloc(total_size);
    if (result == NULL) {
        generator->error = true;
        generator->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    result[0] = '\0';
    
    // Generate assembly for each instruction
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = ternary_compiler_get_instruction(generator->compiler, i);
        if (instruction != NULL && instruction->valid) {
            char* instruction_str = ternary_generator_format_instruction(generator, instruction, i);
            if (instruction_str != NULL) {
                strcat(result, instruction_str);
                strcat(result, "\n");
                free(instruction_str);
            }
        }
    }
    
    return result;
}

char* ternary_generator_format_instruction(ternary_generator_t* generator, t3_instruction_t* instruction, size_t index) {
    (void)index;
    if (generator == NULL || instruction == NULL) {
        return NULL;
    }
    
    char* result = malloc(256);
    if (result == NULL) {
        generator->error = true;
        generator->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    const char* opcode_str = t3_opcode_to_string(instruction->opcode);
    
    switch (instruction->opcode) {
        case T3_OPCODE_LOAD:
        case T3_OPCODE_STORE:
        case T3_OPCODE_NOT:
        case T3_OPCODE_JZ:
        case T3_OPCODE_JNZ:
        case T3_OPCODE_PUSH:
        case T3_OPCODE_POP:
            snprintf(result, 256, "%s R%d, R%d", opcode_str, instruction->operand1, instruction->operand2);
            break;
            
        case T3_OPCODE_ADD:
        case T3_OPCODE_SUB:
        case T3_OPCODE_MUL:
        case T3_OPCODE_DIV:
        case T3_OPCODE_AND:
        case T3_OPCODE_OR:
        case T3_OPCODE_XOR:
        case T3_OPCODE_CMP:
            snprintf(result, 256, "%s R%d, R%d, R%d", opcode_str, instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_JMP:
        case T3_OPCODE_CALL:
            snprintf(result, 256, "%s #%d", opcode_str, instruction->immediate);
            break;
            
        case T3_OPCODE_RET:
        case T3_OPCODE_HALT:
        case T3_OPCODE_NOP:
            snprintf(result, 256, "%s", opcode_str);
            break;
            
        default:
            snprintf(result, 256, "UNKNOWN %d %d %d %d", instruction->opcode, instruction->operand1, instruction->operand2, instruction->operand3);
            break;
    }
    
    return result;
}

// =============================================================================
// TERNARY GENERATOR OPTIMIZATION
// =============================================================================

bool ternary_generator_optimize_code(ternary_generator_t* generator, ternary_optimization_level_t level) {
    if (generator == NULL) {
        return false;
    }
    
    // Set optimization level
    ternary_optimizer_set_optimization_level(generator->optimizer, level);
    
    // Get instructions
    size_t instruction_count = ternary_compiler_get_instruction_count(generator->compiler);
    if (instruction_count == 0) {
        generator->error = true;
        generator->error_message = strdup("No instructions to optimize");
        return false;
    }
    
    // Optimize instructions
    if (!ternary_optimizer_optimize(generator->optimizer, generator->compiler->instructions, instruction_count)) {
        generator->error = true;
        generator->error_message = strdup("Optimization failed");
        return false;
    }
    
    return true;
}

// =============================================================================
// TERNARY GENERATOR METRICS
// =============================================================================

size_t ternary_generator_get_instruction_count(ternary_generator_t* generator) {
    if (generator == NULL) return 0;
    return ternary_compiler_get_instruction_count(generator->compiler);
}

size_t ternary_generator_get_binary_size(ternary_generator_t* generator) {
    if (generator == NULL) return 0;
    return ternary_generator_get_instruction_count(generator) * 4;
}

// =============================================================================
// TERNARY GENERATOR ERROR HANDLING
// =============================================================================

bool ternary_generator_has_error(ternary_generator_t* generator) {
    return generator != NULL && generator->error;
}

const char* ternary_generator_get_error_message(ternary_generator_t* generator) {
    if (generator == NULL) return NULL;
    return generator->error_message;
}

void ternary_generator_set_error(ternary_generator_t* generator, const char* message) {
    if (generator == NULL) return;
    
    generator->error = true;
    if (generator->error_message != NULL) {
        free(generator->error_message);
    }
    generator->error_message = message != NULL ? strdup(message) : NULL;
}

// =============================================================================
// TERNARY GENERATOR UTILITY FUNCTIONS
// =============================================================================

void ternary_generator_print_generation(ternary_generator_t* generator) {
    if (generator == NULL) {
        printf("Ternary Generator Generation: NULL\n");
        return;
    }
    
    printf("Ternary Generator Generation:\n");
    printf("  Instruction Count: %zu\n", ternary_generator_get_instruction_count(generator));
    printf("  Binary Size: %zu bytes\n", ternary_generator_get_binary_size(generator));
    printf("  Error: %s\n", generator->error ? "true" : "false");
}

void ternary_generator_debug(ternary_generator_t* generator) {
    if (generator == NULL) {
        printf("Ternary Generator Debug: NULL\n");
        return;
    }
    
    printf("Ternary Generator Debug:\n");
    printf("  Error: %s\n", generator->error ? "true" : "false");
    printf("  Error Message: %s\n", generator->error_message != NULL ? generator->error_message : "None");
    printf("  Instruction Count: %zu\n", ternary_generator_get_instruction_count(generator));
    printf("  Binary Size: %zu bytes\n", ternary_generator_get_binary_size(generator));
}
