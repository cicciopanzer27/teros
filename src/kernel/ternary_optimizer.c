/**
 * @file ternary_optimizer.c
 * @brief Ternary code optimizer implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_optimizer.h"
#include "trit.h"
#include "t3_isa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Forward declarations for helper functions
static bool ternary_optimizer_instructions_independent(t3_instruction_t* a, t3_instruction_t* b);
static bool ternary_optimizer_should_swap(t3_instruction_t* a, t3_instruction_t* b);
static int ternary_optimizer_get_write_register(t3_instruction_t* instruction);
static int ternary_optimizer_get_read_register(t3_instruction_t* instruction);
static bool ternary_optimizer_is_arithmetic(t3_instruction_t* instruction);
static bool ternary_optimizer_is_memory(t3_instruction_t* instruction);

// =============================================================================
// TERNARY OPTIMIZER IMPLEMENTATION
// =============================================================================

ternary_optimizer_t* ternary_optimizer_create(void) {
    ternary_optimizer_t* optimizer = malloc(sizeof(ternary_optimizer_t));
    if (optimizer == NULL) return NULL;
    
    optimizer->optimizations_enabled = 0;
    optimizer->optimization_level = TERNARY_OPTIMIZATION_LEVEL_NONE;
    optimizer->error = false;
    optimizer->error_message = NULL;
    
    return optimizer;
}

void ternary_optimizer_destroy(ternary_optimizer_t* optimizer) {
    if (optimizer != NULL) {
        if (optimizer->error_message != NULL) {
            free(optimizer->error_message);
        }
        free(optimizer);
    }
}

// =============================================================================
// TERNARY OPTIMIZER CONFIGURATION
// =============================================================================

void ternary_optimizer_set_optimization_level(ternary_optimizer_t* optimizer, ternary_optimization_level_t level) {
    if (optimizer == NULL) return;
    
    optimizer->optimization_level = level;
    
    // Set optimizations based on level
    switch (level) {
        case TERNARY_OPTIMIZATION_LEVEL_NONE:
            optimizer->optimizations_enabled = 0;
            break;
        case TERNARY_OPTIMIZATION_LEVEL_BASIC:
            optimizer->optimizations_enabled = TERNARY_OPTIMIZATION_CONSTANT_FOLDING |
                                             TERNARY_OPTIMIZATION_DEAD_CODE_ELIMINATION;
            break;
        case TERNARY_OPTIMIZATION_LEVEL_ADVANCED:
            optimizer->optimizations_enabled = TERNARY_OPTIMIZATION_CONSTANT_FOLDING |
                                             TERNARY_OPTIMIZATION_DEAD_CODE_ELIMINATION |
                                             TERNARY_OPTIMIZATION_LOOP_UNROLLING |
                                             TERNARY_OPTIMIZATION_INSTRUCTION_SCHEDULING;
            break;
        case TERNARY_OPTIMIZATION_LEVEL_AGGRESSIVE:
            optimizer->optimizations_enabled = TERNARY_OPTIMIZATION_CONSTANT_FOLDING |
                                             TERNARY_OPTIMIZATION_DEAD_CODE_ELIMINATION |
                                             TERNARY_OPTIMIZATION_LOOP_UNROLLING |
                                             TERNARY_OPTIMIZATION_INSTRUCTION_SCHEDULING |
                                             TERNARY_OPTIMIZATION_REGISTER_ALLOCATION |
                                             TERNARY_OPTIMIZATION_PEEPHOLE;
            break;
    }
}

void ternary_optimizer_enable_optimization(ternary_optimizer_t* optimizer, ternary_optimization_t optimization) {
    if (optimizer == NULL) return;
    
    optimizer->optimizations_enabled |= optimization;
}

void ternary_optimizer_disable_optimization(ternary_optimizer_t* optimizer, ternary_optimization_t optimization) {
    if (optimizer == NULL) return;
    
    optimizer->optimizations_enabled &= ~optimization;
}

bool ternary_optimizer_is_optimization_enabled(ternary_optimizer_t* optimizer, ternary_optimization_t optimization) {
    return optimizer != NULL && (optimizer->optimizations_enabled & optimization) != 0;
}

// =============================================================================
// TERNARY OPTIMIZER OPTIMIZATION
// =============================================================================

bool ternary_optimizer_optimize(ternary_optimizer_t* optimizer, t3_instruction_t* instructions, size_t instruction_count) {
    if (optimizer == NULL || instructions == NULL || instruction_count == 0) {
        return false;
    }
    
    optimizer->error = false;
    
    // Apply optimizations based on enabled flags
    if (ternary_optimizer_is_optimization_enabled(optimizer, TERNARY_OPTIMIZATION_CONSTANT_FOLDING)) {
        if (!ternary_optimizer_constant_folding(optimizer, instructions, instruction_count)) {
            return false;
        }
    }
    
    if (ternary_optimizer_is_optimization_enabled(optimizer, TERNARY_OPTIMIZATION_DEAD_CODE_ELIMINATION)) {
        if (!ternary_optimizer_dead_code_elimination(optimizer, instructions, instruction_count)) {
            return false;
        }
    }
    
    if (ternary_optimizer_is_optimization_enabled(optimizer, TERNARY_OPTIMIZATION_LOOP_UNROLLING)) {
        if (!ternary_optimizer_loop_unrolling(optimizer, instructions, instruction_count)) {
            return false;
        }
    }
    
    if (ternary_optimizer_is_optimization_enabled(optimizer, TERNARY_OPTIMIZATION_INSTRUCTION_SCHEDULING)) {
        if (!ternary_optimizer_instruction_scheduling(optimizer, instructions, instruction_count)) {
            return false;
        }
    }
    
    if (ternary_optimizer_is_optimization_enabled(optimizer, TERNARY_OPTIMIZATION_REGISTER_ALLOCATION)) {
        if (!ternary_optimizer_register_allocation(optimizer, instructions, instruction_count)) {
            return false;
        }
    }
    
    if (ternary_optimizer_is_optimization_enabled(optimizer, TERNARY_OPTIMIZATION_PEEPHOLE)) {
        if (!ternary_optimizer_peephole(optimizer, instructions, instruction_count)) {
            return false;
        }
    }
    
    return true;
}

// =============================================================================
// TERNARY OPTIMIZER CONSTANT FOLDING
// =============================================================================

bool ternary_optimizer_constant_folding(ternary_optimizer_t* optimizer, t3_instruction_t* instructions, size_t instruction_count) {
    if (optimizer == NULL || instructions == NULL || instruction_count == 0) {
        return false;
    }
    
    // Simple constant folding - replace arithmetic operations with constants
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = &instructions[i];
        
        if (instruction->opcode == T3_OPCODE_ADD && 
            instruction->operand2 == 0 && instruction->operand3 == 0) {
            // ADD R1, 0, 0 -> LOAD R1, 0
            instruction->opcode = T3_OPCODE_LOAD;
            instruction->operand2 = 0;
            instruction->operand3 = 0;
            instruction->immediate = 0;
        }
        else if (instruction->opcode == T3_OPCODE_MUL && 
                 instruction->operand2 == 0 && instruction->operand3 == 0) {
            // MUL R1, 0, 0 -> LOAD R1, 0
            instruction->opcode = T3_OPCODE_LOAD;
            instruction->operand2 = 0;
            instruction->operand3 = 0;
            instruction->immediate = 0;
        }
        else if (instruction->opcode == T3_OPCODE_MUL && 
                 instruction->operand2 == 1 && instruction->operand3 == 1) {
            // MUL R1, 1, 1 -> LOAD R1, 1
            instruction->opcode = T3_OPCODE_LOAD;
            instruction->operand2 = 0;
            instruction->operand3 = 0;
            instruction->immediate = 1;
        }
    }
    
    return true;
}

// =============================================================================
// TERNARY OPTIMIZER DEAD CODE ELIMINATION
// =============================================================================

bool ternary_optimizer_dead_code_elimination(ternary_optimizer_t* optimizer, t3_instruction_t* instructions, size_t instruction_count) {
    if (optimizer == NULL || instructions == NULL || instruction_count == 0) {
        return false;
    }
    
    // Simple dead code elimination - remove unreachable code after HALT
    for (size_t i = 0; i < instruction_count; i++) {
        if (instructions[i].opcode == T3_OPCODE_HALT) {
            // Mark all instructions after HALT as NOP
            for (size_t j = i + 1; j < instruction_count; j++) {
                instructions[j].opcode = T3_OPCODE_NOP;
                instructions[j].operand1 = 0;
                instructions[j].operand2 = 0;
                instructions[j].operand3 = 0;
                instructions[j].immediate = 0;
            }
            break;
        }
    }
    
    return true;
}

// =============================================================================
// TERNARY OPTIMIZER LOOP UNROLLING
// =============================================================================

bool ternary_optimizer_loop_unrolling(ternary_optimizer_t* optimizer, t3_instruction_t* instructions, size_t instruction_count) {
    if (optimizer == NULL || instructions == NULL || instruction_count == 0) {
        return false;
    }
    
    // Simple loop unrolling - unroll small loops
    for (size_t i = 0; i < instruction_count - 1; i++) {
        if (instructions[i].opcode == T3_OPCODE_JMP && 
            instructions[i].immediate < (int)i) {
            // Found a backward jump (loop)
            int loop_start = instructions[i].immediate;
            int loop_size = i - loop_start;
            
            if (loop_size <= 4) {  // Only unroll small loops
                // Unroll the loop 2 times
                for (int j = 0; j < loop_size; j++) {
                    if (i + (size_t)j + 1 < instruction_count) {
                        instructions[i + j + 1] = instructions[loop_start + j];
                    }
                }
                // Remove the jump
                instructions[i].opcode = T3_OPCODE_NOP;
            }
        }
    }
    
    return true;
}

// =============================================================================
// TERNARY OPTIMIZER INSTRUCTION SCHEDULING
// =============================================================================

bool ternary_optimizer_instruction_scheduling(ternary_optimizer_t* optimizer, t3_instruction_t* instructions, size_t instruction_count) {
    if (optimizer == NULL || instructions == NULL || instruction_count == 0) {
        return false;
    }
    
    // Simple instruction scheduling - reorder independent instructions
    for (size_t i = 0; i < instruction_count - 1; i++) {
        t3_instruction_t* current = &instructions[i];
        t3_instruction_t* next = &instructions[i + 1];
        
        // Check if instructions are independent
        if (ternary_optimizer_instructions_independent(current, next)) {
            // Swap instructions if it improves performance
            if (ternary_optimizer_should_swap(current, next)) {
                t3_instruction_t temp = *current;
                *current = *next;
                *next = temp;
            }
        }
    }
    
    return true;
}

static bool ternary_optimizer_instructions_independent(t3_instruction_t* a, t3_instruction_t* b) {
    if (a == NULL || b == NULL) return false;
    
    // Check if instruction A writes to a register that B reads
    int a_writes = ternary_optimizer_get_write_register(a);
    int b_reads = ternary_optimizer_get_read_register(b);
    
    if (a_writes != -1 && b_reads != -1 && a_writes == b_reads) {
        return false;
    }
    
    // Check if instruction B writes to a register that A reads
    int b_writes = ternary_optimizer_get_write_register(b);
    int a_reads = ternary_optimizer_get_read_register(a);
    
    if (b_writes != -1 && a_reads != -1 && b_writes == a_reads) {
        return false;
    }
    
    return true;
}

static bool ternary_optimizer_should_swap(t3_instruction_t* a, t3_instruction_t* b) {
    if (a == NULL || b == NULL) return false;
    
    // Simple heuristic - prefer arithmetic operations before memory operations
    if (ternary_optimizer_is_arithmetic(a) && ternary_optimizer_is_memory(b)) {
        return true;
    }
    
    return false;
}

static int ternary_optimizer_get_write_register(t3_instruction_t* instruction) {
    if (instruction == NULL) return -1;
    
    switch (instruction->opcode) {
        case T3_OPCODE_LOAD:
        case T3_OPCODE_ADD:
        case T3_OPCODE_SUB:
        case T3_OPCODE_MUL:
        case T3_OPCODE_DIV:
        case T3_OPCODE_AND:
        case T3_OPCODE_OR:
        case T3_OPCODE_NOT:
        case T3_OPCODE_XOR:
            return instruction->operand1;
        default:
            return -1;
    }
}

static int ternary_optimizer_get_read_register(t3_instruction_t* instruction) {
    if (instruction == NULL) return -1;
    
    switch (instruction->opcode) {
        case T3_OPCODE_STORE:
        case T3_OPCODE_ADD:
        case T3_OPCODE_SUB:
        case T3_OPCODE_MUL:
        case T3_OPCODE_DIV:
        case T3_OPCODE_AND:
        case T3_OPCODE_OR:
        case T3_OPCODE_XOR:
        case T3_OPCODE_CMP:
            return instruction->operand2;
        default:
            return -1;
    }
}

static bool ternary_optimizer_is_arithmetic(t3_instruction_t* instruction) {
    if (instruction == NULL) return false;
    
    return instruction->opcode == T3_OPCODE_ADD ||
           instruction->opcode == T3_OPCODE_SUB ||
           instruction->opcode == T3_OPCODE_MUL ||
           instruction->opcode == T3_OPCODE_DIV;
}

static bool ternary_optimizer_is_memory(t3_instruction_t* instruction) {
    if (instruction == NULL) return false;
    
    return instruction->opcode == T3_OPCODE_LOAD ||
           instruction->opcode == T3_OPCODE_STORE;
}

// =============================================================================
// TERNARY OPTIMIZER REGISTER ALLOCATION
// =============================================================================

bool ternary_optimizer_register_allocation(ternary_optimizer_t* optimizer, t3_instruction_t* instructions, size_t instruction_count) {
    if (optimizer == NULL || instructions == NULL || instruction_count == 0) {
        return false;
    }
    
    // Simple register allocation - use lower numbered registers first
    // TODO: Implement advanced register allocation using register_map
    (void)optimizer;  // Unused for now
    
    // Map registers to optimize usage
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = &instructions[i];
        
        // Map operands to optimized registers
        if (instruction->operand1 >= T3_REGISTER_COUNT) {
            instruction->operand1 = instruction->operand1 % T3_REGISTER_COUNT;
        }
        if (instruction->operand2 >= T3_REGISTER_COUNT) {
            instruction->operand2 = instruction->operand2 % T3_REGISTER_COUNT;
        }
        if (instruction->operand3 >= T3_REGISTER_COUNT) {
            instruction->operand3 = instruction->operand3 % T3_REGISTER_COUNT;
        }
    }
    
    return true;
}

// =============================================================================
// TERNARY OPTIMIZER PEEPHOLE
// =============================================================================

bool ternary_optimizer_peephole(ternary_optimizer_t* optimizer, t3_instruction_t* instructions, size_t instruction_count) {
    if (optimizer == NULL || instructions == NULL || instruction_count == 0) {
        return false;
    }
    
    // Simple peephole optimization - remove redundant instructions
    for (size_t i = 0; i < instruction_count - 1; i++) {
        t3_instruction_t* current = &instructions[i];
        t3_instruction_t* next = &instructions[i + 1];
        
        // Remove redundant LOAD instructions
        if (current->opcode == T3_OPCODE_LOAD && next->opcode == T3_OPCODE_LOAD &&
            current->operand1 == next->operand1) {
            next->opcode = T3_OPCODE_NOP;
        }
        
        // Remove redundant STORE instructions
        if (current->opcode == T3_OPCODE_STORE && next->opcode == T3_OPCODE_STORE &&
            current->operand1 == next->operand1 && current->operand2 == next->operand2) {
            next->opcode = T3_OPCODE_NOP;
        }
    }
    
    return true;
}

// =============================================================================
// TERNARY OPTIMIZER ERROR HANDLING
// =============================================================================

bool ternary_optimizer_has_error(ternary_optimizer_t* optimizer) {
    return optimizer != NULL && optimizer->error;
}

const char* ternary_optimizer_get_error_message(ternary_optimizer_t* optimizer) {
    if (optimizer == NULL) return NULL;
    return optimizer->error_message;
}

void ternary_optimizer_set_error(ternary_optimizer_t* optimizer, const char* message) {
    if (optimizer == NULL) return;
    
    optimizer->error = true;
    if (optimizer->error_message != NULL) {
        free(optimizer->error_message);
    }
    optimizer->error_message = message != NULL ? strdup(message) : NULL;
}

// =============================================================================
// TERNARY OPTIMIZER UTILITY FUNCTIONS
// =============================================================================

void ternary_optimizer_print_optimizations(ternary_optimizer_t* optimizer) {
    if (optimizer == NULL) {
        printf("Ternary Optimizer: NULL\n");
        return;
    }
    
    printf("Ternary Optimizer:\n");
    printf("  Optimization Level: %d\n", optimizer->optimization_level);
    printf("  Optimizations Enabled: 0x%02x\n", optimizer->optimizations_enabled);
    printf("  Error: %s\n", optimizer->error ? "true" : "false");
    printf("  Error Message: %s\n", optimizer->error_message != NULL ? optimizer->error_message : "None");
}

void ternary_optimizer_debug(ternary_optimizer_t* optimizer) {
    if (optimizer == NULL) {
        printf("Ternary Optimizer Debug: NULL\n");
        return;
    }
    
    printf("Ternary Optimizer Debug:\n");
    printf("  Optimization Level: %d\n", optimizer->optimization_level);
    printf("  Optimizations Enabled: 0x%02x\n", optimizer->optimizations_enabled);
    printf("  Error: %s\n", optimizer->error ? "true" : "false");
    printf("  Error Message: %s\n", optimizer->error_message != NULL ? optimizer->error_message : "None");
}
