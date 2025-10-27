# TEROS Build Guide

## Prerequisites

### Required Toolchain

- **GCC**: 9.0+ (with x86-64 support)
- **NASM**: 2.14+
- **GNU LD**: 2.34+
- **GNU Make**: 4.0+
- **QEMU**: 4.0+ (for testing)

### Installation

#### Windows (WSL2 Recommended)

```powershell
# Install WSL2
wsl --install -d Ubuntu

# Inside WSL2 Ubuntu
sudo apt update
sudo apt install build-essential nasm gcc-multilib qemu-system-x86
```

#### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential nasm gcc-multilib qemu-system-x86
```

#### macOS

```bash
brew install nasm gcc qemu
```

### Verification

After installation, verify toolchain:

```bash
gcc --version      # Should be 9.0+
nasm --version     # Should be 2.14+
ld --version       # Should be 2.34+
make --version     # Should be 4.0+
qemu-system-x86_64 --version  # Should be 4.0+
```

## Building TEROS

### Clean Build

```bash
# Remove all build artifacts
make clean

# Build kernel
make

# Expected output:
# - build/boot/boot.o
# - build/kernel/*.o
# - build/kernel/mm/*.o
# - build/kernel/proc/*.o
# - build/kernel/fs/*.o
# - build/kernel/drivers/*.o
# - bin/teros.bin (final kernel binary)
```

### Build Output

Successful build produces:

- **bin/teros.bin**: Bootable kernel binary (ELF format)
- **build/**: Directory with all intermediate object files

Expected size: `bin/teros.bin` should be > 100KB

### Verify Binary

```bash
# Check file type
file bin/teros.bin
# Output: bin/teros.bin: ELF 64-bit LSB executable, x86-64...

# Check sections
objdump -h bin/teros.bin
# Should show: .text, .rodata, .data, .bss sections

# Check entry point
readelf -h bin/teros.bin | grep Entry
# Output: Entry point address: 0x100000 (1MB)
```

## Build System Details

### Makefile Structure

The Makefile is organized into:

1. **Compiler flags** (CFLAGS):
   - `-ffreestanding`: No hosted environment
   - `-nostdlib`: No standard library linking
   - `-m64`: Force 64-bit compilation
   - `-mno-red-zone`: Kernel requirement (no red zone optimization)
   - `-mno-mmx -mno-sse -mno-sse2`: Disable FPU instructions
   - `-Wall -Wextra -Werror`: Strict error checking

2. **Source organization**:
   - `src/boot/*.S`: Assembly bootloader
   - `src/kernel/*.c`: Core kernel
   - `src/kernel/mm/*.c`: Memory management
   - `src/kernel/proc/*.c`: Process management
   - `src/kernel/fs/*.c`: File system
   - `src/kernel/drivers/*.c`: Device drivers
   - `src/lib/libc/*.c`: C library

3. **Linker script** (`linker.ld`):
   - Entry point: `_start` (from boot.S)
   - Load address: 1MB (0x100000)
   - Sections: .text, .rodata, .data, .bss

### Build Targets

```bash
make              # Build kernel (default)
make clean        # Remove build artifacts
make help         # Show available targets
make iso          # Create bootable ISO (requires GRUB)
make run          # Build and run in QEMU
make debug        # Build and run with GDB debugging
make test         # Run test suite (Python)
make test-c       # Run C unit tests
```

## Running TEROS

### Basic QEMU Test

```bash
# Build kernel
make

# Run in QEMU
qemu-system-x86_64 -kernel bin/teros.bin -m 128M -serial stdio
```

**Expected behavior**:
- QEMU window opens
- Console shows boot messages
- Kernel initializes
- System enters main loop or shell prompt

### QEMU with Serial Output

```bash
qemu-system-x86_64 -kernel bin/teros.bin -m 128M -serial stdio -nographic
```

All console output will appear in terminal.

### QEMU with Debugging

Terminal 1:
```bash
qemu-system-x86_64 -kernel bin/teros.bin -m 128M -s -S
```

Terminal 2:
```bash
gdb bin/teros.bin
(gdb) target remote :1234
(gdb) break kernel_main
(gdb) continue
```

## Troubleshooting

### Common Build Errors

#### Error: "gcc: command not found"

**Solution**: Install GCC
```bash
# Ubuntu/Debian
sudo apt install build-essential

# macOS
brew install gcc
```

#### Error: "nasm: command not found"

**Solution**: Install NASM
```bash
# Ubuntu/Debian
sudo apt install nasm

# macOS
brew install nasm
```

#### Error: "ld: cannot find -lstdc++"

**Solution**: This is expected. TEROS is freestanding and doesn't use standard library.
Verify `-nostdlib` flag is in CFLAGS.

#### Error: "undefined reference to `memset`"

**Solution**: Implement `memset` in kernel or libc. Required by GCC for struct initialization.

#### Error: Multiple definition of `console_putchar`

**Solution**: Function defined in multiple files. Check:
1. `src/kernel/console.c`
2. `src/drivers/char/console.c`

Remove duplicate definition.

#### Error: "relocation truncated to fit"

**Solution**: Increase memory allocation or fix linker script.
Check `linker.ld` for proper memory layout.

### QEMU Issues

#### QEMU shows "invalid kernel"

**Causes**:
1. Multiboot header missing or incorrect
2. Entry point not at expected address
3. Binary format incorrect

**Solution**:
```bash
# Verify multiboot header
hexdump -C bin/teros.bin | head -20
# Look for: 0x1BADB002 (magic number)

# Verify entry point
readelf -h bin/teros.bin | grep Entry
# Should be: 0x100000 or similar
```

#### QEMU triple faults immediately

**Causes**:
1. Stack not initialized in boot.S
2. Paging enabled incorrectly
3. Interrupt triggered before IDT setup

**Solution**: Add debug output early in `kernel_main()`:
```c
void kernel_main(void) {
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    vga[0] = 0x0F48;  // Print 'H' to screen
    while(1) { asm("hlt"); }
}
```

#### No output in QEMU

**Solution**: Use VGA text mode for initial output:
```c
void console_puts(const char* str) {
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    for (int i = 0; str[i]; i++) {
        vga[i] = 0x0F00 | str[i];  // White on black
    }
}
```

### Linking Errors

#### Error: "undefined reference to `_start`"

**Solution**: `_start` must be defined in `boot.S`:
```asm
.global _start
_start:
    # ... boot code
```

#### Error: "section .text will not fit in region"

**Solution**: Kernel too large for memory layout.
1. Check `linker.ld` memory regions
2. Reduce kernel size (remove unused code)
3. Increase available memory

## Build Configuration

### Custom CFLAGS

Edit `Makefile` to add custom flags:

```makefile
CFLAGS += -DDEBUG_MODE    # Enable debug logging
CFLAGS += -DVERBOSE       # Verbose output
CFLAGS += -O0             # No optimization (easier debugging)
```

### Cross-Compilation

For cross-platform builds:

```bash
# Build on Linux for bare-metal x86-64
CC=x86_64-elf-gcc make

# With custom toolchain
CC=/opt/cross/bin/x86_64-elf-gcc make
```

## Next Steps

After successful build:

1. **Test in QEMU**: Verify kernel boots
2. **Check console output**: Verify initialization messages
3. **Test system calls**: Verify basic operations work
4. **Run test suite**: `make test`

## Additional Resources

- **Linker Script**: `linker.ld` - Memory layout configuration
- **Boot Code**: `src/boot/boot.S` - Assembly bootloader
- **Kernel Entry**: `src/kernel/kernel_main.c` - Kernel initialization
- **Environment Setup**: `docs/ENVIRONMENT.md` - Toolchain details

---

**Last Updated**: 2025-01-27  
**TEROS Version**: 0.7.0-dev  
**Status**: Build system corrected, ready for compilation

