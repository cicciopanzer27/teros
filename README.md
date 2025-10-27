# TEROS - Ternary Operating System

Experimental operating system kernel implementing ternary logic on x86-64 hardware.

## Project Status

**Build**: Compiles successfully  
**Boot**: Not tested  
**Stage**: Early development - core components in various states of completion

---

## Repository Structure

### `/src/`
Core source code

#### `/src/boot/`
Bootloader components
- `boot32.S` - 32-bit Multiboot entry, Long Mode transition - **done**
- `boot64.S` - 64-bit entry point - **done**

#### `/src/kernel/`
Kernel implementation

**Core Components**:
- `kernel_main.c/h` - Kernel entry and init sequence - **done**
- `console.c/h` - VGA text mode driver - **done**
- `interrupt.c/h` - IDT, exception/IRQ handlers - **done**
- `timer.c/h` - PIT timer driver - **done**
- `syscall.c/h` - System call dispatcher - **mock** (handlers incomplete)
- `trap.c/h` - Trap handling - **done**
- `keyboard.c/h` - PS/2 keyboard driver - **mock**
- `serial.c/h` - Serial port driver - **mock**

**Memory Management** (`mm/`):
- `pmm.c/h` - Physical memory manager (buddy allocator) - **done**
- `vmm.c/h` - Virtual memory manager (page tables) - **done**
- `kmalloc.c/h` - Kernel heap allocator (slab) - **done**
- `memory.h` - Memory subsystem interface - **done**

**Process Management** (`proc/`):
- `process.c/h` - Process control blocks - **done**
- `scheduler.c/h` - Round-robin scheduler - **done**
- `context.S` - Context switch (assembly) - **done**
- `context_switch.S` - Context switch helpers - **done**
- `x86_context.h` - x86-64 context definitions - **done**

**File System** (`fs/`):
- `vfs.c/h` - Virtual file system layer - **mock**
- `simplefs.c/h` - Simple file system implementation - **partial** (I/O incomplete)

**Drivers** (`drivers/`):
- `block_device.c/h` - Block device abstraction - **done**
- `ramdisk.c/h` - RAM disk driver (4MB) - **done**
- `disk.c/h` - Disk driver interface - **mock**

**IPC**:
- `ipc.c/h` - Inter-process communication (pipes, signals, shm) - **partial**
- `fd_table.c/h` - File descriptor table - **done**

**Security**:
- `security.c/h` - Basic security checks - **mock**

**Networking**:
- `networking.c/h` - Network stack (Ethernet, IP, TCP, UDP) - **stub** (not functional)

**Ternary Computing**:
- `trit.c/h` - Ternary digit (trit) implementation - **done**
- `trit_array.c/h` - Trit array operations - **done**
- `ternary_alu.c/h` - Ternary arithmetic logic unit - **done**
- `ternary_memory.c/h` - Ternary memory abstraction - **done**
- `tvm.c/h` - Ternary Virtual Machine - **done**
- `t3_isa.c/h` - T3 instruction set architecture - **done**

**Ternary Toolchain** (excluded from kernel build):
- `ternary_assembler.c/h` - T3 assembler - **demo**
- `ternary_compiler.c/h` - Ternary compiler - **demo**
- `ternary_interpreter.c/h` - T3 interpreter - **demo**
- `ternary_optimizer.c/h` - Code optimizer - **demo**
- `ternary_debugger.c/h` - Debugger - **demo**
- `ternary_profiler.c/h` - Profiler - **demo**
- `ternary_simulator.c/h` - Hardware simulator - **demo**
- `ternary_emulator.c/h` - Emulator - **demo**
- `ternary_analyzer.c/h` - Code analyzer - **demo**
- `ternary_disassembler.c/h` - Disassembler - **demo**
- `ternary_formatter.c/h` - Code formatter - **demo**
- `ternary_generator.c/h` - Code generator - **demo**
- `ternary_linter.c/h` - Linter - **demo**
- `ternary_system.c/h` - System interface - **demo**
- `ternary_transpiler.c/h` - Transpiler - **demo**
- `ternary_validator.c/h` - Validator - **demo**

**Lambda Calculus**:
- `lambda_engine.c/h` - Lambda calculus engine - **partial** (bytecode incomplete)

**Testing** (excluded from kernel build):
- `test_isa.c` - T3-ISA tests
- `test_isa_comprehensive.c` - Comprehensive ISA tests
- `test_lambda_engine.c` - Lambda engine tests

#### `/src/lib/`
Libraries

**C Runtime**:
- `crt0.S` - C runtime startup - **mock**

**LibC** (`libc/`):
- String functions (`string.c/h`) - **done** (basic)
- Memory functions (`memory.c/h`) - **done** (basic)
- Type functions (`ctype.c/h`) - **done**
- Standard I/O (`stdio.c/h`) - **mock**
- Printf (`printf_stub.c`) - **stub**
- Errno (`errno.c/h`) - **done**
- musl integration (`musl_*`) - **partial**

**Python Library** (`teros/`):
Development tools and scripts (not part of kernel)
- 87 Python files for development, testing, simulation

#### `/src/userspace/`
Userspace programs (not compiled in current build)
- `init.c` - Init process (PID 1) - **partial**
- `sh.c` - Shell - **partial**
- `ls.c` - List directory - **stub**
- `cat.c` - Concatenate files - **stub**
- `echo.c` - Echo arguments - **stub**
- `ps.c` - Process status - **stub**
- `kill.c` - Send signals - **stub**

#### `/src/drivers/char/`
Duplicate console driver (not used)
- `console.c/h` - **duplicate** of `src/kernel/console.c`

### `/tests/`
Test suite
- `framework.py` - Test framework - **done**
- `lambda3/` - Lambda calculus tests - **done** (4 files)
- `unit/` - Unit tests - **partial** (3 files)
- `test_phase1_integration.py` - Integration tests - **done**
- C test files (`.c`) - **not integrated**

### `/tools/`
Development tools
- `t3_linker.c/h` - T3 linker - **mock**

### `/docs/`
Documentation
- `ABI.md` - Application Binary Interface - **draft**
- `BUILD.md` - Build instructions - **done**
- `ENVIRONMENT.md` - Development environment - **done**
- `SYSCALLS.md` - System call documentation - **partial**
- `T3-ISA.md` - T3 instruction set documentation - **done**
- `TESTING.md` - Testing guide - **partial**

### `/integrations/`
External project integrations (not used in build)
- `lwip/` - Lightweight TCP/IP stack - **reference only**
- `musl/` - musl libc - **reference only**
- `serenity/` - SerenityOS code - **reference only**

### Root Files

**Build System**:
- `Makefile` - Build configuration - **done**
- `linker.ld` - Linker script - **done**
- `requirements.txt` - Python dependencies - **done**
- `pytest.ini` - Pytest configuration - **done**

**Scripts**:
- `RUN_TEROS.sh` - QEMU launcher script - **done**
- `run_teros_windows_qemu.sh` - Windows QEMU script - **done**
- `integrate_musl.ps1` - musl integration script - **incomplete**

**Docker**:
- `Dockerfile` - Container definition - **done**
- `docker-compose.yml` - Docker compose config - **done**

**Documentation**:
- `README.md` - This file
- `TEROS_MASTER_BLUEPRINT.md` - Architecture documentation - **outdated**

**Build Artifacts** (generated):
- `bin/` - Compiled binaries (teros.bin, teros.iso)
- `build/` - Object files
- `isodir/` - ISO staging directory
- `.log` files - Build logs

---

## Build Status

### Successful Compilation

```bash
make
```

**Output**: `bin/teros.bin` (408KB ELF 64-bit executable)

### Components Compiled

**Kernel Core** (17 files):
- console, fd_table, interrupt, ipc, kernel_main, keyboard, security
- syscall, t3_isa, ternary_alu, ternary_memory, timer, trap
- trit_array, trit, tvm

**Memory Management** (3 files):
- kmalloc, pmm, vmm

**Process Management** (4 files):
- process, scheduler, context (2 ASM files)

**File System** (2 files):
- simplefs, vfs

**Drivers** (3 files):
- block_device, disk, ramdisk

**LibC** (7 files):
- ctype, errno, memory, printf_stub, stdlib, string, syscalls

**Boot** (2 ASM files):
- boot32, boot64

**Total**: 38 compiled files

### Components Excluded from Build

Intentionally excluded (use `printf` or external dependencies):
- All `ternary_*` tools (16 files)
- `lambda_engine.c`
- `networking.c`
- `serial.c`
- Test files (3 files)

### Undefined References

Multiple functions referenced but not implemented:
- `init_memory_map()` - Memory map initialization
- `register_exception_handlers()` - Exception handler registration
- Various VFS operations
- Userspace program entry points

### TODO Count

43 TODO/FIXME comments across codebase:
- Networking: 19 (TCP/IP stack stubs)
- Floating-point: 8 (SSE disabled workarounds)
- File system: 1 (path parsing)
- IPC: 1 (scheduler integration)
- Other: 14 (misc enhancements)

---

## Build Instructions

### Prerequisites
- GCC 9.0+ with x86-64 support
- GNU Make 4.0+
- GRUB tools (for ISO)

### Commands

```bash
# Build kernel
make

# Create bootable ISO
make iso

# Clean
make clean
```

### Build Flags

```makefile
CFLAGS = -Wall -Wextra -Werror -std=gnu11 \
         -ffreestanding -nostdlib -m64 \
         -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
         -fno-stack-protector -O2 -g
```

---

## Testing Status

### Compiled: ✅ Yes
- Kernel compiles without errors
- Linker produces valid ELF binary

### Booted: ❌ No
- Not tested in QEMU
- Not tested in VirtualBox
- Boot sequence unverified

### Unit Tests: Partial
- Python tests run successfully
- C tests not integrated in build

---

## Known Limitations

1. **Boot Untested** - Kernel has never been booted
2. **Incomplete Syscalls** - Many syscall handlers are stubs
3. **No Userspace** - Userspace programs not compiled
4. **Mock Drivers** - Keyboard, serial, disk are mocks
5. **Incomplete Filesystem** - Device I/O not fully implemented
6. **No Networking** - Network stack is non-functional stubs
7. **Lambda Engine** - Bytecode generation incomplete
8. **Missing Functions** - Several referenced but undefined

---

## Code Metrics

- **Total Source Files**: ~100 C/ASM files
- **Compiled Files**: 38 files
- **Lines of Code**: ~10,000 (kernel only)
- **Binary Size**: 408KB
- **Architecture**: x86-64 Long Mode
- **Boot Protocol**: Multiboot

---

## License

[Specify license]

## Authors

[Specify authors]
