/**
 * @file disk.c
 * @brief Disk Device Driver
 * @author TEROS Development Team
 * @date 2025
 */

#include "disk.h"
#include "console.h"
#include "kmalloc.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Disk state
static disk_driver_t* disks[MAX_DISKS];
static int disk_count = 0;
static bool disk_system_initialized = false;

int disk_init(void) {
    if (disk_system_initialized) {
        return 0;
    }
    
    console_puts("DISK: Initializing disk subsystem...\n");
    
    // Initialize disk array
    for (int i = 0; i < MAX_DISKS; i++) {
        disks[i] = NULL;
    }
    
    disk_system_initialized = true;
    console_puts("DISK: Initialized\n");
    
    return 0;
}

int disk_register(disk_driver_t* disk) {
    if (disk == NULL) {
        console_puts("DISK: ERROR - Null disk driver\n");
        return -1;
    }
    
    if (disk_count >= MAX_DISKS) {
        console_puts("DISK: ERROR - Too many disks\n");
        return -1;
    }
    
    disk->disk_id = disk_count;
    disks[disk_count] = disk;
    disk_count++;
    
    console_puts("DISK: Registered disk device\n");
    
    return 0;
}

disk_driver_t* disk_get(int disk_id) {
    if (disk_id < 0 || disk_id >= disk_count) {
        return NULL;
    }
    
    return disks[disk_id];
}

int disk_read(int disk_id, uint64_t lba, uint32_t count, void* buffer) {
    disk_driver_t* disk = disk_get(disk_id);
    
    if (disk == NULL || disk->read == NULL) {
        console_puts("DISK: ERROR - Invalid disk or read function\n");
        return -1;
    }
    
    return disk->read(disk, lba, count, buffer);
}

int disk_write(int disk_id, uint64_t lba, uint32_t count, const void* buffer) {
    disk_driver_t* disk = disk_get(disk_id);
    
    if (disk == NULL || disk->write == NULL) {
        console_puts("DISK: ERROR - Invalid disk or write function\n");
        return -1;
    }
    
    return disk->write(disk, lba, count, buffer);
}

int disk_get_info(int disk_id, disk_info_t* info) {
    disk_driver_t* disk = disk_get(disk_id);
    
    if (disk == NULL || disk->get_info == NULL) {
        console_puts("DISK: ERROR - Invalid disk or get_info function\n");
        return -1;
    }
    
    return disk->get_info(disk, info);
}

int disk_count_disks(void) {
    return disk_count;
}

