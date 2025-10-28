/**
 * @file serial.c
 * @brief Serial Port Driver (COM1-COM4)
 * @author TEROS Development Team
 * @date 2025
 */

#include "serial.h"
#include "console.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

// Helper prototypes
static inline void outb(uint16_t port, uint8_t value);
static inline uint8_t inb(uint16_t port);
static inline size_t strlen(const char* str);

// =============================================================================
// SERIAL PORT IMPLEMENTATION
// =============================================================================

#define SERIAL_COM1 0x3F8
#define SERIAL_COM2 0x2F8
#define SERIAL_COM3 0x3E8
#define SERIAL_COM4 0x2E8

#define SERIAL_DATA(base) (base)
#define SERIAL_INTERRUPT_ENABLE(base) (base + 1)
#define SERIAL_FIFO_CONTROL(base) (base + 2)
#define SERIAL_LINE_CONTROL(base) (base + 3)
#define SERIAL_MODEM_CONTROL(base) (base + 4)
#define SERIAL_LINE_STATUS(base) (base + 5)
#define SERIAL_MODEM_STATUS(base) (base + 6)
#define SERIAL_SCRATCH(base) (base + 7)

// Line Control Register flags
#define SERIAL_LCR_DATA_SIZE_5 0x00
#define SERIAL_LCR_DATA_SIZE_6 0x01
#define SERIAL_LCR_DATA_SIZE_7 0x02
#define SERIAL_LCR_DATA_SIZE_8 0x03
#define SERIAL_LCR_STOP_BITS_1 0x00
#define SERIAL_LCR_STOP_BITS_2 0x04
#define SERIAL_LCR_PARITY_NONE 0x00
#define SERIAL_LCR_PARITY_ODD 0x08
#define SERIAL_LCR_PARITY_EVEN 0x18
#define SERIAL_LCR_PARITY_MARK 0x28
#define SERIAL_LCR_PARITY_SPACE 0x38
#define SERIAL_LCR_DLAB 0x80

// FIFO Control Register flags
#define SERIAL_FCR_FIFO_ENABLE 0x01
#define SERIAL_FCR_CLEAR_RECEIVE 0x02
#define SERIAL_FCR_CLEAR_TRANSMIT 0x04
#define SERIAL_FCR_DMA_MODE 0x08
#define SERIAL_FCR_14_BYTES 0x00
#define SERIAL_FCR_64_BYTES 0xC0

// Line Status Register
#define SERIAL_LSR_DATA_READY 0x01
#define SERIAL_LSR_OVERRUN 0x02
#define SERIAL_LSR_PARITY_ERROR 0x04
#define SERIAL_LSR_FRAMING_ERROR 0x08
#define SERIAL_LSR_BREAK 0x10
#define SERIAL_LSR_TRANSMIT_EMPTY 0x20
#define SERIAL_LSR_TRANSMIT_IDLE 0x40
#define SERIAL_LSR_IMPRESSTANTABLE_ERROR 0x80

// Modem Control Register
#define SERIAL_MCR_DTR 0x01
#define SERIAL_MCR_RTS 0x02
#define SERIAL_MCR_AUX_OUTPUT1 0x04
#define SERIAL_MCR_AUX_OUTPUT2 0x08
#define SERIAL_MCR_LOOPBACK 0x10

// Modem Status Register
#define SERIAL_MSR_DELTA_CTS 0x01
#define SERIAL_MSR_DELTA_DSR 0x02
#define SERIAL_MSR_RI 0x04
#define SERIAL_MSR_DELTA_RLSD 0x08
#define SERIAL_MSR_CTS 0x10
#define SERIAL_MSR_DSR 0x20
#define SERIAL_MSR_RI_OUTPUT 0x40
#define SERIAL_MSR_RLSD 0x80

typedef struct serial_port {
    uint16_t base;
    uint32_t baud_rate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity;
    bool initialized;
    bool interrupts_enabled;
    
    uint8_t rx_buffer[256];
    uint32_t rx_head;
    uint32_t rx_tail;
    uint32_t rx_count;
    
    // Ternary flow control state
    flow_state_t flow_state;
    bool rts;
    bool dtr;
} serial_state_t;

static serial_state_t serial_ports[4];

// =============================================================================
// SERIAL PORT FUNCTIONS
// =============================================================================

void serial_init_port(uint16_t port, uint32_t baud_rate, uint8_t data_bits, 
                     uint8_t stop_bits, uint8_t parity) {
    int port_index = -1;
    
    switch (port) {
        case SERIAL_COM1:
            port_index = 0;
            break;
        case SERIAL_COM2:
            port_index = 1;
            break;
        case SERIAL_COM3:
            port_index = 2;
            break;
        case SERIAL_COM4:
            port_index = 3;
            break;
    }
    
    if (port_index < 0) {
        console_puts("SERIAL: ERROR - Invalid port\n");
        return;
    }
    
    serial_state_t* s = &serial_ports[port_index];
    s->base = port;
    s->baud_rate = baud_rate;
    s->data_bits = data_bits;
    s->stop_bits = stop_bits;
    s->parity = parity;
    
    // Disable interrupts
    outb(SERIAL_INTERRUPT_ENABLE(port), 0x00);
    
    // Enable DLAB to set baud rate divisor
    outb(SERIAL_LINE_CONTROL(port), SERIAL_LCR_DLAB);
    
    // Set baud rate divisor
    uint16_t divisor = 115200 / baud_rate;
    outb(SERIAL_DATA(port), divisor & 0xFF);
    outb(SERIAL_INTERRUPT_ENABLE(port), (divisor >> 8) & 0xFF);
    
    // Set line control
    uint8_t lcr = 0;
    switch (data_bits) {
        case 5: lcr |= SERIAL_LCR_DATA_SIZE_5; break;
        case 6: lcr |= SERIAL_LCR_DATA_SIZE_6; break;
        case 7: lcr |= SERIAL_LCR_DATA_SIZE_7; break;
        case 8: lcr |= SERIAL_LCR_DATA_SIZE_8; break;
    }
    
    if (stop_bits == 2) {
        lcr |= SERIAL_LCR_STOP_BITS_2;
    }
    
    switch (parity) {
        case SERIAL_PARITY_NONE: lcr |= SERIAL_LCR_PARITY_NONE; break;
        case SERIAL_PARITY_ODD: lcr |= SERIAL_LCR_PARITY_ODD; break;
        case SERIAL_PARITY_EVEN: lcr |= SERIAL_LCR_PARITY_EVEN; break;
        case SERIAL_PARITY_MARK: lcr |= SERIAL_LCR_PARITY_MARK; break;
        case SERIAL_PARITY_SPACE: lcr |= SERIAL_LCR_PARITY_SPACE; break;
    }
    
    outb(SERIAL_LINE_CONTROL(port), lcr);
    
    // Enable FIFO and clear
    outb(SERIAL_FIFO_CONTROL(port), 
         SERIAL_FCR_FIFO_ENABLE | SERIAL_FCR_CLEAR_RECEIVE | 
         SERIAL_FCR_CLEAR_TRANSMIT | SERIAL_FCR_64_BYTES);
    
    // Set modem control (enable RTS/CTS and DTR/DSR)
    s->rts = true;
    s->dtr = true;
    outb(SERIAL_MODEM_CONTROL(port), 
         SERIAL_MCR_DTR | SERIAL_MCR_RTS | SERIAL_MCR_AUX_OUTPUT2);
    
    // Initialize ternary flow control state (start with GO)
    s->flow_state = FLOW_GO;
    
    // Enable interrupts for data available (optional - can be disabled)
    s->interrupts_enabled = false;
    outb(SERIAL_INTERRUPT_ENABLE(port), 0x01);
    
    s->rx_head = 0;
    s->rx_tail = 0;
    s->rx_count = 0;
    s->initialized = true;
    
    console_puts("SERIAL: Port initialized at ");
    printf("0x%x", port);
    console_puts(", ");
    printf("%u", baud_rate);
    console_puts(" baud\n");
}

void serial_init(void) {
    // Initialize COM1
    serial_init_port(SERIAL_COM1, 9600, 8, 1, SERIAL_PARITY_NONE);
    
    console_puts("SERIAL: Serial driver initialized\n");
}

bool serial_is_transmit_empty(serial_state_t* s) {
    return (inb(SERIAL_LINE_STATUS(s->base)) & SERIAL_LSR_TRANSMIT_EMPTY) != 0;
}

void serial_putchar(serial_state_t* s, char c) {
    if (!s->initialized) {
        return;
    }
    
    // Wait for transmitter to be ready
    while (!serial_is_transmit_empty(s)) {
        // Busy wait
    }
    
    // Send character
    outb(SERIAL_DATA(s->base), c);
}

char serial_getchar(serial_state_t* s) {
    if (!s->initialized) {
        return 0;
    }
    
    // Wait for data
    while ((inb(SERIAL_LINE_STATUS(s->base)) & SERIAL_LSR_DATA_READY) == 0) {
        // Busy wait
    }
    
    // Read character
    return inb(SERIAL_DATA(s->base));
}

void serial_write(serial_state_t* s, const char* data, size_t size) {
    if (!s->initialized) {
        return;
    }
    
    for (size_t i = 0; i < size; i++) {
        serial_putchar(s, data[i]);
    }
}

void serial_puts(serial_state_t* s, const char* str) {
    serial_write(s, str, strlen(str));
}

size_t serial_read(serial_state_t* s, char* buffer, size_t size) {
    if (!s->initialized) {
        return 0;
    }
    
    size_t count = 0;
    while (count < size && (inb(SERIAL_LINE_STATUS(s->base)) & SERIAL_LSR_DATA_READY) != 0) {
        buffer[count++] = inb(SERIAL_DATA(s->base));
    }
    
    return count;
}

serial_state_t* serial_get_port(uint16_t port) {
    switch (port) {
        case SERIAL_COM1: return &serial_ports[0];
        case SERIAL_COM2: return &serial_ports[1];
        case SERIAL_COM3: return &serial_ports[2];
        case SERIAL_COM4: return &serial_ports[3];
        default: return NULL;
    }
}

bool serial_is_initialized(uint16_t port) {
    serial_state_t* s = serial_get_port(port);
    return s != NULL && s->initialized;
}

void serial_set_interrupts(serial_state_t* s, bool enable) {
    if (s == NULL || !s->initialized) {
        return;
    }
    
    s->interrupts_enabled = enable;
    uint8_t ier = enable ? 0x0F : 0x00;  // Enable all interrupts when enabled
    outb(SERIAL_INTERRUPT_ENABLE(s->base), ier);
}

flow_state_t serial_get_flow_state(serial_state_t* s) {
    if (s == NULL || !s->initialized) {
        return FLOW_STOP;
    }
    
    // Read modem status register to determine actual flow state
    uint8_t msr = inb(SERIAL_MODEM_STATUS(s->base));
    
    // Ternary flow control logic:
    // -1 (STOP) if CTS is low (remote says stop)
    // 0 (HOLD) if CTS is transitioning
    // +1 (GO) if CTS is high (remote says go)
    
    if ((msr & SERIAL_MSR_CTS) == 0) {
        s->flow_state = FLOW_STOP;  // Clear to send is off - stop
    } else {
        s->flow_state = FLOW_GO;    // Clear to send is on - go
    }
    
    return s->flow_state;
}

void serial_set_flow_control(serial_state_t* s, bool rts, bool dtr) {
    if (s == NULL || !s->initialized) {
        return;
    }
    
    s->rts = rts;
    s->dtr = dtr;
    
    uint8_t mcr = 0;
    if (dtr) mcr |= SERIAL_MCR_DTR;
    if (rts) mcr |= SERIAL_MCR_RTS;
    mcr |= SERIAL_MCR_AUX_OUTPUT2;  // Always enable aux output
    
    outb(SERIAL_MODEM_CONTROL(s->base), mcr);
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

static inline size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

