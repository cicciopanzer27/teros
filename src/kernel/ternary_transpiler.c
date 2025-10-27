/**
 * @file ternary_transpiler.c
 * @brief Ternary code transpiler implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_transpiler.h"
#include "trit.h"
#include "t3_isa.h"
#include "ternary_compiler.h"
#include "ternary_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TERNARY TRANSPILER IMPLEMENTATION
// =============================================================================

ternary_transpiler_t* ternary_transpiler_create(void) {
    ternary_transpiler_t* transpiler = malloc(sizeof(ternary_transpiler_t));
    if (transpiler == NULL) return NULL;
    
    transpiler->compiler = ternary_compiler_create();
    if (transpiler->compiler == NULL) {
        free(transpiler);
        return NULL;
    }
    
    transpiler->generator = ternary_generator_create();
    if (transpiler->generator == NULL) {
        ternary_compiler_destroy(transpiler->compiler);
        free(transpiler);
        return NULL;
    }
    
    transpiler->error = false;
    transpiler->error_message = NULL;
    
    return transpiler;
}

void ternary_transpiler_destroy(ternary_transpiler_t* transpiler) {
    if (transpiler != NULL) {
        if (transpiler->compiler != NULL) {
            ternary_compiler_destroy(transpiler->compiler);
        }
        if (transpiler->generator != NULL) {
            ternary_generator_destroy(transpiler->generator);
        }
        if (transpiler->error_message != NULL) {
            free(transpiler->error_message);
        }
        free(transpiler);
    }
}

// =============================================================================
// TERNARY TRANSPILER CODE TRANSPILATION
// =============================================================================

char* ternary_transpiler_transpile_code(ternary_transpiler_t* transpiler, const char* source_code, ternary_target_t target) {
    if (transpiler == NULL || source_code == NULL) {
        return NULL;
    }
    
    transpiler->error = false;
    
    // Compile source code
    if (!ternary_compiler_generate_code(transpiler->compiler, source_code)) {
        transpiler->error = true;
        transpiler->error_message = strdup("Compilation failed");
        return NULL;
    }
    
    // Generate code for target
    switch (target) {
        case TERNARY_TARGET_C:
            return ternary_transpiler_transpile_to_c(transpiler);
        case TERNARY_TARGET_PYTHON:
            return ternary_transpiler_transpile_to_python(transpiler);
        case TERNARY_TARGET_JAVASCRIPT:
            return ternary_transpiler_transpile_to_javascript(transpiler);
        case TERNARY_TARGET_RUST:
            return ternary_transpiler_transpile_to_rust(transpiler);
        case TERNARY_TARGET_GO:
            return ternary_transpiler_transpile_to_go(transpiler);
        default:
            transpiler->error = true;
            transpiler->error_message = strdup("Unsupported target");
            return NULL;
    }
}

// =============================================================================
// TERNARY TRANSPILER C TARGET
// =============================================================================

char* ternary_transpiler_transpile_to_c(ternary_transpiler_t* transpiler) {
    if (transpiler == NULL) {
        return NULL;
    }
    
    size_t instruction_count = ternary_compiler_get_instruction_count(transpiler->compiler);
    if (instruction_count == 0) {
        return strdup("");
    }
    
    // Calculate total size needed
    size_t total_size = instruction_count * 512;  // Estimate per instruction
    char* result = malloc(total_size);
    if (result == NULL) {
        transpiler->error = true;
        transpiler->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    result[0] = '\0';
    
    // Add C header
    strcat(result, "#include <stdio.h>\n");
    strcat(result, "#include <stdlib.h>\n");
    strcat(result, "\n");
    strcat(result, "int main() {\n");
    strcat(result, "    // Ternary program\n");
    
    // Generate C code for each instruction
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = ternary_compiler_get_instruction(transpiler->compiler, i);
        if (instruction != NULL && instruction->valid) {
            char* instruction_c = ternary_transpiler_format_instruction_c(transpiler, instruction, i);
            if (instruction_c != NULL) {
                strcat(result, "    ");
                strcat(result, instruction_c);
                strcat(result, "\n");
                free(instruction_c);
            }
        }
    }
    
    strcat(result, "    return 0;\n");
    strcat(result, "}\n");
    
    return result;
}

char* ternary_transpiler_format_instruction_c(ternary_transpiler_t* transpiler, t3_instruction_t* instruction, size_t index) {
    (void)index;
    if (transpiler == NULL || instruction == NULL) {
        return NULL;
    }
    
    char* result = malloc(256);
    if (result == NULL) {
        transpiler->error = true;
        transpiler->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    const char* opcode_str = t3_opcode_to_string(instruction->opcode);
    
    switch (instruction->opcode) {
        case T3_OPCODE_LOAD:
            snprintf(result, 256, "r%d = %d;", instruction->operand1, instruction->operand2);
            break;
            
        case T3_OPCODE_STORE:
            snprintf(result, 256, "r%d = r%d;", instruction->operand2, instruction->operand1);
            break;
            
        case T3_OPCODE_ADD:
            snprintf(result, 256, "r%d = r%d + r%d;", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_SUB:
            snprintf(result, 256, "r%d = r%d - r%d;", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_MUL:
            snprintf(result, 256, "r%d = r%d * r%d;", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_DIV:
            snprintf(result, 256, "r%d = r%d / r%d;", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_AND:
            snprintf(result, 256, "r%d = r%d && r%d;", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_OR:
            snprintf(result, 256, "r%d = r%d || r%d;", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_NOT:
            snprintf(result, 256, "r%d = !r%d;", instruction->operand1, instruction->operand2);
            break;
            
        case T3_OPCODE_XOR:
            snprintf(result, 256, "r%d = r%d ^ r%d;", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_CMP:
            snprintf(result, 256, "r%d = (r%d == r%d) ? 1 : 0;", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_JMP:
            snprintf(result, 256, "goto label_%d;", instruction->immediate);
            break;
            
        case T3_OPCODE_JZ:
            snprintf(result, 256, "if (r%d == 0) goto label_%d;", instruction->operand1, instruction->immediate);
            break;
            
        case T3_OPCODE_JNZ:
            snprintf(result, 256, "if (r%d != 0) goto label_%d;", instruction->operand1, instruction->immediate);
            break;
            
        case T3_OPCODE_CALL:
            snprintf(result, 256, "call_function_%d();", instruction->immediate);
            break;
            
        case T3_OPCODE_RET:
            snprintf(result, 256, "return;");
            break;
            
        case T3_OPCODE_PUSH:
            snprintf(result, 256, "push(r%d);", instruction->operand1);
            break;
            
        case T3_OPCODE_POP:
            snprintf(result, 256, "r%d = pop();", instruction->operand1);
            break;
            
        case T3_OPCODE_HALT:
            snprintf(result, 256, "exit(0);");
            break;
            
        case T3_OPCODE_NOP:
            snprintf(result, 256, "// NOP");
            break;
            
        default:
            snprintf(result, 256, "// Unknown instruction: %s", opcode_str);
            break;
    }
    
    return result;
}

// =============================================================================
// TERNARY TRANSPILER PYTHON TARGET
// =============================================================================

char* ternary_transpiler_transpile_to_python(ternary_transpiler_t* transpiler) {
    if (transpiler == NULL) {
        return NULL;
    }
    
    size_t instruction_count = ternary_compiler_get_instruction_count(transpiler->compiler);
    if (instruction_count == 0) {
        return strdup("");
    }
    
    // Calculate total size needed
    size_t total_size = instruction_count * 512;  // Estimate per instruction
    char* result = malloc(total_size);
    if (result == NULL) {
        transpiler->error = true;
        transpiler->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    result[0] = '\0';
    
    // Add Python header
    strcat(result, "#!/usr/bin/env python3\n");
    strcat(result, "# Ternary program\n");
    strcat(result, "\n");
    strcat(result, "def main():\n");
    
    // Generate Python code for each instruction
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = ternary_compiler_get_instruction(transpiler->compiler, i);
        if (instruction != NULL && instruction->valid) {
            char* instruction_py = ternary_transpiler_format_instruction_python(transpiler, instruction, i);
            if (instruction_py != NULL) {
                strcat(result, "    ");
                strcat(result, instruction_py);
                strcat(result, "\n");
                free(instruction_py);
            }
        }
    }
    
    strcat(result, "\nif __name__ == '__main__':\n");
    strcat(result, "    main()\n");
    
    return result;
}

char* ternary_transpiler_format_instruction_python(ternary_transpiler_t* transpiler, t3_instruction_t* instruction, size_t index) {
    (void)index;
    if (transpiler == NULL || instruction == NULL) {
        return NULL;
    }
    
    char* result = malloc(256);
    if (result == NULL) {
        transpiler->error = true;
        transpiler->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    const char* opcode_str = t3_opcode_to_string(instruction->opcode);
    
    switch (instruction->opcode) {
        case T3_OPCODE_LOAD:
            snprintf(result, 256, "r%d = %d", instruction->operand1, instruction->operand2);
            break;
            
        case T3_OPCODE_STORE:
            snprintf(result, 256, "r%d = r%d", instruction->operand2, instruction->operand1);
            break;
            
        case T3_OPCODE_ADD:
            snprintf(result, 256, "r%d = r%d + r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_SUB:
            snprintf(result, 256, "r%d = r%d - r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_MUL:
            snprintf(result, 256, "r%d = r%d * r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_DIV:
            snprintf(result, 256, "r%d = r%d / r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_AND:
            snprintf(result, 256, "r%d = r%d and r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_OR:
            snprintf(result, 256, "r%d = r%d or r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_NOT:
            snprintf(result, 256, "r%d = not r%d", instruction->operand1, instruction->operand2);
            break;
            
        case T3_OPCODE_XOR:
            snprintf(result, 256, "r%d = r%d ^ r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_CMP:
            snprintf(result, 256, "r%d = 1 if r%d == r%d else 0", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_JMP:
            snprintf(result, 256, "goto label_%d", instruction->immediate);
            break;
            
        case T3_OPCODE_JZ:
            snprintf(result, 256, "if r%d == 0: goto label_%d", instruction->operand1, instruction->immediate);
            break;
            
        case T3_OPCODE_JNZ:
            snprintf(result, 256, "if r%d != 0: goto label_%d", instruction->operand1, instruction->immediate);
            break;
            
        case T3_OPCODE_CALL:
            snprintf(result, 256, "call_function_%d()", instruction->immediate);
            break;
            
        case T3_OPCODE_RET:
            snprintf(result, 256, "return");
            break;
            
        case T3_OPCODE_PUSH:
            snprintf(result, 256, "push(r%d)", instruction->operand1);
            break;
            
        case T3_OPCODE_POP:
            snprintf(result, 256, "r%d = pop()", instruction->operand1);
            break;
            
        case T3_OPCODE_HALT:
            snprintf(result, 256, "exit(0)");
            break;
            
        case T3_OPCODE_NOP:
            snprintf(result, 256, "# NOP");
            break;
            
        default:
            snprintf(result, 256, "# Unknown instruction: %s", opcode_str);
            break;
    }
    
    return result;
}

// =============================================================================
// TERNARY TRANSPILER JAVASCRIPT TARGET
// =============================================================================

char* ternary_transpiler_transpile_to_javascript(ternary_transpiler_t* transpiler) {
    if (transpiler == NULL) {
        return NULL;
    }
    
    size_t instruction_count = ternary_compiler_get_instruction_count(transpiler->compiler);
    if (instruction_count == 0) {
        return strdup("");
    }
    
    // Calculate total size needed
    size_t total_size = instruction_count * 512;  // Estimate per instruction
    char* result = malloc(total_size);
    if (result == NULL) {
        transpiler->error = true;
        transpiler->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    result[0] = '\0';
    
    // Add JavaScript header
    strcat(result, "// Ternary program\n");
    strcat(result, "function main() {\n");
    
    // Generate JavaScript code for each instruction
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = ternary_compiler_get_instruction(transpiler->compiler, i);
        if (instruction != NULL && instruction->valid) {
            char* instruction_js = ternary_transpiler_format_instruction_javascript(transpiler, instruction, i);
            if (instruction_js != NULL) {
                strcat(result, "    ");
                strcat(result, instruction_js);
                strcat(result, ";\n");
                free(instruction_js);
            }
        }
    }
    
    strcat(result, "}\n");
    strcat(result, "\n");
    strcat(result, "main();\n");
    
    return result;
}

char* ternary_transpiler_format_instruction_javascript(ternary_transpiler_t* transpiler, t3_instruction_t* instruction, size_t index) {
    (void)index;
    if (transpiler == NULL || instruction == NULL) {
        return NULL;
    }
    
    char* result = malloc(256);
    if (result == NULL) {
        transpiler->error = true;
        transpiler->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    const char* opcode_str = t3_opcode_to_string(instruction->opcode);
    
    switch (instruction->opcode) {
        case T3_OPCODE_LOAD:
            snprintf(result, 256, "r%d = %d", instruction->operand1, instruction->operand2);
            break;
            
        case T3_OPCODE_STORE:
            snprintf(result, 256, "r%d = r%d", instruction->operand2, instruction->operand1);
            break;
            
        case T3_OPCODE_ADD:
            snprintf(result, 256, "r%d = r%d + r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_SUB:
            snprintf(result, 256, "r%d = r%d - r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_MUL:
            snprintf(result, 256, "r%d = r%d * r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_DIV:
            snprintf(result, 256, "r%d = r%d / r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_AND:
            snprintf(result, 256, "r%d = r%d && r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_OR:
            snprintf(result, 256, "r%d = r%d || r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_NOT:
            snprintf(result, 256, "r%d = !r%d", instruction->operand1, instruction->operand2);
            break;
            
        case T3_OPCODE_XOR:
            snprintf(result, 256, "r%d = r%d ^ r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_CMP:
            snprintf(result, 256, "r%d = (r%d === r%d) ? 1 : 0", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_JMP:
            snprintf(result, 256, "goto label_%d", instruction->immediate);
            break;
            
        case T3_OPCODE_JZ:
            snprintf(result, 256, "if (r%d === 0) goto label_%d", instruction->operand1, instruction->immediate);
            break;
            
        case T3_OPCODE_JNZ:
            snprintf(result, 256, "if (r%d !== 0) goto label_%d", instruction->operand1, instruction->immediate);
            break;
            
        case T3_OPCODE_CALL:
            snprintf(result, 256, "call_function_%d()", instruction->immediate);
            break;
            
        case T3_OPCODE_RET:
            snprintf(result, 256, "return");
            break;
            
        case T3_OPCODE_PUSH:
            snprintf(result, 256, "push(r%d)", instruction->operand1);
            break;
            
        case T3_OPCODE_POP:
            snprintf(result, 256, "r%d = pop()", instruction->operand1);
            break;
            
        case T3_OPCODE_HALT:
            snprintf(result, 256, "process.exit(0)");
            break;
            
        case T3_OPCODE_NOP:
            snprintf(result, 256, "// NOP");
            break;
            
        default:
            snprintf(result, 256, "// Unknown instruction: %s", opcode_str);
            break;
    }
    
    return result;
}

// =============================================================================
// TERNARY TRANSPILER RUST TARGET
// =============================================================================

char* ternary_transpiler_transpile_to_rust(ternary_transpiler_t* transpiler) {
    if (transpiler == NULL) {
        return NULL;
    }
    
    size_t instruction_count = ternary_compiler_get_instruction_count(transpiler->compiler);
    if (instruction_count == 0) {
        return strdup("");
    }
    
    // Calculate total size needed
    size_t total_size = instruction_count * 512;  // Estimate per instruction
    char* result = malloc(total_size);
    if (result == NULL) {
        transpiler->error = true;
        transpiler->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    result[0] = '\0';
    
    // Add Rust header
    strcat(result, "// Ternary program\n");
    strcat(result, "fn main() {\n");
    
    // Generate Rust code for each instruction
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = ternary_compiler_get_instruction(transpiler->compiler, i);
        if (instruction != NULL && instruction->valid) {
            char* instruction_rust = ternary_transpiler_format_instruction_rust(transpiler, instruction, i);
            if (instruction_rust != NULL) {
                strcat(result, "    ");
                strcat(result, instruction_rust);
                strcat(result, ";\n");
                free(instruction_rust);
            }
        }
    }
    
    strcat(result, "}\n");
    
    return result;
}

char* ternary_transpiler_format_instruction_rust(ternary_transpiler_t* transpiler, t3_instruction_t* instruction, size_t index) {
    (void)index;
    if (transpiler == NULL || instruction == NULL) {
        return NULL;
    }
    
    char* result = malloc(256);
    if (result == NULL) {
        transpiler->error = true;
        transpiler->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    const char* opcode_str = t3_opcode_to_string(instruction->opcode);
    
    switch (instruction->opcode) {
        case T3_OPCODE_LOAD:
            snprintf(result, 256, "r%d = %d", instruction->operand1, instruction->operand2);
            break;
            
        case T3_OPCODE_STORE:
            snprintf(result, 256, "r%d = r%d", instruction->operand2, instruction->operand1);
            break;
            
        case T3_OPCODE_ADD:
            snprintf(result, 256, "r%d = r%d + r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_SUB:
            snprintf(result, 256, "r%d = r%d - r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_MUL:
            snprintf(result, 256, "r%d = r%d * r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_DIV:
            snprintf(result, 256, "r%d = r%d / r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_AND:
            snprintf(result, 256, "r%d = r%d && r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_OR:
            snprintf(result, 256, "r%d = r%d || r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_NOT:
            snprintf(result, 256, "r%d = !r%d", instruction->operand1, instruction->operand2);
            break;
            
        case T3_OPCODE_XOR:
            snprintf(result, 256, "r%d = r%d ^ r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_CMP:
            snprintf(result, 256, "r%d = if r%d == r%d { 1 } else { 0 }", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_JMP:
            snprintf(result, 256, "goto label_%d", instruction->immediate);
            break;
            
        case T3_OPCODE_JZ:
            snprintf(result, 256, "if r%d == 0 { goto label_%d }", instruction->operand1, instruction->immediate);
            break;
            
        case T3_OPCODE_JNZ:
            snprintf(result, 256, "if r%d != 0 { goto label_%d }", instruction->operand1, instruction->immediate);
            break;
            
        case T3_OPCODE_CALL:
            snprintf(result, 256, "call_function_%d()", instruction->immediate);
            break;
            
        case T3_OPCODE_RET:
            snprintf(result, 256, "return");
            break;
            
        case T3_OPCODE_PUSH:
            snprintf(result, 256, "push(r%d)", instruction->operand1);
            break;
            
        case T3_OPCODE_POP:
            snprintf(result, 256, "r%d = pop()", instruction->operand1);
            break;
            
        case T3_OPCODE_HALT:
            snprintf(result, 256, "std::process::exit(0)");
            break;
            
        case T3_OPCODE_NOP:
            snprintf(result, 256, "// NOP");
            break;
            
        default:
            snprintf(result, 256, "// Unknown instruction: %s", opcode_str);
            break;
    }
    
    return result;
}

// =============================================================================
// TERNARY TRANSPILER GO TARGET
// =============================================================================

char* ternary_transpiler_transpile_to_go(ternary_transpiler_t* transpiler) {
    if (transpiler == NULL) {
        return NULL;
    }
    
    size_t instruction_count = ternary_compiler_get_instruction_count(transpiler->compiler);
    if (instruction_count == 0) {
        return strdup("");
    }
    
    // Calculate total size needed
    size_t total_size = instruction_count * 512;  // Estimate per instruction
    char* result = malloc(total_size);
    if (result == NULL) {
        transpiler->error = true;
        transpiler->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    result[0] = '\0';
    
    // Add Go header
    strcat(result, "package main\n");
    strcat(result, "\n");
    strcat(result, "import \"fmt\"\n");
    strcat(result, "\n");
    strcat(result, "func main() {\n");
    
    // Generate Go code for each instruction
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = ternary_compiler_get_instruction(transpiler->compiler, i);
        if (instruction != NULL && instruction->valid) {
            char* instruction_go = ternary_transpiler_format_instruction_go(transpiler, instruction, i);
            if (instruction_go != NULL) {
                strcat(result, "    ");
                strcat(result, instruction_go);
                strcat(result, "\n");
                free(instruction_go);
            }
        }
    }
    
    strcat(result, "}\n");
    
    return result;
}

char* ternary_transpiler_format_instruction_go(ternary_transpiler_t* transpiler, t3_instruction_t* instruction, size_t index) {
    (void)index;
    if (transpiler == NULL || instruction == NULL) {
        return NULL;
    }
    
    char* result = malloc(256);
    if (result == NULL) {
        transpiler->error = true;
        transpiler->error_message = strdup("Memory allocation failed");
        return NULL;
    }
    
    const char* opcode_str = t3_opcode_to_string(instruction->opcode);
    
    switch (instruction->opcode) {
        case T3_OPCODE_LOAD:
            snprintf(result, 256, "r%d := %d", instruction->operand1, instruction->operand2);
            break;
            
        case T3_OPCODE_STORE:
            snprintf(result, 256, "r%d = r%d", instruction->operand2, instruction->operand1);
            break;
            
        case T3_OPCODE_ADD:
            snprintf(result, 256, "r%d = r%d + r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_SUB:
            snprintf(result, 256, "r%d = r%d - r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_MUL:
            snprintf(result, 256, "r%d = r%d * r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_DIV:
            snprintf(result, 256, "r%d = r%d / r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_AND:
            snprintf(result, 256, "r%d = r%d && r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_OR:
            snprintf(result, 256, "r%d = r%d || r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_NOT:
            snprintf(result, 256, "r%d = !r%d", instruction->operand1, instruction->operand2);
            break;
            
        case T3_OPCODE_XOR:
            snprintf(result, 256, "r%d = r%d ^ r%d", instruction->operand1, instruction->operand2, instruction->operand3);
            break;
            
        case T3_OPCODE_CMP:
            snprintf(result, 256, "if r%d == r%d { r%d = 1 } else { r%d = 0 }", instruction->operand2, instruction->operand3, instruction->operand1, instruction->operand1);
            break;
            
        case T3_OPCODE_JMP:
            snprintf(result, 256, "goto label_%d", instruction->immediate);
            break;
            
        case T3_OPCODE_JZ:
            snprintf(result, 256, "if r%d == 0 { goto label_%d }", instruction->operand1, instruction->immediate);
            break;
            
        case T3_OPCODE_JNZ:
            snprintf(result, 256, "if r%d != 0 { goto label_%d }", instruction->operand1, instruction->immediate);
            break;
            
        case T3_OPCODE_CALL:
            snprintf(result, 256, "call_function_%d()", instruction->immediate);
            break;
            
        case T3_OPCODE_RET:
            snprintf(result, 256, "return");
            break;
            
        case T3_OPCODE_PUSH:
            snprintf(result, 256, "push(r%d)", instruction->operand1);
            break;
            
        case T3_OPCODE_POP:
            snprintf(result, 256, "r%d = pop()", instruction->operand1);
            break;
            
        case T3_OPCODE_HALT:
            snprintf(result, 256, "os.Exit(0)");
            break;
            
        case T3_OPCODE_NOP:
            snprintf(result, 256, "// NOP");
            break;
            
        default:
            snprintf(result, 256, "// Unknown instruction: %s", opcode_str);
            break;
    }
    
    return result;
}

// =============================================================================
// TERNARY TRANSPILER ERROR HANDLING
// =============================================================================

bool ternary_transpiler_has_error(ternary_transpiler_t* transpiler) {
    return transpiler != NULL && transpiler->error;
}

const char* ternary_transpiler_get_error_message(ternary_transpiler_t* transpiler) {
    if (transpiler == NULL) return NULL;
    return transpiler->error_message;
}

void ternary_transpiler_set_error(ternary_transpiler_t* transpiler, const char* message) {
    if (transpiler == NULL) return;
    
    transpiler->error = true;
    if (transpiler->error_message != NULL) {
        free(transpiler->error_message);
    }
    transpiler->error_message = message != NULL ? strdup(message) : NULL;
}

// =============================================================================
// TERNARY TRANSPILER UTILITY FUNCTIONS
// =============================================================================

void ternary_transpiler_print_transpilation(ternary_transpiler_t* transpiler) {
    if (transpiler == NULL) {
        printf("Ternary Transpiler Transpilation: NULL\n");
        return;
    }
    
    printf("Ternary Transpiler Transpilation:\n");
    printf("  Error: %s\n", transpiler->error ? "true" : "false");
    printf("  Error Message: %s\n", transpiler->error_message != NULL ? transpiler->error_message : "None");
}

void ternary_transpiler_debug(ternary_transpiler_t* transpiler) {
    if (transpiler == NULL) {
        printf("Ternary Transpiler Debug: NULL\n");
        return;
    }
    
    printf("Ternary Transpiler Debug:\n");
    printf("  Error: %s\n", transpiler->error ? "true" : "false");
    printf("  Error Message: %s\n", transpiler->error_message != NULL ? transpiler->error_message : "None");
}
