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

#define MAX_NETWORK_INTERFACES 8
#define MAX_TCP_SOCKETS 128
#define TCP_BUFFER_SIZE 8192

static network_interface_t* interfaces[MAX_NETWORK_INTERFACES];
static int interface_count = 0;

static tcp_socket_t* tcp_sockets[MAX_TCP_SOCKETS];
static int tcp_socket_count = 0;
static uint16_t next_port = 32768;

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

int ethernet_send(const uint8_t* dest_mac, uint16_t ethertype, const void* data, size_t len) {
    if (dest_mac == NULL || data == NULL || len == 0) {
        return -1;
    }
    
    // TODO: Implement Ethernet frame transmission
    console_puts("NETWORK: Sending Ethernet frame (");
    printf("%zu", len);
    console_puts(" bytes)\n");
    
    return 0;
}

int ethernet_receive(ethernet_frame_t* frame, size_t len) {
    if (frame == NULL || len < sizeof(ethernet_frame_t)) {
        return -1;
    }
    
    // TODO: Process received frame
    console_puts("NETWORK: Received Ethernet frame (");
    printf("%zu", len);
    console_puts(" bytes)\n");

    // Dispatch based on ethertype
    switch (frame->ethertype) {
        case ETHERTYPE_IP:
            // TODO: Process IP packet
            console_puts("NETWORK: Received IP packet\n");
            break;
        case ETHERTYPE_ARP:
            // TODO: Process ARP
            console_puts("NETWORK: Received ARP packet\n");
            break;
        default:
            break;
    }
    
    return 0;
}

int ipv4_send(ipv4_addr_t* dest, uint8_t protocol, const void* data, size_t len) {
    if (dest == NULL || data == NULL || len == 0) {
        return -1;
    }
    
    // TODO: Construct and send IP packet
    console_puts("NETWORK: Sending IP packet to ");
    printf("%u.%u.%u.%u", dest->octets[0], dest->octets[1], dest->octets[2], dest->octets[3]);
    console_puts("\n");
    
    return 0;
}

int ipv4_receive(ipv4_header_t* header, const void* data, size_t len) {
    if (header == NULL) {
        return -1;
    }
    
    // TODO: Process IP packet
    console_puts("NETWORK: Received IP packet (protocol: ");
    printf("%u", header->protocol);
    console_puts(")\n");

    // Dispatch based on protocol
    switch (header->protocol) {
        case IP_PROTOCOL_TCP:
            // TODO: Process TCP
            console_puts("NETWORK: TCP packet received\n");
            break;
        case IP_PROTOCOL_UDP:
            // TODO: Process UDP
            console_puts("NETWORK: UDP packet received\n");
            break;
        case IP_PROTOCOL_ICMP:
            // TODO: Process ICMP
            break;
        default:
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

int tcp_listen(tcp_socket_t* socket, int backlog) {
    if (socket == NULL || backlog <= 0) {
        return -1;
    }
    
    socket->state = TCP_LISTEN;
    return 0;
}

int tcp_connect(tcp_socket_t* socket, ipv4_addr_t* addr, uint16_t port) {
    if (socket == NULL || addr == NULL) {
        return -1;
    }
    
    socket->remote_addr = *(uint32_t*)addr;
    socket->remote_port = port;
    socket->state = TCP_SYN_SENT;
    
    // TODO: Send SYN packet
    console_puts("NETWORK: TCP connecting to ");
    // TODO: Print addr and port
    console_puts("\n");
    
    return 0;
}

tcp_socket_t* tcp_accept(tcp_socket_t* socket) {
    if (socket == NULL || socket->state != TCP_LISTEN) {
        return NULL;
    }
    
    // TODO: Wait for incoming connection
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
    
    // TODO: Send TCP data
    console_puts("NETWORK: TCP sending ");
    // TODO: Print len
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
    
    // TODO: Receive TCP data
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
            // TODO: kfree(socket);
            break;
        }
    }
    
    return 0;
}

int udp_sendto(ipv4_addr_t* dest, uint16_t dest_port, uint16_t src_port, const void* buf, size_t len) {
    if (dest == NULL || buf == NULL || len == 0) {
        return -1;
    }
    
    // TODO: Send UDP packet
    console_puts("NETWORK: UDP sending to ");
    // TODO: Print address
    console_puts("\n");
    
    return 0;
}

ssize_t udp_recvfrom(ipv4_addr_t* src, uint16_t* src_port, void* buf, size_t len) {
    if (buf == NULL || len == 0) {
        return -1;
    }
    
    // TODO: Receive UDP packet
    return 0;
}

int networking_init(void) {
    console_puts("NETWORK: Initializing networking stack...\n");
    
    network_init();
    
    console_puts("NETWORK: Initialized\n");
    
    return 0;
}

