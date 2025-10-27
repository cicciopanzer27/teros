/**
 * @file ternary_assembler.c
 * @brief Ternary assembler implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_assembler.h"
#include "trit.h"
#include "t3_isa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TERNARY ASSEMBLER IMPLEMENTATION
// =============================================================================

ternary_assembler_t* ternary_assembler_create(void) {
    ternary_assembler_t* assembler = malloc(sizeof(ternary_assembler_t));
    if (assembler == NULL) return NULL;
    
    assembler->instructions = NULL;
    assembler->instruction_count = 0;
    assembler->instruction_capacity = 0;
    assembler->labels = NULL;
    assembler->label_count = 0;
    assembler->label_capacity = 0;
    assembler->error = false;
    assembler->error_message = NULL;
    
    return assembler;
}

void ternary_assembler_destroy(ternary_assembler_t* assembler) {
    if (assembler != NULL) {
        if (assembler->instructions != NULL) {
            free(assembler->instructions);
        }
        if (assembler->labels != NULL) {
            for (size_t i = 0; i < assembler->label_count; i++) {
                if (assembler->labels[i].name != NULL) {
                    free(assembler->labels[i].name);
                }
            }
            free(assembler->labels);
        }
        if (assembler->error_message != NULL) {
            free(assembler->error_message);
        }
        free(assembler);
    }
}

// =============================================================================
// TERNARY ASSEMBLER INSTRUCTION MANAGEMENT
// =============================================================================

bool ternary_assembler_add_instruction(ternary_assembler_t* assembler, t3_instruction_t* instruction) {
    if (assembler == NULL || instruction == NULL) {
        return false;
    }
    
    // Resize instruction array if needed
    if (assembler->instruction_count >= assembler->instruction_capacity) {
        size_t new_capacity = assembler->instruction_capacity == 0 ? 16 : assembler->instruction_capacity * 2;
        t3_instruction_t* new_instructions = realloc(assembler->instructions, new_capacity * sizeof(t3_instruction_t));
        if (new_instructions == NULL) {
            return false;
        }
        assembler->instructions = new_instructions;
        assembler->instruction_capacity = new_capacity;
    }
    
    // Add instruction
    assembler->instructions[assembler->instruction_count] = *instruction;
    assembler->instruction_count++;
    
    return true;
}

t3_instruction_t* ternary_assembler_get_instruction(ternary_assembler_t* assembler, size_t index) {
    if (assembler == NULL || index >= assembler->instruction_count) {
        return NULL;
    }
    
    return &assembler->instructions[index];
}

size_t ternary_assembler_get_instruction_count(ternary_assembler_t* assembler) {
    if (assembler == NULL) return 0;
    return assembler->instruction_count;
}

// =============================================================================
// TERNARY ASSEMBLER LABEL MANAGEMENT
// =============================================================================

bool ternary_assembler_add_label(ternary_assembler_t* assembler, const char* name, int address) {
    if (assembler == NULL || name == NULL || address < 0) {
        return false;
    }
    
    // Check if label already exists
    if (ternary_assembler_find_label(assembler, name) != -1) {
        return false;
    }
    
    // Resize label array if needed
    if (assembler->label_count >= assembler->label_capacity) {
        size_t new_capacity = assembler->label_capacity == 0 ? 16 : assembler->label_capacity * 2;
        ternary_label_t* new_labels = realloc(assembler->labels, new_capacity * sizeof(ternary_label_t));
        if (new_labels == NULL) {
            return false;
        }
        assembler->labels = new_labels;
        assembler->label_capacity = new_capacity;
    }
    
    // Add label
    assembler->labels[assembler->label_count].name = strdup(name);
    assembler->labels[assembler->label_count].address = address;
    assembler->label_count++;
    
    return true;
}

int ternary_assembler_find_label(ternary_assembler_t* assembler, const char* name) {
    if (assembler == NULL || name == NULL) {
        return -1;
    }
    
    for (size_t i = 0; i < assembler->label_count; i++) {
        if (strcmp(assembler->labels[i].name, name) == 0) {
            return assembler->labels[i].address;
        }
    }
    
    return -1;
}

const char* ternary_assembler_get_label_name(ternary_assembler_t* assembler, int address) {
    if (assembler == NULL || address < 0) {
        return NULL;
    }
    
    for (size_t i = 0; i < assembler->label_count; i++) {
        if (assembler->labels[i].address == address) {
            return assembler->labels[i].name;
        }
    }
    
    return NULL;
}

// =============================================================================
// TERNARY ASSEMBLER PARSING
// =============================================================================

bool ternary_assembler_parse_line(ternary_assembler_t* assembler, const char* line) {
    if (assembler == NULL || line == NULL) {
        return false;
    }
    
    // Skip empty lines and comments
    if (strlen(line) == 0 || line[0] == ';' || line[0] == '#') {
        return true;
    }
    
    // Check for label
    char* colon = strchr(line, ':');
    if (colon != NULL) {
        // Extract label name
        size_t label_len = colon - line;
        char* label_name = malloc(label_len + 1);
        if (label_name == NULL) return false;
        
        strncpy(label_name, line, label_len);
        label_name[label_len] = '\0';
        
        // Add label
        bool success = ternary_assembler_add_label(assembler, label_name, assembler->instruction_count);
        free(label_name);
        
        if (!success) {
            return false;
        }
        
        // Parse instruction after label
        char* instruction_line = colon + 1;
        while (*instruction_line == ' ' || *instruction_line == '\t') {
            instruction_line++;
        }
        
        if (strlen(instruction_line) > 0) {
            return ternary_assembler_parse_instruction(assembler, instruction_line);
        }
        
        return true;
    }
    
    // Parse instruction
    return ternary_assembler_parse_instruction(assembler, line);
}

bool ternary_assembler_parse_instruction(ternary_assembler_t* assembler, const char* line) {
    if (assembler == NULL || line == NULL) {
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
    uint8_t opcode = ternary_assembler_parse_opcode(token);
    if (opcode == 0xFF) {
        free(line_copy);
        return false;
    }
    
    t3_instruction_t instruction;
    instruction.opcode = opcode;
    instruction.operand1 = 0;
    instruction.operand2 = 0;
    instruction.operand3 = 0;
    instruction.immediate = 0;
    instruction.valid = true;
    
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
                instruction.operand1 = atoi(token);
            }
            token = strtok(NULL, " \t");
            if (token != NULL) {
                if (token[0] == '@') {
                    // Label reference
                    int label_address = ternary_assembler_find_label(assembler, token + 1);
                    if (label_address == -1) {
                        // Forward reference - will be resolved later
                        instruction.operand2 = -1;  // Mark as unresolved
                    } else {
                        instruction.operand2 = label_address;
                    }
                } else {
                    instruction.operand2 = atoi(token);
                }
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
                instruction.operand1 = atoi(token);
            }
            token = strtok(NULL, " \t");
            if (token != NULL) {
                instruction.operand2 = atoi(token);
            }
            token = strtok(NULL, " \t");
            if (token != NULL) {
                instruction.operand3 = atoi(token);
            }
            break;
            
        case T3_OPCODE_JMP:
        case T3_OPCODE_CALL:
            // 1 operand (immediate or label)
            token = strtok(NULL, " \t");
            if (token != NULL) {
                if (token[0] == '@') {
                    // Label reference
                    int label_address = ternary_assembler_find_label(assembler, token + 1);
                    if (label_address == -1) {
                        // Forward reference - will be resolved later
                        instruction.immediate = -1;  // Mark as unresolved
                    } else {
                        instruction.immediate = label_address;
                    }
                } else {
                    instruction.immediate = atoi(token);
                }
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
    return ternary_assembler_add_instruction(assembler, &instruction);
}

uint8_t ternary_assembler_parse_opcode(const char* opcode_str) {
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
// TERNARY ASSEMBLER CODE GENERATION
// =============================================================================

bool ternary_assembler_assemble(ternary_assembler_t* assembler, const char* source_code) {
    if (assembler == NULL || source_code == NULL) {
        return false;
    }
    
    // Clear existing data
    assembler->instruction_count = 0;
    assembler->label_count = 0;
    assembler->error = false;
    
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
            if (!ternary_assembler_parse_line(assembler, line)) {
                assembler->error = true;
                assembler->error_message = strdup("Parse error");
                free(source_copy);
                return false;
            }
        }
        
        line = strtok(NULL, "\n");
    }
    
    free(source_copy);
    
    // Resolve forward references
    if (!ternary_assembler_resolve_references(assembler)) {
        return false;
    }
    
    return true;
}

bool ternary_assembler_resolve_references(ternary_assembler_t* assembler) {
    if (assembler == NULL) {
        return false;
    }
    
    // Resolve forward references in instructions
    for (size_t i = 0; i < assembler->instruction_count; i++) {
        t3_instruction_t* instruction = &assembler->instructions[i];
        
        // Check for unresolved operand2 (uint8_t, so use 0xFF as sentinel)
        if (instruction->operand2 == 0xFF) {
            // This is a forward reference that should have been resolved
            assembler->error = true;
            assembler->error_message = strdup("Unresolved forward reference");
            return false;
        }
        
        // Check for unresolved immediate (int16_t, so -1 is valid)
        if (instruction->immediate == -1) {
            // This is a forward reference that should have been resolved
            assembler->error = true;
            assembler->error_message = strdup("Unresolved forward reference");
            return false;
        }
    }
    
    return true;
}

// =============================================================================
// TERNARY ASSEMBLER ERROR HANDLING
// =============================================================================

bool ternary_assembler_has_error(ternary_assembler_t* assembler) {
    return assembler != NULL && assembler->error;
}

const char* ternary_assembler_get_error_message(ternary_assembler_t* assembler) {
    if (assembler == NULL) return NULL;
    return assembler->error_message;
}

void ternary_assembler_set_error(ternary_assembler_t* assembler, const char* message) {
    if (assembler == NULL) return;
    
    assembler->error = true;
    if (assembler->error_message != NULL) {
        free(assembler->error_message);
    }
    assembler->error_message = message != NULL ? strdup(message) : NULL;
}

// =============================================================================
// TERNARY ASSEMBLER UTILITY FUNCTIONS
// =============================================================================

void ternary_assembler_print_instructions(ternary_assembler_t* assembler) {
    if (assembler == NULL) {
        printf("Ternary Assembler Instructions: NULL\n");
        return;
    }
    
    printf("Ternary Assembler Instructions (%zu):\n", assembler->instruction_count);
    for (size_t i = 0; i < assembler->instruction_count; i++) {
        t3_instruction_t* instruction = &assembler->instructions[i];
        printf("  [%zu]: %s %d %d %d %d\n", i,
               t3_opcode_to_string(instruction->opcode),
               instruction->operand1, instruction->operand2,
               instruction->operand3, instruction->immediate);
    }
}

void ternary_assembler_print_labels(ternary_assembler_t* assembler) {
    if (assembler == NULL) {
        printf("Ternary Assembler Labels: NULL\n");
        return;
    }
    
    printf("Ternary Assembler Labels (%zu):\n", assembler->label_count);
    for (size_t i = 0; i < assembler->label_count; i++) {
        printf("  %s: %d\n", assembler->labels[i].name, assembler->labels[i].address);
    }
}

void ternary_assembler_debug(ternary_assembler_t* assembler) {
    if (assembler == NULL) {
        printf("Ternary Assembler Debug: NULL\n");
        return;
    }
    
    printf("Ternary Assembler Debug:\n");
    printf("  Instruction Count: %zu\n", assembler->instruction_count);
    printf("  Instruction Capacity: %zu\n", assembler->instruction_capacity);
    printf("  Label Count: %zu\n", assembler->label_count);
    printf("  Label Capacity: %zu\n", assembler->label_capacity);
    printf("  Error: %s\n", assembler->error ? "true" : "false");
    printf("  Error Message: %s\n", assembler->error_message != NULL ? assembler->error_message : "None");
    
    printf("  Instructions:\n");
    for (size_t i = 0; i < assembler->instruction_count; i++) {
        t3_instruction_t* instruction = &assembler->instructions[i];
        printf("    [%zu]: opcode=%d, op1=%d, op2=%d, op3=%d, imm=%d, valid=%s\n", i,
               instruction->opcode, instruction->operand1, instruction->operand2,
               instruction->operand3, instruction->immediate, instruction->valid ? "true" : "false");
    }
    
    printf("  Labels:\n");
    for (size_t i = 0; i < assembler->label_count; i++) {
        printf("    %s: %d\n", assembler->labels[i].name, assembler->labels[i].address);
    }
}
