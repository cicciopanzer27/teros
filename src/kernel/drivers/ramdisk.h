/**
 * @file ramdisk.h
 * @brief RAM Disk Block Device Driver Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef RAMDISK_H
#define RAMDISK_H

#include <stdint.h>
#include "block_device.h"

/**
 * ramdisk_register - Register ramdisk as a block device
 * @return: 0 on success, -1 on failure
 */
int ramdisk_register(void);

/**
 * ramdisk_get_device - Get ramdisk block device
 * @return: Pointer to block device structure
 */
block_device_t* ramdisk_get_device(void);

/**
 * ramdisk_format - Format the ramdisk (clear all data)
 */
void ramdisk_format(void);

/**
 * ramdisk_get_size - Get ramdisk size in bytes
 * @return: Size in bytes
 */
uint32_t ramdisk_get_size(void);

/**
 * ramdisk_get_sector_size - Get sector size in bytes
 * @return: Sector size in bytes
 */
uint32_t ramdisk_get_sector_size(void);

#endif /* RAMDISK_H */

