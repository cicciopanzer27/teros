/**
 * @file e1000.h
 * @brief Intel E1000 Ethernet Controller Driver Header
 */

#ifndef E1000_H
#define E1000_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uintptr_t mmio_base;
    uintptr_t io_base;
} e1000_device_t;

int e1000_init(uintptr_t mmio_base, uintptr_t io_base);
int e1000_send_packet(const uint8_t* buffer, size_t length);
int e1000_receive_packet(uint8_t* buffer, size_t max_length);
int e1000_get_mac_address(uint8_t* mac_addr);

#endif // E1000_H

