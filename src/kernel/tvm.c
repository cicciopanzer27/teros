/**
 * @file tvm.c
 * @brief Ternary Virtual Machine (TVM) implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "tvm.h"
#include "trit.h"
#include "trit_array.h"
#include "t3_isa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TVM IMPLEMENTATION
// =============================================================================

tvm_t* tvm_create(size_t memory_size) {
    tvm_t* vm = malloc(sizeof(tvm_t));
    if (vm == NULL) return NULL;
    
    vm->memory_size = memory_size;
    vm->memory = malloc(memory_size * sizeof(trit_t));
    if (vm->memory == NULL) {
        free(vm);
        return NULL;
    }
    
    // Initialize instruction cache (64 entries, direct-mapped)
    vm->icache_size = 64;
    vm->icache_mask = 0x3F; // 64-1
    vm->icache = calloc(vm->icache_size, sizeof(icache_entry_t));
    if (vm->icache == NULL) {
        free(vm->memory);
        free(vm);
        return NULL;
    }
    
    // Initialize branch predictor (256 entries)
    vm->bp_table_size = 256;
    vm->bp_table = calloc(vm->bp_table_size, sizeof(branch_predictor_t));
    if (vm->bp_table == NULL) {
        free(vm->icache);
        free(vm->memory);
        free(vm);
        return NULL;
    }
    
    // Initialize registers
    for (int i = 0; i < T3_REGISTER_COUNT; i++) {
        vm->registers[i] = trit_create(TERNARY_NEUTRAL);
    }
    
    // Initialize memory
    for (size_t i = 0; i < memory_size; i++) {
        vm->memory[i] = trit_create(TERNARY_NEUTRAL);
    }
    
    vm->running = false;
    vm->halted = false;
    vm->error = false;
    vm->instructions_executed = 0;
    vm->cache_hits = 0;
    vm->cache_misses = 0;
    vm->branch_predictions = 0;
    vm->branch_mispredictions = 0;
    
    return vm;
}

void tvm_destroy(tvm_t* vm) {
    if (vm != NULL) {
        if (vm->memory != NULL) {
            free(vm->memory);
        }
        if (vm->icache != NULL) {
            // Free cached instructions
            for (size_t i = 0; i < vm->icache_size; i++) {
                if (vm->icache[i].valid && vm->icache[i].instruction != NULL) {
                    free(vm->icache[i].instruction);
                }
            }
            free(vm->icache);
        }
        if (vm->bp_table != NULL) {
            free(vm->bp_table);
        }
        free(vm);
    }
}

// =============================================================================
// TVM EXECUTION
// =============================================================================

trit_t tvm_execute_instruction(tvm_t* vm, t3_instruction_t* instruction) {
    if (vm == NULL || instruction == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return t3_instruction_execute(instruction, vm->registers);
}

bool tvm_load_program(tvm_t* vm, t3_instruction_t* program, size_t program_size) {
    if (vm == NULL || program == NULL || program_size == 0) {
        return false;
    }
    
    // Simple program loading - store instructions in memory
    // In a real implementation, this would be more sophisticated
    for (size_t i = 0; i < program_size && i < vm->memory_size; i++) {
        vm->memory[i] = trit_create(program[i].opcode);
    }
    
    return true;
}

trit_t tvm_run(tvm_t* vm) {
    if (vm == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    vm->running = true;
    vm->halted = false;
    vm->error = false;
    
    while (vm->running && !vm->halted && !vm->error) {
        // Get current instruction
        int pc = trit_to_int(vm->registers[T3_REGISTER_PC]);
        if (pc < 0 || pc >= (int)vm->memory_size) {
            vm->error = true;
            break;
        }
        
        // Create instruction from memory (simplified)
        t3_instruction_t instruction;
        instruction.opcode = trit_to_int(vm->memory[pc]);
        instruction.operand1 = 0;
        instruction.operand2 = 0;
        instruction.operand3 = 0;
        instruction.immediate = 0;
        instruction.valid = true;
        
        // Execute instruction
        trit_t result = tvm_execute_instruction(vm, &instruction);
        
        if (!trit_is_valid(result)) {
            vm->error = true;
            break;
        }
        
        if (instruction.opcode == T3_OPCODE_HALT) {
            vm->halted = true;
            break;
        }
        
        // Increment PC
        int new_pc = pc + 1;
        vm->registers[T3_REGISTER_PC] = trit_create(new_pc);
    }
    
    vm->running = false;
    
    if (vm->error) {
        return trit_create(TERNARY_UNKNOWN);
    } else if (vm->halted) {
        return trit_create(TERNARY_NEUTRAL);
    } else {
        return trit_create(TERNARY_POSITIVE);
    }
}

// Optimized instruction fetch with caching
t3_instruction_t* tvm_fetch_cached(tvm_t* vm, uint32_t address) {
    if (vm == NULL) return NULL;
    
    // Check cache
    uint32_t cache_idx = address & vm->icache_mask;
    icache_entry_t* entry = &vm->icache[cache_idx];
    
    if (entry->valid && entry->address == address) {
        vm->cache_hits++;
        return entry->instruction;
    }
    
    // Cache miss - decode instruction
    vm->cache_misses++;
    
    t3_instruction_t* instruction = malloc(sizeof(t3_instruction_t));
    if (instruction == NULL) return NULL;
    
    // Simplified instruction decoding
    instruction->opcode = trit_to_int(vm->memory[address]);
    instruction->operand1 = 0;
    instruction->operand2 = 0;
    instruction->operand3 = 0;
    instruction->immediate = 0;
    instruction->valid = true;
    
    // Update cache
    if (entry->valid && entry->instruction != NULL) {
        free(entry->instruction);
    }
    entry->address = address;
    entry->instruction = instruction;
    entry->valid = true;
    
    return instruction;
}

void tvm_halt(tvm_t* vm) {
    if (vm != NULL) {
        vm->running = false;
        vm->halted = true;
    }
}

void tvm_reset(tvm_t* vm) {
    if (vm == NULL) return;
    
    // Reset registers
    for (int i = 0; i < T3_REGISTER_COUNT; i++) {
        vm->registers[i] = trit_create(TERNARY_NEUTRAL);
    }
    
    // Reset memory
    for (size_t i = 0; i < vm->memory_size; i++) {
        vm->memory[i] = trit_create(TERNARY_NEUTRAL);
    }
    
    vm->running = false;
    vm->halted = false;
    vm->error = false;
}

// =============================================================================
// TVM MEMORY OPERATIONS
// =============================================================================

trit_t tvm_memory_read(tvm_t* vm, size_t address) {
    if (vm == NULL || address >= vm->memory_size) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return vm->memory[address];
}

void tvm_memory_write(tvm_t* vm, size_t address, trit_t value) {
    if (vm == NULL || address >= vm->memory_size) {
        return;
    }
    
    vm->memory[address] = value;
}

trit_t tvm_memory_read_register(tvm_t* vm, int register_index) {
    if (vm == NULL || register_index < 0 || register_index >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return vm->registers[register_index];
}

void tvm_memory_write_register(tvm_t* vm, int register_index, trit_t value) {
    if (vm == NULL || register_index < 0 || register_index >= T3_REGISTER_COUNT) {
        return;
    }
    
    vm->registers[register_index] = value;
}

// =============================================================================
// TVM STATUS OPERATIONS
// =============================================================================

bool tvm_is_running(tvm_t* vm) {
    return vm != NULL && vm->running;
}

bool tvm_is_halted(tvm_t* vm) {
    return vm != NULL && vm->halted;
}

bool tvm_has_error(tvm_t* vm) {
    return vm != NULL && vm->error;
}

trit_t tvm_get_register(tvm_t* vm, int register_index) {
    if (vm == NULL || register_index < 0 || register_index >= T3_REGISTER_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return vm->registers[register_index];
}

void tvm_set_register(tvm_t* vm, int register_index, trit_t value) {
    if (vm == NULL || register_index < 0 || register_index >= T3_REGISTER_COUNT) {
        return;
    }
    
    vm->registers[register_index] = value;
}

// =============================================================================
// TVM UTILITY FUNCTIONS
// =============================================================================

void tvm_print_status(tvm_t* vm) {
    if (vm == NULL) {
        printf("TVM Status: NULL\n");
        return;
    }
    
    printf("TVM Status: running=%s, halted=%s, error=%s\n",
           vm->running ? "true" : "false",
           vm->halted ? "true" : "false",
           vm->error ? "true" : "false");
    
    printf("Registers: ");
    for (int i = 0; i < T3_REGISTER_COUNT; i++) {
        printf("R%d=%s ", i, trit_to_string(vm->registers[i]));
    }
    printf("\n");
}

void tvm_print_memory(tvm_t* vm, size_t start, size_t count) {
    if (vm == NULL) return;
    
    printf("TVM Memory [%zu-%zu]: ", start, start + count - 1);
    for (size_t i = start; i < start + count && i < vm->memory_size; i++) {
        printf("%s ", trit_to_string(vm->memory[i]));
    }
    printf("\n");
}

void tvm_debug(tvm_t* vm) {
    if (vm == NULL) {
        printf("TVM Debug: NULL\n");
        return;
    }
    
    printf("TVM Debug:\n");
    printf("  Memory Size: %zu\n", vm->memory_size);
    printf("  Running: %s\n", vm->running ? "true" : "false");
    printf("  Halted: %s\n", vm->halted ? "true" : "false");
    printf("  Error: %s\n", vm->error ? "true" : "false");
    
    printf("  Registers:\n");
    for (int i = 0; i < T3_REGISTER_COUNT; i++) {
        printf("    R%d: %s\n", i, trit_to_string(vm->registers[i]));
    }
    
    printf("  Memory (first 16 words):\n");
    for (size_t i = 0; i < 16 && i < vm->memory_size; i++) {
        printf("    [%zu]: %s\n", i, trit_to_string(vm->memory[i]));
    }
}

// =============================================================================
// TVM STACK OPERATIONS
// =============================================================================

void tvm_stack_push(tvm_t* vm, trit_t value) {
    if (vm == NULL) return;
    
    int sp = trit_to_int(vm->registers[T3_REGISTER_SP]);
    if (sp > 0 && sp < (int)vm->memory_size) {
        vm->memory[sp] = value;
        vm->registers[T3_REGISTER_SP] = trit_create(sp - 1);
    }
}

trit_t tvm_stack_pop(tvm_t* vm) {
    if (vm == NULL) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    int sp = trit_to_int(vm->registers[T3_REGISTER_SP]);
    if (sp >= 0 && sp < (int)vm->memory_size - 1) {
        trit_t value = vm->memory[sp + 1];
        vm->registers[T3_REGISTER_SP] = trit_create(sp + 1);
        return value;
    }
    
    return trit_create(TERNARY_UNKNOWN);
}

// =============================================================================
// TVM INTERRUPT HANDLING
// =============================================================================

void tvm_handle_interrupt(tvm_t* vm, int interrupt_type) {
    if (vm == NULL) return;
    
    // Simple interrupt handling
    switch (interrupt_type) {
        case TVM_INTERRUPT_TIMER:
            // Timer interrupt
            break;
        case TVM_INTERRUPT_IO:
            // I/O interrupt
            break;
        case TVM_INTERRUPT_MEMORY:
            // Memory interrupt
            vm->error = true;
            break;
        case TVM_INTERRUPT_SYSTEM:
            // System interrupt
            vm->halted = true;
            break;
        default:
            // Unknown interrupt
            vm->error = true;
            break;
    }
}

void tvm_get_performance_stats(tvm_t* vm, uint64_t* instructions, 
                               uint64_t* cache_hits, uint64_t* cache_misses,
                               uint64_t* bp_predictions, uint64_t* bp_mispredictions) {
    if (vm == NULL) return;
    
    if (instructions) *instructions = vm->instructions_executed;
    if (cache_hits) *cache_hits = vm->cache_hits;
    if (cache_misses) *cache_misses = vm->cache_misses;
    if (bp_predictions) *bp_predictions = vm->branch_predictions;
    if (bp_mispredictions) *bp_mispredictions = vm->branch_mispredictions;
}

void tvm_reset_performance_stats(tvm_t* vm) {
    if (vm == NULL) return;
    
    vm->instructions_executed = 0;
    vm->cache_hits = 0;
    vm->cache_misses = 0;
    vm->branch_predictions = 0;
    vm->branch_mispredictions = 0;
}
