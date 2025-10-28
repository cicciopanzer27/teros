# Lambda³ Engine Documentation

**Version**: 1.0  
**Date**: January 2025

## Overview

The Lambda³ Engine is TEROS's native ternary lambda calculus runtime. It provides comprehensive support for lambda calculus operations with ternary logic integration, Church encoding, and direct compilation to T3 bytecode.

## Lambda Calculus Fundamentals

### Grammar

```
Term ::= Variable
      |  Abstraction Variable '.' Term
      |  Application Term Term
      |  '(' Term ')'

Variable ::= identifier
```

### Examples

```
x                     -- Variable
λx.x                 -- Identity function
λx.λy.x              -- Constant function (K combinator)
(λx.x) y             -- Application
```

## Beta Reduction

The core computation rule of lambda calculus.

### Formula

```
(λx.M) N → M[x := N]
```

Replace every free occurrence of x in M with N.

### Implementation

```c
LambdaTerm* result = lambda_beta_reduce(
    abstraction,  // λx.M
    argument      // N
);
```

### Example

```c
// (λx.x) y → y
LambdaTerm* id = lambda_create_abs(0, lambda_create_var(0));
LambdaTerm* y = lambda_create_var(1);
LambdaTerm* app = lambda_create_app(id, y);

ReductionContext ctx = {0};
ctx.max_steps = 100;
LambdaTerm* result = lambda_reduce_to_normal_form(app, &ctx);
// result = y
```

## Alpha Conversion

Renaming bound variables while preserving meaning.

### Formula

```
λx.M ≡_α λy.M[x := y]
```

(If y is not free in M)

### Implementation

```c
// Automatically handled during substitution
LambdaTerm* result = lambda_substitute(term, old_var, new_var);
```

## Eta Conversion

Removing redundant abstractions.

### Formula

```
λx.(M x) ≡_η M
```

(If x is not free in M)

## Church Encoding

### Church Numerals

Represent numbers as functions.

```
0 = λf.λx.x                        -- Apply f zero times
1 = λf.λx.f x                      -- Apply f once
2 = λf.λx.f (f x)                  -- Apply f twice
n = λf.λx.f^n x                    -- Apply f n times
```

#### Successor Function

```
SUCC = λn.λf.λx.f (n f x)
```

Applies f once more than n does.

#### Addition

```
ADD = λm.λn.λf.λx.m f (n f x)
```

Applies f m times after n times.

#### Multiplication

```
MUL = λm.λn.λf.m (n f)
```

Composes n·f, applies it m times.

### Church Booleans

```
TRUE  = λx.λy.x    -- Select first argument
FALSE = λx.λy.y    -- Select second argument
```

#### Logical Operations

```
AND = λa.λb.a b FALSE
OR  = λa.λb.a TRUE b
NOT = λb.b FALSE TRUE
```

### Church Pairs

```
PAIR = λx.λy.λf.f x y
```

A pair is a function that applies a selector to its components.

```
FIRST  = λp.p TRUE
SECOND = λp.p FALSE
```

## API Reference

### Term Creation

```c
// Variable
LambdaTerm* lambda_create_var(int32_t var_id);

// Abstraction: λx.M
LambdaTerm* lambda_create_abs(int32_t var_id, LambdaTerm* body);

// Application: M N
LambdaTerm* lambda_create_app(LambdaTerm* func, LambdaTerm* arg);
```

### Reduction

```c
// Single-step reduction
LambdaTerm* lambda_reduce_step(LambdaTerm* term, ReductionContext* ctx);

// Reduce to normal form
LambdaTerm* lambda_reduce_to_normal_form(LambdaTerm* term, ReductionContext* ctx);

// Substitution
LambdaTerm* lambda_substitute(LambdaTerm* term, int32_t var_id, LambdaTerm* replacement);
```

### Church Encoding

```c
// Numerals
LambdaTerm* lambda_church_zero(void);
LambdaTerm* lambda_church_one(void);
LambdaTerm* lambda_church_two(void);
LambdaTerm* lambda_church_successor(void);
LambdaTerm* lambda_church_add(void);
LambdaTerm* lambda_church_multiply(void);

// Booleans
LambdaTerm* lambda_church_true(void);
LambdaTerm* lambda_church_false(void);
LambdaTerm* lambda_church_and(void);
LambdaTerm* lambda_church_or(void);
LambdaTerm* lambda_church_not(void);

// Pairs
LambdaTerm* lambda_church_pair(void);
LambdaTerm* lambda_church_first(void);
LambdaTerm* lambda_church_second(void);
```

### Memory Management

```c
void lambda_retain(LambdaTerm* term);   // Increment ref count
void lambda_release(LambdaTerm* term);  // Decrement and free if 0
LambdaTerm* lambda_clone(LambdaTerm* term);  // Deep copy
```

## Reduction Strategies

### Call-by-Value (Eager)

Reduce arguments before application.

```
(λx.M) N → M[x := N_normal]
```

### Call-by-Name (Lazy)

Reduce function before arguments.

```
(λx.M_normal) N → M_normal[x := N]
```

### Normal Order

Always reduce the leftmost outermost redex first.

Guarantees termination if term has normal form.

## Y Combinator

Fixed-point combinator for recursion.

### Definition

```
Y = λf.(λx.f (x x)) (λx.f (x x))
```

### Property

```
Y f ≡ f (Y f)
```

### Usage

```c
LambdaTerm* y_combinator = lambda_create_app(
    lambda_create_abs(2, lambda_create_app(
        lambda_create_var(0),  // f
        lambda_create_app(lambda_create_var(1), lambda_create_var(1))
    )),
    lambda_create_abs(2, lambda_create_app(
        lambda_create_var(0),  // f
        lambda_create_app(lambda_create_var(1), lambda_create_var(1))
    ))
);
```

## Integration with T3-ISA

### Compilation

Lambda terms can be compiled to T3 bytecode:

```c
uint8_t bytecode[1024];
int32_t size;
int32_t result = lambda_compile_to_t3(
    lambda_term,
    bytecode,
    1024,
    &size
);
```

### Execution

```c
LambdaTerm* result = lambda_execute_on_tvm(lambda_term, tvm);
```

### Optimization

```c
LambdaTerm* optimized = lambda_optimize_term(lambda_term);
```

## Performance Considerations

### Time Complexity
- **Beta reduction**: O(n) per step, where n is term size
- **Normal form**: O(k·n) where k is reduction steps
- **Worst case**: Infinite (non-terminating terms)

### Space Complexity
- **Term storage**: O(n) where n is term size
- **Substitution**: O(n·m) where n is term size, m is replacement size

### Optimization Techniques
1. **Graph reduction**: Share common subexpressions
2. **Memoization**: Cache reduction results
3. **Compilation**: Convert to native code
4. **Garbage collection**: Automatic memory management

## Example Programs

### Identity Function

```c
// λx.x
LambdaTerm* identity = lambda_create_abs(0, lambda_create_var(0));

// Apply to value
LambdaTerm* value = lambda_create_var(1);
LambdaTerm* app = lambda_create_app(identity, value);

// Reduce: (λx.x) y → y
ReductionContext ctx = {0};
ctx.max_steps = 100;
LambdaTerm* result = lambda_reduce(app, &ctx);
```

### Church Numerals

```c
// 0 = λf.λx.x
LambdaTerm* zero = lambda_church_zero();

// 1 = λf.λx.f x
LambdaTerm* one = lambda_church_one();

// 2 = λf.λx.f (f x)
LambdaTerm* two = lambda_church_two();

// SUCC 0 = 1
LambdaTerm* succ = lambda_church_successor();
LambdaTerm* succ_zero = lambda_create_app(succ, zero);

ReductionContext ctx = {0};
ctx.max_steps = 100;
LambdaTerm* result = lambda_reduce_to_normal_form(succ_zero, &ctx);
```

## Type System

### Simply Typed Lambda Calculus

```
Type ::= BaseType
      |  Type '→' Type

BaseType ::= bool | int | trit
```

### Example Types

```
λx:bool.x         -- bool → bool
λx:trit.x         -- trit → trit
λx:bool.λy:int.x  -- bool → int → bool
```

### Type Checking

```c
// TODO: Implement type checking
bool lambda_type_check(LambdaTerm* term, LambdaType* type);
```

## Integration with Ternary Logic

### Ternary Lambda Calculus

Lambda³ extends standard lambda calculus with ternary logic:

- **Ternary variables**: Can hold values {-1, 0, +1}
- **Ternary operations**: Use ternary gates in reductions
- **Ternary types**: Extend type system with trit type

### Example

```c
// Ternary identity: λx:trit.x
LambdaTerm* tern_id = lambda_create_abs(0, lambda_create_var(0));

// Apply to ternary value
LambdaTerm* value = trit_to_lambda_term(trit_create(TERNARY_POSITIVE));
LambdaTerm* app = lambda_create_app(tern_id, value);

// Reduce with ternary operations
ReductionContext ctx = {0};
LambdaTerm* result = lambda_reduce(app, &ctx);
```

## Limitations and Future Work

### Current Limitations

1. **No type checking**: Untyped lambda calculus only
2. **No module system**: Single namespace
3. **Limited I/O**: Kernel-space only
4. **Performance**: Not optimized for production

### Planned Enhancements

1. **Type system**: Simply typed and System F
2. **Modules**: Separate compilation units
3. **Advanced reduction**: Optimized graph reduction
4. **JIT compilation**: Runtime code generation
5. **Ternary-specific optimizations**

## References

1. Church, A. - "An unsolvable problem of elementary number theory" (1936)
2. Kleene, S.C. - "Introduction to Metamathematics" (1952)
3. Curry, H.B. & Feys, R. - "Combinatory Logic, Vol. I" (1958)
4. Barendregt, H. - "The Lambda Calculus: Its Syntax and Semantics" (1984)
5. Pierce, B.C. - "Types and Programming Languages" (2002)

---

**Author**: TEROS Development Team  
**License**: See project LICENSE

