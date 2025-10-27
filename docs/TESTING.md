# TEROS Testing Guide

## Prerequisites

### Install QEMU and GRUB tools in WSL

```bash
sudo apt update
sudo apt install -y qemu-system-x86 grub-pc-bin xorriso mtools
```

## Testing Methods

### Method 1: QEMU with ISO (Recommended)

```bash
# Build ISO with GRUB
make iso

# Run with QEMU
qemu-system-x86_64 -cdrom bin/teros.iso -serial stdio -m 512M
```

### Method 2: QEMU with direct kernel loading (requires 32-bit boot stub)

```bash
# Build kernel
make

# Run with QEMU
make run
# Or manually:
# qemu-system-x86_64 -kernel bin/teros.bin -serial stdio -m 512M
```

### Method 3: Debug with GDB

```bash
# Terminal 1: Start QEMU with GDB server
make debug

# Terminal 2: Connect GDB
gdb bin/teros.bin
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
```

## QEMU Options Explained

- `-cdrom bin/teros.iso` - Boot from ISO image
- `-kernel bin/teros.bin` - Direct kernel loading (faster for development)
- `-serial stdio` - Redirect serial output to terminal
- `-m 512M` - Allocate 512MB RAM
- `-s` - Start GDB server on port 1234
- `-S` - Pause execution at start (wait for GDB)
- `-d int,cpu_reset` - Debug interrupts and CPU resets
- `-no-reboot` - Exit on reboot instead of restarting
- `-no-shutdown` - Don't exit on halt

## Verification Steps

### 1. Check Multiboot Header

```bash
# Verify multiboot magic number at offset 0x1000
hexdump -C bin/teros.bin | grep "1b ad b0 02"
```

Expected output should show `02 b0 ad 1b` (little-endian `0x1BADB002`)

### 2. Check ELF Format

```bash
# Verify 64-bit ELF
file bin/teros.bin

# Check sections
readelf -S bin/teros.bin

# Check program headers
readelf -l bin/teros.bin
```

### 3. Check Entry Point

```bash
readelf -h bin/teros.bin | grep Entry
```

Should show: `Entry point address: 0x10000c`

## Expected Boot Sequence

1. **Multiboot Loader** (GRUB or QEMU) loads kernel at `0x100000` (1MB)
2. **boot32.S** (32-bit):
   - Receives control from Multiboot
   - Checks for Long Mode support
   - Sets up identity mapping (first 2MB)
   - Enables PAE, Long Mode, and paging
   - Loads 64-bit GDT
   - Far jumps to `long_mode_start`
3. **boot64.S** (64-bit):
   - Sets up 64-bit segments
   - Configures stack
   - Calls `kernel_main`
4. **kernel_main** (C):
   - Initializes console
   - Initializes memory management
   - Initializes interrupts
   - Initializes processes and scheduler
   - Enters main kernel loop

## Common Issues

### "Cannot load x86-64 image, give a 32bit one"

**Cause:** QEMU with `-kernel` expects a pure 32-bit Multiboot kernel when using legacy Multiboot 1.

**Solution:** Use ISO method with GRUB, or ensure your kernel has proper 32→64 bit transition.

### "No multiboot header found"

**Cause:** Multiboot header not at the beginning of the `.text` section.

**Solution:** Check `linker.ld` ensures `*(.multiboot)` is first in `.text`.

### QEMU hangs at "Booting from Hard Disk..."

**Cause:** ISO not properly created or GRUB config incorrect.

**Solution:** 
```bash
make clean
make iso
```

Verify `isodir/boot/grub/grub.cfg` contains:
```
menuentry "TEROS" { multiboot /boot/teros.bin }
```

## Bochs Alternative

If QEMU doesn't work, try Bochs:

```bash
sudo apt install bochs bochs-x

# Create bochsrc.txt
cat > bochsrc.txt << EOF
megs: 512
romimage: file=/usr/share/bochs/BIOS-bochs-latest
vgaromimage: file=/usr/share/bochs/VGABIOS-lgpl-latest
ata0-master: type=cdrom, path=bin/teros.iso, status=inserted
boot: cdrom
log: bochs.log
EOF

# Run
bochs -f bochsrc.txt
```

## Ternary VM Testing

TEROS includes a Ternary Virtual Machine (TVM) for executing ternary bytecode:

### Test Ternary ALU

```c
// In kernel or test program
trit_t a = trit_create(TERNARY_POSITIVE);
trit_t b = trit_create(TERNARY_NEGATIVE);
trit_t result = trit_add(a, b);
```

### Test TVM Execution

```c
tvm_t* vm = tvm_create(65536); // 64KB memory
tvm_load_program(vm, bytecode, size);
trit_t result = tvm_run(vm, max_instructions);
tvm_destroy(vm);
```

## CI/CD Integration

For automated testing:

```bash
# GitHub Actions / GitLab CI
apt-get install -y qemu-system-x86 grub-pc-bin xorriso mtools
make clean
make iso
timeout 30 qemu-system-x86_64 -cdrom bin/teros.iso -serial stdio -display none -m 512M &
QEMU_PID=$!
sleep 10
kill $QEMU_PID || true
echo "Boot test completed"
```

## Performance Testing

```bash
# Test with KVM acceleration (if available)
qemu-system-x86_64 -enable-kvm -cdrom bin/teros.iso -serial stdio -m 512M

# Benchmark boot time
time make run

# Memory usage
qemu-system-x86_64 -cdrom bin/teros.iso -monitor stdio
(qemu) info mem
(qemu) info registers
```

## Troubleshooting

### Enable QEMU logging

```bash
qemu-system-x86_64 -kernel bin/teros.bin -serial stdio -d int,cpu_reset -D qemu.log
```

### Check assembly output

```bash
objdump -d -M intel bin/teros.bin | less
objdump -d -M intel build/boot/boot32.o | less
objdump -d -M intel build/boot/boot64.o | less
```

### Verify symbols

```bash
nm bin/teros.bin | grep -E '(kernel_main|_start|long_mode_start)'
```

Expected:
```
00000000001000XX T _start
00000000001000YY T long_mode_start
00000000001XXXXX T kernel_main
```

