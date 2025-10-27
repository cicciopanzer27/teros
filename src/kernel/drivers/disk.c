/**
 * @file disk.c
 * @brief Disk Device Driver
 * @author TEROS Development Team
 * @date 2025
 */

#include "disk.h"
#include "../console.h"
#include "../kmalloc.h"
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

/**
 * @file disk.h
 * @brief Disk Device Driver Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_DISKS 16
#define SECTOR_SIZE 512

typedef struct disk_info {
    uint64_t sector_count;
    uint32_t sector_size;
    uint32_t block_size;
    char model[64];
    char serial[32];
} disk_info_t;

typedef struct disk_driver {
    int disk_id;
    const char* name;
    void* private_data;
    
    int (*read)(struct disk_driver* disk, uint64_t lba, uint32_t count, void* buffer);
    int (*write)(struct disk_driver* disk, uint64_t lba, uint32_t count, const void* buffer);
    int (*get_info)(struct disk_driver* disk, disk_info_t* info);
    int (*init)(struct disk_driver* disk);
    void (*deinit)(struct disk_driver* disk);
} disk_driver_t;

int disk_init(void);
int disk_register(disk_driver_t* disk);
disk_driver_t* disk_get(int disk_id);
int disk_read(int disk_id, uint64_t lba, uint32_t count, void* buffer);
int disk_write(int disk_id, uint64_t lba, uint32_t count, const void* buffer);
int disk_get_info(int disk_id, disk_info_t* info);
int disk_count_disks(void);

#endif // DISK_H

