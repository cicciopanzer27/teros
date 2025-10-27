/**
 * @file ternary_compiler.c
 * @brief Ternary compiler implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_compiler.h"
#include "trit.h"
#include "trit_array.h"
#include "t3_isa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TERNARY COMPILER IMPLEMENTATION
// =============================================================================

ternary_compiler_t* ternary_compiler_create(void) {
    ternary_compiler_t* compiler = malloc(sizeof(ternary_compiler_t));
    if (compiler == NULL) return NULL;
    
    compiler->instructions = NULL;
    compiler->instruction_count = 0;
    compiler->instruction_capacity = 0;
    compiler->error = false;
    compiler->error_message = NULL;
    
    return compiler;
}

void ternary_compiler_destroy(ternary_compiler_t* compiler) {
    if (compiler != NULL) {
        if (compiler->instructions != NULL) {
            free(compiler->instructions);
        }
        if (compiler->error_message != NULL) {
            free(compiler->error_message);
        }
        free(compiler);
    }
}

// =============================================================================
// TERNARY COMPILER INSTRUCTION MANAGEMENT
// =============================================================================

bool ternary_compiler_add_instruction(ternary_compiler_t* compiler, t3_instruction_t* instruction) {
    if (compiler == NULL || instruction == NULL) {
        return false;
    }
    
    // Resize instruction array if needed
    if (compiler->instruction_count >= compiler->instruction_capacity) {
        size_t new_capacity = compiler->instruction_capacity == 0 ? 16 : compiler->instruction_capacity * 2;
        t3_instruction_t* new_instructions = realloc(compiler->instructions, new_capacity * sizeof(t3_instruction_t));
        if (new_instructions == NULL) {
            return false;
        }
        compiler->instructions = new_instructions;
        compiler->instruction_capacity = new_capacity;
    }
    
    // Add instruction
    compiler->instructions[compiler->instruction_count] = *instruction;
    compiler->instruction_count++;
    
    return true;
}

t3_instruction_t* ternary_compiler_get_instruction(ternary_compiler_t* compiler, size_t index) {
    if (compiler == NULL || index >= compiler->instruction_count) {
        return NULL;
    }
    
    return &compiler->instructions[index];
}

size_t ternary_compiler_get_instruction_count(ternary_compiler_t* compiler) {
    if (compiler == NULL) return 0;
    return compiler->instruction_count;
}

// =============================================================================
// TERNARY COMPILER PARSING
// =============================================================================

bool ternary_compiler_parse_line(ternary_compiler_t* compiler, const char* line) {
    if (compiler == NULL || line == NULL) {
        return false;
    }
    
    // Skip empty lines and comments
    if (strlen(line) == 0 || line[0] == ';' || line[0] == '#') {
        return true;
    }
    
    // Parse instruction
    t3_instruction_t instruction;
    if (!ternary_compiler_parse_instruction(compiler, line, &instruction)) {
        return false;
    }
    
    // Add instruction
    return ternary_compiler_add_instruction(compiler, &instruction);
}

bool ternary_compiler_parse_instruction(ternary_compiler_t* compiler, const char* line, t3_instruction_t* instruction) {
    if (compiler == NULL || line == NULL || instruction == NULL) {
        return false;
    }
    
    // Simple parsing - split by spaces
    char* line_copy = strdup(line);
    if (line_copy == NULL) return false;
    
    char* token = strtok(line_copy, " \t");
    if (token == NULL) {
        free(line_copy);
        return false;
    }
    
    // Parse opcode
    uint8_t opcode = ternary_compiler_parse_opcode(token);
    if (opcode == 0xFF) {
        free(line_copy);
        return false;
    }
    
    instruction->opcode = opcode;
    instruction->operand1 = 0;
    instruction->operand2 = 0;
    instruction->operand3 = 0;
    instruction->immediate = 0;
    instruction->valid = true;
    
    // Parse operands based on opcode
    switch (opcode) {
        case T3_OPCODE_LOAD:
        case T3_OPCODE_STORE:
        case T3_OPCODE_NOT:
        case T3_OPCODE_JZ:
        case T3_OPCODE_JNZ:
        case T3_OPCODE_PUSH:
        case T3_OPCODE_POP:
            // 2 operands
            token = strtok(NULL, " \t");
            if (token != NULL) {
                instruction->operand1 = atoi(token);
            }
            token = strtok(NULL, " \t");
            if (token != NULL) {
                instruction->operand2 = atoi(token);
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
            token = strtok(NULL, " \t");
            if (token != NULL) {
                instruction->operand1 = atoi(token);
            }
            token = strtok(NULL, " \t");
            if (token != NULL) {
                instruction->operand2 = atoi(token);
            }
            token = strtok(NULL, " \t");
            if (token != NULL) {
                instruction->operand3 = atoi(token);
            }
            break;
            
        case T3_OPCODE_JMP:
        case T3_OPCODE_CALL:
            // 1 operand (immediate)
            token = strtok(NULL, " \t");
            if (token != NULL) {
                instruction->immediate = atoi(token);
            }
            break;
            
        case T3_OPCODE_RET:
        case T3_OPCODE_HALT:
        case T3_OPCODE_NOP:
            // No operands
            break;
            
        default:
            free(line_copy);
            return false;
    }
    
    free(line_copy);
    return true;
}

uint8_t ternary_compiler_parse_opcode(const char* opcode_str) {
    if (opcode_str == NULL) return 0xFF;
    
    if (strcmp(opcode_str, "LOAD") == 0) return T3_OPCODE_LOAD;
    if (strcmp(opcode_str, "STORE") == 0) return T3_OPCODE_STORE;
    if (strcmp(opcode_str, "ADD") == 0) return T3_OPCODE_ADD;
    if (strcmp(opcode_str, "SUB") == 0) return T3_OPCODE_SUB;
    if (strcmp(opcode_str, "MUL") == 0) return T3_OPCODE_MUL;
    if (strcmp(opcode_str, "DIV") == 0) return T3_OPCODE_DIV;
    if (strcmp(opcode_str, "AND") == 0) return T3_OPCODE_AND;
    if (strcmp(opcode_str, "OR") == 0) return T3_OPCODE_OR;
    if (strcmp(opcode_str, "NOT") == 0) return T3_OPCODE_NOT;
    if (strcmp(opcode_str, "XOR") == 0) return T3_OPCODE_XOR;
    if (strcmp(opcode_str, "CMP") == 0) return T3_OPCODE_CMP;
    if (strcmp(opcode_str, "JMP") == 0) return T3_OPCODE_JMP;
    if (strcmp(opcode_str, "JZ") == 0) return T3_OPCODE_JZ;
    if (strcmp(opcode_str, "JNZ") == 0) return T3_OPCODE_JNZ;
    if (strcmp(opcode_str, "CALL") == 0) return T3_OPCODE_CALL;
    if (strcmp(opcode_str, "RET") == 0) return T3_OPCODE_RET;
    if (strcmp(opcode_str, "PUSH") == 0) return T3_OPCODE_PUSH;
    if (strcmp(opcode_str, "POP") == 0) return T3_OPCODE_POP;
    if (strcmp(opcode_str, "HALT") == 0) return T3_OPCODE_HALT;
    if (strcmp(opcode_str, "NOP") == 0) return T3_OPCODE_NOP;
    
    return 0xFF;
}

// =============================================================================
// TERNARY COMPILER CODE GENERATION
// =============================================================================

bool ternary_compiler_generate_code(ternary_compiler_t* compiler, const char* source_code) {
    if (compiler == NULL || source_code == NULL) {
        return false;
    }
    
    // Clear existing instructions
    compiler->instruction_count = 0;
    compiler->error = false;
    
    // Parse source code line by line
    char* source_copy = strdup(source_code);
    if (source_copy == NULL) return false;
    
    char* line = strtok(source_copy, "\n");
    while (line != NULL) {
        // Remove leading/trailing whitespace
        while (*line == ' ' || *line == '\t') line++;
        char* end = line + strlen(line) - 1;
        while (end > line && (*end == ' ' || *end == '\t' || *end == '\r')) end--;
        *(end + 1) = '\0';
        
        if (strlen(line) > 0) {
            if (!ternary_compiler_parse_line(compiler, line)) {
                compiler->error = true;
                compiler->error_message = strdup("Parse error");
                free(source_copy);
                return false;
            }
        }
        
        line = strtok(NULL, "\n");
    }
    
    free(source_copy);
    return true;
}

// =============================================================================
// TERNARY COMPILER ERROR HANDLING
// =============================================================================

bool ternary_compiler_has_error(ternary_compiler_t* compiler) {
    return compiler != NULL && compiler->error;
}

const char* ternary_compiler_get_error_message(ternary_compiler_t* compiler) {
    if (compiler == NULL) return NULL;
    return compiler->error_message;
}

void ternary_compiler_set_error(ternary_compiler_t* compiler, const char* message) {
    if (compiler == NULL) return;
    
    compiler->error = true;
    if (compiler->error_message != NULL) {
        free(compiler->error_message);
    }
    compiler->error_message = message != NULL ? strdup(message) : NULL;
}

// =============================================================================
// TERNARY COMPILER UTILITY FUNCTIONS
// =============================================================================

void ternary_compiler_print_instructions(ternary_compiler_t* compiler) {
    if (compiler == NULL) {
        printf("Ternary Compiler: NULL\n");
        return;
    }
    
    printf("Ternary Compiler Instructions (%zu):\n", compiler->instruction_count);
    for (size_t i = 0; i < compiler->instruction_count; i++) {
        t3_instruction_t* instruction = &compiler->instructions[i];
        printf("  [%zu]: %s %d %d %d %d\n", i,
               t3_opcode_to_string(instruction->opcode),
               instruction->operand1, instruction->operand2,
               instruction->operand3, instruction->immediate);
    }
}

void ternary_compiler_debug(ternary_compiler_t* compiler) {
    if (compiler == NULL) {
        printf("Ternary Compiler Debug: NULL\n");
        return;
    }
    
    printf("Ternary Compiler Debug:\n");
    printf("  Instruction Count: %zu\n", compiler->instruction_count);
    printf("  Instruction Capacity: %zu\n", compiler->instruction_capacity);
    printf("  Error: %s\n", compiler->error ? "true" : "false");
    printf("  Error Message: %s\n", compiler->error_message != NULL ? compiler->error_message : "None");
    
    printf("  Instructions:\n");
    for (size_t i = 0; i < compiler->instruction_count; i++) {
        t3_instruction_t* instruction = &compiler->instructions[i];
        printf("    [%zu]: opcode=%d, op1=%d, op2=%d, op3=%d, imm=%d, valid=%s\n", i,
               instruction->opcode, instruction->operand1, instruction->operand2,
               instruction->operand3, instruction->immediate, instruction->valid ? "true" : "false");
    }
}
