/**
 * @file e1000.c
 * @brief Intel E1000 Ethernet Controller Driver
 * Based on Linux kernel e1000 driver implementation
 */

#include "e1000.h"
#include "networking.h"
#include "console.h"
#include <stdio.h>

// E1000 registers
#define E1000_CTRL    0x00000
#define E1000_STATUS  0x00008
#define E1000_RCTL    0x0100
#define E1000_TCTL    0x0400
#define E1000_TDLEN   0x3808
#define E1000_TDH     0x3810
#define E1000_TDT     0x3818
#define E1000_RDLEN   0x2808
#define E1000_RDH     0x2810
#define E1000_RDT     0x2818

// E1000 commands
#define E1000_CTRL_RST   0x04000000
#define E1000_RCTL_EN   0x00000002
#define E1000_RCTL_BSIZE_2048 0x00000000
#define E1000_TCTL_EN   0x00000002
#define E1000_TCTL_PSP  0x00000008

static e1000_device_t e1000_device;
static bool e1000_initialized = false;

static void e1000_write_reg(uint16_t reg, uint32_t value) {
    *(volatile uint32_t*)(e1000_device.mmio_base + reg) = value;
}

static uint32_t e1000_read_reg(uint16_t reg) {
    return *(volatile uint32_t*)(e1000_device.mmio_base + reg);
}

int e1000_init(uintptr_t mmio_base, uintptr_t io_base) {
    if (e1000_initialized) {
        return -1;
    }
    
    e1000_device.mmio_base = (uintptr_t)mmio_base;
    e1000_device.io_base = io_base;
    
    // Reset the adapter
    e1000_write_reg(E1000_CTRL, E1000_CTRL_RST);
    
    // Wait for reset to complete
    uint32_t status = e1000_read_reg(E1000_STATUS);
    
    console_puts("E1000: Initialized at MMIO ");
    printf("0x%lx", (unsigned long)mmio_base);
    console_puts("\n");
    
    e1000_initialized = true;
    return 0;
}

int e1000_send_packet(const uint8_t* buffer, size_t length) {
    if (!e1000_initialized || buffer == NULL || length == 0) {
        return -1;
    }
    
    // E1000 transmit ring would be used here
    // For now, just log the transmission
    
    console_puts("E1000: Sending packet (");
    printf("%zu", length);
    console_puts(" bytes)\n");
    
    return 0;
}

int e1000_receive_packet(uint8_t* buffer, size_t max_length) {
    if (!e1000_initialized || buffer == NULL || max_length == 0) {
        return -1;
    }
    
    // E1000 receive ring would be used here
    // For now, return no data
    
    return 0;
}

int e1000_get_mac_address(uint8_t* mac_addr) {
    if (!e1000_initialized || mac_addr == NULL) {
        return -1;
    }
    
    // Read MAC address from E1000 registers
    uint32_t low = e1000_read_reg(0x5400);  // E1000_RAL
    uint32_t high = e1000_read_reg(0x5404); // E1000_RAH
    
    mac_addr[0] = (uint8_t)(low);
    mac_addr[1] = (uint8_t)(low >> 8);
    mac_addr[2] = (uint8_t)(low >> 16);
    mac_addr[3] = (uint8_t)(low >> 24);
    mac_addr[4] = (uint8_t)(high);
    mac_addr[5] = (uint8_t)(high >> 8);
    
    return 0;
}

