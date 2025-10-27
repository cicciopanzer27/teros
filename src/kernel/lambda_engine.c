/**
 * Lambda Engine - Implementation
 * Native Ternary Lambda Calculus Runtime for TEROS
 */

#include "lambda_engine.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// GLOBAL CONFIGURATION
// ============================================================================

bool LAMBDA_USE_GRAPH_REDUCTION = true;
bool LAMBDA_USE_MEMOIZATION = true;
int32_t LAMBDA_MAX_REDUCTION_STEPS = 10000;

// ============================================================================
// MEMORY POOL (for fast allocation)
// ============================================================================

#define LAMBDA_POOL_SIZE 1024
static LambdaTerm lambda_pool[LAMBDA_POOL_SIZE];
static bool pool_used[LAMBDA_POOL_SIZE] = {false};
static int32_t pool_next = 0;

/**
 * Allocate lambda term from pool (fast path)
 */
static LambdaTerm* lambda_alloc() {
    // Try pool first
    for (int i = 0; i < LAMBDA_POOL_SIZE; i++) {
        int idx = (pool_next + i) % LAMBDA_POOL_SIZE;
        if (!pool_used[idx]) {
            pool_used[idx] = true;
            pool_next = (idx + 1) % LAMBDA_POOL_SIZE;
            memset(&lambda_pool[idx], 0, sizeof(LambdaTerm));
            lambda_pool[idx].ref_count = 1;
            return &lambda_pool[idx];
        }
    }
    
    // Pool exhausted, allocate on heap
    LambdaTerm* term = (LambdaTerm*)malloc(sizeof(LambdaTerm));
    memset(term, 0, sizeof(LambdaTerm));
    term->ref_count = 1;
    return term;
}

/**
 * Free lambda term back to pool
 */
static void lambda_free(LambdaTerm* term) {
    // Check if from pool
    if (term >= lambda_pool && term < lambda_pool + LAMBDA_POOL_SIZE) {
        int idx = term - lambda_pool;
        pool_used[idx] = false;
    } else {
        free(term);
    }
}

// ============================================================================
// CREATION API
// ============================================================================

LambdaTerm* lambda_create_var(int32_t var_id) {
    LambdaTerm* term = lambda_alloc();
    term->type = LAMBDA_VAR;
    term->data.var.var_id = var_id;
    term->hash = lambda_hash(term);
    return term;
}

LambdaTerm* lambda_create_abs(int32_t var_id, LambdaTerm* body) {
    LambdaTerm* term = lambda_alloc();
    term->type = LAMBDA_ABS;
    term->data.abs.var_id = var_id;
    term->data.abs.body = body;
    lambda_retain(body);  // Increment body's ref count
    term->hash = lambda_hash(term);
    return term;
}

LambdaTerm* lambda_create_app(LambdaTerm* func, LambdaTerm* arg) {
    LambdaTerm* term = lambda_alloc();
    term->type = LAMBDA_APP;
    term->data.app.func = func;
    term->data.app.arg = arg;
    lambda_retain(func);
    lambda_retain(arg);
    term->hash = lambda_hash(term);
    return term;
}

// ============================================================================
// MEMORY MANAGEMENT
// ============================================================================

void lambda_retain(LambdaTerm* term) {
    if (term) {
        term->ref_count++;
    }
}

void lambda_release(LambdaTerm* term) {
    if (!term) return;
    
    term->ref_count--;
    if (term->ref_count <= 0) {
        // Release children
        switch (term->type) {
            case LAMBDA_ABS:
                lambda_release(term->data.abs.body);
                break;
            case LAMBDA_APP:
                lambda_release(term->data.app.func);
                lambda_release(term->data.app.arg);
                break;
            case LAMBDA_VAR:
                // No children
                break;
        }
        lambda_free(term);
    }
}

LambdaTerm* lambda_clone(LambdaTerm* term) {
    if (!term) return NULL;
    
    switch (term->type) {
        case LAMBDA_VAR:
            return lambda_create_var(term->data.var.var_id);
        
        case LAMBDA_ABS: {
            LambdaTerm* body_clone = lambda_clone(term->data.abs.body);
            LambdaTerm* result = lambda_create_abs(term->data.abs.var_id, body_clone);
            lambda_release(body_clone);  // create_abs retained it
            return result;
        }
        
        case LAMBDA_APP: {
            LambdaTerm* func_clone = lambda_clone(term->data.app.func);
            LambdaTerm* arg_clone = lambda_clone(term->data.app.arg);
            LambdaTerm* result = lambda_create_app(func_clone, arg_clone);
            lambda_release(func_clone);  // create_app retained them
            lambda_release(arg_clone);
            return result;
        }
    }
    
    return NULL;
}

// ============================================================================
// SUBSTITUTION (Capture-avoiding)
// ============================================================================

LambdaTerm* lambda_substitute(LambdaTerm* term, int32_t var_id, LambdaTerm* replacement) {
    if (!term) return NULL;
    
    switch (term->type) {
        case LAMBDA_VAR:
            if (term->data.var.var_id == var_id) {
                // Replace this variable
                return lambda_clone(replacement);
            } else {
                // Different variable, keep as is
                return lambda_clone(term);
            }
        
        case LAMBDA_ABS:
            if (term->data.abs.var_id == var_id) {
                // Variable shadowed, no substitution in body
                return lambda_clone(term);
            } else {
                // Substitute in body
                LambdaTerm* new_body = lambda_substitute(term->data.abs.body, var_id, replacement);
                LambdaTerm* result = lambda_create_abs(term->data.abs.var_id, new_body);
                lambda_release(new_body);
                return result;
            }
        
        case LAMBDA_APP: {
            // Substitute in both func and arg
            LambdaTerm* new_func = lambda_substitute(term->data.app.func, var_id, replacement);
            LambdaTerm* new_arg = lambda_substitute(term->data.app.arg, var_id, replacement);
            LambdaTerm* result = lambda_create_app(new_func, new_arg);
            lambda_release(new_func);
            lambda_release(new_arg);
            return result;
        }
    }
    
    return NULL;
}

// ============================================================================
// REDUCTION
// ============================================================================

LambdaTerm* lambda_reduce_step(LambdaTerm* term, ReductionContext* ctx) {
    if (!term || !ctx) return term;
    
    // Check timeout
    if (ctx->reduction_count >= ctx->max_steps) {
        ctx->timeout = true;
        return term;
    }
    
    ctx->reduction_count++;
    
    switch (term->type) {
        case LAMBDA_VAR:
            // Variables are already in normal form
            return lambda_clone(term);
        
        case LAMBDA_ABS:
            // Reduce body (under abstraction)
            {
                LambdaTerm* reduced_body = lambda_reduce_step(term->data.abs.body, ctx);
                LambdaTerm* result = lambda_create_abs(term->data.abs.var_id, reduced_body);
                lambda_release(reduced_body);
                return result;
            }
        
        case LAMBDA_APP:
            // Check if this is a β-redex: (λx.M) N
            if (term->data.app.func->type == LAMBDA_ABS) {
                // β-reduction: (λx.M) N → M[x := N]
                LambdaTerm* body = term->data.app.func->data.abs.body;
                int32_t var_id = term->data.app.func->data.abs.var_id;
                LambdaTerm* arg = term->data.app.arg;
                
                // Perform substitution
                LambdaTerm* result = lambda_substitute(body, var_id, arg);
                return result;
            } else {
                // Not a redex, reduce func first
                LambdaTerm* reduced_func = lambda_reduce_step(term->data.app.func, ctx);
                if (reduced_func->type == LAMBDA_ABS) {
                    // Now it's a redex!
                    LambdaTerm* temp = lambda_create_app(reduced_func, term->data.app.arg);
                    LambdaTerm* result = lambda_reduce_step(temp, ctx);
                    lambda_release(temp);
                    lambda_release(reduced_func);
                    return result;
                } else {
                    // Still not a redex, reduce arg too
                    LambdaTerm* reduced_arg = lambda_reduce_step(term->data.app.arg, ctx);
                    LambdaTerm* result = lambda_create_app(reduced_func, reduced_arg);
                    lambda_release(reduced_func);
                    lambda_release(reduced_arg);
                    return result;
                }
            }
    }
    
    return lambda_clone(term);
}

LambdaTerm* lambda_reduce(LambdaTerm* term, ReductionContext* ctx) {
    return lambda_reduce_step(term, ctx);
}

LambdaTerm* lambda_reduce_to_normal_form(LambdaTerm* term, ReductionContext* ctx) {
    LambdaTerm* current = lambda_clone(term);
    LambdaTerm* previous = NULL;
    
    while (!ctx->timeout) {
        LambdaTerm* next = lambda_reduce_step(current, ctx);
        
        // Check if changed
        if (lambda_alpha_equiv(current, next)) {
            // No change, we're done
            lambda_release(next);
            break;
        }
        
        // Continue reduction
        if (previous) lambda_release(previous);
        previous = current;
        current = next;
    }
    
    if (previous) lambda_release(previous);
    return current;
}

// ============================================================================
// UTILITIES
// ============================================================================

bool lambda_is_normal_form(LambdaTerm* term) {
    if (!term) return true;
    
    switch (term->type) {
        case LAMBDA_VAR:
            return true;
        
        case LAMBDA_ABS:
            return lambda_is_normal_form(term->data.abs.body);
        
        case LAMBDA_APP:
            // Not normal form if func is abstraction (redex)
            if (term->data.app.func->type == LAMBDA_ABS) {
                return false;
            }
            return lambda_is_normal_form(term->data.app.func) && 
                   lambda_is_normal_form(term->data.app.arg);
    }
    
    return true;
}

void lambda_print(LambdaTerm* term) {
    if (!term) {
        printf("null");
        return;
    }
    
    switch (term->type) {
        case LAMBDA_VAR:
            printf("x%d", term->data.var.var_id);
            break;
        
        case LAMBDA_ABS:
            printf("(λx%d.", term->data.abs.var_id);
            lambda_print(term->data.abs.body);
            printf(")");
            break;
        
        case LAMBDA_APP:
            printf("(");
            lambda_print(term->data.app.func);
            printf(" ");
            lambda_print(term->data.app.arg);
            printf(")");
            break;
    }
}

uint64_t lambda_hash(LambdaTerm* term) {
    if (!term) return 0;
    
    uint64_t hash = 0;
    switch (term->type) {
        case LAMBDA_VAR:
            hash = 1 + term->data.var.var_id;
            break;
        
        case LAMBDA_ABS:
            hash = 2 + term->data.abs.var_id * 31 + lambda_hash(term->data.abs.body);
            break;
        
        case LAMBDA_APP:
            hash = 3 + lambda_hash(term->data.app.func) * 31 + lambda_hash(term->data.app.arg);
            break;
    }
    
    return hash;
}

bool lambda_alpha_equiv(LambdaTerm* t1, LambdaTerm* t2) {
    if (!t1 && !t2) return true;
    if (!t1 || !t2) return false;
    if (t1->type != t2->type) return false;
    
    switch (t1->type) {
        case LAMBDA_VAR:
            return t1->data.var.var_id == t2->data.var.var_id;
        
        case LAMBDA_ABS:
            return t1->data.abs.var_id == t2->data.abs.var_id &&
                   lambda_alpha_equiv(t1->data.abs.body, t2->data.abs.body);
        
        case LAMBDA_APP:
            return lambda_alpha_equiv(t1->data.app.func, t2->data.app.func) &&
                   lambda_alpha_equiv(t1->data.app.arg, t2->data.app.arg);
    }
    
    return false;
}

// ============================================================================
// T3 INTEGRATION (Stub for now)
// ============================================================================

int32_t lambda_encode_t3(LambdaTerm* term, uint8_t* bytecode, int32_t size) {
    // TODO: Implement T3 bytecode generation
    // This will map lambda operations to T3-ISA opcodes
    return 0;
}

LambdaTerm* lambda_execute_on_tvm(LambdaTerm* term, void* tvm) {
    // TODO: Execute on TVM
    // For now, just use the C reducer
    ReductionContext ctx = {0};
    ctx.max_steps = LAMBDA_MAX_REDUCTION_STEPS;
    return lambda_reduce_to_normal_form(term, &ctx);
}

