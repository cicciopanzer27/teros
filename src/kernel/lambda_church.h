/**
 * @file lambda_church.h
 * @brief Church Encoding Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef LAMBDA_CHURCH_H
#define LAMBDA_CHURCH_H

#include "lambda_engine.h"

// =============================================================================
// CHURCH NUMERALS
// =============================================================================

/** Create Church numeral 0 = λf.λx.x */
LambdaTerm* lambda_church_zero(void);

/** Create Church numeral 1 = λf.λx.f x */
LambdaTerm* lambda_church_one(void);

/** Create Church numeral 2 = λf.λx.f (f x) */
LambdaTerm* lambda_church_two(void);

/** Church Successor: SUCC = λn.λf.λx.f (n f x) */
LambdaTerm* lambda_church_successor(void);

/** Church Addition: ADD = λm.λn.λf.λx.m f (n f x) */
LambdaTerm* lambda_church_add(void);

/** Church Multiplication: MUL = λm.λn.λf.m (n f) */
LambdaTerm* lambda_church_multiply(void);

/** Create Church numeral n */
LambdaTerm* lambda_church_numeral(int n);

// =============================================================================
// CHURCH BOOLEANS
// =============================================================================

/** Church TRUE = λx.λy.x */
LambdaTerm* lambda_church_true(void);

/** Church FALSE = λx.λy.y */
LambdaTerm* lambda_church_false(void);

/** Church AND = λa.λb.a b FALSE */
LambdaTerm* lambda_church_and(void);

/** Church OR = λa.λb.a TRUE b */
LambdaTerm* lambda_church_or(void);

/** Church NOT = λb.b FALSE TRUE */
LambdaTerm* lambda_church_not(void);

// =============================================================================
// CHURCH PAIRS
// =============================================================================

/** Church PAIR constructor = λx.λy.λf.f x y */
LambdaTerm* lambda_church_pair(void);

/** Church FIRST = λp.p TRUE */
LambdaTerm* lambda_church_first(void);

/** Church SECOND = λp.p FALSE */
LambdaTerm* lambda_church_second(void);

#endif // LAMBDA_CHURCH_H

