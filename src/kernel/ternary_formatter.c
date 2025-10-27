/**
 * @file ternary_formatter.c
 * @brief Ternary code formatter implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_formatter.h"
#include "trit.h"
#include "t3_isa.h"
#include "ternary_compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TERNARY FORMATTER IMPLEMENTATION
// =============================================================================

ternary_formatter_t* ternary_formatter_create(void) {
    ternary_formatter_t* formatter = malloc(sizeof(ternary_formatter_t));
    if (formatter == NULL) return NULL;
    
    formatter->compiler = ternary_compiler_create();
    if (formatter->compiler == NULL) {
        free(formatter);
        return NULL;
    }
    
    formatter->indent_level = 0;
    formatter->indent_size = 4;
    formatter->line_length = 80;
    formatter->error = false;
    formatter->error_message = NULL;
    
    return formatter;
}

void ternary_formatter_destroy(ternary_formatter_t* formatter) {
    if (formatter != NULL) {
        if (formatter->compiler != NULL) {
            ternary_compiler_destroy(formatter->compiler);
        }
        if (formatter->error_message != NULL) {
            free(formatter->error_message);
        }
        free(formatter);
    }
}

// =============================================================================
// TERNARY FORMATTER CONFIGURATION
// =============================================================================

void ternary_formatter_set_indent_size(ternary_formatter_t* formatter, int indent_size) {
    if (formatter == NULL) return;
    
    formatter->indent_size = indent_size;
}

void ternary_formatter_set_line_length(ternary_formatter_t* formatter, int line_length) {
    if (formatter == NULL) return;
    
    formatter->line_length = line_length;
}

int ternary_formatter_get_indent_size(ternary_formatter_t* formatter) {
    if (formatter == NULL) return 0;
    return formatter->indent_size;
}

int ternary_formatter_get_line_length(ternary_formatter_t* formatter) {
    if (formatter == NULL) return 0;
    return formatter->line_length;
}

// =============================================================================
// TERNARY FORMATTER CODE FORMATTING
// =============================================================================

char* ternary_formatter_format_code(ternary_formatter_t* formatter, const char* source_code) {
    if (formatter == NULL || source_code == NULL) {
        return NULL;
    }
    
    formatter->error = false;
    
    // Compile source code to get instructions
    if (!ternary_compiler_generate_code(formatter->compiler, source_code)) {
        formatter->error = true;
        formatter->error_message = strdup("Compilation failed");
        return NULL;
    }
    
    // Format instructions
    return ternary_formatter_format_instructions(formatter);
}

char* ternary_formatter_format_instructions(ternary_formatter_t* formatter) {
    if (formatter == NULL) {
        return NULL;
    }
    
    size_t instruction_count = ternary_compiler_get_instruction_count(formatter->compiler);
    if (instruction_count == 0) {
        return strdup("");
    }
    
    // Calculate total size needed
    size_t total_size = instruction_count * 256;  // Estimate per instruction
    char* result = malloc(total_size);
    if (result == NULL) {
        formatter->error = true;
        formatter->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    result[0] = '\0';
    
    // Format each instruction
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = ternary_compiler_get_instruction(formatter->compiler, i);
        if (instruction != NULL && instruction->valid) {
            char* instruction_str = ternary_formatter_format_instruction(formatter, instruction, i);
            if (instruction_str != NULL) {
                strcat(result, instruction_str);
                strcat(result, "\n");
                free(instruction_str);
            }
        }
    }
    
    return result;
}

char* ternary_formatter_format_instruction(ternary_formatter_t* formatter, t3_instruction_t* instruction, size_t index) {
    if (formatter == NULL || instruction == NULL) {
        return NULL;
    }
    
    char* result = malloc(256);
    if (result == NULL) {
        formatter->error = true;
        formatter->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    // Add indentation
    for (int i = 0; i < formatter->indent_level; i++) {
        for (int j = 0; j < formatter->indent_size; j++) {
            strcat(result, " ");
        }
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
            snprintf(result + strlen(result), 256 - strlen(result), "%s R%d, R%d", 
                     opcode_str, instruction->operand1, instruction->operand2);
            break;
            
        case T3_OPCODE_ADD:
        case T3_OPCODE_SUB:
        case T3_OPCODE_MUL:
        case T3_OPCODE_DIV:
        case T3_OPCODE_AND:
        case T3_OPCODE_OR:
        case T3_OPCODE_XOR:
        case T3_OPCODE_CMP:
            snprintf(result + strlen(result), 256 - strlen(result), "%s R%d, R%d, R%d", 
                     opcode_str, instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_JMP:
        case T3_OPCODE_CALL:
            snprintf(result + strlen(result), 256 - strlen(result), "%s #%d", 
                     opcode_str, instruction->immediate);
            break;
            
        case T3_OPCODE_RET:
        case T3_OPCODE_HALT:
        case T3_OPCODE_NOP:
            snprintf(result + strlen(result), 256 - strlen(result), "%s", opcode_str);
            break;
            
        default:
            snprintf(result + strlen(result), 256 - strlen(result), "UNKNOWN %d %d %d %d", 
                     instruction->opcode, instruction->operand1, instruction->operand2, instruction->operand3);
            break;
    }
    
    return result;
}

// =============================================================================
// TERNARY FORMATTER INDENTATION
// =============================================================================

void ternary_formatter_increase_indent(ternary_formatter_t* formatter) {
    if (formatter == NULL) return;
    
    formatter->indent_level++;
}

void ternary_formatter_decrease_indent(ternary_formatter_t* formatter) {
    if (formatter == NULL) return;
    
    if (formatter->indent_level > 0) {
        formatter->indent_level--;
    }
}

void ternary_formatter_reset_indent(ternary_formatter_t* formatter) {
    if (formatter == NULL) return;
    
    formatter->indent_level = 0;
}

int ternary_formatter_get_indent_level(ternary_formatter_t* formatter) {
    if (formatter == NULL) return 0;
    return formatter->indent_level;
}

// =============================================================================
// TERNARY FORMATTER LINE BREAKING
// =============================================================================

bool ternary_formatter_should_break_line(ternary_formatter_t* formatter, const char* line) {
    if (formatter == NULL || line == NULL) {
        return false;
    }
    
    return strlen(line) > formatter->line_length;
}

char* ternary_formatter_break_line(ternary_formatter_t* formatter, const char* line) {
    if (formatter == NULL || line == NULL) {
        return NULL;
    }
    
    if (!ternary_formatter_should_break_line(formatter, line)) {
        return strdup(line);
    }
    
    // Simple line breaking - break at spaces
    char* result = malloc(strlen(line) + 100);
    if (result == NULL) {
        formatter->error = true;
        formatter->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    strcpy(result, line);
    
    // Find last space before line length
    int break_point = formatter->line_length;
    while (break_point > 0 && result[break_point] != ' ') {
        break_point--;
    }
    
    if (break_point > 0) {
        result[break_point] = '\n';
        
        // Add indentation for continuation
        for (int i = 0; i < formatter->indent_level; i++) {
            for (int j = 0; j < formatter->indent_size; j++) {
                // Insert spaces after newline
                memmove(result + break_point + 1 + (i * formatter->indent_size) + j, 
                        result + break_point + 1 + (i * formatter->indent_size) + j, 
                        strlen(result) - break_point - 1);
                result[break_point + 1 + (i * formatter->indent_size) + j] = ' ';
            }
        }
    }
    
    return result;
}

// =============================================================================
// TERNARY FORMATTER COMMENT HANDLING
// =============================================================================

char* ternary_formatter_add_comment(ternary_formatter_t* formatter, const char* line, const char* comment) {
    if (formatter == NULL || line == NULL || comment == NULL) {
        return NULL;
    }
    
    size_t total_length = strlen(line) + strlen(comment) + 10;  // Extra space for formatting
    char* result = malloc(total_length);
    if (result == NULL) {
        formatter->error = true;
        formatter->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    // Calculate padding to align comments
    int padding = formatter->line_length - strlen(line);
    if (padding < 2) {
        padding = 2;
    }
    
    snprintf(result, total_length, "%s%*s; %s", line, padding, "", comment);
    
    return result;
}

// =============================================================================
// TERNARY FORMATTER ERROR HANDLING
// =============================================================================

bool ternary_formatter_has_error(ternary_formatter_t* formatter) {
    return formatter != NULL && formatter->error;
}

const char* ternary_formatter_get_error_message(ternary_formatter_t* formatter) {
    if (formatter == NULL) return NULL;
    return formatter->error_message;
}

void ternary_formatter_set_error(ternary_formatter_t* formatter, const char* message) {
    if (formatter == NULL) return;
    
    formatter->error = true;
    if (formatter->error_message != NULL) {
        free(formatter->error_message);
    }
    formatter->error_message = message != NULL ? strdup(message) : NULL;
}

// =============================================================================
// TERNARY FORMATTER UTILITY FUNCTIONS
// =============================================================================

void ternary_formatter_print_config(ternary_formatter_t* formatter) {
    if (formatter == NULL) {
        printf("Ternary Formatter Config: NULL\n");
        return;
    }
    
    printf("Ternary Formatter Config:\n");
    printf("  Indent Size: %d\n", formatter->indent_size);
    printf("  Line Length: %d\n", formatter->line_length);
    printf("  Indent Level: %d\n", formatter->indent_level);
}

void ternary_formatter_debug(ternary_formatter_t* formatter) {
    if (formatter == NULL) {
        printf("Ternary Formatter Debug: NULL\n");
        return;
    }
    
    printf("Ternary Formatter Debug:\n");
    printf("  Indent Size: %d\n", formatter->indent_size);
    printf("  Line Length: %d\n", formatter->line_length);
    printf("  Indent Level: %d\n", formatter->indent_level);
    printf("  Error: %s\n", formatter->error ? "true" : "false");
    printf("  Error Message: %s\n", formatter->error_message != NULL ? formatter->error_message : "None");
}
