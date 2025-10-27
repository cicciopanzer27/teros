/**
 * @file ternary_disassembler.c
 * @brief Ternary disassembler implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_disassembler.h"
#include "trit.h"
#include "t3_isa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TERNARY DISASSEMBLER IMPLEMENTATION
// =============================================================================

ternary_disassembler_t* ternary_disassembler_create(void) {
    ternary_disassembler_t* disassembler = malloc(sizeof(ternary_disassembler_t));
    if (disassembler == NULL) return NULL;
    
    disassembler->instructions = NULL;
    disassembler->instruction_count = 0;
    disassembler->instruction_capacity = 0;
    disassembler->labels = NULL;
    disassembler->label_count = 0;
    disassembler->label_capacity = 0;
    disassembler->error = false;
    disassembler->error_message = NULL;
    
    return disassembler;
}

void ternary_disassembler_destroy(ternary_disassembler_t* disassembler) {
    if (disassembler != NULL) {
        if (disassembler->instructions != NULL) {
            free(disassembler->instructions);
        }
        if (disassembler->labels != NULL) {
            for (size_t i = 0; i < disassembler->label_count; i++) {
                if (disassembler->labels[i].name != NULL) {
                    free(disassembler->labels[i].name);
                }
            }
            free(disassembler->labels);
        }
        if (disassembler->error_message != NULL) {
            free(disassembler->error_message);
        }
        free(disassembler);
    }
}

// =============================================================================
// TERNARY DISASSEMBLER INSTRUCTION MANAGEMENT
// =============================================================================

bool ternary_disassembler_add_instruction(ternary_disassembler_t* disassembler, t3_instruction_t* instruction) {
    if (disassembler == NULL || instruction == NULL) {
        return false;
    }
    
    // Resize instruction array if needed
    if (disassembler->instruction_count >= disassembler->instruction_capacity) {
        size_t new_capacity = disassembler->instruction_capacity == 0 ? 16 : disassembler->instruction_capacity * 2;
        t3_instruction_t* new_instructions = realloc(disassembler->instructions, new_capacity * sizeof(t3_instruction_t));
        if (new_instructions == NULL) {
            return false;
        }
        disassembler->instructions = new_instructions;
        disassembler->instruction_capacity = new_capacity;
    }
    
    // Add instruction
    disassembler->instructions[disassembler->instruction_count] = *instruction;
    disassembler->instruction_count++;
    
    return true;
}

t3_instruction_t* ternary_disassembler_get_instruction(ternary_disassembler_t* disassembler, size_t index) {
    if (disassembler == NULL || index >= disassembler->instruction_count) {
        return NULL;
    }
    
    return &disassembler->instructions[index];
}

size_t ternary_disassembler_get_instruction_count(ternary_disassembler_t* disassembler) {
    if (disassembler == NULL) return 0;
    return disassembler->instruction_count;
}

// =============================================================================
// TERNARY DISASSEMBLER LABEL MANAGEMENT
// =============================================================================

bool ternary_disassembler_add_label(ternary_disassembler_t* disassembler, const char* name, int address) {
    if (disassembler == NULL || name == NULL || address < 0) {
        return false;
    }
    
    // Check if label already exists
    if (ternary_disassembler_find_label(disassembler, name) != -1) {
        return false;
    }
    
    // Resize label array if needed
    if (disassembler->label_count >= disassembler->label_capacity) {
        size_t new_capacity = disassembler->label_capacity == 0 ? 16 : disassembler->label_capacity * 2;
        ternary_label_t* new_labels = realloc(disassembler->labels, new_capacity * sizeof(ternary_label_t));
        if (new_labels == NULL) {
            return false;
        }
        disassembler->labels = new_labels;
        disassembler->label_capacity = new_capacity;
    }
    
    // Add label
    disassembler->labels[disassembler->label_count].name = strdup(name);
    disassembler->labels[disassembler->label_count].address = address;
    disassembler->label_count++;
    
    return true;
}

int ternary_disassembler_find_label(ternary_disassembler_t* disassembler, const char* name) {
    if (disassembler == NULL || name == NULL) {
        return -1;
    }
    
    for (size_t i = 0; i < disassembler->label_count; i++) {
        if (strcmp(disassembler->labels[i].name, name) == 0) {
            return disassembler->labels[i].address;
        }
    }
    
    return -1;
}

const char* ternary_disassembler_get_label_name(ternary_disassembler_t* disassembler, int address) {
    if (disassembler == NULL || address < 0) {
        return NULL;
    }
    
    for (size_t i = 0; i < disassembler->label_count; i++) {
        if (disassembler->labels[i].address == address) {
            return disassembler->labels[i].name;
        }
    }
    
    return NULL;
}

// =============================================================================
// TERNARY DISASSEMBLER DISASSEMBLY
// =============================================================================

bool ternary_disassembler_disassemble(ternary_disassembler_t* disassembler, const uint8_t* binary_data, size_t data_size) {
    if (disassembler == NULL || binary_data == NULL || data_size == 0) {
        return false;
    }
    
    // Clear existing data
    disassembler->instruction_count = 0;
    disassembler->label_count = 0;
    disassembler->error = false;
    
    // Simple disassembly - each instruction is 4 bytes
    size_t instruction_count = data_size / 4;
    
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t instruction;
        
        // Parse instruction from binary data
        instruction.opcode = binary_data[i * 4];
        instruction.operand1 = binary_data[i * 4 + 1];
        instruction.operand2 = binary_data[i * 4 + 2];
        instruction.operand3 = binary_data[i * 4 + 3];
        instruction.immediate = 0;
        instruction.valid = true;
        
        // Add instruction
        if (!ternary_disassembler_add_instruction(disassembler, &instruction)) {
            return false;
        }
    }
    
    return true;
}

// =============================================================================
// TERNARY DISASSEMBLER INSTRUCTION FORMATTING
// =============================================================================

char* ternary_disassembler_format_instruction(ternary_disassembler_t* disassembler, t3_instruction_t* instruction) {
    if (disassembler == NULL || instruction == NULL) {
        return NULL;
    }
    
    char* result = malloc(256);
    if (result == NULL) return NULL;
    
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

char* ternary_disassembler_format_instruction_with_labels(ternary_disassembler_t* disassembler, t3_instruction_t* instruction, int address) {
    if (disassembler == NULL || instruction == NULL) {
        return NULL;
    }
    
    char* result = malloc(256);
    if (result == NULL) return NULL;
    
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
            {
                const char* label_name = ternary_disassembler_get_label_name(disassembler, instruction->immediate);
                if (label_name != NULL) {
                    snprintf(result, 256, "%s @%s", opcode_str, label_name);
                } else {
                    snprintf(result, 256, "%s #%d", opcode_str, instruction->immediate);
                }
            }
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
// TERNARY DISASSEMBLER SOURCE GENERATION
// =============================================================================

char* ternary_disassembler_generate_source(ternary_disassembler_t* disassembler) {
    if (disassembler == NULL) {
        return NULL;
    }
    
    // Calculate total size needed
    size_t total_size = 0;
    for (size_t i = 0; i < disassembler->instruction_count; i++) {
        total_size += 256;  // Estimate per instruction
    }
    
    char* source = malloc(total_size);
    if (source == NULL) return NULL;
    
    source[0] = '\0';
    
    for (size_t i = 0; i < disassembler->instruction_count; i++) {
        t3_instruction_t* instruction = &disassembler->instructions[i];
        
        // Check for label at this address
        const char* label_name = ternary_disassembler_get_label_name(disassembler, i);
        if (label_name != NULL) {
            strcat(source, label_name);
            strcat(source, ":\n");
        }
        
        // Format instruction
        char* instruction_str = ternary_disassembler_format_instruction_with_labels(disassembler, instruction, i);
        if (instruction_str != NULL) {
            strcat(source, "  ");
            strcat(source, instruction_str);
            strcat(source, "\n");
            free(instruction_str);
        }
    }
    
    return source;
}

// =============================================================================
// TERNARY DISASSEMBLER ERROR HANDLING
// =============================================================================

bool ternary_disassembler_has_error(ternary_disassembler_t* disassembler) {
    return disassembler != NULL && disassembler->error;
}

const char* ternary_disassembler_get_error_message(ternary_disassembler_t* disassembler) {
    if (disassembler == NULL) return NULL;
    return disassembler->error_message;
}

void ternary_disassembler_set_error(ternary_disassembler_t* disassembler, const char* message) {
    if (disassembler == NULL) return;
    
    disassembler->error = true;
    if (disassembler->error_message != NULL) {
        free(disassembler->error_message);
    }
    disassembler->error_message = message != NULL ? strdup(message) : NULL;
}

// =============================================================================
// TERNARY DISASSEMBLER UTILITY FUNCTIONS
// =============================================================================

void ternary_disassembler_print_instructions(ternary_disassembler_t* disassembler) {
    if (disassembler == NULL) {
        printf("Ternary Disassembler Instructions: NULL\n");
        return;
    }
    
    printf("Ternary Disassembler Instructions (%zu):\n", disassembler->instruction_count);
    for (size_t i = 0; i < disassembler->instruction_count; i++) {
        t3_instruction_t* instruction = &disassembler->instructions[i];
        char* instruction_str = ternary_disassembler_format_instruction(disassembler, instruction);
        if (instruction_str != NULL) {
            printf("  [%zu]: %s\n", i, instruction_str);
            free(instruction_str);
        }
    }
}

void ternary_disassembler_print_labels(ternary_disassembler_t* disassembler) {
    if (disassembler == NULL) {
        printf("Ternary Disassembler Labels: NULL\n");
        return;
    }
    
    printf("Ternary Disassembler Labels (%zu):\n", disassembler->label_count);
    for (size_t i = 0; i < disassembler->label_count; i++) {
        printf("  %s: %d\n", disassembler->labels[i].name, disassembler->labels[i].address);
    }
}

void ternary_disassembler_debug(ternary_disassembler_t* disassembler) {
    if (disassembler == NULL) {
        printf("Ternary Disassembler Debug: NULL\n");
        return;
    }
    
    printf("Ternary Disassembler Debug:\n");
    printf("  Instruction Count: %zu\n", disassembler->instruction_count);
    printf("  Instruction Capacity: %zu\n", disassembler->instruction_capacity);
    printf("  Label Count: %zu\n", disassembler->label_count);
    printf("  Label Capacity: %zu\n", disassembler->label_capacity);
    printf("  Error: %s\n", disassembler->error ? "true" : "false");
    printf("  Error Message: %s\n", disassembler->error_message != NULL ? disassembler->error_message : "None");
    
    printf("  Instructions:\n");
    for (size_t i = 0; i < disassembler->instruction_count; i++) {
        t3_instruction_t* instruction = &disassembler->instructions[i];
        printf("    [%zu]: opcode=%d, op1=%d, op2=%d, op3=%d, imm=%d, valid=%s\n", i,
               instruction->opcode, instruction->operand1, instruction->operand2,
               instruction->operand3, instruction->immediate, instruction->valid ? "true" : "false");
    }
    
    printf("  Labels:\n");
    for (size_t i = 0; i < disassembler->label_count; i++) {
        printf("    %s: %d\n", disassembler->labels[i].name, disassembler->labels[i].address);
    }
}
