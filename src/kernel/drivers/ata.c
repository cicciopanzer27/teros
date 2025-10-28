/**
 * @file ata.c
 * @brief ATA/SATA Disk Driver with ternary addressing
 */

#include "ata.h"
#include "console.h"
#include "trit.h"
#include "ternary_alu.h"
#include <stdio.h>

static ata_device_t primary_ata;
static ata_device_t secondary_ata;

// Port I/O functions
static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t value;
    asm volatile("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void outw(uint16_t port, uint16_t value) {
    asm volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}

// Wait for device to be ready
static int ata_wait_ready(uint16_t base) {
    uint8_t status;
    int timeout = 10000;
    
    while (timeout--) {
        status = inb(base + ATA_REG_STATUS);
        
        // Ternary error state check
        if (status & ATA_STATUS_ERR) {
            return -1;  // Error state
        }
        if (!(status & ATA_STATUS_BUSY)) {
            return 0;   // Ready state
        }
    }
    
    return 1;  // Timeout state
}

// Read a sector using ternary addressing
int ata_read_sector(ata_device_t* device, uint64_t lba, void* buffer, size_t sector_count) {
    if (device == NULL || !device->initialized || buffer == NULL || sector_count == 0) {
        return -1;
    }
    
    uint16_t base = device->base_port;
    
    // Wait for device ready
    int ready_status = ata_wait_ready(base);
    if (ready_status != 0) {
        console_puts("ATA: Device not ready\n");
        return -1;
    }
    
    // Set sector count
    outb(base + ATA_REG_SECTOR_COUNT, sector_count);
    
    // Set LBA address with ternary boundary checking
    // Ternary check: before start (-1), valid (0), beyond end (+1)
    if (lba >= device->total_sectors) {
        return 1;  // Beyond end
    }
    
    outb(base + ATA_REG_LBA_LOW, lba & 0xFF);
    outb(base + ATA_REG_LBA_MID, (lba >> 8) & 0xFF);
    outb(base + ATA_REG_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(base + ATA_REG_DEVICE, 0xE0 | ((lba >> 24) & 0x0F));
    
    // Send read command
    outb(base + ATA_REG_COMMAND, ATA_CMD_READ_PIO);
    
    // Wait for data ready
    ready_status = ata_wait_ready(base);
    if (ready_status != 0) {
        return -1;
    }
    
    // Read data
    uint16_t* buffer_w = (uint16_t*)buffer;
    size_t words = (sector_count * 512) / 2;
    
    for (size_t i = 0; i < words; i++) {
        buffer_w[i] = inw(base + ATA_REG_DATA);
    }
    
    console_puts("ATA: Read ");
    printf("%zu", sector_count);
    console_puts(" sectors\n");
    
    return 0;
}

int ata_write_sector(ata_device_t* device, uint64_t lba, const void* buffer, size_t sector_count) {
    if (device == NULL || !device->initialized || buffer == NULL || sector_count == 0) {
        return -1;
    }
    
    uint16_t base = device->base_port;
    
    // Wait for device ready
    if (ata_wait_ready(base) != 0) {
        return -1;
    }
    
    // Set sector count
    outb(base + ATA_REG_SECTOR_COUNT, sector_count);
    
    // Set LBA address with ternary error checking
    if (lba >= device->total_sectors) {
        return 1;  // Beyond end
    }
    
    outb(base + ATA_REG_LBA_LOW, lba & 0xFF);
    outb(base + ATA_REG_LBA_MID, (lba >> 8) & 0xFF);
    outb(base + ATA_REG_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(base + ATA_REG_DEVICE, 0xE0 | ((lba >> 24) & 0x0F));
    
    // Send write command
    outb(base + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);
    
    // Wait for data ready
    if (ata_wait_ready(base) != 0) {
        return -1;
    }
    
    // Write data
    const uint16_t* buffer_w = (const uint16_t*)buffer;
    size_t words = (sector_count * 512) / 2;
    
    for (size_t i = 0; i < words; i++) {
        outw(base + ATA_REG_DATA, buffer_w[i]);
    }
    
    // Wait for write to complete
    if (ata_wait_ready(base) != 0) {
        return -1;
    }
    
    console_puts("ATA: Wrote ");
    printf("%zu", sector_count);
    console_puts(" sectors\n");
    
    return 0;
}

int ata_identify_device(ata_device_t* device) {
    if (device == NULL) {
        return -1;
    }
    
    uint16_t base = device->base_port;
    
    // Select master drive
    outb(base + ATA_REG_DEVICE, 0xA0);
    
    // Send identify command
    outb(base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    
    // Wait for response
    if (ata_wait_ready(base) != 0) {
        return -1;
    }
    
    // Read identify data (would parse here)
    device->initialized = true;
    device->sector_size = 512;
    device->total_sectors = 1024 * 1024;  // Default 512MB
    
    console_puts("ATA: Device identified\n");
    return 0;
}

int ata_init(uint16_t base_port) {
    ata_device_t* device;
    
    if (base_port == ATA_PRIMARY_BASE) {
        device = &primary_ata;
    } else if (base_port == ATA_SECONDARY_BASE) {
        device = &secondary_ata;
    } else {
        return -1;
    }
    
    device->base_port = base_port;
    device->initialized = false;
    
    // Identify device
    if (ata_identify_device(device) != 0) {
        console_puts("ATA: Failed to identify device\n");
        return -1;
    }
    
    console_puts("ATA: Initialized at port 0x");
    printf("%03x", base_port);
    console_puts("\n");
    
    return 0;
}

