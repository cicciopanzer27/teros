/**
 * @file lambda_church.c
 * @brief Church Encoding for Lambda Calculus
 * @author TEROS Development Team
 * @date 2025
 * 
 * Implements Church encoding for numerals, booleans, and pairs
 * Based on lambda calculus encoding by Alonzo Church
 */

#include "lambda_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// =============================================================================
// CHURCH NUMERALS
// =============================================================================

/**
 * Church Numeral Definition:
 * 
 * 0 = λf.λx.x                           (apply f zero times)
 * 1 = λf.λx.f x                         (apply f once)
 * 2 = λf.λx.f (f x)                     (apply f twice)
 * n = λf.λx.f^n x                       (apply f n times)
 * 
 * General form: n = λf.λx.f^n(x)
 */

/**
 * Create Church numeral 0
 * 0 = λf.λx.x
 */
LambdaTerm* lambda_church_zero(void)
{
    // 0 = λf.λx.x
    LambdaTerm* x = lambda_create_var(1);  // x
    LambdaTerm* inner_abs = lambda_create_abs(1, x);  // λx.x
    LambdaTerm* result = lambda_create_abs(0, inner_abs);  // λf.λx.x
    lambda_release(inner_abs);
    lambda_release(x);
    return result;
}

/**
 * Create Church numeral 1
 * 1 = λf.λx.f x
 */
LambdaTerm* lambda_church_one(void)
{
    // 1 = λf.λx.f x
    LambdaTerm* f = lambda_create_var(0);   // f
    LambdaTerm* x = lambda_create_var(1);   // x
    LambdaTerm* fx = lambda_create_app(f, x);  // f x
    lambda_release(f);
    lambda_release(x);
    
    LambdaTerm* inner_abs = lambda_create_abs(1, fx);  // λx.f x
    lambda_release(fx);
    
    LambdaTerm* result = lambda_create_abs(0, inner_abs);  // λf.λx.f x
    lambda_release(inner_abs);
    return result;
}

/**
 * Create Church numeral 2
 * 2 = λf.λx.f (f x)
 */
LambdaTerm* lambda_church_two(void)
{
    // 2 = λf.λx.f (f x)
    LambdaTerm* f = lambda_create_var(0);   // f
    LambdaTerm* x = lambda_create_var(1);   // x
    LambdaTerm* fx = lambda_create_app(f, x);  // f x
    LambdaTerm* ffx = lambda_create_app(f, fx);  // f (f x)
    lambda_release(f);
    lambda_release(x);
    lambda_release(fx);
    
    LambdaTerm* inner_abs = lambda_create_abs(1, ffx);  // λx.f (f x)
    lambda_release(ffx);
    
    LambdaTerm* result = lambda_create_abs(0, inner_abs);  // λf.λx.f (f x)
    lambda_release(inner_abs);
    return result;
}

/**
 * Church Successor Function
 * SUCC = λn.λf.λx.f (n f x)
 * 
 * Formula: SUCC n = λf.λx.f(n f x)
 * Applies f once more than n does
 */
LambdaTerm* lambda_church_successor(void)
{
    // SUCC = λn.λf.λx.f (n f x)
    
    // Inner lambda: λx.f (n f x)
    LambdaTerm* n = lambda_create_var(2);  // n (outermost bound var)
    LambdaTerm* f = lambda_create_var(0);  // f
    LambdaTerm* x = lambda_create_var(1);  // x
    
    // Build: n f x
    LambdaTerm* nf = lambda_create_app(n, f);  // n f
    lambda_release(n);
    LambdaTerm* nfx = lambda_create_app(nf, x);  // n f x
    lambda_release(f);
    lambda_release(nf);
    
    // Build: f (n f x)
    LambdaTerm* f_var = lambda_create_var(0);  // f (again, for outer scope)
    LambdaTerm* fnfx = lambda_create_app(f_var, nfx);  // f (n f x)
    lambda_release(f_var);
    lambda_release(nfx);
    lambda_release(x);
    
    // λx.f (n f x)
    LambdaTerm* inner_abs = lambda_create_abs(1, fnfx);
    lambda_release(fnfx);
    
    // λf.λx.f (n f x)
    LambdaTerm* middle_abs = lambda_create_abs(0, inner_abs);
    lambda_release(inner_abs);
    
    // λn.λf.λx.f (n f x)
    LambdaTerm* result = lambda_create_abs(2, middle_abs);
    lambda_release(middle_abs);
    
    return result;
}

/**
 * Church Addition
 * ADD = λm.λn.λf.λx.m f (n f x)
 * 
 * Formula: m + n = m applications of f after n applications
 */
LambdaTerm* lambda_church_add(void)
{
    // ADD = λm.λn.λf.λx.m f (n f x)
    
    // x
    LambdaTerm* x = lambda_create_var(1);  // x
    
    // f
    LambdaTerm* f = lambda_create_var(0);  // f
    
    // n
    LambdaTerm* n = lambda_create_var(3);  // n
    
    // n f
    LambdaTerm* nf = lambda_create_app(n, f);
    lambda_release(n);
    
    // n f x
    LambdaTerm* nfx = lambda_create_app(nf, x);
    lambda_release(nf);
    lambda_release(x);
    
    // m
    LambdaTerm* m = lambda_create_var(4);  // m
    LambdaTerm* f_var = lambda_create_var(0);  // f (again)
    lambda_release(f);
    
    // m f
    LambdaTerm* mf = lambda_create_app(m, f_var);
    lambda_release(m);
    
    // m f (n f x)
    LambdaTerm* mfnfx = lambda_create_app(mf, nfx);
    lambda_release(mf);
    lambda_release(nfx);
    
    // λx.m f (n f x)
    LambdaTerm* x_abs = lambda_create_abs(1, mfnfx);
    lambda_release(mfnfx);
    
    // λf.λx.m f (n f x)
    LambdaTerm* f_abs = lambda_create_abs(0, x_abs);
    lambda_release(x_abs);
    
    // λn.λf.λx.m f (n f x)
    LambdaTerm* n_abs = lambda_create_abs(3, f_abs);
    lambda_release(f_abs);
    
    // λm.λn.λf.λx.m f (n f x)
    LambdaTerm* result = lambda_create_abs(4, n_abs);
    lambda_release(n_abs);
    
    lambda_release(f_var);
    
    return result;
}

/**
 * Church Multiplication
 * MUL = λm.λn.λf.m (n f)
 * 
 * Formula: m * n = n compositions of f, applied m times
 */
LambdaTerm* lambda_church_multiply(void)
{
    // MUL = λm.λn.λf.m (n f)
    
    // f
    LambdaTerm* f = lambda_create_var(0);
    
    // n
    LambdaTerm* n = lambda_create_var(3);
    
    // n f
    LambdaTerm* nf = lambda_create_app(n, f);
    lambda_release(n);
    
    // m
    LambdaTerm* m = lambda_create_var(4);
    
    // m (n f)
    LambdaTerm* mnf = lambda_create_app(m, nf);
    lambda_release(m);
    lambda_release(nf);
    lambda_release(f);
    
    // λf.m (n f)
    LambdaTerm* f_abs = lambda_create_abs(0, mnf);
    lambda_release(mnf);
    
    // λn.λf.m (n f)
    LambdaTerm* n_abs = lambda_create_abs(3, f_abs);
    lambda_release(f_abs);
    
    // λm.λn.λf.m (n f)
    LambdaTerm* result = lambda_create_abs(4, n_abs);
    lambda_release(n_abs);
    
    return result;
}

// =============================================================================
// CHURCH BOOLEANS
// =============================================================================

/**
 * Church Boolean Definition:
 * 
 * TRUE  = λx.λy.x  (select first argument)
 * FALSE = λx.λy.y  (select second argument)
 */

/**
 * Create Church TRUE
 * TRUE = λx.λy.x
 */
LambdaTerm* lambda_church_true(void)
{
    // TRUE = λx.λy.x
    LambdaTerm* x = lambda_create_var(0);  // x
    LambdaTerm* inner = lambda_create_abs(1, x);  // λy.x
    lambda_release(x);
    LambdaTerm* result = lambda_create_abs(0, inner);  // λx.λy.x
    lambda_release(inner);
    return result;
}

/**
 * Create Church FALSE
 * FALSE = λx.λy.y
 */
LambdaTerm* lambda_church_false(void)
{
    // FALSE = λx.λy.y
    LambdaTerm* y = lambda_create_var(1);  // y
    LambdaTerm* inner = lambda_create_abs(1, y);  // λy.y
    lambda_release(y);
    LambdaTerm* result = lambda_create_abs(0, inner);  // λx.λy.y
    lambda_release(inner);
    return result;
}

/**
 * Church AND
 * AND = λa.λb.a b FALSE
 * 
 * If a is TRUE (selects first arg): returns b
 * If a is FALSE (selects second arg): returns FALSE
 */
LambdaTerm* lambda_church_and(void)
{
    // AND = λa.λb.a b FALSE
    LambdaTerm* FALSE = lambda_church_false();  // FALSE
    
    LambdaTerm* b = lambda_create_var(2);  // b
    
    // a
    LambdaTerm* a = lambda_create_var(3);  // a
    
    // a b
    LambdaTerm* ab = lambda_create_app(a, b);
    lambda_release(a);
    lambda_release(b);
    
    // a b FALSE
    LambdaTerm* abFALSE = lambda_create_app(ab, FALSE);
    lambda_release(ab);
    lambda_release(FALSE);
    
    // λb.a b FALSE
    LambdaTerm* b_abs = lambda_create_abs(2, abFALSE);
    lambda_release(abFALSE);
    
    // λa.λb.a b FALSE
    LambdaTerm* result = lambda_create_abs(3, b_abs);
    lambda_release(b_abs);
    
    return result;
}

/**
 * Church OR
 * OR = λa.λb.a TRUE b
 * 
 * If a is TRUE: returns TRUE
 * If a is FALSE: returns b
 */
LambdaTerm* lambda_church_or(void)
{
    // OR = λa.λb.a TRUE b
    LambdaTerm* TRUE = lambda_church_true();  // TRUE
    
    LambdaTerm* b = lambda_create_var(2);  // b
    
    LambdaTerm* a = lambda_create_var(3);  // a
    
    // a TRUE
    LambdaTerm* aTRUE = lambda_create_app(a, TRUE);
    lambda_release(a);
    lambda_release(TRUE);
    
    // a TRUE b
    LambdaTerm* aTRUEb = lambda_create_app(aTRUE, b);
    lambda_release(aTRUE);
    lambda_release(b);
    
    // λb.a TRUE b
    LambdaTerm* b_abs = lambda_create_abs(2, aTRUEb);
    lambda_release(aTRUEb);
    
    // λa.λb.a TRUE b
    LambdaTerm* result = lambda_create_abs(3, b_abs);
    lambda_release(b_abs);
    
    return result;
}

/**
 * Church NOT
 * NOT = λb.b FALSE TRUE
 * 
 * If b is TRUE: returns FALSE
 * If b is FALSE: returns TRUE
 */
LambdaTerm* lambda_church_not(void)
{
    // NOT = λb.b FALSE TRUE
    LambdaTerm* TRUE = lambda_church_true();
    LambdaTerm* FALSE = lambda_church_false();
    
    LambdaTerm* b = lambda_create_var(0);  // b
    
    // b FALSE
    LambdaTerm* bFALSE = lambda_create_app(b, FALSE);
    lambda_release(b);
    lambda_release(FALSE);
    
    // b FALSE TRUE
    LambdaTerm* bFALSETRUE = lambda_create_app(bFALSE, TRUE);
    lambda_release(bFALSE);
    lambda_release(TRUE);
    
    // λb.b FALSE TRUE
    LambdaTerm* result = lambda_create_abs(0, bFALSETRUE);
    lambda_release(bFALSETRUE);
    
    return result;
}

// =============================================================================
// CHURCH PAIRS
// =============================================================================

/**
 * Church Pair Definition:
 * 
 * PAIR = λx.λy.λf.f x y
 * 
 * A pair is a function that, given a selector function f,
 * applies f to its two components
 */

/**
 * Church PAIR constructor
 * PAIR = λx.λy.λf.f x y
 */
LambdaTerm* lambda_church_pair(void)
{
    // PAIR = λx.λy.λf.f x y
    
    // y
    LambdaTerm* y = lambda_create_var(1);  // y
    
    // x
    LambdaTerm* x = lambda_create_var(2);  // x
    
    // f
    LambdaTerm* f = lambda_create_var(0);  // f
    
    // f x
    LambdaTerm* fx = lambda_create_app(f, x);
    lambda_release(f);
    lambda_release(x);
    
    // f x y
    LambdaTerm* fxy = lambda_create_app(fx, y);
    lambda_release(fx);
    lambda_release(y);
    
    // λf.f x y
    LambdaTerm* f_abs = lambda_create_abs(0, fxy);
    lambda_release(fxy);
    
    // λy.λf.f x y
    LambdaTerm* y_abs = lambda_create_abs(1, f_abs);
    lambda_release(f_abs);
    
    // λx.λy.λf.f x y
    LambdaTerm* result = lambda_create_abs(2, y_abs);
    lambda_release(y_abs);
    
    return result;
}

/**
 * Church FIRST (first element of pair)
 * FIRST = λp.p (λx.λy.x)
 * 
 * Applies pair to TRUE (which selects first element)
 */
LambdaTerm* lambda_church_first(void)
{
    // FIRST = λp.p TRUE
    LambdaTerm* TRUE = lambda_church_true();
    
    LambdaTerm* p = lambda_create_var(0);  // p
    
    // p TRUE
    LambdaTerm* pTRUE = lambda_create_app(p, TRUE);
    lambda_release(p);
    lambda_release(TRUE);
    
    // λp.p TRUE
    LambdaTerm* result = lambda_create_abs(0, pTRUE);
    lambda_release(pTRUE);
    
    return result;
}

/**
 * Church SECOND (second element of pair)
 * SECOND = λp.p (λx.λy.y)
 * 
 * Applies pair to FALSE (which selects second element)
 */
LambdaTerm* lambda_church_second(void)
{
    // SECOND = λp.p FALSE
    LambdaTerm* FALSE = lambda_church_false();
    
    LambdaTerm* p = lambda_create_var(0);  // p
    
    // p FALSE
    LambdaTerm* pFALSE = lambda_create_app(p, FALSE);
    lambda_release(p);
    lambda_release(FALSE);
    
    // λp.p FALSE
    LambdaTerm* result = lambda_create_abs(0, pFALSE);
    lambda_release(pFALSE);
    
    return result;
}

// =============================================================================
// PREDEFINED VALUES
// =============================================================================

/**
 * Create Church numeral n
 * Use SUCC applied n times to zero
 */
LambdaTerm* lambda_church_numeral(int n)
{
    if (n == 0) {
        return lambda_church_zero();
    } else if (n == 1) {
        return lambda_church_one();
    } else if (n == 2) {
        return lambda_church_two();
    }
    
    // For n > 2, apply SUCC repeatedly
    LambdaTerm* result = lambda_church_zero();
    LambdaTerm* SUCC = lambda_church_successor();
    
    for (int i = 0; i < n; i++) {
        LambdaTerm* temp = lambda_create_app(SUCC, result);
        lambda_release(result);
        result = temp;
    }
    
    lambda_release(SUCC);
    return result;
}

