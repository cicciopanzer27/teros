/**
 * @file tvm.h
 * @brief Ternary Virtual Machine (TVM) header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TVM_H
#define TVM_H

#include "trit.h"
#include "t3_isa.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TVM CONSTANTS
// =============================================================================

#define TVM_DEFAULT_MEMORY_SIZE 1024
#define TVM_MAX_MEMORY_SIZE 65536

// Interrupt types
#define TVM_INTERRUPT_TIMER   0
#define TVM_INTERRUPT_IO      1
#define TVM_INTERRUPT_MEMORY  2
#define TVM_INTERRUPT_SYSTEM  3

// =============================================================================
// TVM STRUCTURES
// =============================================================================

// Instruction cache entry
typedef struct {
    uint32_t address;
    t3_instruction_t* instruction;
    bool valid;
} icache_entry_t;

// Branch predictor state (2-bit saturating counter)
typedef enum {
    BP_STRONGLY_NOT_TAKEN = 0,
    BP_WEAKLY_NOT_TAKEN = 1,
    BP_WEAKLY_TAKEN = 2,
    BP_STRONGLY_TAKEN = 3
} branch_predictor_state_t;

typedef struct {
    uint32_t address;
    branch_predictor_state_t state;
} branch_predictor_t;

typedef struct {
    trit_t* memory;
    trit_t registers[T3_REGISTER_COUNT];
    size_t memory_size;
    bool running;
    bool halted;
    bool error;
    
    // Performance optimizations
    icache_entry_t* icache;
    size_t icache_size;
    uint32_t icache_mask;
    uint64_t instructions_executed;
    uint64_t cache_hits;
    uint64_t cache_misses;
    
    // Branch predictor
    branch_predictor_t* bp_table;
    size_t bp_table_size;
    uint64_t branch_predictions;
    uint64_t branch_mispredictions;
} tvm_t;

// =============================================================================
// TVM CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new TVM instance
 * @param memory_size Size of the virtual memory
 * @return A new TVM instance, or NULL on failure
 */
tvm_t* tvm_create(size_t memory_size);

/**
 * @brief Destroy a TVM instance
 * @param vm The TVM instance to destroy
 */
void tvm_destroy(tvm_t* vm);

// =============================================================================
// TVM EXECUTION
// =============================================================================

/**
 * @brief Execute a single instruction
 * @param vm The TVM instance
 * @param instruction The instruction to execute
 * @return Result of execution
 */
trit_t tvm_execute_instruction(tvm_t* vm, t3_instruction_t* instruction);

/**
 * @brief Load a program into the TVM
 * @param vm The TVM instance
 * @param program Array of instructions
 * @param program_size Number of instructions
 * @return true on success, false on failure
 */
bool tvm_load_program(tvm_t* vm, t3_instruction_t* program, size_t program_size);

/**
 * @brief Run the TVM until halt or error
 * @param vm The TVM instance
 * @return Result of execution
 */
trit_t tvm_run(tvm_t* vm);

/**
 * @brief Halt the TVM
 * @param vm The TVM instance
 */
void tvm_halt(tvm_t* vm);

/**
 * @brief Reset the TVM to initial state
 * @param vm The TVM instance
 */
void tvm_reset(tvm_t* vm);

// =============================================================================
// TVM MEMORY OPERATIONS
// =============================================================================

/**
 * @brief Read a trit from memory
 * @param vm The TVM instance
 * @param address Memory address
 * @return The trit at the address
 */
trit_t tvm_memory_read(tvm_t* vm, size_t address);

/**
 * @brief Write a trit to memory
 * @param vm The TVM instance
 * @param address Memory address
 * @param value The trit to write
 */
void tvm_memory_write(tvm_t* vm, size_t address, trit_t value);

/**
 * @brief Read a register value
 * @param vm The TVM instance
 * @param register_index Register index
 * @return The register value
 */
trit_t tvm_memory_read_register(tvm_t* vm, int register_index);

/**
 * @brief Write a register value
 * @param vm The TVM instance
 * @param register_index Register index
 * @param value The value to write
 */
void tvm_memory_write_register(tvm_t* vm, int register_index, trit_t value);

// =============================================================================
// TVM STATUS OPERATIONS
// =============================================================================

/**
 * @brief Check if TVM is running
 * @param vm The TVM instance
 * @return true if running, false otherwise
 */
bool tvm_is_running(tvm_t* vm);

/**
 * @brief Check if TVM is halted
 * @param vm The TVM instance
 * @return true if halted, false otherwise
 */
bool tvm_is_halted(tvm_t* vm);

/**
 * @brief Check if TVM has an error
 * @param vm The TVM instance
 * @return true if error, false otherwise
 */
bool tvm_has_error(tvm_t* vm);

/**
 * @brief Get a register value
 * @param vm The TVM instance
 * @param register_index Register index
 * @return The register value
 */
trit_t tvm_get_register(tvm_t* vm, int register_index);

/**
 * @brief Set a register value
 * @param vm The TVM instance
 * @param register_index Register index
 * @param value The value to set
 */
void tvm_set_register(tvm_t* vm, int register_index, trit_t value);

// =============================================================================
// TVM UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print TVM status
 * @param vm The TVM instance
 */
void tvm_print_status(tvm_t* vm);

/**
 * @brief Print TVM memory
 * @param vm The TVM instance
 * @param start Starting address
 * @param count Number of words to print
 */
void tvm_print_memory(tvm_t* vm, size_t start, size_t count);

/**
 * @brief Debug print TVM information
 * @param vm The TVM instance
 */
void tvm_debug(tvm_t* vm);

// =============================================================================
// TVM STACK OPERATIONS
// =============================================================================

/**
 * @brief Push a value onto the stack
 * @param vm The TVM instance
 * @param value The value to push
 */
void tvm_stack_push(tvm_t* vm, trit_t value);

/**
 * @brief Pop a value from the stack
 * @param vm The TVM instance
 * @return The popped value
 */
trit_t tvm_stack_pop(tvm_t* vm);

// =============================================================================
// TVM INTERRUPT HANDLING
// =============================================================================

/**
 * @brief Handle an interrupt
 * @param vm The TVM instance
 * @param interrupt_type Type of interrupt
 */
void tvm_handle_interrupt(tvm_t* vm, int interrupt_type);

// =============================================================================
// TVM PERFORMANCE FUNCTIONS
// =============================================================================

/**
 * @brief Fetch instruction with caching
 * @param vm The TVM instance
 * @param address Instruction address
 * @return Pointer to cached instruction
 */
t3_instruction_t* tvm_fetch_cached(tvm_t* vm, uint32_t address);

/**
 * @brief Get performance statistics
 * @param vm The TVM instance
 * @param instructions Pointer to store instruction count
 * @param cache_hits Pointer to store cache hit count
 * @param cache_misses Pointer to store cache miss count
 * @param bp_predictions Pointer to store branch predictions
 * @param bp_mispredictions Pointer to store mispredictions
 */
void tvm_get_performance_stats(tvm_t* vm, uint64_t* instructions, 
                               uint64_t* cache_hits, uint64_t* cache_misses,
                               uint64_t* bp_predictions, uint64_t* bp_mispredictions);

/**
 * @brief Clear performance statistics
 * @param vm The TVM instance
 */
void tvm_reset_performance_stats(tvm_t* vm);

#endif // TVM_H
