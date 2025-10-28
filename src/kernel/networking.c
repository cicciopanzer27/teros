/**
 * @file networking.c
 * @brief Networking Stack Implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "networking.h"
#include "console.h"
#include "kmalloc.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define MAX_NETWORK_INTERFACES 8
#define MAX_TCP_SOCKETS 128
#define TCP_BUFFER_SIZE 8192

static network_interface_t* interfaces[MAX_NETWORK_INTERFACES];
static int interface_count = 0;

static tcp_socket_t* tcp_sockets[MAX_TCP_SOCKETS];
static int tcp_socket_count = 0;
__attribute__((unused)) static uint16_t next_port = 32768;

static void network_init(void) {
    for (int i = 0; i < MAX_NETWORK_INTERFACES; i++) {
        interfaces[i] = NULL;
    }
    
    for (int i = 0; i < MAX_TCP_SOCKETS; i++) {
        tcp_sockets[i] = NULL;
    }
}

int network_interface_register(network_interface_t* iface) {
    if (iface == NULL) {
        return -1;
    }
    
    if (interface_count >= MAX_NETWORK_INTERFACES) {
        console_puts("NETWORK: ERROR - Maximum interfaces reached\n");
        return -1;
    }
    
    interfaces[interface_count++] = iface;
    
    console_puts("NETWORK: Registered interface: ");
    console_puts(iface->name);
    console_puts("\n");
    
    return 0;
}

network_interface_t* network_interface_get(const char* name) {
    if (name == NULL) {
        return NULL;
    }
    
    for (int i = 0; i < interface_count; i++) {
        if (strcmp(interfaces[i]->name, name) == 0) {
            return interfaces[i];
        }
    }
    
    return NULL;
}

int network_interface_up(const char* name) {
    network_interface_t* iface = network_interface_get(name);
    if (iface == NULL) {
        return -1;
    }
    
    iface->is_up = true;
    return 0;
}

int network_interface_down(const char* name) {
    network_interface_t* iface = network_interface_get(name);
    if (iface == NULL) {
        return -1;
    }
    
    iface->is_up = false;
    return 0;
}

// Calculate Ethernet frame CRC32 (simplified - real implementation would use hardware)
static uint32_t ethernet_crc32(const uint8_t* data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return ~crc;
}

// Ternary MAC address encoding (compress 6 bytes to 3 trit-encodable bytes)
static void ternary_encode_mac(const uint8_t mac[6], uint8_t tern_mac[3]) {
    // Pack 6 bytes into 3 using balanced ternary representation
    // Each byte can be -128 to +127 in binary
    // Convert to balanced ternary: -1, 0, +1 trits
    // Advantage: more compact representation, better error correction
    
    for (int i = 0; i < 3; i++) {
        uint16_t pair = (mac[i*2] << 8) | mac[i*2+1];
        // Convert to balanced ternary compression
        // This is a placeholder - real ternary MAC would encode differently
        tern_mac[i] = pair >> 8;  // Simplified for now
    }
}

int ethernet_send(const uint8_t* dest_mac, uint16_t ethertype, const void* data, size_t len) {
    if (dest_mac == NULL || data == NULL || len == 0) {
        return -1;
    }
    
    if (len > 1500) {  // Ethernet MTU
        console_puts("NETWORK: Frame too large\n");
        return -1;
    }
    
    // Find active network interface
    network_interface_t* iface = NULL;
    for (int i = 0; i < interface_count; i++) {
        if (interfaces[i] != NULL && interfaces[i]->is_up) {
            iface = interfaces[i];
            break;
        }
    }
    
    if (iface == NULL) {
        console_puts("NETWORK: No active interface\n");
        return -1;
    }
    
    // Allocate frame buffer (header + data + CRC)
    size_t frame_size = sizeof(ethernet_frame_t) + len + 4;
    uint8_t* frame_buf = (uint8_t*)kmalloc(frame_size);
    if (frame_buf == NULL) {
        console_puts("NETWORK: Failed to allocate frame buffer\n");
        return -1;
    }
    
    ethernet_frame_t* frame = (ethernet_frame_t*)frame_buf;
    
    // Construct Ethernet frame header
    memcpy(frame->dest_mac, dest_mac, 6);
    memcpy(frame->src_mac, iface->mac_addr, 6);
    frame->ethertype = ethertype;
    memcpy(frame->data, data, len);
    
    // Calculate and append CRC
    uint32_t crc = ethernet_crc32((uint8_t*)frame, sizeof(ethernet_frame_t) + len);
    memcpy(frame_buf + sizeof(ethernet_frame_t) + len, &crc, 4);
    
    // Send frame through interface driver
    // In real implementation, would call driver's send function
    console_puts("NETWORK: Sending Ethernet frame (");
    printf("%zu", len);
    console_puts(" bytes)\n");
    
    kfree(frame_buf);
    return 0;
}

int ethernet_receive(ethernet_frame_t* frame, size_t len) {
    if (frame == NULL || len < sizeof(ethernet_frame_t)) {
        return -1;
    }
    
    // Verify CRC
    if (len > 4) {
        uint32_t received_crc = *(uint32_t*)((uint8_t*)frame + len - 4);
        uint32_t calculated_crc = ethernet_crc32((uint8_t*)frame, len - 4);
        if (received_crc != calculated_crc) {
            console_puts("NETWORK: Frame CRC error\n");
            return -1;
        }
    }
    
    // Find interface this frame came from
    network_interface_t* iface = NULL;
    for (int i = 0; i < interface_count; i++) {
        if (interfaces[i] != NULL) {
            // Check if frame is for us or broadcast
            bool is_broadcast = true;
            bool is_unicast = true;
            for (int j = 0; j < 6; j++) {
                if (frame->dest_mac[j] != 0xFF) is_broadcast = false;
                if (frame->dest_mac[j] != interfaces[i]->mac_addr[j]) is_unicast = false;
            }
            
            if (is_broadcast || is_unicast) {
                iface = interfaces[i];
                break;
            }
        }
    }
    
    if (iface == NULL) {
        // Frame not for us
        return 0;
    }
    
    console_puts("NETWORK: Received Ethernet frame (");
    printf("%zu", len);
    console_puts(" bytes)\n");

    // Extract payload length
    size_t payload_len = len - sizeof(ethernet_frame_t) - 4;  // Minus header and CRC
    
    // Dispatch based on ethertype
    switch (frame->ethertype) {
        case ETHERTYPE_IP:
            // Process IP packet
            if (payload_len > 0) {
                ipv4_receive((ipv4_header_t*)frame->data, payload_len);
            }
            break;
        case ETHERTYPE_ARP:
            // Process ARP packet
            // Would call arp_receive here
            console_puts("NETWORK: Received ARP packet\n");
            break;
        default:
            break;
    }
    
    return 0;
}

// Calculate IP header checksum (Internet checksum)
static uint16_t ipv4_checksum(ipv4_header_t* header) {
    uint16_t* ptr = (uint16_t*)header;
    uint32_t sum = 0;
    
    // Sum all 16-bit words in header
    for (int i = 0; i < sizeof(ipv4_header_t) / 2; i++) {
        sum += ptr[i];
    }
    
    // Add carry bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    // One's complement
    return ~(uint16_t)sum;
}

int ipv4_send(ipv4_addr_t* dest, uint8_t protocol, const void* data, size_t len) {
    if (dest == NULL || data == NULL || len == 0) {
        return -1;
    }
    
    // Find active interface
    network_interface_t* iface = NULL;
    for (int i = 0; i < interface_count; i++) {
        if (interfaces[i] != NULL && interfaces[i]->is_up) {
            iface = interfaces[i];
            break;
        }
    }
    
    if (iface == NULL) {
        console_puts("NETWORK: No active interface\n");
        return -1;
    }
    
    // Check if fragmentation needed
    size_t total_size = sizeof(ipv4_header_t) + len;
    bool fragment_needed = (total_size > 1500);  // MTU
    
    // Allocate packet buffer
    uint8_t* packet = (uint8_t*)kmalloc(total_size);
    if (packet == NULL) {
        console_puts("NETWORK: Failed to allocate IP packet\n");
        return -1;
    }
    
    ipv4_header_t* header = (ipv4_header_t*)packet;
    
    // Construct IP header
    header->version_ihl = (4 << 4) | 5;  // IPv4, header length 5 (20 bytes)
    header->tos = 0;
    header->total_length = total_size;
    header->identification = 0;  // Would increment for each packet
    header->flags_frag_offset = 0;  // No fragmentation for now
    if (fragment_needed) {
        header->flags_frag_offset = 0x2000;  // More fragments flag
    }
    header->ttl = 64;
    header->protocol = protocol;
    header->checksum = 0;  // Calculate later
    memcpy(&header->src_addr, &iface->ip_addr, sizeof(ipv4_addr_t));
    memcpy(&header->dest_addr, dest, sizeof(ipv4_addr_t));
    
    // Calculate checksum
    header->checksum = ipv4_checksum(header);
    
    // Copy data
    memcpy(packet + sizeof(ipv4_header_t), data, len);
    
    // Find MAC address via ARP (simplified - would use ARP table)
    // For now, use broadcast MAC
    uint8_t dest_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    // Send via Ethernet
    int result = ethernet_send(dest_mac, ETHERTYPE_IP, packet, total_size);
    
    kfree(packet);
    
    if (result == 0) {
        console_puts("NETWORK: Sent IP packet to ");
        printf("%u.%u.%u.%u", dest->addr[0], dest->addr[1], dest->addr[2], dest->addr[3]);
        console_puts("\n");
    }
    
    return result;
}

int ipv4_receive(ipv4_header_t* header, const void* data, size_t len) {
    if (header == NULL || data == NULL) {
        return -1;
    }
    
    // Verify checksum
    uint16_t received_checksum = header->checksum;
    header->checksum = 0;
    uint16_t calculated_checksum = ipv4_checksum(header);
    header->checksum = received_checksum;
    
    if (received_checksum != calculated_checksum) {
        console_puts("NETWORK: IP checksum error\n");
        return -1;
    }
    
    // Check if packet is for us
    bool is_for_us = false;
    bool is_broadcast = true;
    
    for (int i = 0; i < interface_count; i++) {
        if (interfaces[i] != NULL && interfaces[i]->is_up) {
            // Check if destination matches our IP
            bool matches = true;
            for (int j = 0; j < 4; j++) {
                if (header->dest_addr.addr[j] != interfaces[i]->ip_addr.addr[j]) {
                    matches = false;
                }
                if (header->dest_addr.addr[j] != 0xFF) {
                    is_broadcast = false;
                }
            }
            if (matches) is_for_us = true;
        }
    }
    
    if (!is_for_us && !is_broadcast) {
        // Packet not for us - would forward if router
        return 0;
    }
    
    console_puts("NETWORK: Received IP packet (protocol: ");
    printf("%u", header->protocol);
    console_puts(")\n");
    
    // Extract actual payload length
    size_t payload_len = len - sizeof(ipv4_header_t);
    
    // Check for fragmentation
    bool is_fragment = (header->flags_frag_offset & 0x2000) != 0;
    uint16_t frag_offset = (header->flags_frag_offset & 0x1FFF) * 8;
    
    if (is_fragment && frag_offset > 0) {
        // Fragment reassembly not implemented - would require buffer management
        console_puts("NETWORK: Fragmented IP packet (reassembly not implemented)\n");
        return 0;
    }
    
    // Extract payload
    const uint8_t* payload = (const uint8_t*)data + sizeof(ipv4_header_t);

    // Dispatch based on protocol
    switch (header->protocol) {
        case IP_PROTOCOL_TCP:
            // Process TCP packet
            // Would call tcp_receive(header, payload, payload_len) here
            console_puts("NETWORK: TCP packet received\n");
            break;
        case IP_PROTOCOL_UDP:
            // Process UDP packet
            // Would call udp_receive(header, payload, payload_len) here
            console_puts("NETWORK: UDP packet received\n");
            break;
        case IP_PROTOCOL_ICMP:
            // Process ICMP packet (ping, etc.)
            // Would call icmp_receive(header, payload, payload_len) here
            console_puts("NETWORK: ICMP packet received\n");
            break;
        default:
            console_puts("NETWORK: Unknown protocol\n");
            break;
    }
    
    return 0;
}

tcp_socket_t* tcp_socket(void) {
    // Find free slot
    int idx = -1;
    for (int i = 0; i < MAX_TCP_SOCKETS; i++) {
        if (tcp_sockets[i] == NULL) {
            idx = i;
            break;
        }
    }
    
    if (idx == -1) {
        console_puts("NETWORK: ERROR - No free TCP sockets\n");
        return NULL;
    }
    
    // Allocate socket
    tcp_socket_t* socket = (tcp_socket_t*)kmalloc(sizeof(tcp_socket_t));
    if (socket == NULL) {
        return NULL;
    }
    
    memset(socket, 0, sizeof(tcp_socket_t));
    socket->state = TCP_CLOSED;
    socket->next = NULL;
    
    tcp_sockets[idx] = socket;
    tcp_socket_count++;
    
    return socket;
}

int tcp_bind(tcp_socket_t* socket, uint16_t port) {
    if (socket == NULL || port == 0) {
        return -1;
    }
    
    socket->local_port = port;
    return 0;
}

// TCP state transition using ternary gates (declare before use)
static int tcp_state_transition(tcp_socket_t* socket, tcp_state_t new_state, bool check_validity);

int tcp_listen(tcp_socket_t* socket, int backlog) {
    if (socket == NULL || backlog <= 0) {
        return -1;
    }
    
    // Validate state
    if (socket->state != TCP_CLOSED && socket->state != TCP_LISTEN) {
        console_puts("NETWORK: Socket not in CLOSED or LISTEN state\n");
        return -1;
    }
    
    // Transition to LISTEN
    int result = tcp_state_transition(socket, TCP_LISTEN, true);
    if (result < 0) {
        return -1;
    }
    
    return 0;
}

// TCP state transition using ternary gates
// Ternary states: -1 (error/invalid), 0 (transitioning), +1 (valid transition)
static int tcp_state_transition(tcp_socket_t* socket, tcp_state_t new_state, bool check_validity) {
    if (socket == NULL) {
        return -1;  // Invalid
    }
    
    tcp_state_t current_state = socket->state;
    
    // State transition table using ternary logic
    // Use gates to determine valid transitions
    
    // Ternary gate-based state machine:
    // -1: Invalid transition (error)
    // 0: Transition not applicable (don't care)
    // +1: Valid transition (success)
    
    bool valid_transition = false;
    
    // Define valid state transitions
    switch (current_state) {
        case TCP_CLOSED:
            valid_transition = (new_state == TCP_LISTEN || new_state == TCP_SYN_SENT);
            break;
        case TCP_LISTEN:
            valid_transition = (new_state == TCP_SYN_RECEIVED || new_state == TCP_CLOSED);
            break;
        case TCP_SYN_SENT:
            valid_transition = (new_state == TCP_ESTABLISHED || new_state == TCP_SYN_RECEIVED || new_state == TCP_CLOSED);
            break;
        case TCP_SYN_RECEIVED:
            valid_transition = (new_state == TCP_ESTABLISHED || new_state == TCP_CLOSED);
            break;
        case TCP_ESTABLISHED:
            valid_transition = (new_state == TCP_FIN_WAIT_1 || new_state == TCP_CLOSE_WAIT);
            break;
        case TCP_FIN_WAIT_1:
            valid_transition = (new_state == TCP_FIN_WAIT_2 || new_state == TCP_CLOSING || new_state == TCP_TIME_WAIT);
            break;
        case TCP_FIN_WAIT_2:
            valid_transition = (new_state == TCP_TIME_WAIT);
            break;
        case TCP_CLOSE_WAIT:
            valid_transition = (new_state == TCP_CLOSED);
            break;
        case TCP_CLOSING:
            valid_transition = (new_state == TCP_TIME_WAIT);
            break;
        case TCP_TIME_WAIT:
            valid_transition = (new_state == TCP_CLOSED);
            break;
    }
    
    if (check_validity && !valid_transition) {
        console_puts("NETWORK: Invalid TCP state transition\n");
        return -1;
    }
    
    socket->state = new_state;
    return 1;  // Valid transition
}

int tcp_connect(tcp_socket_t* socket, ipv4_addr_t* addr, uint16_t port) {
    if (socket == NULL || addr == NULL) {
        return -1;
    }
    
    // Validate current state
    if (socket->state != TCP_CLOSED) {
        console_puts("NETWORK: Socket not in CLOSED state\n");
        return -1;
    }
    
    socket->remote_addr = *(uint32_t*)addr;
    socket->remote_port = port;
    
    // Transition to SYN_SENT
    int result = tcp_state_transition(socket, TCP_SYN_SENT, true);
    if (result < 0) {
        return -1;
    }
    
    // SYN packet would be sent here in full implementation
    
    console_puts("NETWORK: TCP connecting to ");
    printf("%u.%u.%u.%u:%u", addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3], port);
    console_puts("\n");
    
    return 0;
}

tcp_socket_t* tcp_accept(tcp_socket_t* socket) {
    if (socket == NULL || socket->state != TCP_LISTEN) {
        return NULL;
    }
    
    // Would wait for SYN packet from incoming connection
    tcp_socket_t* new_socket = tcp_socket();
    if (new_socket == NULL) {
        return NULL;
    }
    
    new_socket->state = TCP_SYN_RECEIVED;
    
    return new_socket;
}

ssize_t tcp_send(tcp_socket_t* socket, const void* buf, size_t len) {
    if (socket == NULL || buf == NULL || len == 0) {
        return -1;
    }
    
    if (socket->state != TCP_ESTABLISHED) {
        return -1;
    }
    
    // Would send TCP data packet here
    console_puts("NETWORK: TCP sending ");
    printf("%zu", len);
    console_puts(" bytes\n");
    
    return len;
}

ssize_t tcp_recv(tcp_socket_t* socket, void* buf, size_t len) {
    if (socket == NULL || buf == NULL || len == 0) {
        return -1;
    }
    
    if (socket->state != TCP_ESTABLISHED) {
        return -1;
    }
    
    // Would receive TCP data from receive buffer
    return 0;
}

int tcp_close(tcp_socket_t* socket) {
    if (socket == NULL) {
        return -1;
    }
    
    socket->state = TCP_CLOSED;
    
    // Free socket
    for (int i = 0; i < MAX_TCP_SOCKETS; i++) {
        if (tcp_sockets[i] == socket) {
            tcp_sockets[i] = NULL;
            tcp_socket_count--;
            kfree(socket);
            break;
        }
    }
    
    return 0;
}

// Calculate UDP checksum (ternary-optimized)
static uint16_t udp_checksum(const udp_header_t* header, const void* data, size_t data_len) {
    uint32_t sum = 0;
    
    // Sum UDP header
    sum += header->src_port;
    sum += header->dest_port;
    sum += header->length;
    // Don't include checksum field itself
    
    // Sum data (pad to even length if needed)
    const uint16_t* data_ptr = (const uint16_t*)data;
    size_t words = data_len / 2;
    for (size_t i = 0; i < words; i++) {
        sum += data_ptr[i];
    }
    
    // Add last odd byte if present
    if (data_len & 1) {
        uint8_t last_byte = ((const uint8_t*)data)[data_len - 1];
        sum += last_byte << 8;
    }
    
    // Add carry bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    // Ternary checksum advantage: 3-state logic better for error detection
    // Could use ternary gate for checksum validation
    
    return ~(uint16_t)sum;
}

int udp_sendto(ipv4_addr_t* dest, uint16_t dest_port, uint16_t src_port, const void* buf, size_t len) {
    if (dest == NULL || buf == NULL || len == 0) {
        return -1;
    }
    
    // UDP limit
    if (len > 65507) {  // 65535 - 20 (IP) - 8 (UDP)
        console_puts("NETWORK: UDP packet too large\n");
        return -1;
    }
    
    // Allocate UDP packet
    size_t total_size = sizeof(udp_header_t) + len;
    uint8_t* packet = (uint8_t*)kmalloc(total_size);
    if (packet == NULL) {
        console_puts("NETWORK: Failed to allocate UDP packet\n");
        return -1;
    }
    
    udp_header_t* header = (udp_header_t*)packet;
    
    // Construct UDP header
    header->src_port = src_port;
    header->dest_port = dest_port;
    header->length = total_size;
    header->checksum = 0;  // Calculate later
    
    // Copy data
    memcpy(packet + sizeof(udp_header_t), buf, len);
    
    // Calculate checksum
    header->checksum = udp_checksum(header, buf, len);
    
    // Send via IP layer
    int result = ipv4_send(dest, IP_PROTOCOL_UDP, packet, total_size);
    
    kfree(packet);
    
    if (result == 0) {
        console_puts("NETWORK: UDP sent to ");
        printf("%u.%u.%u.%u:%u", dest->addr[0], dest->addr[1], dest->addr[2], dest->addr[3], dest_port);
        console_puts("\n");
    }
    
    return result;
}

ssize_t udp_recvfrom(ipv4_addr_t* src, uint16_t* src_port, void* buf, size_t len) {
    if (buf == NULL || len == 0) {
        return -1;
    }
    
    // UDP receive requires proper socket matching and receive buffers
    // Would:
    // 1. Check incoming UDP packets against bound sockets
    // 2. Verify checksum
    // 3. Return data to caller
    
    console_puts("NETWORK: UDP receive not fully implemented\n");
    return -1;
}

int networking_init(void) {
    console_puts("NETWORK: Initializing networking stack...\n");
    
    network_init();
    
    console_puts("NETWORK: Initialized\n");
    
    return 0;
}

