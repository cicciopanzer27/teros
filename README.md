# TEROS - Ternary Operating System

**Un sistema operativo basato su logica ternaria bilanciata**

---

## 🎯 Obiettivo

TEROS è un sistema operativo sperimentale progettato per esplorare il computing ternario. Utilizza logica a 3 valori (-1, 0, +1) invece della tradizionale logica binaria (0, 1).

**Caratteristiche principali:**
- ISA ternaria custom (T3-ISA)
- Virtual Machine ternaria (TVM)
- Kernel multitasking
- File system
- Toolchain completa (assembler, linker, compiler)

---

## 📁 Struttura Progetto

```
teros/
├── src/
│   ├── boot/          # Bootloader e inizializzazione
│   ├── kernel/        # Kernel core (ISA, VM, scheduler, memory)
│   ├── drivers/       # Device drivers (console, keyboard, disk)
│   ├── fs/            # File system (VFS, SimpleFS)
│   └── lib/           # Librerie (libc, teros core)
├── include/           # Header files
├── tools/             # Build tools (assembler, linker)
├── docs/              # Documentazione
├── tests/             # Test suite
├── examples/          # Programmi esempio
├── TODO.md            # Roadmap sviluppo
├── Makefile           # Build system
└── README.md          # Questo file
```

---

## 🔧 Componenti

### T3-ISA (Ternary 3-Instruction Set Architecture)

ISA custom con 20+ istruzioni:
- **Data Movement:** LOAD, STORE
- **Arithmetic:** ADD, SUB, MUL, DIV
- **Logic:** AND, OR, NOT, XOR
- **Control Flow:** JMP, JZ, JNZ, CALL, RET
- **Stack:** PUSH, POP
- **System:** HALT, SYSCALL

**File:** `src/kernel/t3_isa.{c,h}`

### TVM (Ternary Virtual Machine)

Virtual machine che esegue codice T3-ISA:
- 16 registri ternari (R0-R7, PC, SP, FP, LR, CR, ACC, TMP, ZERO)
- Memoria ternaria
- Interrupt handling
- Debugging support

**File:** `src/kernel/ternary_emulator.{c,h}`

### Kernel

Kernel multitasking con:
- Memory management (physical, virtual, heap)
- Process management (scheduler, context switch)
- System calls
- Interrupt handling
- IPC (pipes, signals, shared memory)

**File:** `src/kernel/`

### File System

VFS (Virtual File System) con implementazione SimpleFS:
- Inodes
- Directory entries
- File operations (open, read, write, close)
- Path resolution

**File:** `src/fs/`

### Drivers

Device drivers per I/O:
- Console (VGA text mode)
- Keyboard (PS/2)
- Serial port (COM1)
- Block device (VirtIO)

**File:** `src/drivers/`

### Toolchain

Toolchain completa per sviluppo:
- **Assembler:** T3-ASM → bytecode
- **Linker:** Link object files
- **Compiler:** C → T3-ASM (backend)

**File:** `tools/`

---

## 🚀 Build & Run

### Prerequisiti

```bash
# Installare dipendenze
sudo apt-get install build-essential qemu-system-x86 nasm

# Python per tools
pip install -r requirements.txt
```

### Build

```bash
# Build completo
make all

# Build kernel only
make kernel

# Build ISO bootable
make iso
```

### Run

```bash
# Run in QEMU
make run

# Run con debugging
make debug

# Run tests
make test
```

---

## 📚 Documentazione

### Per Utenti

- **Getting Started:** `docs/getting_started.md`
- **User Manual:** `docs/user_manual.md`
- **Examples:** `examples/`

### Per Sviluppatori

- **Architecture:** `docs/architecture.md`
- **API Reference:** `docs/api/`
- **Development Guide:** `docs/development.md`
- **TODO:** `TODO.md` (roadmap completa)

---

## 🧪 Testing

```bash
# Run all tests
make test

# Run unit tests
make test-unit

# Run integration tests
make test-integration

# Coverage report
make coverage
```

---

## 📊 Stato Attuale

| Componente | Stato | Completamento |
|:-----------|:------|:--------------|
| **Trit Core** | ✅ Completo | 100% |
| **T3-ISA** | ✅ Funzionante | 95% |
| **TVM** | ✅ Funzionante | 90% |
| **Assembler** | ✅ Funzionante | 85% |
| **Kernel Bootstrap** | ⚠️ In sviluppo | 20% |
| **Memory Management** | ⚠️ In sviluppo | 60% |
| **Process Management** | ⚠️ In sviluppo | 50% |
| **File System** | ❌ Da implementare | 0% |
| **Drivers** | ❌ Da implementare | 0% |
| **Networking** | ❌ Da implementare | 0% |

**Prossimi step:** Vedi `TODO.md`

---

## 🤝 Contribuire

TEROS è un progetto sperimentale. Contributi benvenuti!

### Come contribuire

1. Fork del repository
2. Crea branch per feature (`git checkout -b feature/nome`)
3. Commit changes (`git commit -am 'Add feature'`)
4. Push al branch (`git push origin feature/nome`)
5. Apri Pull Request

### Guidelines

- Segui coding style esistente
- Aggiungi test per nuove feature
- Documenta API pubbliche
- Mantieni backward compatibility

---

## 📄 Licenza

MIT License - vedi `LICENSE` file

---

## 🙏 Riconoscimenti

- **Balanced Ternary:** Donald Knuth, Setun computer
- **OS Development:** OSDev wiki, Linux, Minix, xv6
- **Ternary Logic:** Kleene, Łukasiewicz

---

## 📞 Contatti

- **Issues:** GitHub Issues
- **Discussions:** GitHub Discussions
- **Email:** [da definire]

---

## 🗺️ Roadmap

### Fase 1: Kernel Minimale 
- [x] T3-ISA completa
- [x] TVM funzionante
- [ ] Kernel boots
- [ ] Memory management
- [ ] Process scheduler
- [ ] Basic I/O

### Fase 2: Self-Hosting 
- [ ] File system
- [ ] Compiler backend
- [ ] Libc
- [ ] Core utilities
- [ ] Build TEROS on TEROS

### Fase 3: Networking 
- [ ] TCP/IP stack
- [ ] Network drivers
- [ ] Socket API
- [ ] Network utilities

### Fase 4: Advanced 
- [ ] GUI
- [ ] Multi-threading
- [ ] Security features
- [ ] Performance optimization

**Dettagli:** Vedi `TODO.md`

---

*TEROS - Exploring the future of computing through ternary logic*

