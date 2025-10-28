# TVM (Ternary Virtual Machine) Architecture

**Version**: 1.0  
**Date**: January 2025

## Overview

TVM (Ternary Virtual Machine) is TEROS's register-based virtual machine for executing T3-ISA (Ternary 3-Instruction Set Architecture) bytecode. It provides a complete execution environment for ternary programs with support for ternary logic operations, memory management, and system integration.

## Architecture

### Register Set

TVM has 16 general-purpose registers:

| Register | Name | Description |
|----------|------|-------------|
| %t0 - %t7 | General Purpose | Data and computation |
| %t8 (PC) | Program Counter | Current instruction address |
| %t9 (SP) | Stack Pointer | Stack top address |
| %t10 (FP) | Frame Pointer | Current stack frame |
| %t11 (LR) | Link Register | Return address |
| %t12 (CR) | Condition Register | Flags (Z, N, P) |
| %t13 (ACC) | Accumulator | Result storage |
| %t14 (TMP) | Temporary | Scratch register |
| %t15 (ZERO) | Zero | Always 0 |

### Register Layout

Each register stores a single trit value:

```c
typedef trit_t t3_register_t;

t3_register_t registers[T3_REGISTER_COUNT];
```

### Memory Model

TVM uses trit-addressable memory:

- **Addressable Unit**: Single trit
- **Address Space**: Configurable (default 1024 trits)
- **Alignment**: Natural (1 trit)

## T3-ISA Instruction Set

### Instruction Encoding

T3-ISA uses base-3 encoding:

```
Opcode: d0 + d1·3¹ + d2·3²  where di ∈ {0, 1, 2}
```

Total: 27 possible opcodes (3³)

### Instruction Format

```
+---------------+------+
| Opcode (3b)   | 000  |
| Operand 1 (4b)| 0000 |
| Operand 2 (4b)| 0000 |
| Operand 3 (4b)| 0000 |
| Immediate (16b)| 0000 0000 0000 0000 |
+---------------+------+
```

### Opcode Space

| Range | Category | Opcodes |
|-------|----------|---------|
| 0-2 | Load/Store | LOAD, STORE, PUSH, POP |
| 3-8 | Arithmetic | ADD, SUB, MUL, DIV, MOD |
| 9-13 | Logic | AND, OR, NOT, XOR, NAND |
| 14-17 | Comparison | CMP, TST |
| 18-22 | Control | JMP, JZ, CALL, RET, HALT |
| 23-26 | System | SYSCALL, IRET, CLI, STI |
| 27-29 | Extended | CPUID, RDTSC, INT, MOV, LEA, TST, TGATE |

### T3-ISA Instruction Reference

#### Data Movement

```
LOAD  reg, [addr]     -- Load from memory
STORE [addr], reg     -- Store to memory
MOV   reg1, reg2      -- Move register
PUSH  reg             -- Push to stack
POP   reg             -- Pop from stack
```

#### Arithmetic

```
ADD  reg_dst, reg_src1, reg_src2    -- reg_dst = reg_src1 + reg_src2
SUB  reg_dst, reg_src1, reg_src2    -- reg_dst = reg_src1 - reg_src2
MUL  reg_dst, reg_src1, reg_src2    -- reg_dst = reg_src1 × reg_src2
DIV  reg_dst, reg_src1, reg_src2    -- reg_dst = reg_src1 / reg_src2
MOD  reg_dst, reg_src1, reg_src2    -- reg_dst = reg_src1 mod reg_src2
```

#### Logic

```
AND  reg_dst, reg_src1, reg_src2    -- AND operation
OR   reg_dst, reg_src1, reg_src2    -- OR operation
NOT  reg_dst, reg_src               -- NOT operation
XOR  reg_dst, reg_src1, reg_src2    -- XOR operation
NAND reg_dst, reg_src1, reg_src2    -- NAND operation
```

#### Ternary Gates

```
TGATE gate_id, reg_src1, reg_src2, reg_dst
```

Evaluates one of 19,683 ternary gates.

**Example**:
```
TGATE 15633, %t1, %t2, %t0  -- t0 = KLEENE_AND(t1, t2)
TGATE 19569, %t1, %t2, %t0  -- t0 = KLEENE_OR(t1, t2)
```

#### Comparison

```
CMP  reg1, reg2              -- Compare reg1 with reg2
TST  reg                     -- Test reg (sets flags)
```

Sets condition register (CR) flags:
- **Z** (Zero): reg1 == reg2
- **N** (Negative): reg1 < reg2
- **P** (Positive): reg1 > reg2

#### Control Flow

```
JMP  target                  -- Jump to target
JZ   target                  -- Jump if zero
JNZ  target                  -- Jump if not zero
CALL target                  -- Call subroutine
RET                          -- Return from subroutine
HALT                         -- Halt execution
NOP                          -- No operation
```

#### System

```
SYSCALL num                  -- System call
IRET                         -- Return from interrupt
CLI                          -- Clear interrupts (disable)
STI                          -- Set interrupts (enable)
CPUID                        -- Get CPU info
RDTSC                        -- Read time stamp counter
INT  num                     -- Software interrupt
```

## TVM Execution Model

### Fetch-Decode-Execute Cycle

```
1. FETCH:   Read instruction at PC
2. DECODE:  Parse opcode and operands
3. EXECUTE: Perform operation
4. UPDATE:  Update PC, flags, etc.
```

### Instruction Execution

```c
trit_t tvm_execute_instruction(tvm_t* vm, t3_instruction_t* inst) {
    switch (inst->opcode) {
        case T3_OPCODE_ADD:
            return t3_execute_add(inst, vm->registers);
        case T3_OPCODE_TGATE:
            return t3_execute_tgate(inst, vm->registers);
        // ... other opcodes
    }
}
```

### Registers

```c
tvm_t* vm = tvm_create(1024);

// Set register
vm->registers[T3_REGISTER_R0] = trit_create(TERNARY_POSITIVE);

// Get register
trit_t value = vm->registers[T3_REGISTER_R0];
```

## Memory Management

### Trit-Addressable Memory

```c
// Load from memory
trit_array_t* memory = vm->memory;
trit_t value = trit_array_get(memory, address);

// Store to memory
trit_array_set(memory, address, value);
```

### Stack Operations

```c
// Push to stack
stack_push(vm, trit_create(TERNARY_POSITIVE));

// Pop from stack
trit_t value = stack_pop(vm);
```

## Performance Optimizations

### Instruction Cache

TVM implements a 64-entry direct-mapped instruction cache:

```c
typedef struct {
    uint32_t address;
    bool valid;
    t3_instruction_t* instruction;
} icache_entry_t;

icache_entry_t icache[64];
```

**Access Pattern**:
```c
uint32_t cache_idx = address & 0x3F;  // 64-entry mask
if (icache[cache_idx].valid && icache[cache_idx].address == address) {
    return icache[cache_idx].instruction;  // Cache hit
}
// Cache miss - decode instruction
```

### Branch Predictor

Simple 256-entry branch history table:

```c
typedef struct {
    uint32_t target;
    uint8_t state;  // 2-bit saturating counter
} branch_predictor_t;

branch_predictor_t bp_table[256];
```

### Statistics

TVM tracks execution statistics:

```c
typedef struct {
    uint64_t instructions_executed;
    uint64_t cache_hits;
    uint64_t cache_misses;
    uint64_t branch_predictions;
    uint64_t branch_mispredictions;
} tvm_stats_t;
```

## Integration with Lambda³

### Compilation to T3 Bytecode

Lambda terms can be compiled to T3 instructions:

```c
LambdaTerm* term = /* ... lambda term ... */;
uint8_t bytecode[1024];
int32_t size;

int32_t result = lambda_compile_to_t3(term, bytecode, 1024, &size);
```

### Execution on TVM

```c
tvm_t* vm = tvm_create(1024);
tvm_load_program(vm, program, program_size);
trit_t result = tvm_run(vm);
```

### Example: Church Numeral Execution

```c
// Compile Church numeral 2
LambdaTerm* two = lambda_church_two();
uint8_t bytecode[256];
int32_t size;
lambda_compile_to_t3(two, bytecode, 256, &size);

// Load into TVM
tvm_load_program(vm, bytecode, size);

// Execute
trit_t result = tvm_run(vm);
```

## Debugging and Monitoring

### Instruction Tracing

Enable tracing for debugging:

```c
void tvm_set_trace(tvm_t* vm, bool enabled) {
    vm->trace_enabled = enabled;
}
```

### Breakpoints

Set breakpoints for debugging:

```c
void tvm_set_breakpoint(tvm_t* vm, uint32_t address) {
    vm->breakpoints[address] = true;
}
```

### Statistics

Query execution statistics:

```c
tvm_stats_t stats = tvm_get_statistics(vm);
printf("Instructions: %lld\n", stats.instructions_executed);
printf("Cache hit rate: %.2f%%\n",
    100.0 * stats.cache_hits / (stats.cache_hits + stats.cache_misses));
```

## Example Program

### Simple Calculation

```c
// Pseudo-assembly:
//   LOAD  %t1, 1        -- Load 1
//   LOAD  %t2, 2        -- Load 2
//   ADD   %t0, %t1, %t2 -- t0 = t1 + t2

t3_instruction_t program[] = {
    {T3_OPCODE_LOAD, T3_REGISTER_R1, 0, 0, 1},
    {T3_OPCODE_LOAD, T3_REGISTER_R2, 0, 0, 2},
    {T3_OPCODE_ADD, T3_REGISTER_R0, T3_REGISTER_R1, T3_REGISTER_R2, 0},
};

tvm_t* vm = tvm_create(1024);
tvm_load_program(vm, program, 3);
trit_t result = tvm_run(vm);
```

## Performance Characteristics

### Execution Speed

- **Native ternary gates**: O(1) lookup
- **Basic arithmetic**: ~10-50 CPU cycles
- **Memory operations**: ~5-20 CPU cycles
- **Branch**: ~3-15 CPU cycles (depends on prediction)

### Benchmarks

Typical execution on modern CPU:
- **Simple programs**: ~1000-10000 instructions/second
- **Complex programs**: ~100-1000 instructions/second
- **With JIT**: Can reach native code speeds

## Limitations

### Current Limitations

1. **Single-threaded**: One VM instance per thread
2. **Limited memory**: Fixed-size memory allocation
3. **No FPU**: Floating-point emulation only
4. **Basic I/O**: Console output only

### Future Enhancements

1. **Multi-threading**: Parallel VM execution
2. **Dynamic memory**: Heap allocation
3. **JIT compilation**: Native code generation
4. **Advanced debugging**: Source-level debugging
5. **Profiling**: Performance analysis tools

## References

1. "Computer Systems: A Programmer's Perspective" - Bryant & O'Hallaron
2. "Modern Processor Design" - Shen & Lipasti
3. "Computer Architecture: A Quantitative Approach" - Hennessy & Patterson
4. "Virtual Machine Design and Implementation in C/C++" - Brian T. Lewis

---

**Author**: TEROS Development Team  
**License**: See project LICENSE

