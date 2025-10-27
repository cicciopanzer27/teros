# TEROS Makefile
# Build system for Ternary Operating System

# Compiler and tools
CC = gcc
AS = gcc
LD = ld
AR = ar

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include

# Flags
CFLAGS = -Wall -Wextra -Werror -std=gnu11 \
	-ffreestanding -nostdlib -m64 \
	-mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
	-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 \
	-fno-stack-protector \
	-I$(SRC_DIR)/kernel \
	-I$(SRC_DIR)/kernel/mm \
	-I$(SRC_DIR)/kernel/proc \
	-I$(SRC_DIR)/kernel/fs \
	-I$(SRC_DIR)/kernel/drivers \
	-I$(SRC_DIR)/drivers/char \
	-I$(INCLUDE_DIR) \
	-g -O2
ASFLAGS = -m32 -c -nostdlib
ASFLAGS_64 = -m64 -c -nostdlib
LDFLAGS = -nostdlib -static -z max-page-size=0x1000

# Source files
# Exclude test files, development tools, and files with printf/external deps from kernel build
# All ternary_* tools are excluded as they use printf in debug/print functions.
# Only CORE ternary components are included: ternary_alu, ternary_memory, trit, trit_array, tvm, t3_isa
KERNEL_SRCS = $(filter-out \
	$(SRC_DIR)/kernel/test_isa.c \
	$(SRC_DIR)/kernel/test_isa_comprehensive.c \
	$(SRC_DIR)/kernel/test_lambda_engine.c \
	$(SRC_DIR)/kernel/lambda_engine.c \
	$(SRC_DIR)/kernel/ternary_analyzer.c \
	$(SRC_DIR)/kernel/ternary_assembler.c \
	$(SRC_DIR)/kernel/ternary_compiler.c \
	$(SRC_DIR)/kernel/ternary_debugger.c \
	$(SRC_DIR)/kernel/ternary_disassembler.c \
	$(SRC_DIR)/kernel/ternary_emulator.c \
	$(SRC_DIR)/kernel/ternary_formatter.c \
	$(SRC_DIR)/kernel/ternary_generator.c \
	$(SRC_DIR)/kernel/ternary_interpreter.c \
	$(SRC_DIR)/kernel/ternary_linter.c \
	$(SRC_DIR)/kernel/ternary_optimizer.c \
	$(SRC_DIR)/kernel/ternary_profiler.c \
	$(SRC_DIR)/kernel/ternary_system.c \
	$(SRC_DIR)/kernel/ternary_transpiler.c \
	$(SRC_DIR)/kernel/ternary_simulator.c \
	$(SRC_DIR)/kernel/ternary_validator.c \
	$(SRC_DIR)/kernel/networking.c \
	$(SRC_DIR)/kernel/serial.c, \
	$(wildcard $(SRC_DIR)/kernel/*.c))
MM_SRCS = $(wildcard $(SRC_DIR)/kernel/mm/*.c)
PROC_SRCS = $(wildcard $(SRC_DIR)/kernel/proc/*.c)
FS_SRCS = $(wildcard $(SRC_DIR)/kernel/fs/*.c)
# Exclude src/drivers/char/console.c (duplicate of src/kernel/console.c)
DRIVER_SRCS = $(wildcard $(SRC_DIR)/kernel/drivers/*.c) $(filter-out $(SRC_DIR)/drivers/char/console.c, $(wildcard $(SRC_DIR)/drivers/char/*.c))
# Boot sources - order matters: 32-bit first, then 64-bit
BOOT_SRCS = $(SRC_DIR)/boot/boot32.S $(SRC_DIR)/boot/boot64.S
PROC_ASM_SRCS = $(wildcard $(SRC_DIR)/kernel/proc/*.S)
TOOL_SRCS = $(wildcard tools/*.c)
# LibC sources (exclude files not compatible with kernel environment):
# - math.c: floating-point not allowed in kernel with -mno-sse
# - musl_atoi.c: requires __ctype_b_loc (glibc internal) not available in kernel
# - musl_strtol.c: requires internal musl headers not available
# - stdarg.c: stdarg.h is a compiler builtin, no .c file needed
# - stdio.c, stdio_expanded.c: userspace libraries, require syscalls not available in kernel
LIBC_SRCS = $(filter-out \
	$(SRC_DIR)/lib/libc/math.c \
	$(SRC_DIR)/lib/libc/musl_atoi.c \
	$(SRC_DIR)/lib/libc/musl_strtol.c \
	$(SRC_DIR)/lib/libc/stdarg.c \
	$(SRC_DIR)/lib/libc/stdio.c \
	$(SRC_DIR)/lib/libc/stdio_expanded.c, \
	$(wildcard $(SRC_DIR)/lib/libc/*.c))

# Object files
KERNEL_OBJS = $(KERNEL_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
MM_OBJS = $(MM_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
PROC_OBJS = $(PROC_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
FS_OBJS = $(FS_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DRIVER_OBJS = $(DRIVER_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
BOOT_OBJS = $(BOOT_SRCS:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)
PROC_ASM_OBJS = $(PROC_ASM_SRCS:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)
TOOL_OBJS = $(TOOL_SRCS:tools/%.c=$(BUILD_DIR)/tools/%.o)
LIBC_OBJS = $(LIBC_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

ALL_OBJS = $(BOOT_OBJS) $(KERNEL_OBJS) $(MM_OBJS) $(PROC_OBJS) $(PROC_ASM_OBJS) \
	   $(FS_OBJS) $(DRIVER_OBJS) $(LIBC_OBJS)

# Tools
T3_LINKER = $(BIN_DIR)/t3_linker

# Output
KERNEL_BIN = $(BIN_DIR)/teros.bin
ISO_FILE = $(BIN_DIR)/teros.iso

# Default target
all: $(KERNEL_BIN)

# Create directories
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Compile C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile boot32.S (32-bit), then convert to elf64 format for linking
$(BUILD_DIR)/boot/boot32.o: $(SRC_DIR)/boot/boot32.S | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@.elf32
	objcopy -O elf64-x86-64 $@.elf32 $@
	@rm -f $@.elf32

# Compile boot64.S (64-bit)
$(BUILD_DIR)/boot/boot64.o: $(SRC_DIR)/boot/boot64.S | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS_64) $< -o $@

# Compile assembly files - context.S and others (64-bit kernel)
$(BUILD_DIR)/kernel/proc/context.o: $(SRC_DIR)/kernel/proc/context.S | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS_64) $< -o $@

# Default assembly rule (use 64-bit for kernel code)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS_64) $< -o $@

# Build tools (commented out - tools/t3_linker.c doesn't exist yet)
# tools: $(T3_LINKER)
# $(T3_LINKER): $(BUILD_DIR)/tools/t3_linker.o | $(BIN_DIR)
#	$(CC) -o $@ $^

# Link kernel
$(KERNEL_BIN): $(ALL_OBJS) | $(BIN_DIR)
	$(LD) -T linker.ld -o $@ $^

# Create ISO
iso: $(KERNEL_BIN)
	mkdir -p isodir/boot/grub
	cp $(KERNEL_BIN) isodir/boot/teros.bin
	echo 'menuentry "TEROS" { multiboot /boot/teros.bin }' > isodir/boot/grub/grub.cfg
	/usr/bin/grub-mkrescue -o $(ISO_FILE) isodir
	rm -rf isodir

# Run in QEMU with ISO (recommended - uses proper boot sequence)
run: iso
	@echo "Avvio TEROS con QEMU..."
	@echo "Per uscire: Ctrl+A, poi X"
	@echo ""
	qemu-system-x86_64 -cdrom $(ISO_FILE) -serial stdio -m 512M -display none

# Run with direct kernel loading (may have issues with Multiboot)
run-kernel: $(KERNEL_BIN)
	qemu-system-x86_64 -kernel $(KERNEL_BIN) -serial stdio -m 512M

# Run with debugging
debug: iso
	qemu-system-x86_64 -cdrom $(ISO_FILE) -serial stdio -m 512M -s -S

# Debug with direct kernel loading
debug-kernel: $(KERNEL_BIN)
	qemu-system-x86_64 -kernel $(KERNEL_BIN) -serial stdio -s -S

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) isodir

# Tests
test:
	@echo "Running tests..."
	@python3 -m pytest tests/ -v

test-unit:
	@echo "Running unit tests..."
	@python3 -m pytest tests/unit/ -v

test-integration:
	@echo "Running integration tests..."
	@python3 -m pytest tests/integration/ -v

# Test C modules
test-c: test_trit test_trit_array

test_trit: $(BUILD_DIR)/test_trit
	./$(BUILD_DIR)/test_trit

test_trit_array: $(BUILD_DIR)/test_trit_array
	./$(BUILD_DIR)/test_trit_array

# C Test executables
$(BUILD_DIR)/test_trit: tests/unit/test_trit.c src/kernel/trit.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR)/kernel -o $@ $^ -lm

$(BUILD_DIR)/test_trit_array: tests/unit/test_trit_array.c src/kernel/trit.c src/kernel/trit_array.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR)/kernel -o $@ $^ -lm

# Help
help:
	@echo "TEROS Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all         - Build kernel (default)"
	@echo "  iso         - Create bootable ISO"
	@echo "  run         - Run TEROS in QEMU (from ISO)"
	@echo "  run-kernel  - Run TEROS directly (may have issues)"
	@echo "  debug       - Run in QEMU with GDB (from ISO)"
	@echo "  debug-kernel- Debug with direct kernel loading"
	@echo "  test        - Run test suite"
	@echo "  clean       - Remove build artifacts"
	@echo "  help        - Show this help"

.PHONY: all iso run run-kernel debug debug-kernel test test-unit test-integration test-c test_trit test_trit_array tools clean help

