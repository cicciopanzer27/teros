/**
 * @file t3_isa.c
 * @brief T3-ISA (Ternary 3-Instruction Set Architecture) implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "t3_isa.h"
#include "trit.h"
#include "trit_array.h"
#include "ternary_gates.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// T3-ISA IMPLEMENTATION
// =============================================================================

t3_instruction_t* t3_instruction_create(void) {
    t3_instruction_t* instruction = malloc(sizeof(t3_instruction_t));
    if (instruction == NULL) return NULL;
    
    instruction->opcode = 0;
    instruction->operand1 = 0;
    instruction->operand2 = 0;
    instruction->operand3 = 0;
    instruction->immediate = 0;
    instruction->valid = false;
    
    return instruction;
}

void t3_instruction_destroy(t3_instruction_t* instruction) {
    if (instruction != NULL) {
        free(instruction);
    }
}

void t3_instruction_set(t3_instruction_t* instruction, uint8_t opcode, 
                       uint8_t operand1, uint8_t operand2, uint8_t operand3, 
                       int16_t immediate) {
    if (instruction != NULL) {
        instruction->opcode = opcode;
        instruction->operand1 = operand1;
        instruction->operand2 = operand2;
        instruction->operand3 = operand3;
        instruction->immediate = immediate;
        instruction->valid = true;
    }
}

bool t3_instruction_is_valid(t3_instruction_t* instruction) {
    return instruction != NULL && instruction->valid;
}

// =============================================================================
// T3-ISA INSTRUCTION EXECUTION
// =============================================================================

trit_t t3_instruction_execute(t3_instruction_t* instruction, t3_register_t* registers) {
    if (!t3_instruction_is_valid(instruction) || registers == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    switch (instruction->opcode) {
        case T3_OPCODE_LOAD:
            return t3_execute_load(instruction, registers);
        case T3_OPCODE_STORE:
            return t3_execute_store(instruction, registers);
        case T3_OPCODE_ADD:
            return t3_execute_add(instruction, registers);
        case T3_OPCODE_SUB:
            return t3_execute_sub(instruction, registers);
        case T3_OPCODE_MUL:
            return t3_execute_mul(instruction, registers);
        case T3_OPCODE_DIV:
            return t3_execute_div(instruction, registers);
        case T3_OPCODE_AND:
            return t3_execute_and(instruction, registers);
        case T3_OPCODE_OR:
            return t3_execute_or(instruction, registers);
        case T3_OPCODE_NOT:
            return t3_execute_not(instruction, registers);
        case T3_OPCODE_XOR:
            return t3_execute_xor(instruction, registers);
        case T3_OPCODE_CMP:
            return t3_execute_cmp(instruction, registers);
        case T3_OPCODE_JMP:
            return t3_execute_jmp(instruction, registers);
        case T3_OPCODE_JZ:
            return t3_execute_jz(instruction, registers);
        case T3_OPCODE_JNZ:
            return t3_execute_jnz(instruction, registers);
        case T3_OPCODE_CALL:
            return t3_execute_call(instruction, registers);
        case T3_OPCODE_RET:
            return t3_execute_ret(instruction, registers);
        case T3_OPCODE_PUSH:
            return t3_execute_push(instruction, registers);
        case T3_OPCODE_POP:
            return t3_execute_pop(instruction, registers);
        case T3_OPCODE_HALT:
            return t3_execute_halt(instruction, registers);
        case T3_OPCODE_SYSCALL:
            return t3_execute_syscall(instruction, registers);
        case T3_OPCODE_IRET:
            return t3_execute_iret(instruction, registers);
        case T3_OPCODE_CLI:
            return t3_execute_cli(instruction, registers);
        case T3_OPCODE_STI:
            return t3_execute_sti(instruction, registers);
        case T3_OPCODE_CPUID:
            return t3_execute_cpuid(instruction, registers);
        case T3_OPCODE_RDTSC:
            return t3_execute_rdtsc(instruction, registers);
        case T3_OPCODE_INT:
            return t3_execute_int(instruction, registers);
        case T3_OPCODE_MOV:
            return t3_execute_mov(instruction, registers);
        case T3_OPCODE_LEA:
            return t3_execute_lea(instruction, registers);
        case T3_OPCODE_TST:
            return t3_execute_tst(instruction, registers);
        case T3_OPCODE_TGATE:
            return t3_execute_tgate(instruction, registers);
        default:
            return trit_create(TERNARY_UNKNOWN);
    }
}

// =============================================================================
// T3-ISA DATA MOVEMENT INSTRUCTIONS
// =============================================================================

trit_t t3_execute_load(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Load immediate value or from another register
    if (instruction->operand2 == 0) {
        // Load immediate
        registers[instruction->operand1] = trit_create(instruction->immediate);
    } else {
        // Load from register
        if (instruction->operand2 < T3_REGISTER_COUNT) {
            registers[instruction->operand1] = registers[instruction->operand2];
        } else {
            return trit_create(TERNARY_UNKNOWN);
        }
    }
    
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_store(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT || instruction->operand2 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Store register value to another register
    registers[instruction->operand2] = registers[instruction->operand1];
    return trit_create(TERNARY_POSITIVE);
}

// =============================================================================
// T3-ISA ARITHMETIC INSTRUCTIONS
// =============================================================================

trit_t t3_execute_add(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT || 
        instruction->operand2 >= T3_REGISTER_COUNT || 
        instruction->operand3 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_add(registers[instruction->operand2], registers[instruction->operand3]);
    registers[instruction->operand1] = result;
    return result;
}

trit_t t3_execute_sub(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT || 
        instruction->operand2 >= T3_REGISTER_COUNT || 
        instruction->operand3 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_subtract(registers[instruction->operand2], registers[instruction->operand3]);
    registers[instruction->operand1] = result;
    return result;
}

trit_t t3_execute_mul(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT || 
        instruction->operand2 >= T3_REGISTER_COUNT || 
        instruction->operand3 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_multiply(registers[instruction->operand2], registers[instruction->operand3]);
    registers[instruction->operand1] = result;
    return result;
}

trit_t t3_execute_div(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT || 
        instruction->operand2 >= T3_REGISTER_COUNT || 
        instruction->operand3 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_divide(registers[instruction->operand2], registers[instruction->operand3]);
    registers[instruction->operand1] = result;
    return result;
}

// =============================================================================
// T3-ISA LOGIC INSTRUCTIONS
// =============================================================================

trit_t t3_execute_and(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT || 
        instruction->operand2 >= T3_REGISTER_COUNT || 
        instruction->operand3 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_and(registers[instruction->operand2], registers[instruction->operand3]);
    registers[instruction->operand1] = result;
    return result;
}

trit_t t3_execute_or(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT || 
        instruction->operand2 >= T3_REGISTER_COUNT || 
        instruction->operand3 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_or(registers[instruction->operand2], registers[instruction->operand3]);
    registers[instruction->operand1] = result;
    return result;
}

trit_t t3_execute_not(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT || 
        instruction->operand2 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_not(registers[instruction->operand2]);
    registers[instruction->operand1] = result;
    return result;
}

trit_t t3_execute_xor(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT || 
        instruction->operand2 >= T3_REGISTER_COUNT || 
        instruction->operand3 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_xor(registers[instruction->operand2], registers[instruction->operand3]);
    registers[instruction->operand1] = result;
    return result;
}

// =============================================================================
// T3-ISA COMPARISON INSTRUCTIONS
// =============================================================================

trit_t t3_execute_cmp(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT || 
        instruction->operand2 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_equal(registers[instruction->operand1], registers[instruction->operand2]);
    return result;
}

// =============================================================================
// T3-ISA CONTROL FLOW INSTRUCTIONS
// =============================================================================

trit_t t3_execute_jmp(t3_instruction_t* instruction, t3_register_t* registers) {
    if (registers[T3_REGISTER_PC].valid) {
        int pc = trit_to_int(registers[T3_REGISTER_PC]);
        pc = instruction->immediate;
        registers[T3_REGISTER_PC] = trit_create(pc);
    }
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_jz(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    if (trit_is_neutral(registers[instruction->operand1])) {
        return t3_execute_jmp(instruction, registers);
    }
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_jnz(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    if (!trit_is_neutral(registers[instruction->operand1])) {
        return t3_execute_jmp(instruction, registers);
    }
    return trit_create(TERNARY_POSITIVE);
}

// =============================================================================
// T3-ISA STACK INSTRUCTIONS
// =============================================================================

trit_t t3_execute_call(t3_instruction_t* instruction, t3_register_t* registers) {
    // Save return address to stack
    if (registers[T3_REGISTER_SP].valid && registers[T3_REGISTER_PC].valid) {
        int sp = trit_to_int(registers[T3_REGISTER_SP]);
        // PC is read but not used currently - in full implementation would be pushed to stack
        // int pc = trit_to_int(registers[T3_REGISTER_PC]);
        
        // Push PC to stack (simplified)
        registers[T3_REGISTER_SP] = trit_create(sp - 1);
        
        // Jump to target
        registers[T3_REGISTER_PC] = trit_create(instruction->immediate);
    }
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_ret(t3_instruction_t* instruction, t3_register_t* registers) {
    (void)instruction;
    // Restore return address from stack
    if (registers[T3_REGISTER_SP].valid) {
        int sp = trit_to_int(registers[T3_REGISTER_SP]);
        
        // Pop PC from stack (simplified)
        registers[T3_REGISTER_SP] = trit_create(sp + 1);
        registers[T3_REGISTER_PC] = trit_create(sp + 1); // Simplified
    }
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_push(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT || !registers[T3_REGISTER_SP].valid) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    int sp = trit_to_int(registers[T3_REGISTER_SP]);
    registers[T3_REGISTER_SP] = trit_create(sp - 1);
    // In a real implementation, store the value to memory at SP
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_pop(t3_instruction_t* instruction, t3_register_t* registers) {
    if (instruction->operand1 >= T3_REGISTER_COUNT || !registers[T3_REGISTER_SP].valid) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    int sp = trit_to_int(registers[T3_REGISTER_SP]);
    registers[T3_REGISTER_SP] = trit_create(sp + 1);
    // In a real implementation, load the value from memory at SP
    return trit_create(TERNARY_POSITIVE);
}

// =============================================================================
// T3-ISA SYSTEM INSTRUCTIONS
// =============================================================================

trit_t t3_execute_halt(t3_instruction_t* instruction, t3_register_t* registers) {
    (void)instruction;
    (void)registers;
    // Halt execution
    return trit_create(TERNARY_NEUTRAL);
}

// =============================================================================
// T3-ISA EXTENDED INSTRUCTIONS
// =============================================================================

trit_t t3_execute_syscall(t3_instruction_t* instruction, t3_register_t* registers) {
    (void)instruction;
    // Syscall interface - syscall number in operand1
    // Parameters in R1-R5, result in R0
    // Store return address for IRET
    if (registers[T3_REGISTER_LR].valid) {
        // Save return address
        registers[T3_REGISTER_LR] = registers[T3_REGISTER_PC];
    }
    
    // Jump to syscall handler
    // In real implementation, this would dispatch to kernel syscall table
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_iret(t3_instruction_t* instruction, t3_register_t* registers) {
    (void)instruction;
    // Interrupt return - restore return address
    if (registers[T3_REGISTER_LR].valid) {
        registers[T3_REGISTER_PC] = registers[T3_REGISTER_LR];
    }
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_cli(t3_instruction_t* instruction, t3_register_t* registers) {
    (void)instruction;
    (void)registers;
    // Clear interrupt flag (CLI - Clear Interrupts)
    // Set interrupt disabled flag in status register
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_sti(t3_instruction_t* instruction, t3_register_t* registers) {
    (void)instruction;
    (void)registers;
    // Set interrupt flag (STI - Set Interrupts)
    // Clear interrupt disabled flag in status register
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_cpuid(t3_instruction_t* instruction, t3_register_t* registers) {
    // CPU identification - store CPU info in registers
    if (instruction->operand1 < T3_REGISTER_COUNT) {
        // Store CPU ID in destination register
        registers[instruction->operand1] = trit_create(1); // Simplified
    }
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_rdtsc(t3_instruction_t* instruction, t3_register_t* registers) {
    // Read timestamp counter - store current time in register
    if (instruction->operand1 < T3_REGISTER_COUNT) {
        // In a real implementation, this would read a hardware counter
        registers[instruction->operand1] = trit_create(0); // Simplified
    }
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_int(t3_instruction_t* instruction, t3_register_t* registers) {
    (void)instruction;
    (void)registers;
    // Software interrupt - trigger interrupt vector
    // Immediate value specifies interrupt number
    // In a real implementation, this would call the interrupt handler
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_mov(t3_instruction_t* instruction, t3_register_t* registers) {
    // Move instruction - simplified version of load
    if (instruction->operand1 >= T3_REGISTER_COUNT || 
        instruction->operand2 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    registers[instruction->operand1] = registers[instruction->operand2];
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_lea(t3_instruction_t* instruction, t3_register_t* registers) {
    // Load effective address - calculate address
    if (instruction->operand1 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Calculate address from base + offset
    registers[instruction->operand1] = trit_create(instruction->immediate);
    return trit_create(TERNARY_POSITIVE);
}

trit_t t3_execute_tst(t3_instruction_t* instruction, t3_register_t* registers) {
    // Test instruction - set condition flags based on value
    if (instruction->operand1 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Update condition register
    trit_t value = registers[instruction->operand1];
    registers[T3_REGISTER_CR] = value;
    
    return trit_create(TERNARY_POSITIVE);
}

// =============================================================================
// T3-ISA UTILITY FUNCTIONS
// =============================================================================

void t3_instruction_print(t3_instruction_t* instruction) {
    // DEBUG: printf version disabled (requires printf)
    if (instruction == NULL) {
        return;
    }
    // Would print: opcode, operand1, operand2, operand3, immediate, valid
    (void)instruction;
}

const char* t3_opcode_to_string(uint8_t opcode) {
    switch (opcode) {
        case T3_OPCODE_LOAD: return "LOAD";
        case T3_OPCODE_STORE: return "STORE";
        case T3_OPCODE_ADD: return "ADD";
        case T3_OPCODE_SUB: return "SUB";
        case T3_OPCODE_MUL: return "MUL";
        case T3_OPCODE_DIV: return "DIV";
        case T3_OPCODE_AND: return "AND";
        case T3_OPCODE_OR: return "OR";
        case T3_OPCODE_NOT: return "NOT";
        case T3_OPCODE_XOR: return "XOR";
        case T3_OPCODE_CMP: return "CMP";
        case T3_OPCODE_JMP: return "JMP";
        case T3_OPCODE_JZ: return "JZ";
        case T3_OPCODE_JNZ: return "JNZ";
        case T3_OPCODE_CALL: return "CALL";
        case T3_OPCODE_RET: return "RET";
        case T3_OPCODE_PUSH: return "PUSH";
        case T3_OPCODE_POP: return "POP";
        case T3_OPCODE_HALT: return "HALT";
        case T3_OPCODE_NOP: return "NOP";
        case T3_OPCODE_SYSCALL: return "SYSCALL";
        case T3_OPCODE_IRET: return "IRET";
        case T3_OPCODE_CLI: return "CLI";
        case T3_OPCODE_STI: return "STI";
        case T3_OPCODE_CPUID: return "CPUID";
        case T3_OPCODE_RDTSC: return "RDTSC";
        case T3_OPCODE_INT: return "INT";
        case T3_OPCODE_MOV: return "MOV";
        case T3_OPCODE_LEA: return "LEA";
        case T3_OPCODE_TST: return "TST";
        case T3_OPCODE_TGATE: return "TGATE";
        default: return "UNKNOWN";
    }
}

// =============================================================================
// TERNARY GATE INSTRUCTION
// =============================================================================

/**
 * Execute ternary gate operation
 * TGATE gate_id, reg_src1, reg_src2, reg_dst
 * 
 * Evaluates a ternary logic gate using the 19,683 lookup tables
 */
trit_t t3_execute_tgate(t3_instruction_t* instruction, t3_register_t* registers)
{
    if (instruction->operand1 >= T3_REGISTER_COUNT ||
        instruction->operand2 >= T3_REGISTER_COUNT ||
        instruction->operand3 >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Extract gate ID from immediate value
    uint16_t gate_id = (uint16_t)instruction->immediate;
    
    // Get source values
    trit_t src1 = registers[instruction->operand2];
    trit_t src2 = registers[instruction->operand3];
    
    // Evaluate gate
    trit_t result = ternary_gate_eval(gate_id, src1, src2);
    
    // Store result
    registers[instruction->operand1] = result;
    
    return result;
}
