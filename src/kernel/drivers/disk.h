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

