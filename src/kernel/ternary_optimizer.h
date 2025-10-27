/**
 * @file ternary_optimizer.h
 * @brief Ternary code optimizer header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_OPTIMIZER_H
#define TERNARY_OPTIMIZER_H

#include "trit.h"
#include "t3_isa.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY OPTIMIZER CONSTANTS
// =============================================================================

typedef enum {
    TERNARY_OPTIMIZATION_LEVEL_NONE = 0,
    TERNARY_OPTIMIZATION_LEVEL_BASIC = 1,
    TERNARY_OPTIMIZATION_LEVEL_ADVANCED = 2,
    TERNARY_OPTIMIZATION_LEVEL_AGGRESSIVE = 3
} ternary_optimization_level_t;

typedef enum {
    TERNARY_OPTIMIZATION_CONSTANT_FOLDING = 0x01,
    TERNARY_OPTIMIZATION_DEAD_CODE_ELIMINATION = 0x02,
    TERNARY_OPTIMIZATION_LOOP_UNROLLING = 0x04,
    TERNARY_OPTIMIZATION_INSTRUCTION_SCHEDULING = 0x08,
    TERNARY_OPTIMIZATION_REGISTER_ALLOCATION = 0x10,
    TERNARY_OPTIMIZATION_PEEPHOLE = 0x20
} ternary_optimization_t;

// =============================================================================
// TERNARY OPTIMIZER STRUCTURE
// =============================================================================

typedef struct {
    uint32_t optimizations_enabled;
    ternary_optimization_level_t optimization_level;
    bool error;
    char* error_message;
} ternary_optimizer_t;

// =============================================================================
// TERNARY OPTIMIZER CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary optimizer
 * @return A new ternary optimizer, or NULL on failure
 */
ternary_optimizer_t* ternary_optimizer_create(void);

/**
 * @brief Destroy a ternary optimizer
 * @param optimizer The optimizer to destroy
 */
void ternary_optimizer_destroy(ternary_optimizer_t* optimizer);

// =============================================================================
// TERNARY OPTIMIZER CONFIGURATION
// =============================================================================

/**
 * @brief Set optimization level
 * @param optimizer The optimizer instance
 * @param level The optimization level
 */
void ternary_optimizer_set_optimization_level(ternary_optimizer_t* optimizer, ternary_optimization_level_t level);

/**
 * @brief Enable a specific optimization
 * @param optimizer The optimizer instance
 * @param optimization The optimization to enable
 */
void ternary_optimizer_enable_optimization(ternary_optimizer_t* optimizer, ternary_optimization_t optimization);

/**
 * @brief Disable a specific optimization
 * @param optimizer The optimizer instance
 * @param optimization The optimization to disable
 */
void ternary_optimizer_disable_optimization(ternary_optimizer_t* optimizer, ternary_optimization_t optimization);

/**
 * @brief Check if an optimization is enabled
 * @param optimizer The optimizer instance
 * @param optimization The optimization to check
 * @return true if enabled, false otherwise
 */
bool ternary_optimizer_is_optimization_enabled(ternary_optimizer_t* optimizer, ternary_optimization_t optimization);

// =============================================================================
// TERNARY OPTIMIZER OPTIMIZATION
// =============================================================================

/**
 * @brief Optimize a sequence of instructions
 * @param optimizer The optimizer instance
 * @param instructions The instructions to optimize
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_optimizer_optimize(ternary_optimizer_t* optimizer, t3_instruction_t* instructions, size_t instruction_count);

// =============================================================================
// TERNARY OPTIMIZER CONSTANT FOLDING
// =============================================================================

/**
 * @brief Apply constant folding optimization
 * @param optimizer The optimizer instance
 * @param instructions The instructions to optimize
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_optimizer_constant_folding(ternary_optimizer_t* optimizer, t3_instruction_t* instructions, size_t instruction_count);

// =============================================================================
// TERNARY OPTIMIZER DEAD CODE ELIMINATION
// =============================================================================

/**
 * @brief Apply dead code elimination optimization
 * @param optimizer The optimizer instance
 * @param instructions The instructions to optimize
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_optimizer_dead_code_elimination(ternary_optimizer_t* optimizer, t3_instruction_t* instructions, size_t instruction_count);

// =============================================================================
// TERNARY OPTIMIZER LOOP UNROLLING
// =============================================================================

/**
 * @brief Apply loop unrolling optimization
 * @param optimizer The optimizer instance
 * @param instructions The instructions to optimize
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_optimizer_loop_unrolling(ternary_optimizer_t* optimizer, t3_instruction_t* instructions, size_t instruction_count);

// =============================================================================
// TERNARY OPTIMIZER INSTRUCTION SCHEDULING
// =============================================================================

/**
 * @brief Apply instruction scheduling optimization
 * @param optimizer The optimizer instance
 * @param instructions The instructions to optimize
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_optimizer_instruction_scheduling(ternary_optimizer_t* optimizer, t3_instruction_t* instructions, size_t instruction_count);

// =============================================================================
// TERNARY OPTIMIZER REGISTER ALLOCATION
// =============================================================================

/**
 * @brief Apply register allocation optimization
 * @param optimizer The optimizer instance
 * @param instructions The instructions to optimize
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_optimizer_register_allocation(ternary_optimizer_t* optimizer, t3_instruction_t* instructions, size_t instruction_count);

// =============================================================================
// TERNARY OPTIMIZER PEEPHOLE
// =============================================================================

/**
 * @brief Apply peephole optimization
 * @param optimizer The optimizer instance
 * @param instructions The instructions to optimize
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_optimizer_peephole(ternary_optimizer_t* optimizer, t3_instruction_t* instructions, size_t instruction_count);

// =============================================================================
// TERNARY OPTIMIZER ERROR HANDLING
// =============================================================================

/**
 * @brief Check if optimizer has error
 * @param optimizer The optimizer instance
 * @return true if error, false otherwise
 */
bool ternary_optimizer_has_error(ternary_optimizer_t* optimizer);

/**
 * @brief Get optimizer error message
 * @param optimizer The optimizer instance
 * @return The error message, or NULL if no error
 */
const char* ternary_optimizer_get_error_message(ternary_optimizer_t* optimizer);

/**
 * @brief Set optimizer error
 * @param optimizer The optimizer instance
 * @param message The error message
 */
void ternary_optimizer_set_error(ternary_optimizer_t* optimizer, const char* message);

// =============================================================================
// TERNARY OPTIMIZER UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print optimization information
 * @param optimizer The optimizer instance
 */
void ternary_optimizer_print_optimizations(ternary_optimizer_t* optimizer);

/**
 * @brief Debug print optimizer information
 * @param optimizer The optimizer instance
 */
void ternary_optimizer_debug(ternary_optimizer_t* optimizer);

#endif // TERNARY_OPTIMIZER_H
