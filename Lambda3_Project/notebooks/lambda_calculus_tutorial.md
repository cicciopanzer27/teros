# Lambda Calculus in 30 Minutes
**Interactive Tutorial for Lambda³**

---

## Part 1: What is Lambda Calculus?

Lambda calculus is a **formal system** for expressing computation based on function abstraction and application.

### The Three Constructs:

1. **Variable**: `x`, `y`, `z`
2. **Abstraction** (function): `λx.M` or `\x.M`
3. **Application**: `M N`

That's it! Everything in computation can be expressed with just these three.

---

## Part 2: Variables

Variables are just names:

```python
from lambda3.parser.parser import parse

# Parse a variable
term = parse("x")
print(term)  # x
```

**Try it yourself:**
```python
# Parse different variables
y = parse("y")
z = parse("z")
print(f"y = {y}")
print(f"z = {z}")
```

---

## Part 3: Abstraction (Functions)

An abstraction `λx.M` is a function that:
- Takes input `x`
- Returns result `M`

### Identity Function:

```python
# Identity: λx.x (returns its input)
identity = parse(r"\x.x")
print(identity)  # (\x.x)
```

### Constant Function:

```python
# Const: λx.λy.x (returns first argument, ignores second)
const = parse(r"\x.\y.x")
print(const)  # (\x.(\y.x))
```

**Try it:**
```python
# Create your own function
my_func = parse(r"\a.\b.a")
print(my_func)
```

---

## Part 4: Application

Application `M N` means "apply function M to argument N":

```python
from lambda3.engine.reducer import reduce

# Apply identity to y
app = parse(r"(\x.x) y")
result = reduce(app)
print(f"(\x.x) y = {result}")  # y
```

**What happened?**
1. `\x.x` is the identity function
2. `y` is the argument
3. Application substitutes `y` for `x`
4. Result: `y`

**Try it:**
```python
# Apply const to two arguments
app2 = parse(r"((\x.\y.x) a) b")
result2 = reduce(app2)
print(f"Result: {result2}")  # Should be 'a'
```

---

## Part 5: Church Numerals

We can encode numbers as functions!

### Church Numeral N = "apply f, N times"

```python
# Zero: λf.λx.x (apply f zero times)
zero = parse(r"\f.\x.x")

# One: λf.λx.f x (apply f once)
one = parse(r"\f.\x.f x")

# Two: λf.λx.f (f x) (apply f twice)
two = parse(r"\f.\x.f (f x)")

print(f"0 = {zero}")
print(f"1 = {one}")
print(f"2 = {two}")
```

**Mind-blown?** Numbers are just functions!

---

## Part 6: Church Booleans

Booleans too!

```python
# TRUE: λx.λy.x (return first argument)
true = parse(r"\x.\y.x")

# FALSE: λx.λy.y (return second argument)
false = parse(r"\x.\y.y")

print(f"TRUE  = {true}")
print(f"FALSE = {false}")
```

**Exercise:** Create an IF-THEN-ELSE:
```python
# if_then_else = λp.λa.λb.p a b
ite = parse(r"\p.\a.\b.p a b")

# Test: if TRUE then x else y
test = parse(r"(((\p.\a.\b.p a b) (\x.\y.x)) x) y")
result = reduce(test, max_steps=100)
print(f"Result: {result}")  # Should be x
```

---

## Part 7: Ternary Encoding

Lambda³ uses **ternary** encoding for optimal efficiency:

```python
from lambda3.ternary.encoder import encode, encoding_efficiency

# Encode identity
term = parse(r"\x.x")
trits = encode(term)
eff = encoding_efficiency(term)

print(f"Term: {term}")
print(f"Trits: {trits}")
print(f"Ternary: {eff['ternary_bits']:.1f} bits")
print(f"Binary: {eff['binary_bits']} bits")
print(f"Savings: {eff['savings_percent']:.1f}%")
```

**Result:** 20.8% more efficient than binary!

---

## Part 8: Type Checking

Ensure your lambda terms are well-typed:

```python
from lambda3.types import type_check

# Type check identity
term = parse(r"\x.x")
type_ = type_check(term)
print(f"Type: {type_}")  # t0 -> t0
```

**Exercise:** Type check const:
```python
const = parse(r"\x.\y.x")
const_type = type_check(const)
print(f"Const type: {const_type}")  # t0 -> t1 -> t0
```

---

## Part 9: Graph Reduction

Optimize performance with graph reduction:

```python
from lambda3.engine.graph_reducer import reduce_with_sharing

# Reduce with sharing
term = parse(r"(\f.\x.f (f x)) g y")
result, stats = reduce_with_sharing(term)

print(f"Result: {result}")
print(f"Stats: {stats}")
print(f"  Reductions: {stats['reductions']}")
print(f"  Sharing hits: {stats['sharing_hits']}")
```

**Insight:** Sharing eliminates redundant computation!

---

## Part 10: Interactive REPL

Use the REPL for exploration:

```bash
$ python run_repl.py

lambda> \x.x
(\x.x) => (\x.x)

lambda> (\x.x) y
((\x.x) y) => y

lambda> :0
:0 = \f.\x.x

lambda> :encode \x.x
Trits: [0, 23, -1, 23]
Savings: 20.8%

lambda> :help
# Shows all commands
```

---

## Part 11: Exercises

### Exercise 1: Implement NOT
```python
# NOT: λp.λa.λb.p b a
# Hint: Swap TRUE/FALSE arguments
```

### Exercise 2: Implement AND
```python
# AND: λp.λq.p q p
# Hint: If p then q else p
```

### Exercise 3: Successor
```python
# SUCC: λn.λf.λx.f (n f x)
# Hint: Apply f one more time than n does
```

### Solutions:
```python
# NOT
not_op = parse(r"\p.\a.\b.p b a")

# AND
and_op = parse(r"\p.\q.p q p")

# SUCC
succ = parse(r"\n.\f.\x.f (n f x)")

# Test SUCC on 0
test = parse(r"((\n.\f.\x.f (n f x)) (\f.\x.x))")
result = reduce(test, max_steps=100)
print(f"SUCC(0) = {result}")  # Should be structure of 1
```

---

## Part 12: Next Steps

### You've learned:
- ✓ Lambda calculus basics
- ✓ Church encodings (numbers, booleans)
- ✓ Ternary encoding (20.8% efficiency)
- ✓ Type checking
- ✓ Graph reduction

### Next level:
1. **Type Inference** - Automatic type derivation
2. **Proof Assistant** - Verify mathematical theorems
3. **Killer Demos** - Symbolic solver, proof verifier
4. **Neural-Symbolic** - Hybrid AI

### Resources:
- `examples/killer_demos/` - Amazing applications
- `LAMBDA3_COMPLETION_GUIDE.md` - Full guide
- `run_repl.py` - Interactive exploration

---

## 🎯 Key Takeaways

1. **Lambda calculus = Universal computation**
   - Just 3 constructs (Var, Abs, App)
   - Turing complete

2. **Ternary encoding = Optimal**
   - 20.8% more efficient than binary
   - Natural fit for lambda (3 constructs!)

3. **Types = Safety**
   - Catch errors at compile time
   - Curry-Howard: Types = Propositions

4. **Formal = Verifiable**
   - Not probabilistic guessing
   - Mathematical proofs

---

## 🚀 "You're not learning syntax. You're learning to THINK."

**Lambda³ + Ternary + Types = Foundation for Verifiable AI**

---

**Congratulations!** 🎉 You now understand lambda calculus!

**Next:** Try the Killer Demos in `examples/killer_demos/`

