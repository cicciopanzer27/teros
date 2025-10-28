/**
 * @file ata.h
 * @brief ATA/SATA Disk Driver Header
 */

#ifndef ATA_H
#define ATA_H

#include <stdint.h>
#include <stdbool.h>

// ATA ports
#define ATA_PRIMARY_BASE     0x1F0
#define ATA_SECONDARY_BASE   0x170

// ATA registers (relative to base)
#define ATA_REG_DATA         0x00
#define ATA_REG_ERROR        0x01
#define ATA_REG_FEATURES     0x01
#define ATA_REG_SECTOR_COUNT 0x02
#define ATA_REG_LBA_LOW      0x03
#define ATA_REG_LBA_MID      0x04
#define ATA_REG_LBA_HIGH     0x05
#define ATA_REG_DEVICE       0x06
#define ATA_REG_STATUS       0x07
#define ATA_REG_COMMAND      0x07

// ATA commands
#define ATA_CMD_READ_PIO     0x20
#define ATA_CMD_WRITE_PIO    0x30
#define ATA_CMD_IDENTIFY     0xEC

// Status bits
#define ATA_STATUS_ERR       0x01
#define ATA_STATUS_DRQ       0x08
#define ATA_STATUS_BUSY      0x80

typedef struct {
    uint16_t base_port;
    bool initialized;
    uint32_t sector_size;
    uint64_t total_sectors;
} ata_device_t;

int ata_init(uint16_t base_port);
int ata_read_sector(ata_device_t* device, uint64_t lba, void* buffer, size_t sector_count);
int ata_write_sector(ata_device_t* device, uint64_t lba, const void* buffer, size_t sector_count);
int ata_identify_device(ata_device_t* device);

#endif // ATA_H

