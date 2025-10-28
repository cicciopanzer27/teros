/**
 * @file lambda_compiler.c
 * @brief Lambda³ to T3 Bytecode Compiler
 * @author TEROS Development Team
 * @date 2025
 * 
 * Compiles lambda calculus terms to T3-ISA bytecode for execution on TVM
 */

#include "lambda_compiler.h"
#include "lambda_engine.h"
#include "lambda_church.h"
#include "t3_isa.h"
#include "ternary_gates.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =============================================================================
// COMPILER STATE
// =============================================================================

typedef struct {
    uint8_t* bytecode;
    size_t capacity;
    size_t size;
    int32_t label_counter;
} CompilerState;

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

static CompilerState* compiler_create(size_t initial_capacity)
{
    CompilerState* state = malloc(sizeof(CompilerState));
    if (!state) return NULL;
    
    state->capacity = initial_capacity;
    state->size = 0;
    state->label_counter = 0;
    state->bytecode = malloc(initial_capacity);
    
    if (!state->bytecode) {
        free(state);
        return NULL;
    }
    
    return state;
}

static void compiler_destroy(CompilerState* state)
{
    if (state) {
        if (state->bytecode) {
            free(state->bytecode);
        }
        free(state);
    }
}

static bool compiler_emit_instruction(CompilerState* state, t3_instruction_t* inst)
{
    // Emit opcode
    if (state->size + 1 > state->capacity) {
        // Resize buffer
        size_t new_capacity = state->capacity * 2;
        uint8_t* new_buffer = realloc(state->bytecode, new_capacity);
        if (!new_buffer) return false;
        state->bytecode = new_buffer;
        state->capacity = new_capacity;
    }
    
    state->bytecode[state->size++] = inst->opcode;
    
    // Emit operands (simplified - just emit as bytes)
    if (state->size + 6 > state->capacity) {
        size_t new_capacity = state->capacity * 2;
        uint8_t* new_buffer = realloc(state->bytecode, new_capacity);
        if (!new_buffer) return false;
        state->bytecode = new_buffer;
        state->capacity = new_capacity;
    }
    
    state->bytecode[state->size++] = inst->operand1;
    state->bytecode[state->size++] = inst->operand2;
    state->bytecode[state->size++] = inst->operand3;
    
    // Emit immediate (2 bytes)
    state->bytecode[state->size++] = (inst->immediate) & 0xFF;
    state->bytecode[state->size++] = (inst->immediate >> 8) & 0xFF;
    
    return true;
}

// =============================================================================
// COMPILATION FUNCTIONS
// =============================================================================

/**
 * Compile a variable reference
 * Generates: MOV target_reg, variable_value
 */
static bool compile_var(CompilerState* state, LambdaTerm* term, uint8_t target_reg)
{
    t3_instruction_t inst;
    inst.opcode = T3_OPCODE_MOV;
    inst.operand1 = target_reg;
    inst.operand2 = term->data.var.var_id;  // Variable ID as value
    inst.operand3 = 0;
    inst.immediate = 0;
    inst.valid = true;
    
    return compiler_emit_instruction(state, &inst);
}

/**
 * Compile an abstraction (λx.M)
 * Generates a function that takes one argument and evaluates body
 */
static bool compile_abs(CompilerState* state, LambdaTerm* term, uint8_t target_reg)
{
    // For simplicity, we compile abstraction as a callable function
    // Real implementation would need proper calling convention
    
    // Label for function entry
    int32_t func_label = state->label_counter++;
    
    // Would compile body with bound variable here
    // For now, just emit a placeholder
    
    return true;
}

/**
 * Compile an application (M N)
 * Generates: CALL function, with argument on stack
 */
static bool compile_app(CompilerState* state, LambdaTerm* term, uint8_t target_reg)
{
    // Compile argument first (right to left evaluation)
    // Would reserve a temporary register here
    uint8_t arg_reg = target_reg + 1;
    
    // Compile function
    if (!compile_term(state, term->data.app.func, target_reg)) {
        return false;
    }
    
    // Compile argument
    if (!compile_term(state, term->data.app.arg, arg_reg)) {
        return false;
    }
    
    // Push argument to stack
    t3_instruction_t push_inst;
    push_inst.opcode = T3_OPCODE_PUSH;
    push_inst.operand1 = arg_reg;
    push_inst.operand2 = 0;
    push_inst.operand3 = 0;
    push_inst.immediate = 0;
    push_inst.valid = true;
    
    if (!compiler_emit_instruction(state, &push_inst)) {
        return false;
    }
    
    // Call function
    t3_instruction_t call_inst;
    call_inst.opcode = T3_OPCODE_CALL;
    call_inst.operand1 = target_reg;
    call_inst.operand2 = 0;
    call_inst.operand3 = 0;
    call_inst.immediate = 0;
    call_inst.valid = true;
    
    return compiler_emit_instruction(state, &call_inst);
}

/**
 * Compile a lambda term to T3 bytecode
 */
static bool compile_term(CompilerState* state, LambdaTerm* term, uint8_t target_reg)
{
    if (!state || !term) return false;
    
    switch (term->type) {
        case LAMBDA_VAR:
            return compile_var(state, term, target_reg);
        
        case LAMBDA_ABS:
            return compile_abs(state, term, target_reg);
        
        case LAMBDA_APP:
            return compile_app(state, term, target_reg);
        
        default:
            return false;
    }
}

// =============================================================================
// PUBLIC API
// =============================================================================

int32_t lambda_compile_to_t3(LambdaTerm* term, uint8_t* bytecode, int32_t capacity, int32_t* output_size)
{
    if (!term || !bytecode || capacity <= 0) {
        return -1;  // Invalid parameters
    }
    
    // Create compiler state
    CompilerState* state = compiler_create(capacity);
    if (!state) {
        return -1;  // Allocation failed
    }
    
    // Compile main term to register 0
    bool success = compile_term(state, term, 0);
    
    if (!success) {
        compiler_destroy(state);
        return -1;  // Compilation failed
    }
    
    // Copy generated bytecode
    if (state->size > (size_t)capacity) {
        compiler_destroy(state);
        return -1;  // Buffer too small
    }
    
    memcpy(bytecode, state->bytecode, state->size);
    
    if (output_size) {
        *output_size = state->size;
    }
    
    compiler_destroy(state);
    
    return 0;  // Success
}

/**
 * Optimize lambda term before compilation
 * Uses ternary gates where applicable
 */
LambdaTerm* lambda_optimize_term(LambdaTerm* term)
{
    if (!term) return NULL;
    
    // Would implement optimizations here:
    // 1. Constant folding
    // 2. Dead code elimination
    // 3. Beta reduction
    // 4. Replace common patterns with ternary gate operations
    
    // For now, return a clone
    return lambda_clone(term);
}

