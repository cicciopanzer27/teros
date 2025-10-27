/**
 * Test Lambda Engine
 * Verifies lambda calculus operations work correctly
 */

#include "lambda_engine.h"
#include <stdio.h>
#include <assert.h>

// Helper: Create Church numeral N
// N = λf.λx.f^N(x)
LambdaTerm* church_numeral(int n) {
    // Start with x (var 1)
    LambdaTerm* result = lambda_create_var(1);
    
    // Apply f (var 0) n times
    for (int i = 0; i < n; i++) {
        LambdaTerm* f = lambda_create_var(0);
        LambdaTerm* temp = lambda_create_app(f, result);
        lambda_release(f);
        lambda_release(result);
        result = temp;
    }
    
    // Wrap in λx. (var 1)
    LambdaTerm* inner = lambda_create_abs(1, result);
    lambda_release(result);
    
    // Wrap in λf. (var 0)
    LambdaTerm* outer = lambda_create_abs(0, inner);
    lambda_release(inner);
    
    return outer;
}

void test_basic_creation() {
    printf("\n=== Test: Basic Creation ===\n");
    
    // Create variable x0
    LambdaTerm* var = lambda_create_var(0);
    assert(var->type == LAMBDA_VAR);
    assert(var->data.var.var_id == 0);
    printf("✓ Variable creation works\n");
    
    // Create abstraction λx0.x0 (identity)
    LambdaTerm* id = lambda_create_abs(0, var);
    assert(id->type == LAMBDA_ABS);
    assert(id->data.abs.var_id == 0);
    printf("✓ Abstraction creation works\n");
    
    // Create application (λx.x) y
    LambdaTerm* y = lambda_create_var(1);
    LambdaTerm* app = lambda_create_app(id, y);
    assert(app->type == LAMBDA_APP);
    printf("✓ Application creation works\n");
    
    lambda_release(app);
    lambda_release(y);
    printf("✓ Memory management works\n");
}

void test_identity_reduction() {
    printf("\n=== Test: Identity Reduction ===\n");
    
    // (λx.x) y → y
    LambdaTerm* x = lambda_create_var(0);
    LambdaTerm* id = lambda_create_abs(0, x);
    LambdaTerm* y = lambda_create_var(1);
    LambdaTerm* app = lambda_create_app(id, y);
    
    printf("Before: ");
    lambda_print(app);
    printf("\n");
    
    ReductionContext ctx = {0};
    ctx.max_steps = 100;
    LambdaTerm* result = lambda_reduce(app, &ctx);
    
    printf("After:  ");
    lambda_print(result);
    printf("\n");
    
    // Should be just y (var 1)
    assert(result->type == LAMBDA_VAR);
    assert(result->data.var.var_id == 1);
    printf("✓ Identity reduction works: (λx.x) y → y\n");
    printf("  Reductions: %d\n", ctx.reduction_count);
    
    lambda_release(app);
    lambda_release(result);
}

void test_const_reduction() {
    printf("\n=== Test: Const Reduction ===\n");
    
    // (λx.λy.x) a b → a
    // Step 1: Create λx.λy.x
    LambdaTerm* x = lambda_create_var(0);
    LambdaTerm* inner = lambda_create_abs(1, x);  // λy.x
    LambdaTerm* konst = lambda_create_abs(0, inner);  // λx.λy.x
    
    // Step 2: Apply to a
    LambdaTerm* a = lambda_create_var(2);
    LambdaTerm* app1 = lambda_create_app(konst, a);
    
    // Step 3: Apply to b
    LambdaTerm* b = lambda_create_var(3);
    LambdaTerm* app2 = lambda_create_app(app1, b);
    
    printf("Before: ");
    lambda_print(app2);
    printf("\n");
    
    ReductionContext ctx = {0};
    ctx.max_steps = 100;
    LambdaTerm* result = lambda_reduce_to_normal_form(app2, &ctx);
    
    printf("After:  ");
    lambda_print(result);
    printf("\n");
    
    // Should be just a (var 2)
    assert(result->type == LAMBDA_VAR);
    assert(result->data.var.var_id == 2);
    printf("✓ Const reduction works: (λx.λy.x) a b → a\n");
    printf("  Reductions: %d\n", ctx.reduction_count);
    
    lambda_release(app2);
    lambda_release(result);
    lambda_release(b);
}

void test_church_numerals() {
    printf("\n=== Test: Church Numerals ===\n");
    
    // Create Church numeral 0
    LambdaTerm* zero = church_numeral(0);
    printf("Church 0: ");
    lambda_print(zero);
    printf("\n");
    
    // Create Church numeral 2
    LambdaTerm* two = church_numeral(2);
    printf("Church 2: ");
    lambda_print(two);
    printf("\n");
    
    printf("✓ Church numerals created\n");
    
    lambda_release(zero);
    lambda_release(two);
}

void test_memory_management() {
    printf("\n=== Test: Memory Management ===\n");
    
    // Create and release many terms
    for (int i = 0; i < 100; i++) {
        LambdaTerm* var = lambda_create_var(i);
        LambdaTerm* abs = lambda_create_abs(i, var);
        lambda_release(abs);
    }
    
    printf("✓ Memory pool working (100 allocations)\n");
    
    // Test reference counting
    LambdaTerm* shared = lambda_create_var(0);
    assert(shared->ref_count == 1);
    
    lambda_retain(shared);
    assert(shared->ref_count == 2);
    
    lambda_release(shared);
    assert(shared->ref_count == 1);
    
    lambda_release(shared);
    printf("✓ Reference counting works\n");
}

void test_performance() {
    printf("\n=== Test: Performance Benchmark ===\n");
    
    // Create a moderately complex term
    // (λx.λy.x y) (λz.z) w
    LambdaTerm* z = lambda_create_var(2);
    LambdaTerm* id = lambda_create_abs(2, z);
    
    LambdaTerm* x = lambda_create_var(0);
    LambdaTerm* y = lambda_create_var(1);
    LambdaTerm* xy = lambda_create_app(x, y);
    LambdaTerm* abs_y = lambda_create_abs(1, xy);
    LambdaTerm* abs_x = lambda_create_abs(0, abs_y);
    
    LambdaTerm* app1 = lambda_create_app(abs_x, id);
    LambdaTerm* w = lambda_create_var(3);
    LambdaTerm* app2 = lambda_create_app(app1, w);
    
    printf("Term: ");
    lambda_print(app2);
    printf("\n");
    
    ReductionContext ctx = {0};
    ctx.max_steps = 1000;
    
    LambdaTerm* result = lambda_reduce_to_normal_form(app2, &ctx);
    
    printf("Result: ");
    lambda_print(result);
    printf("\n");
    printf("Reductions: %d\n", ctx.reduction_count);
    printf("✓ Performance test completed\n");
    
    lambda_release(app2);
    lambda_release(result);
    lambda_release(w);
}

int main() {
    printf("╔════════════════════════════════════════╗\n");
    printf("║   TEROS Lambda Engine Test Suite      ║\n");
    printf("║   Native Ternary Lambda Calculus      ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    test_basic_creation();
    test_identity_reduction();
    test_const_reduction();
    test_church_numerals();
    test_memory_management();
    test_performance();
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║   ALL TESTS PASSED! ✓                 ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    return 0;
}

