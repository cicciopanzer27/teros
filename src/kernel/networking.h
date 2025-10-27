/**
 * @file networking.h
 * @brief Networking Stack Interface
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef NETWORKING_H
#define NETWORKING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// =============================================================================
// IP ADDRESSES
// =============================================================================

typedef struct {
    uint8_t addr[4];
} ipv4_addr_t;

typedef struct {
    uint16_t addr[8];
} ipv6_addr_t;

// =============================================================================
// ETHERNET
// =============================================================================

typedef struct {
    uint8_t dest_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;
    uint8_t data[];
} __attribute__((packed)) ethernet_frame_t;

#define ETHERTYPE_IP  0x0800
#define ETHERTYPE_ARP 0x0806
#define ETHERTYPE_IPV6 0x86DD

// =============================================================================
// IP HEADER
// =============================================================================

typedef struct {
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_frag_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    ipv4_addr_t src_addr;
    ipv4_addr_t dest_addr;
} __attribute__((packed)) ipv4_header_t;

#define IP_PROTOCOL_TCP 6
#define IP_PROTOCOL_UDP 17
#define IP_PROTOCOL_ICMP 1

// =============================================================================
// TCP
// =============================================================================

typedef struct {
    uint16_t src_port;
    uint16_t dest_port;
    uint32_t seq_num;
    uint32_t ack_num;
    uint8_t data_offset;
    uint8_t flags;
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_ptr;
} __attribute__((packed)) tcp_header_t;

#define TCP_FLAG_FIN 0x01
#define TCP_FLAG_SYN 0x02
#define TCP_FLAG_RST 0x04
#define TCP_FLAG_PSH 0x08
#define TCP_FLAG_ACK 0x10
#define TCP_FLAG_URG 0x20

typedef enum {
    TCP_CLOSED,
    TCP_LISTEN,
    TCP_SYN_SENT,
    TCP_SYN_RECEIVED,
    TCP_ESTABLISHED,
    TCP_FIN_WAIT_1,
    TCP_FIN_WAIT_2,
    TCP_CLOSE_WAIT,
    TCP_CLOSING,
    TCP_TIME_WAIT
} tcp_state_t;

typedef struct tcp_socket {
    uint32_t local_addr;
    uint16_t local_port;
    uint32_t remote_addr;
    uint16_t remote_port;
    tcp_state_t state;
    uint32_t seq_num;
    uint32_t ack_num;
    uint32_t send_window;
    uint32_t recv_window;
    struct tcp_socket* next;
} tcp_socket_t;

// =============================================================================
// UDP
// =============================================================================

typedef struct {
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed)) udp_header_t;

// =============================================================================
// NETWORK INTERFACE
// =============================================================================

typedef struct {
    const char* name;
    uint8_t mac_addr[6];
    ipv4_addr_t ip_addr;
    ipv4_addr_t subnet_mask;
    ipv4_addr_t gateway;
    bool is_up;
    void* driver_data;
} network_interface_t;

// =============================================================================
// FUNCTION PROTOTYPES
// =============================================================================

// Ethernet
int ethernet_send(const uint8_t* dest_mac, uint16_t ethertype, const void* data, size_t len);
int ethernet_receive(ethernet_frame_t* frame, size_t len);

// IP
int ipv4_send(ipv4_addr_t* dest, uint8_t protocol, const void* data, size_t len);
int ipv4_receive(ipv4_header_t* header, const void* data, size_t len);

// TCP
tcp_socket_t* tcp_socket(void);
int tcp_connect(tcp_socket_t* socket, ipv4_addr_t* addr, uint16_t port);
int tcp_bind(tcp_socket_t* socket, uint16_t port);
int tcp_listen(tcp_socket_t* socket, int backlog);
tcp_socket_t* tcp_accept(tcp_socket_t* socket);
ssize_t tcp_send(tcp_socket_t* socket, const void* buf, size_t len);
ssize_t tcp_recv(tcp_socket_t* socket, void* buf, size_t len);
int tcp_close(tcp_socket_t* socket);

// UDP
int udp_sendto(ipv4_addr_t* dest, uint16_t dest_port, uint16_t src_port, const void* buf, size_t len);
ssize_t udp_recvfrom(ipv4_addr_t* src, uint16_t* src_port, void* buf, size_t len);

// Network Interface
int network_interface_register(network_interface_t* iface);
int network_interface_up(const char* name);
int network_interface_down(const char* name);
network_interface_t* network_interface_get(const char* name);

// Network Stack Initialization
int networking_init(void);

#endif // NETWORKING_H

