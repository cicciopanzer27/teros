# TEROS Development TODO

**Obiettivo:** ISA ternaria completa + OS funzionante  
**Struttura:** Multi-livello sequenziale per AI coding  
**Approccio:** Bottom-up (foundation → kernel → userspace)

---

## LEVEL 0: FOUNDATION (Prerequisiti)

### L0.1: Trit Core
- [ ] Verificare implementazione Trit base (`src/lib/teros/core/trit.py`)
- [ ] Test completi operazioni ternarie (AND, OR, NOT, XOR, ADD, SUB, MUL)
- [ ] Benchmark performance vs binario
- [ ] Documentare lookup tables

### L0.2: TritArray
- [ ] Implementare TritArray per sequenze di trit
- [ ] Operazioni su array (slice, concat, reverse)
- [ ] Conversione da/a binary encoding efficiente
- [ ] Test con array grandi (1K+ trits)

### L0.3: Ternary Math
- [ ] Implementare aritmetica multi-trit (carry propagation)
- [ ] Division e modulo ternari
- [ ] Floating point ternario (se necessario)
- [ ] Test matematica complessa

**Output:** Foundation solida per tutto il resto

---

## LEVEL 1: ISA COMPLETA

### L1.1: T3-ISA Core Instructions
- [ ] Verificare 20 opcodes esistenti (`src/kernel/t3_isa.c`)
- [ ] Implementare istruzioni mancanti (se presenti)
- [ ] Test unitari per ogni istruzione
- [ ] Documentare semantica di ogni opcode

### L1.2: T3-ISA Extended Instructions
- [ ] Aggiungere istruzioni avanzate:
  - [ ] `SYSCALL` - System call interface
  - [ ] `IRET` - Interrupt return
  - [ ] `CLI/STI` - Disable/enable interrupts
  - [ ] `CPUID` - CPU identification
  - [ ] `RDTSC` - Read timestamp counter
- [ ] Test extended instructions
- [ ] Aggiornare assembler per nuove istruzioni

### L1.3: T3-ISA Privileged Mode
- [ ] Implementare ring levels (user/kernel)
- [ ] Protection checks su istruzioni privilegiate
- [ ] Segment/page protection
- [ ] Test privilege violations

### L1.4: T3-ISA Interrupts
- [ ] Interrupt vector table (IVT)
- [ ] Hardware interrupt handling
- [ ] Software interrupts (INT instruction)
- [ ] Exception handling (divide by zero, invalid opcode, etc.)
- [ ] Test interrupt delivery

**Output:** ISA completa e documentata

---

## LEVEL 2: VIRTUAL MACHINE

### L2.1: TVM Core
- [ ] Verificare implementazione TVM esistente
- [ ] Registri: R0-R7, PC, SP, FP, LR, CR, ACC, TMP, ZERO
- [ ] Flags: NEG, ZERO, POS
- [ ] Test fetch-decode-execute cycle

### L2.2: TVM Memory
- [ ] Implementare memoria fisica (flat array di trits)
- [ ] Load/Store con bounds checking
- [ ] Memory-mapped I/O regions
- [ ] Test accessi memoria

### L2.3: TVM Execution
- [ ] Ottimizzare execution loop
- [ ] Implementare instruction cache
- [ ] Branch prediction (semplice)
- [ ] Performance profiling

### L2.4: TVM Debugging
- [ ] Debugger interface (`src/kernel/ternary_debugger.c`)
- [ ] Breakpoints
- [ ] Single-step execution
- [ ] Register/memory inspection
- [ ] Disassembler integration

**Output:** VM funzionante e debuggabile

---

## LEVEL 3: TOOLCHAIN

### L3.1: Assembler
- [ ] Verificare assembler esistente (`src/kernel/ternary_assembler.c`)
- [ ] Parser per T3-ASM syntax
- [ ] Labels e simboli
- [ ] Macros
- [ ] Multi-pass assembly
- [ ] Test con programmi complessi

### L3.2: Linker
- [ ] Implementare linker per object files
- [ ] Symbol resolution
- [ ] Relocation
- [ ] Output: executable ternario
- [ ] Test linking multipli object files

### L3.3: Compiler Backend
- [ ] Implementare backend T3 per compiler esistente
- [ ] Code generation per T3-ISA
- [ ] Register allocation
- [ ] Optimization passes
- [ ] Test compilazione C → T3-ASM

### L3.4: Standard Library
- [ ] Implementare runtime library (crt0.o)
- [ ] Startup code
- [ ] Exit code
- [ ] System call wrappers
- [ ] Test hello world compilato

**Output:** Toolchain completa (assembler + linker + compiler)

---

## LEVEL 4: BOOTLOADER

### L4.1: Multiboot Header
- [ ] Implementare Multiboot-compliant header
- [ ] Magic number, flags, checksum
- [ ] Memory map request
- [ ] Test con GRUB

### L4.2: Boot Assembly
- [ ] Creare `src/boot/boot.S`
- [ ] Setup stack
- [ ] Clear BSS
- [ ] Call kernel_main
- [ ] Test boot in QEMU

### L4.3: Early Initialization
- [ ] Setup GDT (Global Descriptor Table)
- [ ] Setup IDT (Interrupt Descriptor Table)
- [ ] Enable protected mode (se su x86)
- [ ] Test interrupt delivery

**Output:** Kernel bootabile

---

## LEVEL 5: KERNEL CORE

### L5.1: Memory Management - Physical
- [ ] Creare `src/kernel/pmm.c` (Physical Memory Manager)
- [ ] Page frame allocator (bitmap o buddy system)
- [ ] alloc_page() / free_page()
- [ ] Memory map parsing (da bootloader)
- [ ] Test allocazione/deallocazione

### L5.2: Memory Management - Virtual
- [ ] Creare `src/kernel/vmm.c` (Virtual Memory Manager)
- [ ] Page tables ternarie
- [ ] map_page() / unmap_page()
- [ ] TLB management (se applicabile)
- [ ] Test paging

### L5.3: Memory Management - Heap
- [ ] Creare `src/kernel/kmalloc.c`
- [ ] Heap allocator (slab o dlmalloc)
- [ ] kmalloc() / kfree()
- [ ] Memory leak detection
- [ ] Test allocazioni varie dimensioni

### L5.4: Process Management - Structures
- [ ] Creare `src/kernel/process.c`
- [ ] Process Control Block (PCB)
- [ ] Process states (RUNNING, READY, BLOCKED, ZOMBIE)
- [ ] Process list (linked list o array)
- [ ] Test creazione/distruzione processi

### L5.5: Process Management - Scheduler
- [ ] Creare `src/kernel/scheduler.c`
- [ ] Algoritmo scheduling (round-robin inizialmente)
- [ ] Priority scheduling (ternario: -1, 0, +1)
- [ ] schedule() function
- [ ] Test context switch

### L5.6: Process Management - Context Switch
- [ ] Implementare context_switch() in assembly
- [ ] Save/restore registri TVM
- [ ] Save/restore stack pointer
- [ ] Test switch tra 2+ processi

### L5.7: Interrupt Handling
- [ ] Creare `src/kernel/interrupt.c`
- [ ] IRQ handlers (timer, keyboard, etc.)
- [ ] register_interrupt_handler()
- [ ] EOI (End Of Interrupt)
- [ ] Test interrupt delivery

### L5.8: System Calls
- [ ] Creare `src/kernel/syscall.c`
- [ ] System call table
- [ ] Syscall dispatcher
- [ ] Implementare syscalls base:
  - [ ] `exit()`
  - [ ] `fork()`
  - [ ] `exec()`
  - [ ] `wait()`
  - [ ] `getpid()`
- [ ] Test syscalls da userspace

### L5.9: Timer
- [ ] Creare `src/kernel/timer.c`
- [ ] Timer interrupt handler
- [ ] System uptime tracking
- [ ] Preemptive scheduling trigger
- [ ] Test timer interrupts

**Output:** Kernel multitasking funzionante

---

## LEVEL 6: DRIVERS

### L6.1: Console Driver
- [ ] Creare `src/drivers/console.c`
- [ ] VGA text mode (80x25) o framebuffer
- [ ] putchar() / puts()
- [ ] Scrolling
- [ ] Test output

### L6.2: Keyboard Driver
- [ ] Creare `src/drivers/keyboard.c`
- [ ] PS/2 keyboard controller
- [ ] Scancode → ASCII conversion
- [ ] Keyboard buffer (ring buffer)
- [ ] Test input

### L6.3: Serial Driver
- [ ] Creare `src/drivers/serial.c`
- [ ] COM1 serial port
- [ ] serial_putchar() / serial_getchar()
- [ ] Debug output via serial
- [ ] Test serial communication

### L6.4: Block Device (VirtIO)
- [ ] Creare `src/drivers/virtio_blk.c`
- [ ] VirtIO block device driver
- [ ] read_block() / write_block()
- [ ] DMA setup
- [ ] Test read/write

**Output:** I/O funzionante

---

## LEVEL 7: FILE SYSTEM

### L7.1: VFS (Virtual File System)
- [ ] Creare `src/fs/vfs.c`
- [ ] Inode structure
- [ ] Dentry (directory entry) cache
- [ ] File operations (open, read, write, close)
- [ ] Directory operations (opendir, readdir, closedir)
- [ ] Test VFS interface

### L7.2: Simple FS Implementation
- [ ] Creare `src/fs/simplefs.c`
- [ ] Superblock
- [ ] Inode table
- [ ] Data blocks
- [ ] Free block bitmap
- [ ] mkfs tool per creare filesystem
- [ ] Test create/read/write/delete files

### L7.3: Path Resolution
- [ ] Implementare path_lookup()
- [ ] Absolute paths (/path/to/file)
- [ ] Relative paths (./file, ../file)
- [ ] Symlinks (optional)
- [ ] Test path resolution

### L7.4: File Descriptor Table
- [ ] Implementare per-process FD table
- [ ] open() → allocate FD
- [ ] close() → free FD
- [ ] Standard FDs (0=stdin, 1=stdout, 2=stderr)
- [ ] Test FD management

**Output:** File system funzionante

---

## LEVEL 8: IPC (Inter-Process Communication)

### L8.1: Pipes
- [ ] Creare `src/kernel/pipe.c`
- [ ] Pipe buffer (ring buffer)
- [ ] pipe() syscall
- [ ] read/write su pipe
- [ ] Test: process A → pipe → process B

### L8.2: Signals
- [ ] Creare `src/kernel/signal.c`
- [ ] Signal delivery
- [ ] Signal handlers
- [ ] kill() syscall
- [ ] signal() syscall
- [ ] Test signal delivery

### L8.3: Shared Memory
- [ ] Creare `src/kernel/shm.c`
- [ ] shmget() / shmat() / shmdt()
- [ ] Shared memory segments
- [ ] Test inter-process data sharing

### L8.4: Semaphores
- [ ] Creare `src/kernel/sem.c`
- [ ] semget() / semop()
- [ ] P() / V() operations
- [ ] Test synchronization

**Output:** IPC completo

---

## LEVEL 9: USERSPACE LIBRARY

### L9.1: Minimal Libc
- [ ] Creare `src/lib/libc/`
- [ ] Implementare funzioni base:
  - [ ] `strlen()`, `strcpy()`, `strcmp()`
  - [ ] `memcpy()`, `memset()`, `memcmp()`
  - [ ] `malloc()`, `free()` (userspace heap)
  - [ ] `printf()`, `scanf()` (basic)
  - [ ] `exit()`, `fork()`, `exec()` (wrappers)
- [ ] Test ogni funzione

### L9.2: POSIX Compatibility
- [ ] Implementare syscall wrappers POSIX:
  - [ ] `open()`, `read()`, `write()`, `close()`
  - [ ] `fork()`, `exec()`, `wait()`
  - [ ] `pipe()`, `dup()`, `dup2()`
  - [ ] `kill()`, `signal()`
- [ ] Test compatibilità

### L9.3: Dynamic Linker (Optional)
- [ ] Creare `src/lib/ld.so`
- [ ] ELF loader
- [ ] Symbol resolution
- [ ] Lazy binding
- [ ] Test dynamic linking

**Output:** Libc funzionante

---

## LEVEL 10: CORE UTILITIES

### L10.1: Init System
- [ ] Creare `src/userspace/init.c`
- [ ] PID 1 process
- [ ] Spawn shell
- [ ] Reap zombie processes
- [ ] Test boot to init

### L10.2: Shell
- [ ] Creare `src/userspace/shell.c`
- [ ] Command parsing
- [ ] Built-in commands (cd, exit, help)
- [ ] External command execution
- [ ] Pipes (cmd1 | cmd2)
- [ ] Redirections (>, <)
- [ ] Test interactive shell

### L10.3: Core Commands
- [ ] `ls` - List directory
- [ ] `cat` - Concatenate files
- [ ] `echo` - Print text
- [ ] `cp` - Copy files
- [ ] `mv` - Move files
- [ ] `rm` - Remove files
- [ ] `mkdir` - Make directory
- [ ] `rmdir` - Remove directory
- [ ] `pwd` - Print working directory
- [ ] Test ogni comando

### L10.4: Text Editor
- [ ] Creare `src/userspace/edit.c`
- [ ] Basic text editing (insert, delete)
- [ ] Save/load files
- [ ] Navigation (arrows)
- [ ] Test editing files

**Output:** Userspace utilities funzionanti

---

## LEVEL 11: NETWORKING (Optional)

### L11.1: Network Stack - Link Layer
- [ ] Creare `src/kernel/net/ethernet.c`
- [ ] Ethernet framing
- [ ] ARP protocol
- [ ] Test ARP resolution

### L11.2: Network Stack - Network Layer
- [ ] Creare `src/kernel/net/ipv4.c`
- [ ] IPv4 packet handling
- [ ] ICMP (ping)
- [ ] Routing table
- [ ] Test ping

### L11.3: Network Stack - Transport Layer
- [ ] Creare `src/kernel/net/udp.c`
- [ ] UDP socket implementation
- [ ] sendto() / recvfrom()
- [ ] Test UDP communication

### L11.4: Network Stack - TCP (Advanced)
- [ ] Creare `src/kernel/net/tcp.c`
- [ ] TCP state machine
- [ ] Connection establishment (3-way handshake)
- [ ] Data transfer
- [ ] Connection termination
- [ ] Test TCP communication

### L11.5: Socket API
- [ ] Creare `src/kernel/net/socket.c`
- [ ] socket() / bind() / listen() / accept()
- [ ] connect() / send() / recv()
- [ ] close()
- [ ] Test socket programming

### L11.6: Network Driver
- [ ] Creare `src/drivers/virtio_net.c`
- [ ] VirtIO network driver
- [ ] Packet TX/RX
- [ ] Test network connectivity

**Output:** Networking funzionante

---

## LEVEL 12: ADVANCED FEATURES

### L12.1: Multi-threading
- [ ] Creare `src/kernel/thread.c`
- [ ] Thread structure (lightweight process)
- [ ] pthread_create() / pthread_join()
- [ ] Thread-local storage
- [ ] Test multi-threading

### L12.2: Security
- [ ] Creare `src/kernel/security.c`
- [ ] Users/Groups (UID/GID)
- [ ] File permissions (rwxrwxrwx)
- [ ] Capabilities
- [ ] Test permission checks

### L12.3: Performance
- [ ] Profiling tools
- [ ] Optimization passes
- [ ] Benchmark suite
- [ ] Performance tuning

### L12.4: Documentation
- [ ] Architecture documentation
- [ ] API documentation
- [ ] Developer guide
- [ ] User manual
- [ ] Man pages

**Output:** OS production-ready

---

## LEVEL 13: LAMBDA³ INTEGRATION

### L13.1: Lambda Engine Integration
- [ ] Portare Lambda Engine su TEROS
- [ ] System call per lambda operations
- [ ] Test lambda reduction su TEROS

### L13.2: Neural Model Integration
- [ ] Portare PyTorch model su TEROS (o reimplementare in C)
- [ ] Inference engine
- [ ] Test neural inference

### L13.3: AI Services
- [ ] Creare daemon per AI services
- [ ] IPC per AI queries
- [ ] Test AI-powered applications

**Output:** TEROS + Lambda³ integrato

---

## LEVEL 14: TESTING & VALIDATION

### L14.1: Unit Tests
- [ ] Test suite per ogni modulo
- [ ] Coverage > 80%
- [ ] Automated testing

### L14.2: Integration Tests
- [ ] End-to-end tests
- [ ] System tests
- [ ] Stress tests

### L14.3: Validation
- [ ] Conformance tests (POSIX)
- [ ] Performance benchmarks
- [ ] Security audits

**Output:** OS testato e validato

---

## LEVEL 15: DEPLOYMENT

### L15.1: Build System
- [ ] Makefile completo
- [ ] Cross-compilation support
- [ ] Dependency management

### L15.2: ISO Image
- [ ] Creare bootable ISO
- [ ] GRUB configuration
- [ ] Test boot da ISO

### L15.3: Documentation
- [ ] README completo
- [ ] Installation guide
- [ ] Troubleshooting guide

### L15.4: Release
- [ ] Version tagging
- [ ] Changelog
- [ ] Release notes

**Output:** TEROS release 1.0

---

## DEPENDENCIES GRAPH

```
L0 (Foundation)
  ↓
L1 (ISA) + L2 (VM)
  ↓
L3 (Toolchain)
  ↓
L4 (Bootloader)
  ↓
L5 (Kernel Core)
  ↓
L6 (Drivers) + L7 (File System) + L8 (IPC)
  ↓
L9 (Libc)
  ↓
L10 (Utilities)
  ↓
L11 (Networking) [Optional]
  ↓
L12 (Advanced)
  ↓
L13 (Lambda³)
  ↓
L14 (Testing)
  ↓
L15 (Deployment)
```

---

## NOTES FOR AI CODING

### Principles

1. **Bottom-up:** Completa foundation prima di upper layers
2. **Test-driven:** Scrivi test prima di implementazione
3. **Incremental:** Piccoli step verificabili
4. **Documented:** Commenta ogni funzione

### Best Practices

- **Naming:** Usa prefissi (t3_ per ISA, teros_ per OS)
- **Error handling:** Sempre check return values
- **Memory safety:** No memory leaks, use valgrind
- **Portability:** Minimize platform-specific code

### Verification

Dopo ogni level:
- [ ] Code compiles without warnings
- [ ] All tests pass
- [ ] Documentation updated
- [ ] Commit changes

### Milestones

- **L0-L3 complete:** Toolchain funzionante
- **L4-L5 complete:** Kernel boots
- **L6-L7 complete:** I/O e FS funzionanti
- **L8-L10 complete:** Userspace funzionante
- **L11-L13 complete:** Full-featured OS
- **L14-L15 complete:** Production release

---

## CURRENT STATUS

**Completed:**
- [x] L0.1: Trit Core (esistente)
- [x] L1.1: T3-ISA Core (parziale)
- [x] L2.1: TVM Core (parziale)
- [x] L3.1: Assembler (parziale)

**Next Steps:**
1. Complete L0 (Foundation)
2. Complete L1 (ISA)
3. Complete L2 (VM)
4. Start L4 (Bootloader)

**Priority:** L4 → L5 → L6 → L7 (critical path per OS bootabile)

---

*Questa TODO è progettata per essere seguita sequenzialmente da un AI coding agent. Ogni task è atomico, verificabile e ha dipendenze chiare.*

