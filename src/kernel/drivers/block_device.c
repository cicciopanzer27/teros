/**
 * @file block_device.c
 * @brief Block Device Driver Implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "block_device.h"
#include "console.h"
#include "kmalloc.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_BLOCK_DEVICES 8

static block_device_t* devices[MAX_BLOCK_DEVICES];
static int device_count = 0;

int block_device_register(block_device_t* device) {
    if (device == NULL || device->ops == NULL) {
        return -1;
    }
    
    if (device_count >= MAX_BLOCK_DEVICES) {
        console_puts("BLOCK_DEVICE: ERROR - Maximum devices reached\n");
        return -1;
    }
    
    // Check if device with same name already exists
    for (int i = 0; i < device_count; i++) {
        if (strcmp(devices[i]->ops->name, device->ops->name) == 0) {
            console_puts("BLOCK_DEVICE: ERROR - Device already registered\n");
            return -1;
        }
    }
    
    devices[device_count++] = device;
    device->initialized = true;
    
    console_puts("BLOCK_DEVICE: Registered device: ");
    console_puts(device->ops->name);
    console_puts("\n");
    
    return 0;
}

int block_device_unregister(const char* name) {
    if (name == NULL) {
        return -1;
    }
    
    for (int i = 0; i < device_count; i++) {
        if (strcmp(devices[i]->ops->name, name) == 0) {
            // Shift remaining devices
            for (int j = i; j < device_count - 1; j++) {
                devices[j] = devices[j + 1];
            }
            device_count--;
            return 0;
        }
    }
    
    return -1;
}

block_device_t* block_device_get(const char* name) {
    if (name == NULL) {
        return NULL;
    }
    
    for (int i = 0; i < device_count; i++) {
        if (strcmp(devices[i]->ops->name, name) == 0) {
            return devices[i];
        }
    }
    
    return NULL;
}

uint32_t block_device_read(block_device_t* device, uint32_t sector, void* buffer, uint32_t count) {
    if (device == NULL || buffer == NULL || count == 0) {
        return 0;
    }
    
    if (!device->initialized || device->ops->read == NULL) {
        return 0;
    }
    
    if (!device->ops->is_ready()) {
        console_puts("BLOCK_DEVICE: ERROR - Device not ready\n");
        return 0;
    }
    
    return device->ops->read(sector, buffer, count);
}

uint32_t block_device_write(block_device_t* device, uint32_t sector, const void* buffer, uint32_t count) {
    if (device == NULL || buffer == NULL || count == 0) {
        return 0;
    }
    
    if (!device->initialized || device->ops->write == NULL) {
        return 0;
    }
    
    if (!device->ops->is_ready()) {
        console_puts("BLOCK_DEVICE: ERROR - Device not ready\n");
        return 0;
    }
    
    return device->ops->write(sector, buffer, count);
}

uint32_t block_device_get_sector_count(block_device_t* device) {
    if (device == NULL || device->ops == NULL || device->ops->get_sector_count == NULL) {
        return 0;
    }
    
    return device->ops->get_sector_count();
}

bool block_device_is_ready(block_device_t* device) {
    if (device == NULL || device->ops == NULL || device->ops->is_ready == NULL) {
        return false;
    }
    
    return device->ops->is_ready();
}

void block_device_init_all(void) {
    console_puts("BLOCK_DEVICE: Initializing block devices...\n");
    
    // Initialize all registered devices
    for (int i = 0; i < device_count; i++) {
        if (devices[i]->ops->init != NULL) {
            devices[i]->ops->init();
        }
    }
    
    console_puts("BLOCK_DEVICE: Initialized ");
    // TODO: Print device_count
    console_puts("devices\n");
}

