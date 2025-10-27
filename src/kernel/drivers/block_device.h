/**
 * @file block_device.h
 * @brief Block Device Driver Interface
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define BLOCK_SIZE 512

// Block device operations
typedef struct {
    const char* name;
    uint32_t (*read)(uint32_t sector, void* buffer, uint32_t count);
    uint32_t (*write)(uint32_t sector, const void* buffer, uint32_t count);
    uint32_t (*get_sector_count)(void);
    bool (*is_ready)(void);
    void (*init)(void);
    void (*deinit)(void);
} block_device_ops_t;

// Block device structure
typedef struct {
    block_device_ops_t* ops;
    uint32_t sector_size;
    uint32_t sector_count;
    bool initialized;
    void* private_data;
} block_device_t;

// Function prototypes
int block_device_register(block_device_t* device);
int block_device_unregister(const char* name);
block_device_t* block_device_get(const char* name);
uint32_t block_device_read(block_device_t* device, uint32_t sector, void* buffer, uint32_t count);
uint32_t block_device_write(block_device_t* device, uint32_t sector, const void* buffer, uint32_t count);
uint32_t block_device_get_sector_count(block_device_t* device);
bool block_device_is_ready(block_device_t* device);
void block_device_init_all(void);

#endif // BLOCK_DEVICE_H

