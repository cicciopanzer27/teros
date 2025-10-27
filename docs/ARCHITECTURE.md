# TEROS Architecture

**Architettura del sistema operativo ternario**

---

## Overview

TEROS è strutturato in layer, dal basso (hardware) verso l'alto (applicazioni):

```
┌─────────────────────────────────────────┐
│         USER APPLICATIONS               │
├─────────────────────────────────────────┤
│         CORE UTILITIES                  │
│    (shell, ls, cat, edit, etc.)         │
├─────────────────────────────────────────┤
│         SYSTEM LIBRARIES                │
│         (libc, libm, etc.)              │
├─────────────────────────────────────────┤
│         SYSTEM CALL INTERFACE           │
├═════════════════════════════════════════┤ ← User/Kernel boundary
│              KERNEL                     │
│  ┌───────────────────────────────────┐ │
│  │  Process Management               │ │
│  │  (scheduler, context switch)      │ │
│  ├───────────────────────────────────┤ │
│  │  Memory Management                │ │
│  │  (paging, heap, virtual memory)   │ │
│  ├───────────────────────────────────┤ │
│  │  File System (VFS)                │ │
│  │  (inodes, dentries, operations)   │ │
│  ├───────────────────────────────────┤ │
│  │  Device Drivers                   │ │
│  │  (console, keyboard, disk, net)   │ │
│  ├───────────────────────────────────┤ │
│  │  Interrupt Handling               │ │
│  │  (IRQ, exceptions, syscalls)      │ │
│  └───────────────────────────────────┘ │
├─────────────────────────────────────────┤
│         TERNARY VIRTUAL MACHINE         │
│         (TVM - executes T3-ISA)         │
├─────────────────────────────────────────┤
│         BOOTLOADER (GRUB)               │
├─────────────────────────────────────────┤
│         HARDWARE (x86_64)               │
└─────────────────────────────────────────┘
```

---

## Components

### 1. Hardware Layer

**Platform:** x86_64 (inizialmente)  
**Future:** Ternary hardware nativo (quando disponibile)

**Emulation:** TEROS emula computing ternario su hardware binario usando encoding 2-bit per trit.

### 2. Bootloader

**GRUB** (GNU GRUB 2)
- Carica kernel in memoria
- Passa controllo a kernel entry point
- Fornisce memory map

**Multiboot compliant:** TEROS implementa Multiboot header per compatibilità GRUB.

### 3. Ternary Virtual Machine (TVM)

**Componenti:**
- **Registers:** 16 registri ternari (27 trits ciascuno)
- **Memory:** Spazio indirizzabile ternario
- **Instruction Decoder:** Decodifica T3-ISA opcodes
- **Execution Engine:** Esegue istruzioni

**ISA:** T3-ISA (Ternary 3-Instruction Set Architecture)
- 20+ istruzioni
- Operazioni aritmetiche, logiche, control flow
- System calls

**File:** `src/kernel/ternary_emulator.c`

### 4. Kernel

#### 4.1 Bootstrap

**File:** `src/boot/boot.S`

**Responsabilità:**
1. Setup stack
2. Clear BSS
3. Setup GDT (Global Descriptor Table)
4. Setup IDT (Interrupt Descriptor Table)
5. Enable protected mode
6. Call `kernel_main()`

#### 4.2 Memory Management

**Physical Memory Manager (PMM)**
- **File:** `src/kernel/mm/pmm.c`
- **Algoritmo:** Bitmap o Buddy System
- **API:** `alloc_page()`, `free_page()`

**Virtual Memory Manager (VMM)**
- **File:** `src/kernel/mm/vmm.c`
- **Paging:** Ternary page tables
- **API:** `map_page()`, `unmap_page()`

**Heap Allocator**
- **File:** `src/kernel/mm/kmalloc.c`
- **Algoritmo:** Slab allocator
- **API:** `kmalloc()`, `kfree()`

**Memory Layout:**
```
0x00000000 - 0x000FFFFF: Kernel Code
0x00100000 - 0x001FFFFF: Kernel Data
0x00200000 - 0x002FFFFF: Kernel Heap
0x00300000 - 0x003FFFFF: Kernel Stack
0x00400000 - 0xFFFFFFFF: User Space
```

#### 4.3 Process Management

**Process Control Block (PCB)**
- **File:** `src/kernel/proc/process.c`
- **Struttura:** PID, state, registers, memory map, FD table

**States:**
- `RUNNING` (1): Processo in esecuzione
- `READY` (0): Pronto per esecuzione
- `BLOCKED` (-1): Bloccato su I/O o wait

**Scheduler**
- **File:** `src/kernel/proc/scheduler.c`
- **Algoritmo:** Round-robin (inizialmente), poi CFS
- **Priority:** Ternaria (-1=low, 0=normal, 1=high)

**Context Switch**
- **File:** `src/kernel/proc/context.S`
- Save/restore TVM registers
- Switch page tables
- Update current process

#### 4.4 Interrupt Handling

**Interrupt Descriptor Table (IDT)**
- **File:** `src/kernel/interrupt.c`
- 256 interrupt vectors
- Hardware interrupts (IRQ 0-15)
- Software interrupts (INT 0x80 per syscalls)
- Exceptions (divide by zero, page fault, etc.)

**Timer**
- **File:** `src/kernel/timer.c`
- Programmable Interval Timer (PIT)
- Tick rate: 100 Hz
- Preemptive scheduling

#### 4.5 System Calls

**Interface:** INT 0x80 (x86) o SYSCALL instruction

**Syscall Table:**
```c
0: exit
1: fork
2: exec
3: wait
4: getpid
5: open
6: read
7: write
8: close
9: pipe
10: kill
...
```

**File:** `src/kernel/syscall.c`

#### 4.6 Inter-Process Communication (IPC)

**Pipes**
- **File:** `src/kernel/ipc/pipe.c`
- Ring buffer
- Blocking read/write

**Signals**
- **File:** `src/kernel/ipc/signal.c`
- Asynchronous notifications
- Signal handlers

**Shared Memory**
- **File:** `src/kernel/ipc/shm.c`
- Memory segments condivisi
- Synchronization via semaphores

### 5. File System

#### 5.1 Virtual File System (VFS)

**File:** `src/fs/vfs/vfs.c`

**Abstraction layer** per tutti i file systems.

**Strutture:**
- **Inode:** Rappresentazione file
- **Dentry:** Directory entry (cache)
- **File:** File descriptor

**Operations:**
- `open()`, `read()`, `write()`, `close()`
- `opendir()`, `readdir()`, `closedir()`
- `stat()`, `chmod()`, `chown()`

#### 5.2 SimpleFS

**File:** `src/fs/simplefs/simplefs.c`

**Implementazione semplice:**
- Superblock
- Inode table
- Data blocks
- Free block bitmap

**Layout:**
```
Block 0: Superblock
Block 1-N: Inode table
Block N+1-M: Data blocks
```

### 6. Device Drivers

#### 6.1 Character Devices

**Console**
- **File:** `src/drivers/char/console.c`
- VGA text mode (80x25)
- `putchar()`, `puts()`

**Keyboard**
- **File:** `src/drivers/char/keyboard.c`
- PS/2 keyboard controller
- Scancode → ASCII

**Serial**
- **File:** `src/drivers/char/serial.c`
- COM1 serial port
- Debug output

#### 6.2 Block Devices

**VirtIO Block**
- **File:** `src/drivers/block/virtio_blk.c`
- Virtual block device per QEMU
- `read_block()`, `write_block()`

#### 6.3 Network Devices

**VirtIO Net**
- **File:** `src/drivers/net/virtio_net.c`
- Virtual network device
- Packet TX/RX

### 7. System Libraries

**Libc**
- **File:** `src/lib/libc/`
- Standard C library
- `string.h`, `stdlib.h`, `stdio.h`, `unistd.h`

**POSIX Wrappers**
- System call wrappers
- POSIX-compliant API

### 8. Core Utilities

**Init**
- **File:** `src/userspace/init.c`
- PID 1 process
- Spawn shell

**Shell**
- **File:** `src/userspace/shell.c`
- Command parsing
- Built-ins: cd, exit, help
- External commands

**Commands**
- `ls`, `cat`, `cp`, `mv`, `rm`, `mkdir`, `echo`, `pwd`

---

## Data Flow Examples

### System Call Flow

```
User Program
    ↓ (call open())
Libc wrapper
    ↓ (INT 0x80 or SYSCALL)
Kernel syscall dispatcher
    ↓ (lookup syscall table)
sys_open() implementation
    ↓ (VFS layer)
File system driver
    ↓ (block device)
Disk driver
    ↓
Hardware
```

### Interrupt Flow

```
Hardware (timer tick)
    ↓
CPU (interrupt)
    ↓
IDT lookup
    ↓
Interrupt handler
    ↓
Scheduler (if timer)
    ↓
Context switch
    ↓
Resume process
```

### Process Creation (fork)

```
Parent process
    ↓ (fork() syscall)
Kernel
    ↓ (allocate new PCB)
    ↓ (copy memory)
    ↓ (copy FD table)
    ↓ (add to ready queue)
Scheduler
    ↓
Child process runs
```

---

## Ternary Specifics

### Trit Encoding

**Logical values:**
- `-1`: False/Negative
- `0`: Unknown/Neutral
- `+1`: True/Positive

**Binary encoding (2 bits per trit):**
```
-1 → 00
 0 → 01
+1 → 10
11 → unused
```

### Ternary Operations

**AND:**
```
  | -1  0  1
--+---------
-1| -1 -1 -1
 0| -1  0  0
 1| -1  0  1
```

**OR:**
```
  | -1  0  1
--+---------
-1| -1  0  1
 0|  0  0  1
 1|  1  1  1
```

**NOT:**
```
-1 → 1
 0 → 0
 1 → -1
```

### Ternary Advantages

1. **Efficiency:** 36.9% meno trits vs bits per rappresentare stesso range
2. **Sparse activation:** Neuroni a 0 = skip computation
3. **3-valued logic:** Rappresenta incertezza esplicitamente
4. **Simmetria:** Operazioni simmetriche rispetto a 0

---

## Performance Considerations

### Emulation Overhead

**Problema:** Emulare ternario su hardware binario ha overhead ~2-3x.

**Mitigazioni:**
- Lookup tables per operazioni
- Caching di risultati
- SIMD per operazioni parallele

### Future: Native Ternary Hardware

Quando hardware ternario sarà disponibile:
- Rimuovere emulation layer
- Direct execution di T3-ISA
- Performance nativa

---

## Security

### Privilege Levels

**Ring 0:** Kernel mode  
**Ring 3:** User mode

**Protection:**
- Memory protection (page tables)
- I/O protection (IOPL)
- Syscall validation

### Permissions

**File permissions:** rwxrwxrwx (POSIX-style)  
**Users/Groups:** UID/GID  
**Capabilities:** Granular permissions

---

## Extensibility

### Adding New Syscalls

1. Define syscall number in `include/kernel/syscall.h`
2. Implement handler in `src/kernel/syscall.c`
3. Add to syscall table
4. Add libc wrapper in `src/lib/libc/syscalls.c`

### Adding New Drivers

1. Create driver file in `src/drivers/`
2. Implement init/read/write/ioctl functions
3. Register driver in kernel
4. Create device node in /dev

### Adding New File Systems

1. Create FS implementation in `src/fs/`
2. Implement VFS operations
3. Register FS type
4. Mount FS

---

## Build System

**Makefile-based:**
- `make all`: Build kernel
- `make iso`: Create bootable ISO
- `make run`: Run in QEMU
- `make test`: Run tests

**Cross-compilation:** Supporto per target diversi da host

---

## Testing

**Unit tests:** Per ogni modulo  
**Integration tests:** End-to-end  
**Stress tests:** Performance e stability  

**Framework:** Custom test framework in Python

---

## Future Directions

1. **Multi-core support:** SMP scheduling
2. **GUI:** Framebuffer + window manager
3. **Networking:** Full TCP/IP stack
4. **Security:** SELinux-like MAC
5. **Performance:** JIT compilation per T3-ISA
6. **Hardware:** Port su hardware ternario nativo

---

*Documento in continuo aggiornamento*

