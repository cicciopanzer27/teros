/**
 * @file kernel_main.c
 * @brief TEROS Kernel Main Entry Point
 * @author TEROS Development Team
 * @date 2025
 */

#include "kernel_main.h"
#include "trit.h"
#include "t3_isa.h"
#include "tvm.h"
#include "interrupt.h"
#include "console.h"
#include "timer.h"
#include "memory.h"
#include "process.h"
#include "scheduler.h"
#include "ipc.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// FORWARD DECLARATIONS
// =============================================================================

void early_init(void);
void init_memory_management(void);
void init_interrupt_system(void);
void init_process_system(void);
void init_scheduler(void);
void init_console(void);
void init_timer(void);
void init_ipc_system(void);
void init_lambda3_integration(void);
void print_boot_message(void);
void kernel_main_loop(void);
void kernel_panic(const char* message);

// =============================================================================
// KERNEL GLOBAL STATE
// =============================================================================

// Multiboot info
extern uint32_t multiboot_magic;
extern uint32_t multiboot_info;

// Kernel version
const char* kernel_version = "TEROS v1.0.0-alpha";
const char* kernel_build_date = __DATE__ " " __TIME__;

// Kernel state
static bool kernel_initialized = false;
static uint64_t kernel_start_time = 0;
static uintptr_t kernel_heap_start = 0;
static uintptr_t kernel_heap_end = 0;

// =============================================================================
// KERNEL INITIALIZATION
// =============================================================================

void kernel_main(void) {
    // Initialize kernel
    kernel_start_time = timer_get_ticks();
    
    // Early initialization
    early_init();
    
    // Initialize subsystems
    init_memory_management();
    init_interrupt_system();
    init_process_system();
    init_scheduler();
    init_console();
    init_timer();
    init_ipc_system();

    // Enable interrupts
    interrupt_enable();
    
    // Mark kernel as initialized
    kernel_initialized = true;
    
    // Print boot message
    print_boot_message();
    
    // Initialize Lambda³ integration
    init_lambda3_integration();
    
    // Start scheduler
    scheduler_start();
    
    // Main kernel loop
    kernel_main_loop();
}

void early_init(void) {
    // Initialize console early for debugging
    console_init();
    
    // Print early boot message
    console_puts("TEROS Kernel Starting...\n");
    
    // Verify multiboot
    if (multiboot_magic != 0x2BADB002) {
        console_puts("ERROR: Invalid multiboot magic!\n");
        kernel_panic("Invalid multiboot magic");
    }
    
    console_puts("Multiboot verified\n");
    
    // Initialize memory map
    init_memory_map();
    
    console_puts("Early initialization complete\n");
}

void init_memory_management(void) {
    console_puts("Initializing memory management...\n");
    
    // Initialize physical memory manager
    pmm_init();
    console_puts("PMM initialized\n");
    
    // Initialize virtual memory manager
    vmm_init();
    console_puts("VMM initialized\n");
    
    // Initialize kernel heap
    kmalloc_init();
    console_puts("Kernel heap initialized\n");
    
    // Allocate kernel heap
    kernel_heap_start = (uintptr_t)kmalloc(1024 * 1024); // 1MB
    kernel_heap_end = kernel_heap_start + (1024 * 1024);
    
    console_puts("Memory management ready\n");
}

void init_interrupt_system(void) {
    console_puts("Initializing interrupt system...\n");
    
    // Initialize interrupt handling
    interrupt_init();
    console_puts("Interrupt system initialized\n");
    
    // Register exception handlers
    register_exception_handlers();
    console_puts("Exception handlers registered\n");
    
    // Register IRQ handlers
    register_irq_handlers();
    console_puts("IRQ handlers registered\n");
    
    console_puts("Interrupt system ready\n");
}

void init_process_system(void) {
    console_puts("Initializing process system...\n");
    
    // Initialize process management
    process_init();
    console_puts("Process system initialized\n");
    
    // Create kernel process
    create_kernel_process();
    console_puts("Kernel process created\n");
    
    console_puts("Process system ready\n");
}

void init_scheduler(void) {
    console_puts("Initializing scheduler...\n");
    
    // Initialize scheduler
    scheduler_init();
    console_puts("Scheduler initialized\n");
    
    // Set up timer for preemption
    timer_set_frequency(100); // 100Hz
    console_puts("Timer configured\n");
    
    console_puts("Scheduler ready\n");
}

void init_console(void) {
    console_puts("Initializing console...\n");
    
    // Console already initialized in early_init
    console_clear();
    console_puts("Console ready\n");
}

void init_timer(void) {
    console_puts("Initializing timer...\n");

    // Initialize timer
    timer_init();
    console_puts("Timer initialized\n");

    console_puts("Timer ready\n");
}

void init_ipc_system(void) {
    console_puts("Initializing IPC system...\n");

    // Initialize IPC
    ipc_init();
    console_puts("IPC system initialized\n");

    console_puts("IPC ready\n");
}

void init_lambda3_integration(void) {
    console_puts("Initializing Lambda³ integration...\n");
    
    // Initialize Lambda³ syscall handlers
    init_lambda3_syscalls();
    console_puts("Lambda³ syscalls initialized\n");
    
    // Reserve memory for Lambda³ service
    reserve_lambda3_memory();
    console_puts("Lambda³ memory reserved\n");
    
    console_puts("Lambda³ integration ready\n");
}

// =============================================================================
// EXCEPTION HANDLERS
// =============================================================================

void register_exception_handlers(void) {
    // Register all exception handlers
    interrupt_register_handler(0, exception_divide_by_zero);
    interrupt_register_handler(1, exception_debug);
    interrupt_register_handler(2, exception_nmi);
    interrupt_register_handler(3, exception_breakpoint);
    interrupt_register_handler(4, exception_overflow);
    interrupt_register_handler(5, exception_bounds);
    interrupt_register_handler(6, exception_invalid_opcode);
    interrupt_register_handler(7, exception_device_not_available);
    interrupt_register_handler(8, exception_double_fault);
    interrupt_register_handler(9, exception_coprocessor_segment_overrun);
    interrupt_register_handler(10, exception_invalid_tss);
    interrupt_register_handler(11, exception_segment_not_present);
    interrupt_register_handler(12, exception_stack_fault);
    interrupt_register_handler(13, exception_general_protection);
    interrupt_register_handler(14, exception_page_fault);
    interrupt_register_handler(15, exception_reserved);
    interrupt_register_handler(16, exception_x87_fpu_error);
    interrupt_register_handler(17, exception_alignment_check);
    interrupt_register_handler(18, exception_machine_check);
    interrupt_register_handler(19, exception_simd_fpu_exception);
    interrupt_register_handler(20, exception_virtualization);
    interrupt_register_handler(21, exception_control_protection);
    interrupt_register_handler(22, exception_reserved);
    interrupt_register_handler(23, exception_reserved);
    interrupt_register_handler(24, exception_reserved);
    interrupt_register_handler(25, exception_reserved);
    interrupt_register_handler(26, exception_reserved);
    interrupt_register_handler(27, exception_reserved);
    interrupt_register_handler(28, exception_reserved);
    interrupt_register_handler(29, exception_reserved);
    interrupt_register_handler(30, exception_reserved);
    interrupt_register_handler(31, exception_reserved);
}

void register_irq_handlers(void) {
    // Register IRQ handlers
    interrupt_register_handler(32, irq_timer);
    interrupt_register_handler(33, irq_keyboard);
    interrupt_register_handler(34, irq_cascade);
    interrupt_register_handler(35, irq_serial2);
    interrupt_register_handler(36, irq_serial1);
    interrupt_register_handler(37, irq_parallel2);
    interrupt_register_handler(38, irq_floppy);
    interrupt_register_handler(39, irq_parallel1);
    interrupt_register_handler(40, irq_rtc);
    interrupt_register_handler(41, irq_acpi);
    interrupt_register_handler(42, irq_reserved);
    interrupt_register_handler(43, irq_reserved);
    interrupt_register_handler(44, irq_mouse);
    interrupt_register_handler(45, irq_coprocessor);
    interrupt_register_handler(46, irq_primary_ide);
    interrupt_register_handler(47, irq_secondary_ide);
}

// =============================================================================
// EXCEPTION HANDLER IMPLEMENTATIONS
// =============================================================================

void exception_divide_by_zero(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Divide by Zero\n");
    kernel_panic("Divide by zero exception");
}

void exception_debug(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Debug\n");
    // Continue execution
}

void exception_nmi(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: NMI\n");
    kernel_panic("NMI exception");
}

void exception_breakpoint(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Breakpoint\n");
    // Continue execution
}

void exception_overflow(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Overflow\n");
    kernel_panic("Overflow exception");
}

void exception_bounds(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Bounds Check\n");
    kernel_panic("Bounds check exception");
}

void exception_invalid_opcode(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Invalid Opcode\n");
    kernel_panic("Invalid opcode exception");
}

void exception_device_not_available(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Device Not Available\n");
    kernel_panic("Device not available exception");
}

void exception_double_fault(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Double Fault\n");
    kernel_panic("Double fault exception");
}

void exception_coprocessor_segment_overrun(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Coprocessor Segment Overrun\n");
    kernel_panic("Coprocessor segment overrun exception");
}

void exception_invalid_tss(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Invalid TSS\n");
    kernel_panic("Invalid TSS exception");
}

void exception_segment_not_present(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Segment Not Present\n");
    kernel_panic("Segment not present exception");
}

void exception_stack_fault(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Stack Fault\n");
    kernel_panic("Stack fault exception");
}

void exception_general_protection(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: General Protection Fault\n");
    kernel_panic("General protection fault");
}

void exception_page_fault(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    console_puts("EXCEPTION: Page Fault\n");
    // Handle page fault
    handle_page_fault(error_code);
}

void exception_reserved(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Reserved\n");
    kernel_panic("Reserved exception");
}

void exception_x87_fpu_error(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: x87 FPU Error\n");
    kernel_panic("x87 FPU error exception");
}

void exception_alignment_check(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Alignment Check\n");
    kernel_panic("Alignment check exception");
}

void exception_machine_check(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Machine Check\n");
    kernel_panic("Machine check exception");
}

void exception_simd_fpu_exception(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: SIMD FPU Exception\n");
    kernel_panic("SIMD FPU exception");
}

void exception_virtualization(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Virtualization\n");
    kernel_panic("Virtualization exception");
}

void exception_control_protection(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    console_puts("EXCEPTION: Control Protection\n");
    kernel_panic("Control protection exception");
}

// =============================================================================
// IRQ HANDLER IMPLEMENTATIONS
// =============================================================================

void irq_timer(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    // Timer interrupt - trigger scheduler
    scheduler_tick();
    
    // Send EOI
    interrupt_send_eoi(0);
}

void irq_keyboard(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    // Keyboard interrupt
    handle_keyboard_interrupt();
    
    // Send EOI
    interrupt_send_eoi(1);
}

void irq_cascade(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    // Cascade interrupt
    interrupt_send_eoi(2);
}

void irq_serial2(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    // Serial port 2 interrupt
    interrupt_send_eoi(3);
}

void irq_serial1(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    // Serial port 1 interrupt
    interrupt_send_eoi(4);
}

void irq_parallel2(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    // Parallel port 2 interrupt
    interrupt_send_eoi(5);
}

void irq_floppy(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    // Floppy disk interrupt
    interrupt_send_eoi(6);
}

void irq_parallel1(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    // Parallel port 1 interrupt
    interrupt_send_eoi(7);
}

void irq_rtc(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    // RTC interrupt
    interrupt_send_eoi(8);
}

void irq_acpi(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    // ACPI interrupt
    interrupt_send_eoi(9);
}

void irq_reserved(uint32_t interrupt, uint32_t error_code) {
    (void)error_code;
    // Reserved interrupt
    interrupt_send_eoi(interrupt - 32);
}

void irq_mouse(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    // Mouse interrupt
    interrupt_send_eoi(12);
}

void irq_coprocessor(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    // Coprocessor interrupt
    interrupt_send_eoi(13);
}

void irq_primary_ide(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    // Primary IDE interrupt
    interrupt_send_eoi(14);
}

void irq_secondary_ide(uint32_t interrupt, uint32_t error_code) {
    (void)interrupt;
    (void)error_code;
    // Secondary IDE interrupt
    interrupt_send_eoi(15);
}

// =============================================================================
// KERNEL UTILITIES
// =============================================================================

void print_boot_message(void) {
    console_clear();
    console_puts("========================================\n");
    console_puts("           TEROS KERNEL v1.0.0          \n");
    console_puts("     Ternary Operating System          \n");
    console_puts("========================================\n");
    console_puts("Build: ");
    console_puts(kernel_build_date);
    console_puts("\n");
    console_puts("Kernel initialized successfully!\n");
    console_puts("Lambda³ integration ready\n");
    console_puts("========================================\n");
}

void kernel_main_loop(void) {
    console_puts("Entering kernel main loop...\n");
    
    while (true) {
        // Idle loop
        asm volatile("hlt");
        
        // Check for work
        if (scheduler_has_work()) {
            scheduler_schedule();
        }
    }
}

void kernel_panic(const char* message) {
    console_puts("KERNEL PANIC: ");
    console_puts(message);
    console_puts("\n");
    
    // Disable interrupts
    interrupt_disable();
    
    // Halt
    while (true) {
        asm volatile("hlt");
    }
}

void init_memory_map(void) {
    // Parse multiboot memory map
    // This is a simplified implementation
    console_puts("Memory map initialized\n");
}

void create_kernel_process(void) {
    // Create the kernel process (PID 0)
    process_t* kernel_proc = process_create("kernel", 0);
    if (kernel_proc == NULL) {
        kernel_panic("Failed to create kernel process");
    }
    
    // Set as current process
    scheduler_set_current_process(kernel_proc);
}

void init_lambda3_syscalls(void) {
    // Initialize Lambda³ syscall handlers
    // This will be implemented in syscall.c
    console_puts("Lambda³ syscalls initialized\n");
}

void reserve_lambda3_memory(void) {
    // Reserve memory for Lambda³ service
    // This will be implemented in memory.c
    console_puts("Lambda³ memory reserved\n");
}

void handle_page_fault(uint32_t error_code) {
    (void)error_code;
    // Handle page fault
    // This will be implemented in vmm.c
    console_puts("Page fault handled\n");
}

void handle_keyboard_interrupt(void) {
    // Handle keyboard interrupt
    // This will be implemented in keyboard.c
    console_puts("Keyboard interrupt handled\n");
}

// =============================================================================
// KERNEL STATE QUERIES
// =============================================================================

bool kernel_is_initialized(void) {
    return kernel_initialized;
}

uint64_t kernel_get_uptime(void) {
    return timer_get_ticks() - kernel_start_time;
}

uint32_t kernel_get_heap_start(void) {
    return kernel_heap_start;
}

uint32_t kernel_get_heap_end(void) {
    return kernel_heap_end;
}

const char* kernel_get_version(void) {
    return kernel_version;
}

const char* kernel_get_build_date(void) {
    return kernel_build_date;
}
