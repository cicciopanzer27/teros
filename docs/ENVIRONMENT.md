# TEROS Development Environment

## Current Status (2025-01-27)

### Toolchain Status

**⚠️ TOOLCHAIN NOT YET INSTALLED**

The following tools are required but not currently available:

- **GCC**: Not found
- **NASM**: Not found  
- **GNU LD**: Not found
- **GNU Make**: Not found
- **QEMU**: Not found

### Required Toolchain

#### For Windows Development

**Option 1: WSL2 (Recommended)**
```powershell
# Install WSL2
wsl --install -d Ubuntu

# Inside WSL2
sudo apt update
sudo apt install build-essential nasm gcc-multilib qemu-system-x86
```

**Option 2: MinGW-w64**
```powershell
# Download from: https://www.mingw-w64.org/
# Install to: C:\mingw-w64
# Add to PATH: C:\mingw-w64\bin

# Download NASM from: https://www.nasm.us/
# Download QEMU from: https://www.qemu.org/download/#windows
```

#### For Linux Development

```bash
sudo apt install build-essential nasm gcc-multilib qemu-system-x86
```

#### For macOS Development

```bash
brew install nasm gcc qemu
```

### Minimum Version Requirements

- **GCC**: 9.0+ (with x86-64 support)
- **NASM**: 2.14+
- **GNU LD**: 2.34+
- **GNU Make**: 4.0+
- **QEMU**: 4.0+

### Verification Commands

After installation, verify with:

```bash
gcc --version
nasm --version
ld --version
make --version
qemu-system-x86_64 --version
```

### Next Steps

1. Install toolchain using one of the methods above
2. Run verification commands
3. Update this document with installed versions
4. Proceed with `make` build

## Build System

Once toolchain is installed:

```bash
# Clean build
make clean

# Build kernel
make

# Test in QEMU
qemu-system-x86_64 -kernel bin/teros.bin -m 128M -serial stdio
```

---

**Document created**: 2025-01-27  
**Last updated**: 2025-01-27  
**Status**: Toolchain installation required

