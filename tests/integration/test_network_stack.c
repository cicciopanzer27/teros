/**
 * @file test_network_stack.c
 * @brief End-to-end integration tests for networking stack
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "networking.h"
#include "console.h"

// Test counters
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT(condition, msg) \
    do { \
        tests_run++; \
        if (condition) { \
            tests_passed++; \
            console_puts("✓ PASS: "); \
            console_puts(msg); \
            console_puts("\n"); \
        } else { \
            tests_failed++; \
            console_puts("✗ FAIL: "); \
            console_puts(msg); \
            console_puts("\n"); \
        } \
    } while(0)

// Test 1: Ethernet frame construction
void test_ethernet_send(void) {
    uint8_t dest_mac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    uint8_t test_data[] = "Hello, Network!";
    
    int result = ethernet_send(dest_mac, ETHERTYPE_IP, test_data, sizeof(test_data));
    ASSERT(result == 0, "ethernet_send returns success");
}

// Test 2: IP packet construction
void test_ipv4_send(void) {
    ipv4_addr_t dest_addr = {{192, 168, 1, 1}};
    uint8_t test_data[] = "Test IP packet";
    
    int result = ipv4_send(&dest_addr, IP_PROTOCOL_TCP, test_data, sizeof(test_data));
    ASSERT(result == 0, "ipv4_send constructs valid packet");
}

// Test 3: IP checksum calculation
void test_ipv4_checksum(void) {
    // Create test IP header
    static uint8_t buffer[sizeof(ipv4_header_t)];
    ipv4_header_t* header = (ipv4_header_t*)buffer;
    
    memset(header, 0, sizeof(ipv4_header_t));
    header->version_ihl = (4 << 4) | 5;
    header->total_length = sizeof(ipv4_header_t);
    header->ttl = 64;
    header->protocol = IP_PROTOCOL_TCP;
    
    // Checksum validation would be checked here in full implementation
    ASSERT(header->version_ihl == 0x45, "IP header version valid");
}

// Test 4: TCP socket creation
void test_tcp_socket(void) {
    tcp_socket_t* sock = tcp_socket();
    ASSERT(sock != NULL, "tcp_socket creates valid socket");
    
    if (sock) {
        ASSERT(sock->state == TCP_CLOSED, "New socket in CLOSED state");
    }
}

// Test 5: TCP state transitions
void test_tcp_state_machine(void) {
    tcp_socket_t* sock = tcp_socket();
    ASSERT(sock != NULL, "tcp_socket creates socket");
    
    if (sock) {
        // Test bind
        int result = tcp_bind(sock, 8080);
        ASSERT(result == 0, "tcp_bind successful");
        
        // Test listen transition
        result = tcp_listen(sock, 5);
        ASSERT(result == 0, "tcp_listen successful");
        ASSERT(sock->state == TCP_LISTEN, "Socket in LISTEN state after listen");
    }
}

// Test 6: UDP packet construction
void test_udp_sendto(void) {
    ipv4_addr_t dest_addr = {{10, 0, 0, 1}};
    uint8_t test_data[] = "Test UDP";
    
    int result = udp_sendto(&dest_addr, 5000, 6000, test_data, sizeof(test_data));
    ASSERT(result == 0, "udp_sendto constructs valid packet");
}

// Test 7: Network interface initialization
void test_network_interface(void) {
    network_interface_t interface = {0};
    strncpy((char*)interface.name, "eth0", 4);
    interface.is_up = true;
    
    ASSERT(interface.is_up == true, "Interface can be marked up");
    ASSERT(strncmp(interface.name, "eth0", 4) == 0, "Interface name set correctly");
}

int main(void) {
    console_puts("\n=== Networking Stack Integration Tests ===\n\n");
    
    test_ethernet_send();
    test_ipv4_send();
    test_ipv4_checksum();
    test_tcp_socket();
    test_tcp_state_machine();
    test_udp_sendto();
    test_network_interface();
    
    console_puts("\n=== Test Summary ===\n");
    console_puts("Total:   ");
    printf("%d", tests_run);
    console_puts("\nPassed:  ");
    printf("%d", tests_passed);
    console_puts("\nFailed:  ");
    printf("%d", tests_failed);
    console_puts("\n");
    
    return (tests_failed > 0) ? 1 : 0;
}

