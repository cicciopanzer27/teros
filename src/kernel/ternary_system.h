/**
 * @file ternary_system.h
 * @brief Ternary system integration header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_SYSTEM_H
#define TERNARY_SYSTEM_H

#include "trit.h"
#include "trit_array.h"
#include "t3_isa.h"
#include "tvm.h"
#include "ternary_memory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY SYSTEM CONSTANTS
// =============================================================================

#define TERNARY_SYSTEM_VERSION_MAJOR 1
#define TERNARY_SYSTEM_VERSION_MINOR 0
#define TERNARY_SYSTEM_VERSION_PATCH 0

#define TERNARY_SYSTEM_MAX_PROCESSES 256
#define TERNARY_SYSTEM_MAX_MEMORY_SIZE 1048576  // 1MB
#define TERNARY_SYSTEM_DEFAULT_MEMORY_SIZE 65536  // 64KB

// =============================================================================
// TERNARY SYSTEM STRUCTURES
// =============================================================================

typedef struct {
    tvm_t* vm;
    ternary_memory_t* memory;
    t3_register_t registers[T3_REGISTER_COUNT];
    bool initialized;
    bool running;
    bool error;
} ternary_system_t;

// =============================================================================
// TERNARY SYSTEM CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary system
 * @param memory_size Size of the system memory
 * @return A new ternary system, or NULL on failure
 */
ternary_system_t* ternary_system_create(size_t memory_size);

/**
 * @brief Destroy a ternary system
 * @param system The system to destroy
 */
void ternary_system_destroy(ternary_system_t* system);

// =============================================================================
// TERNARY SYSTEM INITIALIZATION
// =============================================================================

/**
 * @brief Initialize the ternary system
 * @param system The system to initialize
 * @return true on success, false on failure
 */
bool ternary_system_init(ternary_system_t* system);

/**
 * @brief Shutdown the ternary system
 * @param system The system to shutdown
 */
void ternary_system_shutdown(ternary_system_t* system);

// =============================================================================
// TERNARY SYSTEM EXECUTION
// =============================================================================

/**
 * @brief Run the ternary system
 * @param system The system to run
 * @return Result of execution
 */
trit_t ternary_system_run(ternary_system_t* system);

/**
 * @brief Halt the ternary system
 * @param system The system to halt
 */
void ternary_system_halt(ternary_system_t* system);

/**
 * @brief Reset the ternary system
 * @param system The system to reset
 */
void ternary_system_reset(ternary_system_t* system);

// =============================================================================
// TERNARY SYSTEM STATUS
// =============================================================================

/**
 * @brief Check if system is initialized
 * @param system The system to check
 * @return true if initialized, false otherwise
 */
bool ternary_system_is_initialized(ternary_system_t* system);

/**
 * @brief Check if system is running
 * @param system The system to check
 * @return true if running, false otherwise
 */
bool ternary_system_is_running(ternary_system_t* system);

/**
 * @brief Check if system has error
 * @param system The system to check
 * @return true if error, false otherwise
 */
bool ternary_system_has_error(ternary_system_t* system);

// =============================================================================
// TERNARY SYSTEM MEMORY OPERATIONS
// =============================================================================

/**
 * @brief Get system memory
 * @param system The system
 * @return The system memory
 */
ternary_memory_t* ternary_system_get_memory(ternary_system_t* system);

/**
 * @brief Get system VM
 * @param system The system
 * @return The system VM
 */
tvm_t* ternary_system_get_vm(ternary_system_t* system);

// =============================================================================
// TERNARY SYSTEM UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print system status
 * @param system The system to print
 */
void ternary_system_print_status(ternary_system_t* system);

/**
 * @brief Debug print system information
 * @param system The system to debug
 */
void ternary_system_debug(ternary_system_t* system);

/**
 * @brief Get system version
 * @return System version string
 */
const char* ternary_system_get_version(void);

#endif // TERNARY_SYSTEM_H
