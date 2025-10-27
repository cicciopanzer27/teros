/**
 * @file ternary_interpreter.c
 * @brief Ternary interpreter implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_interpreter.h"
#include "trit.h"
#include "trit_array.h"
#include "t3_isa.h"
#include "tvm.h"
#include "ternary_alu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TERNARY INTERPRETER IMPLEMENTATION
// =============================================================================

ternary_interpreter_t* ternary_interpreter_create(void) {
    ternary_interpreter_t* interpreter = malloc(sizeof(ternary_interpreter_t));
    if (interpreter == NULL) return NULL;
    
    interpreter->alu = ternary_alu_create();
    if (interpreter->alu == NULL) {
        free(interpreter);
        return NULL;
    }
    
    interpreter->vm = tvm_create(TVM_DEFAULT_MEMORY_SIZE);
    if (interpreter->vm == NULL) {
        ternary_alu_destroy(interpreter->alu);
        free(interpreter);
        return NULL;
    }
    
    interpreter->running = false;
    interpreter->halted = false;
    interpreter->error = false;
    
    return interpreter;
}

void ternary_interpreter_destroy(ternary_interpreter_t* interpreter) {
    if (interpreter != NULL) {
        if (interpreter->alu != NULL) {
            ternary_alu_destroy(interpreter->alu);
        }
        if (interpreter->vm != NULL) {
            tvm_destroy(interpreter->vm);
        }
        free(interpreter);
    }
}

// =============================================================================
// TERNARY INTERPRETER EXECUTION
// =============================================================================

trit_t ternary_interpreter_execute_instruction(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL || !t3_instruction_is_valid(instruction)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Clear ALU flags before execution
    ternary_alu_clear_flags(interpreter->alu);
    
    // Execute instruction based on opcode
    switch (instruction->opcode) {
        case T3_OPCODE_LOAD:
            return ternary_interpreter_execute_load(interpreter, instruction);
        case T3_OPCODE_STORE:
            return ternary_interpreter_execute_store(interpreter, instruction);
        case T3_OPCODE_ADD:
            return ternary_interpreter_execute_add(interpreter, instruction);
        case T3_OPCODE_SUB:
            return ternary_interpreter_execute_sub(interpreter, instruction);
        case T3_OPCODE_MUL:
            return ternary_interpreter_execute_mul(interpreter, instruction);
        case T3_OPCODE_DIV:
            return ternary_interpreter_execute_div(interpreter, instruction);
        case T3_OPCODE_AND:
            return ternary_interpreter_execute_and(interpreter, instruction);
        case T3_OPCODE_OR:
            return ternary_interpreter_execute_or(interpreter, instruction);
        case T3_OPCODE_NOT:
            return ternary_interpreter_execute_not(interpreter, instruction);
        case T3_OPCODE_XOR:
            return ternary_interpreter_execute_xor(interpreter, instruction);
        case T3_OPCODE_CMP:
            return ternary_interpreter_execute_cmp(interpreter, instruction);
        case T3_OPCODE_JMP:
            return ternary_interpreter_execute_jmp(interpreter, instruction);
        case T3_OPCODE_JZ:
            return ternary_interpreter_execute_jz(interpreter, instruction);
        case T3_OPCODE_JNZ:
            return ternary_interpreter_execute_jnz(interpreter, instruction);
        case T3_OPCODE_CALL:
            return ternary_interpreter_execute_call(interpreter, instruction);
        case T3_OPCODE_RET:
            return ternary_interpreter_execute_ret(interpreter, instruction);
        case T3_OPCODE_PUSH:
            return ternary_interpreter_execute_push(interpreter, instruction);
        case T3_OPCODE_POP:
            return ternary_interpreter_execute_pop(interpreter, instruction);
        case T3_OPCODE_HALT:
            return ternary_interpreter_execute_halt(interpreter, instruction);
        default:
            interpreter->error = true;
            return trit_create(TERNARY_UNKNOWN);
    }
}

// =============================================================================
// TERNARY INTERPRETER DATA MOVEMENT INSTRUCTIONS
// =============================================================================

trit_t ternary_interpreter_execute_load(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Get source value
    trit_t value;
    if (instruction->operand2 == 0) {
        // Load immediate
        value = trit_create(instruction->immediate);
    } else {
        // Load from register
        value = tvm_get_register(interpreter->vm, instruction->operand2);
    }
    
    // Store in destination register
    tvm_set_register(interpreter->vm, instruction->operand1, value);
    
    return trit_create(TERNARY_POSITIVE);
}

trit_t ternary_interpreter_execute_store(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Get source value
    trit_t value = tvm_get_register(interpreter->vm, instruction->operand1);
    
    // Store in destination register
    tvm_set_register(interpreter->vm, instruction->operand2, value);
    
    return trit_create(TERNARY_POSITIVE);
}

// =============================================================================
// TERNARY INTERPRETER ARITHMETIC INSTRUCTIONS
// =============================================================================

trit_t ternary_interpreter_execute_add(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t a = tvm_get_register(interpreter->vm, instruction->operand2);
    trit_t b = tvm_get_register(interpreter->vm, instruction->operand3);
    
    trit_t result = ternary_alu_add(interpreter->alu, a, b);
    tvm_set_register(interpreter->vm, instruction->operand1, result);
    
    return result;
}

trit_t ternary_interpreter_execute_sub(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t a = tvm_get_register(interpreter->vm, instruction->operand2);
    trit_t b = tvm_get_register(interpreter->vm, instruction->operand3);
    
    trit_t result = ternary_alu_subtract(interpreter->alu, a, b);
    tvm_set_register(interpreter->vm, instruction->operand1, result);
    
    return result;
}

trit_t ternary_interpreter_execute_mul(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t a = tvm_get_register(interpreter->vm, instruction->operand2);
    trit_t b = tvm_get_register(interpreter->vm, instruction->operand3);
    
    trit_t result = ternary_alu_multiply(interpreter->alu, a, b);
    tvm_set_register(interpreter->vm, instruction->operand1, result);
    
    return result;
}

trit_t ternary_interpreter_execute_div(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t a = tvm_get_register(interpreter->vm, instruction->operand2);
    trit_t b = tvm_get_register(interpreter->vm, instruction->operand3);
    
    trit_t result = ternary_alu_divide(interpreter->alu, a, b);
    tvm_set_register(interpreter->vm, instruction->operand1, result);
    
    return result;
}

// =============================================================================
// TERNARY INTERPRETER LOGIC INSTRUCTIONS
// =============================================================================

trit_t ternary_interpreter_execute_and(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t a = tvm_get_register(interpreter->vm, instruction->operand2);
    trit_t b = tvm_get_register(interpreter->vm, instruction->operand3);
    
    trit_t result = ternary_alu_and(interpreter->alu, a, b);
    tvm_set_register(interpreter->vm, instruction->operand1, result);
    
    return result;
}

trit_t ternary_interpreter_execute_or(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t a = tvm_get_register(interpreter->vm, instruction->operand2);
    trit_t b = tvm_get_register(interpreter->vm, instruction->operand3);
    
    trit_t result = ternary_alu_or(interpreter->alu, a, b);
    tvm_set_register(interpreter->vm, instruction->operand1, result);
    
    return result;
}

trit_t ternary_interpreter_execute_not(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t a = tvm_get_register(interpreter->vm, instruction->operand2);
    
    trit_t result = ternary_alu_not(interpreter->alu, a);
    tvm_set_register(interpreter->vm, instruction->operand1, result);
    
    return result;
}

trit_t ternary_interpreter_execute_xor(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t a = tvm_get_register(interpreter->vm, instruction->operand2);
    trit_t b = tvm_get_register(interpreter->vm, instruction->operand3);
    
    trit_t result = ternary_alu_xor(interpreter->alu, a, b);
    tvm_set_register(interpreter->vm, instruction->operand1, result);
    
    return result;
}

// =============================================================================
// TERNARY INTERPRETER COMPARISON INSTRUCTIONS
// =============================================================================

trit_t ternary_interpreter_execute_cmp(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t a = tvm_get_register(interpreter->vm, instruction->operand1);
    trit_t b = tvm_get_register(interpreter->vm, instruction->operand2);
    
    trit_t result = ternary_alu_compare(interpreter->alu, a, b);
    
    return result;
}

// =============================================================================
// TERNARY INTERPRETER CONTROL FLOW INSTRUCTIONS
// =============================================================================

trit_t ternary_interpreter_execute_jmp(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    tvm_set_register(interpreter->vm, T3_REGISTER_PC, trit_create(instruction->immediate));
    
    return trit_create(TERNARY_POSITIVE);
}

trit_t ternary_interpreter_execute_jz(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t value = tvm_get_register(interpreter->vm, instruction->operand1);
    
    if (trit_is_neutral(value)) {
        tvm_set_register(interpreter->vm, T3_REGISTER_PC, trit_create(instruction->immediate));
    }
    
    return trit_create(TERNARY_POSITIVE);
}

trit_t ternary_interpreter_execute_jnz(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t value = tvm_get_register(interpreter->vm, instruction->operand1);
    
    if (!trit_is_neutral(value)) {
        tvm_set_register(interpreter->vm, T3_REGISTER_PC, trit_create(instruction->immediate));
    }
    
    return trit_create(TERNARY_POSITIVE);
}

// =============================================================================
// TERNARY INTERPRETER STACK INSTRUCTIONS
// =============================================================================

trit_t ternary_interpreter_execute_call(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Save return address to stack
    trit_t pc = tvm_get_register(interpreter->vm, T3_REGISTER_PC);
    tvm_stack_push(interpreter->vm, pc);
    
    // Jump to target
    tvm_set_register(interpreter->vm, T3_REGISTER_PC, trit_create(instruction->immediate));
    
    return trit_create(TERNARY_POSITIVE);
}

trit_t ternary_interpreter_execute_ret(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Restore return address from stack
    trit_t pc = tvm_stack_pop(interpreter->vm);
    tvm_set_register(interpreter->vm, T3_REGISTER_PC, pc);
    
    return trit_create(TERNARY_POSITIVE);
}

trit_t ternary_interpreter_execute_push(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t value = tvm_get_register(interpreter->vm, instruction->operand1);
    tvm_stack_push(interpreter->vm, value);
    
    return trit_create(TERNARY_POSITIVE);
}

trit_t ternary_interpreter_execute_pop(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t value = tvm_stack_pop(interpreter->vm);
    tvm_set_register(interpreter->vm, instruction->operand1, value);
    
    return trit_create(TERNARY_POSITIVE);
}

// =============================================================================
// TERNARY INTERPRETER SYSTEM INSTRUCTIONS
// =============================================================================

trit_t ternary_interpreter_execute_halt(ternary_interpreter_t* interpreter, t3_instruction_t* instruction) {
    if (interpreter == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    interpreter->halted = true;
    interpreter->running = false;
    
    return trit_create(TERNARY_NEUTRAL);
}

// =============================================================================
// TERNARY INTERPRETER STATUS OPERATIONS
// =============================================================================

bool ternary_interpreter_is_running(ternary_interpreter_t* interpreter) {
    return interpreter != NULL && interpreter->running;
}

bool ternary_interpreter_is_halted(ternary_interpreter_t* interpreter) {
    return interpreter != NULL && interpreter->halted;
}

bool ternary_interpreter_has_error(ternary_interpreter_t* interpreter) {
    return interpreter != NULL && interpreter->error;
}

// =============================================================================
// TERNARY INTERPRETER UTILITY FUNCTIONS
// =============================================================================

void ternary_interpreter_print_status(ternary_interpreter_t* interpreter) {
    if (interpreter == NULL) {
        printf("Ternary Interpreter Status: NULL\n");
        return;
    }
    
    printf("Ternary Interpreter Status:\n");
    printf("  Running: %s\n", interpreter->running ? "true" : "false");
    printf("  Halted: %s\n", interpreter->halted ? "true" : "false");
    printf("  Error: %s\n", interpreter->error ? "true" : "false");
    
    if (interpreter->alu != NULL) {
        printf("  ALU Flags: ");
        ternary_alu_print_flags(interpreter->alu);
    }
}

void ternary_interpreter_debug(ternary_interpreter_t* interpreter) {
    if (interpreter == NULL) {
        printf("Ternary Interpreter Debug: NULL\n");
        return;
    }
    
    printf("Ternary Interpreter Debug:\n");
    printf("  Running: %s\n", interpreter->running ? "true" : "false");
    printf("  Halted: %s\n", interpreter->halted ? "true" : "false");
    printf("  Error: %s\n", interpreter->error ? "true" : "false");
    
    if (interpreter->alu != NULL) {
        printf("  ALU Debug:\n");
        ternary_alu_debug(interpreter->alu);
    }
    
    if (interpreter->vm != NULL) {
        printf("  VM Debug:\n");
        tvm_debug(interpreter->vm);
    }
}
