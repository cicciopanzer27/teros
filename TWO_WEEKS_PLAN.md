# TEROS - 2 Week Sprint Plan
**Goal:** Get bootable kernel running with minimal userspace

## Week 1: Kernel Core (Days 1-7)

### Day 1-2: Memory Management (CRITICAL)
- [x] Complete PMM implementation
- [ ] Implement basic VMM (page tables)
- [ ] Complete kmalloc heap allocator
- [ ] Test memory allocation/deallocation

### Day 3-4: Process Management  
- [ ] Complete context switching
- [ ] Finish scheduler (round-robin)
- [ ] Implement basic process creation
- [ ] Test 2+ processes running

### Day 5: Interrupts & Syscalls
- [ ] Set up IDT
- [ ] Implement IRQ handlers
- [ ] Create syscall interface
- [ ] Test interrupt handling

### Day 6-7: I/O Drivers
- [ ] Complete console driver
- [ ] Keyboard driver (basic)
- [ ] Serial port (optional)
- [ ] Test input/output

## Week 2: Userspace (Days 8-14)

### Day 8-9: Minimal Libc
- [ ] Basic stdio (printf, puts)
- [ ] Memory (malloc, free)
- [ ] String functions (strlen, strcpy)
- [ ] Syscall wrappers

### Day 10-11: File System (Minimal)
- [ ] In-memory VFS
- [ ] Simple file operations
- [ ] Directory support (minimal)
- [ ] Test file I/O

### Day 12-13: Init & Shell
- [ ] Init process
- [ ] Basic shell (commands: ls, cat, echo)
- [ ] Command execution
- [ ] Test shell

### Day 14: Integration & Testing
- [ ] End-to-end testing
- [ ] Bug fixes
- [ ] Documentation
- [ ] Demo

## Strategy: FAST & MINIMAL
- Use simplest algorithms (not optimized)
- Skip security features
- No networking
- Risk bypass (basic error handling)
- Copy-paste from Linux/xv6 where possible
- Test in QEMU only

## Success Criteria (End of Week 2)
✅ Kernel boots in QEMU
✅ Can run 2 processes  
✅ Console I/O works
✅ Basic shell functional
✅ Can execute simple commands

---

**STARTING NOW**

