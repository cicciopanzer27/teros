# TEROS - Ternary Operating System

**Status**: Foundation Complete, Kernel Core In Progress  
**Goal**: Complete T3-ISA + Functional OS (not just line count)  
**Last Updated**: Gennaio 2025

---

## 🎯 OBJECTIVE

Build a **working ternary operating system** with:
1. Complete T3-ISA (Ternary Instruction Set)
2. Bootable kernel with memory & process management
3. Basic userspace with shell
4. Lambda Calculus integration for formal reasoning

**NOT the primary goal**: Just reaching 500K lines

---

## 📊 CURRENT STATUS

### ✅ Completed (24%)
- Trit Core (100%) - Ternary operations
- T3-ISA (95%) - 20+ instructions
- TVM (90%) - Virtual machine
- Bootloader (100%) - Multiboot support
- Toolchain (80%) - Assembler, linker

### ⚠️ Critical Path (In Progress)
- Memory Management (80%) - PMM, VMM ✅, Heap
- Process Management (50%) - PCB, Scheduler
- Interrupts (30%) - IDT setup
- Drivers (20%) - Console, Keyboard, Timer
- File System (40%) - VFS framework

### 🔨 Recently Added
- ✅ **Init System** - First process (PID 1) implemented
- ✅ **Shell** - Basic shell with builtin commands
- ✅ **VMM** - Virtual memory manager complete

### ❌ Missing (Critical for Boot)
- IPC (Pipes, Signals, Shared Memory) - **0%**
- Context Switching - **0%**
- SimpleFS - **0%**

---

## 🏗️ ARCHITECTURE

```
┌─────────────────────────────────┐
│   Userspace                     │
│   ├── Init (PID 1) ✅           │
│   ├── Shell ✅                  │
│   └── Utilities (ls, cat, etc)  │
├─────────────────────────────────┤
│   LibC Minimal (musl-based) ✅  │
├═════════════════════════════════┤
│   Kernel                        │
│   ├── Process Management (50%)  │
│   ├── Memory Management (80%) ✅ │
│   ├── Interrupts (30%)          │
│   ├── Syscalls (80%)            │
│   ├── Drivers (20%)             │
│   └── File System (40%)         │
├─────────────────────────────────┤
│   Foundation                    │
│   ├── T3-ISA (95%) ✅           │
│   ├── TVM (90%) ✅              │
│   └── Bootloader (100%) ✅      │
└─────────────────────────────────┘
```

---

## 🚀 TO GET TO BOOTABLE OS

### Critical Path (Priority Order):
1. **Complete Memory Management** - Finish PMM/VMM implementation
2. **Complete Process Management** - Context switching + scheduler
3. **Implement Interrupts** - IDT + handlers
4. **Complete File System** - SimpleFS fully functional
5. **Add IPC** - Basic pipes and signals
6. **Minimal LibC** - Essential functions only
7. **Init System** - Bootstrap first process
8. **Basic Shell** - Command interpreter

### Estimated Work:
- **Lines**: ~50-80K lines of focused kernel code
- **Time**: 3-6 months with consistent work
- **Goal**: Boot → Login → Shell prompt

---

## 📁 PROJECT STRUCTURE

```
teros/
├── src/
│   ├── boot/           ✅ Complete
│   ├── kernel/         ⚠️ Core in progress
│   │   ├── t3_isa.c   ✅ Complete
│   │   ├── tvm.c      ✅ Complete
│   │   ├── mm/        ⚠️ 60% done
│   │   ├── proc/      ⚠️ 50% done
│   │   ├── drivers/   ⚠️ 20% done
│   │   └── fs/        ⚠️ 40% done
│   ├── lib/
│   │   └── libc/      ❌ Not started
│   └── bin/           ❌ Not started
├── tests/
└── Makefile
```

---

## 🎯 REALISTIC NEXT STEPS

1. **This Week**: Finish Memory Management + Interrupts
2. **This Month**: Complete Process Management + Basic FS
3. **This Quarter**: IPC + LibC + Init + Shell
4. **Next Quarter**: Boot to shell, basic commands work

Then we can worry about 500K lines.

---

## 🛠️ BUILD & RUN

```bash
# Build
make kernel

# Run in QEMU
qemu-system-x86_64 -kernel teros.bin -m 128M

# Run tests
pytest
```

---

## 📄 LICENSE

MIT License

---

## 👥 AUTHORS

TEROS Development Team

