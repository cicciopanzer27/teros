# TEROS Technical Architecture

Experimental ternary operating system kernel for x86-64.

**Version**: Development  
**Status**: Early stage - compilation successful, boot untested  
**Last Updated**: October 2025

---

## Project Overview

TEROS is a research operating system implementing ternary logic (balanced ternary) on standard binary x86-64 hardware through software emulation.

**Goals**:
- Explore ternary computing concepts in OS design
- Implement functional kernel with ternary VM
- Test feasibility of ternary operations on binary CPU

**Non-Goals**:
- Custom ternary hardware
- Production deployment
- Performance optimization (at this stage)

---

## Architecture Overview

### Boot Sequence

```
GRUB (Multiboot) → boot32.S → Long Mode → boot64.S → kernel_main()
```

**Components**:
1. `boot32.S` - Multiboot header, CPU checks, page tables, Long Mode enable
2. `boot64.S` - 64-bit entry, segment setup, kernel call
3. `kernel_main.c` - Subsystem initialization sequence

**Status**: Code complete, untested in real boot

### Memory Layout

```
0x00000000 - 0x000FFFFF  Reserved (1MB)
0x00100000 - 0x001FFFFF  Kernel code/data
0x00100000 - 0x08000000  PMM managed (1MB-128MB)
0x02000000 - 0x04000000  Kernel heap (32MB-64MB)
0xC0000000 - 0xFFFFFFFF  Kernel virtual space
```

### Kernel Components

#### Memory Management
- **PMM** (Physical Memory Manager): Buddy allocator, 4KB pages
- **VMM** (Virtual Memory Manager): Page tables, identity/higher-half mapping
- **Heap**: Slab allocator, 8B-4KB caches

**Files**: `src/kernel/mm/{pmm,vmm,kmalloc}.{c,h}`  
**Status**: Implemented, untested

#### Process Management
- **PCB**: Process control blocks with TVM context
- **Scheduler**: Round-robin, 3 priority levels, 10ms time slice
- **Context Switch**: x86-64 register save/restore in assembly

**Files**: `src/kernel/proc/{process,scheduler,context*}.{c,h,S}`  
**Status**: Implemented, untested

#### Interrupt Handling
- **IDT**: 256 entries (32 exceptions, 224 interrupts)
- **PIC**: 8259 remapping, IRQ 0-15 to INT 32-47
- **Timer**: PIT at 100Hz

**Files**: `src/kernel/{interrupt,timer,trap}.{c,h}`  
**Status**: Implemented, untested

#### File System
- **VFS**: Virtual file system layer (interface only)
- **SimpleFS**: Custom filesystem, 4KB blocks, 256 inodes
- **Block Device**: Ramdisk (4MB), abstract interface

**Files**: `src/kernel/fs/{vfs,simplefs}.{c,h}`, `src/kernel/drivers/*`  
**Status**: Partial - VFS mock, SimpleFS lacks complete device I/O

#### System Calls
- **Dispatcher**: INT 0x80 handler, 256 syscall table entries
- **Categories**: Process, memory, file, directory, signal, IPC
- **Lambda**: Ternary-specific syscalls for lambda calculus

**Files**: `src/kernel/syscall.{c,h}`  
**Status**: Dispatcher done, most handlers are stubs

#### IPC
- **Pipes**: Circular buffer, 4KB
- **Signals**: 32 signals, handler registration
- **Shared Memory**: shmget/shmat/shmdt interface
- **Semaphores**: Binary and counting

**Files**: `src/kernel/ipc.{c,h}`, `src/kernel/fd_table.{c,h}`  
**Status**: Implemented, scheduler integration incomplete

#### Drivers
- **Console**: VGA text mode (80x25, 0xB8000)
- **Keyboard**: PS/2 (mock)
- **Serial**: COM1 (mock)
- **Block**: Ramdisk functional, disk I/O mock

**Files**: `src/kernel/{console,keyboard,serial}.{c,h}`, `src/kernel/drivers/*`  
**Status**: Console done, others mock/incomplete

---

## Ternary Computing

### Core Components

#### Trit (Ternary Digit)
Balanced ternary values: -1, 0, +1

**Operations**:
- Logic: AND, OR, NOT, XOR
- Arithmetic: ADD, SUB, MUL, DIV
- Comparison: EQ, GT, LT

**Implementation**: Lookup tables for performance

**Files**: `src/kernel/trit.{c,h}`  
**Status**: Complete

#### Trit Array
Variable-length sequences of trits

**Operations**:
- Conversion to/from integers
- Bitwise operations
- Array arithmetic

**Files**: `src/kernel/trit_array.{c,h}`  
**Status**: Complete, some TODOs for floating-point

#### Ternary ALU
Arithmetic Logic Unit for ternary operations

**Operations**:
- Balanced ternary addition/subtraction
- Multiplication/division
- Shift operations

**Files**: `src/kernel/ternary_alu.{c,h}`  
**Status**: Complete

#### Ternary Memory
Memory abstraction for ternary storage

**Features**:
- Trit-addressable memory
- Conversion between binary and ternary
- Memory management primitives

**Files**: `src/kernel/ternary_memory.{c,h}`  
**Status**: Complete

### Ternary Virtual Machine (TVM)

Register-based VM executing T3-ISA instructions

**Architecture**:
- 27 general-purpose registers
- Program counter, stack pointer, flags
- Instruction decoder and executor

**Files**: `src/kernel/tvm.{c,h}`  
**Status**: Complete

### T3-ISA (Ternary Instruction Set)

**Instruction Categories**:
- Arithmetic: ADD, SUB, MUL, DIV, MOD
- Logic: AND, OR, NOT, XOR, NAND
- Memory: LOAD, STORE, PUSH, POP
- Control: JMP, JZ, CALL, RET, NOP
- System: SYSCALL, HALT

**Encoding**: 27 opcodes (3^3)

**Files**: `src/kernel/t3_isa.{c,h}`  
**Status**: Complete

### Lambda Calculus Engine

Lambda calculus evaluator integrated with TVM

**Features**:
- Lambda term representation (VAR, ABS, APP)
- Beta reduction
- Church encoding
- Type checking (partial)

**Files**: `src/kernel/lambda_engine.{c,h}`  
**Status**: Partial - bytecode generation incomplete

### Ternary Toolchain

Development tools (not included in kernel build):
- Assembler, compiler, interpreter
- Debugger, profiler, simulator
- Optimizer, analyzer, formatter

**Files**: `src/kernel/ternary_*.{c,h}` (16 files)  
**Status**: Demo implementations, not production-ready

---

## Userspace

### Init Process
PID 1, spawns shell, reaps zombies

**File**: `src/userspace/init.c`  
**Status**: Partial implementation

### Shell
Basic REPL with built-in commands

**File**: `src/userspace/sh.c`  
**Status**: Partial implementation

### Utilities
- `ls` - List directory
- `cat` - Display file
- `echo` - Print text
- `ps` - Process list
- `kill` - Send signal

**Files**: `src/userspace/*.c`  
**Status**: Stubs, not compiled in current build

---

## Build System

### Makefile

**Targets**:
- `make` - Build kernel binary
- `make iso` - Create bootable ISO
- `make clean` - Remove build artifacts
- `make run` - Boot in QEMU (requires ISO)

**Compiled Files**: 38 source files → `bin/teros.bin` (408KB)

**Excluded**:
- Ternary toolchain (16 files)
- Lambda engine
- Networking stack
- Test files
- Serial driver

### Compiler Flags

```makefile
CFLAGS = -Wall -Wextra -Werror -std=gnu11 \
         -ffreestanding -nostdlib -m64 \
         -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
         -fno-stack-protector -O2 -g
```

**Notes**:
- No FPU/SSE in kernel (causes issues with some ternary operations)
- Freestanding environment (no standard library)
- Red zone disabled (kernel requirement)

### Linker Script

**Key Sections**:
- `.multiboot` - Multiboot header at file start
- `.text` - Kernel code at 0x100000 (1MB)
- `.rodata`, `.data`, `.bss` - Data sections
- 16KB stack

**File**: `linker.ld`

---

## Code Organization

### Directory Tree

```
teros/
├── src/
│   ├── boot/              # Bootloader (2 ASM files)
│   ├── kernel/            # Kernel core (~60 C files)
│   │   ├── mm/            # Memory management (3 files)
│   │   ├── proc/          # Process management (4 files)
│   │   ├── fs/            # File systems (2 files)
│   │   └── drivers/       # Device drivers (3 files)
│   ├── lib/
│   │   ├── libc/          # C library (~230 files)
│   │   └── teros/         # Python tools (87 files)
│   └── userspace/         # User programs (7 files)
├── tests/                 # Test suite
├── tools/                 # Build tools
├── docs/                  # Documentation
├── integrations/          # External code (reference)
├── Makefile              # Build system
└── linker.ld             # Linker script
```

### File Count

- **Kernel C files**: ~60
- **Compiled in build**: 38
- **Assembly files**: 4 (2 boot, 2 context switch)
- **Headers**: ~40
- **Test files**: ~10
- **Python files**: ~90 (development tools)

### Lines of Code

Approximate (kernel only, excluding tests and tools):
- Kernel core: ~8,000 LOC
- Ternary components: ~2,500 LOC
- Total: ~10,500 LOC

---

## Known Issues and Limitations

### Critical
1. **Never booted** - Kernel has never been tested in QEMU or real hardware
2. **Undefined functions** - Several functions called but not implemented:
   - `init_memory_map()`
   - `register_exception_handlers()`
   - Various VFS operations

### Incomplete
3. **Syscall handlers** - Most are stubs returning error codes
4. **Userspace** - Programs not compiled or integrated
5. **VFS layer** - Interface only, no real implementation
6. **Device I/O** - Ramdisk works, disk I/O is mock
7. **Networking** - Entire stack is non-functional stubs (19 TODOs)

### Ternary-Specific
8. **Floating-point** - 8 TODOs due to SSE disabled in kernel
9. **Lambda bytecode** - T3 bytecode generation incomplete
10. **Toolchain** - All ternary dev tools are demos, not production

### Design
11. **No multi-core** - Single CPU only
12. **No DMA** - All I/O is synchronous
13. **No virtual terminals** - Single console only
14. **Fixed heap** - No dynamic heap expansion

---

## TODO Summary

**Total**: 43 TODOs in codebase

**By Category**:
- Networking: 19 (TCP/IP stack - non-critical)
- Floating-point: 8 (SSE workarounds - cosmetic)
- File system: 1 (path parsing - non-critical)
- IPC: 1 (scheduler integration - works with busy-wait)
- Security: 3 (group permissions - optional)
- Debug: 11 (printf calls - cosmetic)

**Blocking Issues**: 0  
**Critical Issues**: 0

---

## Testing Status

### Build Testing
- ✅ Compiles without errors
- ✅ Links successfully
- ✅ Produces valid ELF binary
- ✅ Creates bootable ISO

### Runtime Testing
- ❌ Never booted in QEMU
- ❌ Never booted in VirtualBox
- ❌ Never tested on real hardware
- ❌ No integration tests run

### Unit Testing
- ✅ Python tests pass (trit, trit_array)
- ❌ C unit tests not integrated
- ❌ Lambda tests not run in kernel context

---

## Development Setup

### Prerequisites
- GCC 9.0+ (x86-64 cross-compiler)
- GNU Make 4.0+
- GRUB tools (grub-mkrescue, xorriso)
- QEMU 4.0+ (for testing)
- Python 3.8+ (for development tools)

### Quick Start

```bash
# Clone repository
git clone https://github.com/YOUR_REPO/teros.git
cd teros

# Build kernel
make

# Create ISO
make iso

# Run in QEMU (untested)
make run
```

### Build Output

```
bin/teros.bin  - Kernel binary (408KB)
bin/teros.iso  - Bootable ISO (~31MB)
build/         - Object files
```

---

## Documentation

### Core Docs
- `README.md` - Project overview
- `TEROS_MASTER_BLUEPRINT.md` - This file
- `docs/BUILD.md` - Build instructions
- `docs/TESTING.md` - Testing guide

### Technical Docs
- `docs/T3-ISA.md` - Ternary instruction set
- `docs/ABI.md` - Application binary interface (draft)
- `docs/SYSCALLS.md` - System call documentation (partial)
- `docs/ENVIRONMENT.md` - Development environment

---

## Project Timeline

### Completed
- Core ternary components (trit, TVM, T3-ISA)
- Memory management subsystem
- Process management and scheduling
- Basic file system structure
- Build system and compilation
- Console driver
- Interrupt handling framework

### In Progress
- Syscall implementation
- File system device I/O
- Userspace integration

### Not Started
- Actual boot testing
- Performance optimization
- Multi-process testing
- Networking implementation
- Advanced drivers

### Deferred
- SMP support
- Virtual memory paging
- Advanced filesystems
- GUI subsystem

---

## Technical Decisions

### Why Software Ternary?
No existing ternary hardware available. Software emulation allows experimentation with ternary concepts on standard x86-64 CPUs.

### Why x86-64?
- Widely available hardware
- Mature toolchains (GCC, GRUB)
- Well-documented architecture
- QEMU support for testing

### Why Multiboot?
Standard boot protocol, GRUB compatibility, simple implementation.

### Why Slab Allocator?
Efficient for fixed-size kernel allocations, reduces fragmentation.

### Why Round-Robin?
Simple to implement, fair scheduling, sufficient for initial testing.

---

## Future Work

### Short Term
1. Boot testing in QEMU
2. Fix undefined references
3. Implement critical syscalls
4. Test userspace programs

### Medium Term
5. Complete VFS layer
6. Real disk driver
7. Multi-process testing
8. Performance profiling

### Long Term
9. Network stack completion
10. Advanced filesystems
11. SMP support
12. Security hardening

---

## References

### Ternary Computing
- Knuth, TAOCP Vol. 2 - Balanced ternary section
- Brousentsov's Setun computer
- Various academic papers on ternary logic

### OS Development
- OSDev.org wiki
- Intel x86-64 Architecture Manual
- Multiboot specification
- Linux kernel source (reference)

---

## Notes

This is a research/experimental project. Code quality and completeness vary. Many components are demonstrations or placeholders. Not suitable for production use.

**Realistic Assessment**:
- **Strengths**: Interesting concept, compiles successfully, comprehensive ternary subsystem
- **Weaknesses**: Never booted, many stubs, incomplete integration, no real testing
- **Status**: Early development prototype

---

**Last Updated**: October 2025  
**Maintainers**: [Specify]  
**License**: [Specify]
