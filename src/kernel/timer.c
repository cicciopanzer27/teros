/**
 * @file timer.c
 * @brief Programmable Interval Timer (PIT) Driver
 * @author TEROS Development Team
 * @date 2025
 */

#include "timer.h"
#include "console.h"
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// PIT IMPLEMENTATION
// =============================================================================

#define PIT_PORT_CHANNEL0_DATA 0x40
#define PIT_PORT_CHANNEL1_DATA 0x41
#define PIT_PORT_CHANNEL2_DATA 0x42
#define PIT_PORT_COMMAND 0x43

#define PIT_COMMAND_CHANNEL0 0x00
#define PIT_COMMAND_CHANNEL1 0x40
#define PIT_COMMAND_CHANNEL2 0x80
#define PIT_COMMAND_READBACK 0xC0

#define PIT_COMMAND_ACCESS_LATCH 0x00
#define PIT_COMMAND_ACCESS_LO 0x10
#define PIT_COMMAND_ACCESS_HI 0x20
#define PIT_COMMAND_ACCESS_LOHI 0x30

#define PIT_COMMAND_MODE_TERMINAL_COUNT 0x00
#define PIT_COMMAND_MODE_ONESHOT 0x02
#define PIT_COMMAND_MODE_RATE_GENERATOR 0x04
#define PIT_COMMAND_MODE_SQUARE_WAVE 0x06
#define PIT_COMMAND_MODE_SOFTWARE_TRIGGER 0x08
#define PIT_COMMAND_MODE_HARDWARE_TRIGGER 0x0A

#define PIT_COMMAND_BCD_MODE 0x01
#define PIT_COMMAND_BCD_BINARY 0x00

#define PIT_BASE_FREQUENCY 1193182 // Hz
#define PIT_DEFAULT_FREQUENCY 100  // Hz

typedef struct {
    uint32_t ticks;
    uint32_t frequency;
    uint32_t divisor;
    bool initialized;
    uint32_t uptime_ms;
    uint32_t uptime_sec;
    uint32_t last_tick;
} timer_state_t;

static timer_state_t timer;

// =============================================================================
// TIMER FUNCTIONS
// =============================================================================

void timer_init(void) {
    if (timer.initialized) {
        return;
    }
    
    console_puts("TIMER: Initializing PIT...\n");
    
    // Initialize timer state
    timer.ticks = 0;
    timer.frequency = PIT_DEFAULT_FREQUENCY;
    timer.divisor = PIT_BASE_FREQUENCY / PIT_DEFAULT_FREQUENCY;
    timer.uptime_ms = 0;
    timer.uptime_sec = 0;
    timer.last_tick = 0;
    
    // Set PIT divisor
    timer_set_frequency(PIT_DEFAULT_FREQUENCY);
    
    timer.initialized = true;
    console_puts("TIMER: PIT initialized\n");
}

void timer_set_frequency(uint32_t freq) {
    if (freq == 0) {
        console_puts("TIMER: ERROR - Invalid frequency\n");
        return;
    }
    
    timer.frequency = freq;
    timer.divisor = PIT_BASE_FREQUENCY / freq;
    
    // Send command to PIT
    outb(PIT_PORT_COMMAND, 
         PIT_COMMAND_CHANNEL0 | 
         PIT_COMMAND_ACCESS_LOHI | 
         PIT_COMMAND_MODE_SQUARE_WAVE | 
         PIT_COMMAND_BCD_BINARY);
    
    // Send divisor (low byte first)
    outb(PIT_PORT_CHANNEL0_DATA, timer.divisor & 0xFF);
    outb(PIT_PORT_CHANNEL0_DATA, (timer.divisor >> 8) & 0xFF);
    
    console_puts("TIMER: Frequency set to ");
    printf("%u", freq);
    console_puts(" Hz\n");
}

uint32_t timer_get_frequency(void) {
    return timer.frequency;
}

void timer_handle_interrupt(void) {
    timer.ticks++;
    
    // Update uptime
    if (timer.ticks % timer.frequency == 0) {
        timer.uptime_sec++;
    }
    timer.uptime_ms = (timer.ticks * 1000) / timer.frequency;
}

uint64_t timer_get_ticks(void) {
    return timer.ticks;
}

uint32_t timer_get_uptime_ms(void) {
    return timer.uptime_ms;
}

uint32_t timer_get_uptime_sec(void) {
    return timer.uptime_sec;
}

bool timer_is_initialized(void) {
    return timer.initialized;
}

// =============================================================================
// PORT I/O
// =============================================================================

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

