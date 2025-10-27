/**
 * @file kernel_main.h
 * @brief TEROS Kernel Main Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef KERNEL_MAIN_H
#define KERNEL_MAIN_H

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// KERNEL INITIALIZATION
// =============================================================================

/**
 * @brief Main kernel entry point
 */
void kernel_main(void);

/**
 * @brief Early kernel initialization
 */
void early_init(void);

/**
 * @brief Initialize memory management subsystem
 */
void init_memory_management(void);

/**
 * @brief Initialize interrupt system
 */
void init_interrupt_system(void);

/**
 * @brief Initialize process management subsystem
 */
void init_process_system(void);

/**
 * @brief Initialize scheduler
 */
void init_scheduler(void);

/**
 * @brief Initialize console
 */
void init_console(void);

/**
 * @brief Initialize timer
 */
void init_timer(void);

/**
 * @brief Initialize Lambda³ integration
 */
void init_lambda3_integration(void);

// =============================================================================
// EXCEPTION HANDLERS
// =============================================================================

/**
 * @brief Register all exception handlers
 */
void register_exception_handlers(void);

/**
 * @brief Register all IRQ handlers
 */
void register_irq_handlers(void);

// Exception handlers
void exception_divide_by_zero(uint32_t interrupt, uint32_t error_code);
void exception_debug(uint32_t interrupt, uint32_t error_code);
void exception_nmi(uint32_t interrupt, uint32_t error_code);
void exception_breakpoint(uint32_t interrupt, uint32_t error_code);
void exception_overflow(uint32_t interrupt, uint32_t error_code);
void exception_bounds(uint32_t interrupt, uint32_t error_code);
void exception_invalid_opcode(uint32_t interrupt, uint32_t error_code);
void exception_device_not_available(uint32_t interrupt, uint32_t error_code);
void exception_double_fault(uint32_t interrupt, uint32_t error_code);
void exception_coprocessor_segment_overrun(uint32_t interrupt, uint32_t error_code);
void exception_invalid_tss(uint32_t interrupt, uint32_t error_code);
void exception_segment_not_present(uint32_t interrupt, uint32_t error_code);
void exception_stack_fault(uint32_t interrupt, uint32_t error_code);
void exception_general_protection(uint32_t interrupt, uint32_t error_code);
void exception_page_fault(uint32_t interrupt, uint32_t error_code);
void exception_reserved(uint32_t interrupt, uint32_t error_code);
void exception_x87_fpu_error(uint32_t interrupt, uint32_t error_code);
void exception_alignment_check(uint32_t interrupt, uint32_t error_code);
void exception_machine_check(uint32_t interrupt, uint32_t error_code);
void exception_simd_fpu_exception(uint32_t interrupt, uint32_t error_code);
void exception_virtualization(uint32_t interrupt, uint32_t error_code);
void exception_control_protection(uint32_t interrupt, uint32_t error_code);

// IRQ handlers
void irq_timer(uint32_t interrupt, uint32_t error_code);
void irq_keyboard(uint32_t interrupt, uint32_t error_code);
void irq_cascade(uint32_t interrupt, uint32_t error_code);
void irq_serial2(uint32_t interrupt, uint32_t error_code);
void irq_serial1(uint32_t interrupt, uint32_t error_code);
void irq_parallel2(uint32_t interrupt, uint32_t error_code);
void irq_floppy(uint32_t interrupt, uint32_t error_code);
void irq_parallel1(uint32_t interrupt, uint32_t error_code);
void irq_rtc(uint32_t interrupt, uint32_t error_code);
void irq_acpi(uint32_t interrupt, uint32_t error_code);
void irq_reserved(uint32_t interrupt, uint32_t error_code);
void irq_mouse(uint32_t interrupt, uint32_t error_code);
void irq_coprocessor(uint32_t interrupt, uint32_t error_code);
void irq_primary_ide(uint32_t interrupt, uint32_t error_code);
void irq_secondary_ide(uint32_t interrupt, uint32_t error_code);

// =============================================================================
// KERNEL UTILITIES
// =============================================================================

/**
 * @brief Print boot message
 */
void print_boot_message(void);

/**
 * @brief Main kernel loop
 */
void kernel_main_loop(void);

/**
 * @brief Kernel panic handler
 * @param message Panic message
 */
void kernel_panic(const char* message);

/**
 * @brief Initialize memory map from multiboot
 */
void init_memory_map(void);

/**
 * @brief Create kernel process
 */
void create_kernel_process(void);

/**
 * @brief Initialize Lambda³ syscall handlers
 */
void init_lambda3_syscalls(void);

/**
 * @brief Reserve memory for Lambda³ service
 */
void reserve_lambda3_memory(void);

/**
 * @brief Handle page fault
 * @param error_code Page fault error code
 */
void handle_page_fault(uint32_t error_code);

/**
 * @brief Handle keyboard interrupt
 */
void handle_keyboard_interrupt(void);

// =============================================================================
// KERNEL STATE QUERIES
// =============================================================================

/**
 * @brief Check if kernel is initialized
 * @return true if initialized
 */
bool kernel_is_initialized(void);

/**
 * @brief Get kernel uptime in ticks
 * @return Uptime in ticks
 */
uint64_t kernel_get_uptime(void);

/**
 * @brief Get kernel heap start address
 * @return Heap start address
 */
uint32_t kernel_get_heap_start(void);

/**
 * @brief Get kernel heap end address
 * @return Heap end address
 */
uint32_t kernel_get_heap_end(void);

/**
 * @brief Get kernel version string
 * @return Version string
 */
const char* kernel_get_version(void);

/**
 * @brief Get kernel build date string
 * @return Build date string
 */
const char* kernel_get_build_date(void);

#endif // KERNEL_MAIN_H

