# TEROS Makefile
# Build system for Ternary Operating System

# Compiler and tools
CC = gcc
AS = nasm
LD = ld
AR = ar

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include

# Flags
CFLAGS = -Wall -Wextra -Werror -std=c11 \
	-ffreestanding -nostdlib -m64 \
	-mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
	-I$(SRC_DIR)/kernel \
	-I$(SRC_DIR)/kernel/mm \
	-I$(SRC_DIR)/kernel/proc \
	-I$(SRC_DIR)/kernel/fs \
	-I$(SRC_DIR)/kernel/drivers \
	-I$(SRC_DIR)/drivers/char \
	-I$(INCLUDE_DIR) \
	-g -O2
ASFLAGS = -f elf64
LDFLAGS = -nostdlib -static -z max-page-size=0x1000

# Source files
KERNEL_SRCS = $(wildcard $(SRC_DIR)/kernel/*.c)
MM_SRCS = $(wildcard $(SRC_DIR)/kernel/mm/*.c)
PROC_SRCS = $(wildcard $(SRC_DIR)/kernel/proc/*.c)
FS_SRCS = $(wildcard $(SRC_DIR)/kernel/fs/*.c)
DRIVER_SRCS = $(wildcard $(SRC_DIR)/kernel/drivers/*.c) $(wildcard $(SRC_DIR)/drivers/char/*.c)
BOOT_SRCS = $(wildcard $(SRC_DIR)/boot/*.S)
PROC_ASM_SRCS = $(wildcard $(SRC_DIR)/kernel/proc/*.S)
TOOL_SRCS = $(wildcard tools/*.c)
LIBC_SRCS = $(wildcard $(SRC_DIR)/lib/libc/*.c)

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
all: $(KERNEL_BIN) tools

# Create directories
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Compile C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile assembly files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Build tools
tools: $(T3_LINKER)

$(T3_LINKER): $(BUILD_DIR)/tools/t3_linker.o | $(BIN_DIR)
	$(CC) -o $@ $^

# Link kernel
$(KERNEL_BIN): $(ALL_OBJS) | $(BIN_DIR)
	$(LD) -T linker.ld -o $@ $^

# Create ISO
iso: $(KERNEL_BIN)
	mkdir -p isodir/boot/grub
	cp $(KERNEL_BIN) isodir/boot/teros.bin
	echo 'menuentry "TEROS" { multiboot /boot/teros.bin }' > isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_FILE) isodir
	rm -rf isodir

# Run in QEMU
run: $(KERNEL_BIN)
	qemu-system-x86_64 -kernel $(KERNEL_BIN) -serial stdio

# Run with debugging
debug: $(KERNEL_BIN)
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
	@echo "  all      - Build kernel (default)"
	@echo "  iso      - Create bootable ISO"
	@echo "  run      - Run in QEMU"
	@echo "  debug    - Run in QEMU with GDB"
	@echo "  test     - Run test suite"
	@echo "  clean    - Remove build artifacts"
	@echo "  help     - Show this help"

.PHONY: all iso run debug test test-unit test-integration test-c test_trit test_trit_array tools clean help

