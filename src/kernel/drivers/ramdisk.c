/**
 * @file ramdisk.c
 * @brief RAM Disk Block Device Driver
 * @author TEROS Development Team
 * @date 2025
 * 
 * This driver implements a simple RAM disk for testing and development.
 * It allocates a fixed-size buffer in kernel memory and exposes it as a block device.
 */

#include "ramdisk.h"
#include "block_device.h"
#include "console.h"
#include "kmalloc.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// =============================================================================
// RAMDISK CONFIGURATION
// =============================================================================

#define RAMDISK_NAME         "ramdisk0"
#define RAMDISK_SIZE_MB      4                    /* 4MB ramdisk */
#define RAMDISK_SIZE         (RAMDISK_SIZE_MB * 1024 * 1024)
#define RAMDISK_SECTOR_SIZE  512
#define RAMDISK_SECTOR_COUNT (RAMDISK_SIZE / RAMDISK_SECTOR_SIZE)

// =============================================================================
// RAMDISK STATE
// =============================================================================

typedef struct {
    uint8_t* data;                  /* Ramdisk data buffer */
    uint32_t size;                  /* Total size in bytes */
    uint32_t sector_size;           /* Sector size in bytes */
    uint32_t sector_count;          /* Total number of sectors */
    bool initialized;               /* Initialization flag */
    bool ready;                     /* Ready flag */
} ramdisk_state_t;

static ramdisk_state_t ramdisk_state;

// =============================================================================
// RAMDISK OPERATIONS
// =============================================================================

/**
 * ramdisk_init - Initialize the ramdisk
 */
static void ramdisk_init(void) {
    if (ramdisk_state.initialized) {
        return;
    }
    
    console_puts("RAMDISK: Initializing...\n");
    
    // Allocate ramdisk buffer
    ramdisk_state.data = (uint8_t*)kmalloc(RAMDISK_SIZE);
    if (ramdisk_state.data == NULL) {
        console_puts("RAMDISK: ERROR - Failed to allocate memory\n");
        return;
    }
    
    // Clear ramdisk
    memset(ramdisk_state.data, 0, RAMDISK_SIZE);
    
    // Initialize state
    ramdisk_state.size = RAMDISK_SIZE;
    ramdisk_state.sector_size = RAMDISK_SECTOR_SIZE;
    ramdisk_state.sector_count = RAMDISK_SECTOR_COUNT;
    ramdisk_state.initialized = true;
    ramdisk_state.ready = true;
    
    console_puts("RAMDISK: Initialized ");
    console_puts("4MB (");
    // Sector count could be printed here if needed
    console_puts(" sectors)\n");
}

/**
 * ramdisk_read - Read sectors from ramdisk
 * @sector: Starting sector number
 * @buffer: Buffer to read into
 * @count: Number of sectors to read
 * @return: Number of sectors actually read
 */
static uint32_t ramdisk_read(uint32_t sector, void* buffer, uint32_t count) {
    if (buffer == NULL || count == 0) {
        return 0;
    }
    
    if (!ramdisk_state.initialized || !ramdisk_state.ready) {
        console_puts("RAMDISK: ERROR - Not initialized\n");
        return 0;
    }
    
    // Check bounds
    if (sector >= ramdisk_state.sector_count) {
        console_puts("RAMDISK: ERROR - Sector out of bounds\n");
        return 0;
    }
    
    // Adjust count if it would exceed disk size
    if (sector + count > ramdisk_state.sector_count) {
        count = ramdisk_state.sector_count - sector;
    }
    
    // Calculate byte offset
    uint32_t offset = sector * ramdisk_state.sector_size;
    uint32_t size = count * ramdisk_state.sector_size;
    
    // Copy data
    memcpy(buffer, ramdisk_state.data + offset, size);
    
    return count;
}

/**
 * ramdisk_write - Write sectors to ramdisk
 * @sector: Starting sector number
 * @buffer: Buffer to write from
 * @count: Number of sectors to write
 * @return: Number of sectors actually written
 */
static uint32_t ramdisk_write(uint32_t sector, const void* buffer, uint32_t count) {
    if (buffer == NULL || count == 0) {
        return 0;
    }
    
    if (!ramdisk_state.initialized || !ramdisk_state.ready) {
        console_puts("RAMDISK: ERROR - Not initialized\n");
        return 0;
    }
    
    // Check bounds
    if (sector >= ramdisk_state.sector_count) {
        console_puts("RAMDISK: ERROR - Sector out of bounds\n");
        return 0;
    }
    
    // Adjust count if it would exceed disk size
    if (sector + count > ramdisk_state.sector_count) {
        count = ramdisk_state.sector_count - sector;
    }
    
    // Calculate byte offset
    uint32_t offset = sector * ramdisk_state.sector_size;
    uint32_t size = count * ramdisk_state.sector_size;
    
    // Copy data
    memcpy(ramdisk_state.data + offset, buffer, size);
    
    return count;
}

/**
 * ramdisk_get_sector_count - Get total number of sectors
 * @return: Total sector count
 */
static uint32_t ramdisk_get_sector_count(void) {
    return ramdisk_state.sector_count;
}

/**
 * ramdisk_is_ready - Check if ramdisk is ready
 * @return: true if ready, false otherwise
 */
static bool ramdisk_is_ready(void) {
    return ramdisk_state.initialized && ramdisk_state.ready;
}

// =============================================================================
// BLOCK DEVICE OPERATIONS STRUCTURE
// =============================================================================

static block_device_ops_t ramdisk_ops = {
    .name = RAMDISK_NAME,
    .init = ramdisk_init,
    .read = ramdisk_read,
    .write = ramdisk_write,
    .get_sector_count = ramdisk_get_sector_count,
    .is_ready = ramdisk_is_ready
};

static block_device_t ramdisk_device = {
    .ops = &ramdisk_ops,
    .initialized = false
};

// =============================================================================
// PUBLIC INTERFACE
// =============================================================================

/**
 * ramdisk_register - Register ramdisk as a block device
 * @return: 0 on success, -1 on failure
 */
int ramdisk_register(void) {
    console_puts("RAMDISK: Registering block device...\n");
    
    int ret = block_device_register(&ramdisk_device);
    if (ret != 0) {
        console_puts("RAMDISK: ERROR - Failed to register\n");
        return -1;
    }
    
    console_puts("RAMDISK: Registered successfully\n");
    return 0;
}

/**
 * ramdisk_get_device - Get ramdisk block device
 * @return: Pointer to block device structure
 */
block_device_t* ramdisk_get_device(void) {
    return &ramdisk_device;
}

/**
 * ramdisk_format - Format the ramdisk (clear all data)
 */
void ramdisk_format(void) {
    if (!ramdisk_state.initialized) {
        console_puts("RAMDISK: ERROR - Not initialized\n");
        return;
    }
    
    console_puts("RAMDISK: Formatting...\n");
    memset(ramdisk_state.data, 0, RAMDISK_SIZE);
    console_puts("RAMDISK: Format complete\n");
}

/**
 * ramdisk_get_size - Get ramdisk size in bytes
 * @return: Size in bytes
 */
uint32_t ramdisk_get_size(void) {
    return ramdisk_state.size;
}

/**
 * ramdisk_get_sector_size - Get sector size in bytes
 * @return: Sector size in bytes
 */
uint32_t ramdisk_get_sector_size(void) {
    return ramdisk_state.sector_size;
}

