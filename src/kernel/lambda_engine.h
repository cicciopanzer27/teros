/**
 * Lambda Engine - Native Ternary Lambda Calculus Runtime
 * 
 * Part of TEROS - Ternary Operating System
 * Lambda³ Integration Layer
 * 
 * This engine provides native ternary lambda calculus operations
 * running directly on the Ternary Virtual Machine (TVM).
 * 
 * Author: Lambda³ Project Team
 * Date: October 2025
 */

#ifndef LAMBDA_ENGINE_H
#define LAMBDA_ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include "../hal/cpu_emulator.h"
#include "../memory/ternary_memory.h"

// ============================================================================
// CORE DATA STRUCTURES
// ============================================================================

/**
 * Lambda Term Types (mapped to trits)
 * -1 (T-) = Variable
 *  0 (T0) = Abstraction
 * +1 (T+) = Application
 */
typedef enum {
    LAMBDA_VAR = -1,  // Variable
    LAMBDA_ABS =  0,  // Abstraction (λx.M)
    LAMBDA_APP =  1   // Application (M N)
} LambdaTermType;

/**
 * Lambda Term Structure
 * 
 * Memory layout (ternary-optimized):
 * [tag_trit | data1 | data2]
 * 
 * For VAR:  [T- | var_id   | unused  ]
 * For ABS:  [T0 | var_id   | body_ptr]
 * For APP:  [T+ | func_ptr | arg_ptr ]
 */
typedef struct LambdaTerm {
    LambdaTermType type;        // Tag: -1/0/1 (single trit!)
    
    union {
        // Variable: just an ID
        struct {
            int32_t var_id;
        } var;
        
        // Abstraction: λvar.body
        struct {
            int32_t var_id;
            struct LambdaTerm* body;
        } abs;
        
        // Application: func arg
        struct {
            struct LambdaTerm* func;
            struct LambdaTerm* arg;
        } app;
    } data;
    
    // Reference counting for GC
    int32_t ref_count;
    
    // Hash for memoization
    uint64_t hash;
} LambdaTerm;

/**
 * Lambda Environment (for closures)
 * Maps variable IDs to values
 */
typedef struct LambdaEnv {
    int32_t var_id;
    LambdaTerm* value;
    struct LambdaEnv* parent;  // Chain to parent scope
} LambdaEnv;

/**
 * Reduction Context
 * Tracks reduction state for debugging/profiling
 */
typedef struct {
    int32_t reduction_count;    // Number of β-reductions
    int32_t max_depth;          // Maximum recursion depth
    int32_t current_depth;      // Current depth
    bool timeout;               // Timeout flag
    int32_t max_steps;          // Maximum steps before timeout
} ReductionContext;

// ============================================================================
// CORE API - CREATION
// ============================================================================

/**
 * Create a lambda variable
 * @param var_id Variable identifier
 * @return New lambda term (variable)
 */
LambdaTerm* lambda_create_var(int32_t var_id);

/**
 * Create a lambda abstraction (λx.M)
 * @param var_id Variable to bind
 * @param body Body of the abstraction
 * @return New lambda term (abstraction)
 */
LambdaTerm* lambda_create_abs(int32_t var_id, LambdaTerm* body);

/**
 * Create a lambda application (M N)
 * @param func Function term
 * @param arg Argument term
 * @return New lambda term (application)
 */
LambdaTerm* lambda_create_app(LambdaTerm* func, LambdaTerm* arg);

// ============================================================================
// CORE API - REDUCTION
// ============================================================================

/**
 * Perform β-reduction on a lambda term
 * (λx.M) N → M[x := N]
 * 
 * @param term Term to reduce
 * @param ctx Reduction context (for limits/profiling)
 * @return Reduced term (may be same if already in normal form)
 */
LambdaTerm* lambda_reduce(LambdaTerm* term, ReductionContext* ctx);

/**
 * Reduce term to normal form (fully reduce)
 * @param term Term to reduce
 * @param ctx Reduction context
 * @return Normal form (or timeout)
 */
LambdaTerm* lambda_reduce_to_normal_form(LambdaTerm* term, ReductionContext* ctx);

/**
 * Single-step β-reduction (for debugging)
 * @param term Term to reduce
 * @param ctx Reduction context
 * @return Term after one reduction step
 */
LambdaTerm* lambda_reduce_step(LambdaTerm* term, ReductionContext* ctx);

/**
 * Substitution: M[x := N]
 * Replace all free occurrences of x with N in M
 * 
 * @param term Term M
 * @param var_id Variable x
 * @param replacement Term N
 * @return M[x := N]
 */
LambdaTerm* lambda_substitute(LambdaTerm* term, int32_t var_id, LambdaTerm* replacement);

// ============================================================================
// MEMORY MANAGEMENT
// ============================================================================

/**
 * Increment reference count
 * @param term Term to retain
 */
void lambda_retain(LambdaTerm* term);

/**
 * Decrement reference count (and free if 0)
 * @param term Term to release
 */
void lambda_release(LambdaTerm* term);

/**
 * Clone a lambda term (deep copy)
 * @param term Term to clone
 * @return New term (independent copy)
 */
LambdaTerm* lambda_clone(LambdaTerm* term);

// ============================================================================
// UTILITIES
// ============================================================================

/**
 * Check if term is in normal form
 * @param term Term to check
 * @return true if normal form, false otherwise
 */
bool lambda_is_normal_form(LambdaTerm* term);

/**
 * Print lambda term (for debugging)
 * @param term Term to print
 */
void lambda_print(LambdaTerm* term);

/**
 * Compute hash of lambda term (for memoization)
 * @param term Term to hash
 * @return Hash value
 */
uint64_t lambda_hash(LambdaTerm* term);

/**
 * Check α-equivalence (structural equality)
 * @param t1 First term
 * @param t2 Second term
 * @return true if α-equivalent
 */
bool lambda_alpha_equiv(LambdaTerm* t1, LambdaTerm* t2);

// ============================================================================
// T3 INTEGRATION
// ============================================================================

/**
 * Encode lambda term as T3 bytecode
 * @param term Lambda term
 * @param bytecode Output buffer
 * @param size Size of bytecode
 * @return Number of T3 instructions generated
 */
int32_t lambda_encode_t3(LambdaTerm* term, uint8_t* bytecode, int32_t size);

/**
 * Execute lambda term on TVM
 * @param term Lambda term
 * @param tvm TVM instance
 * @return Result of execution
 */
LambdaTerm* lambda_execute_on_tvm(LambdaTerm* term, void* tvm);

// ============================================================================
// OPTIMIZATION FLAGS
// ============================================================================

// Enable graph reduction (sharing)
extern bool LAMBDA_USE_GRAPH_REDUCTION;

// Enable memoization cache
extern bool LAMBDA_USE_MEMOIZATION;

// Maximum reduction steps
extern int32_t LAMBDA_MAX_REDUCTION_STEPS;

#endif // LAMBDA_ENGINE_H

