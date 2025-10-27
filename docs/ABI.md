# TEROS Application Binary Interface (ABI) Specification

**Version:** 0.1  
**Date:** 2025-01-27  
**Status:** Draft

## Overview

This document defines the Application Binary Interface (ABI) for TEROS, including calling conventions, system call interface, register usage, and binary format.

## Architecture

- **ISA:** x86-64 (AMD64)
- **Word Size:** 64-bit
- **Byte Order:** Little-endian
- **Page Size:** 4KB (4096 bytes)
- **Pointer Size:** 8 bytes

## Calling Convention

TEROS uses a simplified calling convention based on System V AMD64 ABI:

### Register Usage

#### Integer Arguments (First 6)
1. `%rdi` / `%edi` - First argument
2. `%rsi` / `%esi` - Second argument
3. `%rdx` / `%edx` - Third argument
4. `%rcx` / `%ecx` - Fourth argument
5. `%r8` / `%r8d` - Fifth argument
6. `%r9` / `%r9d` - Sixth argument

**Additional arguments:** Pushed on stack (right-to-left)

#### Return Value
- `%rax` / `%eax` - Integer/pointer return value
- `%rdx:%rax` - 128-bit return values

#### Callee-Saved Registers
Must be preserved across function calls:
- `%rbx`, `%rbp`, `%r12`, `%r13`, `%r14`, `%r15`
- `%rsp` (stack pointer)

#### Caller-Saved Registers
May be clobbered by called function:
- `%rax`, `%rcx`, `%rdx`, `%rsi`, `%rdi`
- `%r8`, `%r9`, `%r10`, `%r11`

### Stack Frame

```
High Address
+------------------+
| Arguments 7+     |  (if needed)
+------------------+
| Return Address   |  Pushed by CALL
+------------------+
| Saved RBP        |  (optional frame pointer)
+------------------+ <-- RBP
| Local Variables  |
+------------------+
| Spill Space      |
+------------------+ <-- RSP
Low Address
```

**Stack Alignment:** 16-byte boundary before CALL instruction

### Red Zone

TEROS kernel **does NOT use a red zone** (`-mno-red-zone`). Userspace may use 128-byte red zone below RSP for leaf functions.

## System Call Interface

### Invocation Method

**Instruction:** `int $0x80` (software interrupt)

### System Call Number

Passed in `%rax` (or `%eax` for 32-bit)

### Arguments

Arguments passed in registers (same as function calling convention):

| Argument | Register   |
|----------|------------|
| syscall# | %rax       |
| arg0     | %rdi       |
| arg1     | %rsi       |
| arg2     | %rdx       |
| arg3     | %r10       |
| arg4     | %r8        |
| arg5     | %r9        |

**Note:** `%r10` used instead of `%rcx` (which is clobbered by SYSCALL instruction)

### Return Value

- **Success:** Positive value or zero in `%rax`
- **Error:** Negative value in `%rax` (ternary: TERNARY_NEGATIVE)
- **Unknown:** Special value for ternary operations

### Example System Call

```asm
; write(1, "Hello\n", 6)
mov $1, %rax        ; SYS_WRITE
mov $1, %rdi        ; fd = stdout
lea msg(%rip), %rsi ; buf = "Hello\n"
mov $6, %rdx        ; count = 6
int $0x80           ; invoke syscall
```

## Process Memory Layout

```
0xFFFFFFFFFFFFFFFF  ┌─────────────────┐
                    │ Kernel Space    │
0xFFFF800000000000  ├─────────────────┤
                    │ (Invalid)       │
0x00007FFFFFFFFFFF  ├─────────────────┤
                    │ Stack           │ (grows down)
                    │      ↓          │
                    ├─────────────────┤
                    │                 │
                    │   (unmapped)    │
                    │                 │
                    ├─────────────────┤
                    │      ↑          │
                    │ Heap (brk)      │ (grows up)
                    ├─────────────────┤
                    │ BSS (uninit)    │
                    ├─────────────────┤
                    │ Data (init)     │
                    ├─────────────────┤
                    │ Text (code)     │
0x0000000000400000  ├─────────────────┤
                    │ (Reserved)      │
0x0000000000000000  └─────────────────┘
```

### Segment Addresses

- **Text (code):** 0x400000 and up
- **Data:** After text, aligned to 4KB
- **BSS:** After data, zero-filled
- **Heap:** After BSS, grows with `brk()` syscall
- **Stack:** Top of user address space, grows downward

### Stack Size

- **Default:** 8MB
- **Maximum:** Configurable per process

## Binary Format

### Executable Format

TEROS uses **ELF64** (Executable and Linkable Format, 64-bit):

- **Class:** ELFCLASS64
- **Data:** ELFDATA2LSB (little-endian)
- **Machine:** EM_X86_64
- **Type:** ET_EXEC (executable) or ET_DYN (position-independent)

### Entry Point

- Defined in ELF header (`e_entry`)
- Typically points to `_start` symbol
- `_start` calls `main()` after initialization

### Dynamic Linking

**Not yet implemented in MVP.**

Future versions will support:
- Shared libraries (.so)
- Position Independent Executables (PIE)
- Dynamic linker at `/lib64/ld-teros.so.1`

## Ternary Types

TEROS supports ternary logic natively:

### Trit Type

- **Size:** Platform-dependent (currently 8-bit)
- **Values:**
  - `TERNARY_NEGATIVE` (-1)
  - `TERNARY_ZERO` (0)
  - `TERNARY_POSITIVE` (1)
  - `TERNARY_UNKNOWN` (special)

### Trit Operations

Performed using T3-ISA instructions (when available) or emulated in software.

## Signal Handling

### Signal Frame

When a signal is delivered, the kernel creates a signal frame on the user stack:

```c
struct sigframe {
    uint64_t rip;       // Saved instruction pointer
    uint64_t rflags;    // Saved flags
    uint64_t rax;       // Saved registers
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    // ... additional state
};
```

### Signal Return

Use `sigreturn()` syscall to restore context and return from signal handler.

## Thread-Local Storage (TLS)

**Not yet implemented.**

Future versions will use:
- `%fs` segment register for TLS base
- Thread pointer at `%fs:0`

## Floating Point

### FPU State

- **x87 FPU:** Not used in kernel
- **SSE/AVX:** Disabled in kernel (`-mno-sse -mno-sse2`)
- **Userspace:** May use FPU/SSE/AVX

### Context Switching

FPU state must be saved/restored during context switches for userspace processes.

## Version History

- **v0.1 (2025-01-27):** Initial draft
  - Basic calling convention
  - System call interface
  - Memory layout
  - ELF64 format

## See Also

- [T3-ISA Specification](T3-ISA.md)
- [Build Guide](BUILD.md)
- [System Call Reference](SYSCALLS.md)

---

**Maintained by:** TEROS Development Team  
**License:** See project LICENSE

