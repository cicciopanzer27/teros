/**
 * @file ternary_vs_binary.c
 * @brief Benchmark comparison between ternary and binary operations
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "trit.h"
#include "ternary_alu.h"

#define BENCHMARK_ITERATIONS 1000000
#define BENCHMARK_WARMUP 1000

struct benchmark_results {
    uint64_t ternary_time;
    uint64_t binary_time;
    const char* test_name;
};

// Simple cycle counter (requires inline assembly)
static inline uint64_t rdtsc(void) {
    uint64_t low, high;
    asm volatile ("rdtsc" : "=a"(low), "=d"(high));
    return (high << 32) | low;
}

// Benchmark 1: Addition
void benchmark_addition(void) {
    uint64_t start, end;
    
    // Ternary addition
    start = rdtsc();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        trit_t a = trit_create((i % 3) - 1);
        trit_t b = trit_create(((i * 2) % 3) - 1);
        trit_add(a, b);
    }
    end = rdtsc();
    uint64_t ternary_time = end - start;
    
    // Binary addition
    start = rdtsc();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        int a = (i % 2);
        int b = ((i * 2) % 2);
        volatile int result = a + b; (void)result;
    }
    end = rdtsc();
    uint64_t binary_time = end - start;
    
    printf("Addition: Ternary=%lu cycles, Binary=%lu cycles (%.2f%% overhead)\n",
           ternary_time, binary_time, 
           100.0 * (ternary_time - binary_time) / binary_time);
}

// Benchmark 2: Logic Operations
void benchmark_logic_operations(void) {
    uint64_t start, end;
    
    // Ternary AND
    start = rdtsc();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        trit_t a = trit_create((i % 3) - 1);
        trit_t b = trit_create(((i * 2) % 3) - 1);
        trit_and(a, b);
    }
    end = rdtsc();
    uint64_t ternary_time = end - start;
    
    // Binary AND
    start = rdtsc();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        int a = (i % 2);
        int b = ((i * 2) % 2);
        volatile int result = a & b; (void)result;
    }
    end = rdtsc();
    uint64_t binary_time = end - start;
    
    printf("AND Logic: Ternary=%lu cycles, Binary=%lu cycles (%.2f%% overhead)\n",
           ternary_time, binary_time,
           100.0 * (ternary_time - binary_time) / binary_time);
}

// Benchmark 3: Comparisons
void benchmark_comparisons(void) {
    uint64_t start, end;
    
    // Ternary compare
    start = rdtsc();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        trit_t a = trit_create((i % 3) - 1);
        trit_t b = trit_create(((i * 7) % 3) - 1);
        trit_compare(a, b);
    }
    end = rdtsc();
    uint64_t ternary_time = end - start;
    
    // Binary compare
    start = rdtsc();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        int a = (i % 256);
        int b = ((i * 7) % 256);
        volatile int result = (a < b) - (a > b); (void)result;
    }
    end = rdtsc();
    uint64_t binary_time = end - start;
    
    printf("Comparison: Ternary=%lu cycles, Binary=%lu cycles (%.2f%% overhead)\n",
           ternary_time, binary_time,
           100.0 * (ternary_time - binary_time) / binary_time);
}

// Benchmark 4: Memory Operations
void benchmark_memory_operations(void) {
    uint64_t start, end;
    
    // Ternary memory access (simulated)
    start = rdtsc();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        volatile uint8_t value = (i % 3);
        (void)value;
    }
    end = rdtsc();
    uint64_t ternary_time = end - start;
    
    // Binary memory access
    start = rdtsc();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        volatile uint8_t value = (i % 2);
        (void)value;
    }
    end = rdtsc();
    uint64_t binary_time = end - start;
    
    printf("Memory: Ternary=%lu cycles, Binary=%lu cycles (%.2f%% overhead)\n",
           ternary_time, binary_time,
           100.0 * (ternary_time - binary_time) / binary_time);
}

int main(void) {
    printf("=== Ternary vs Binary Benchmark ===\n");
    printf("Iterations: %d\n\n", BENCHMARK_ITERATIONS);
    
    // Warmup
    for (int i = 0; i < BENCHMARK_WARMUP; i++) {
        trit_t a = trit_create((i % 3) - 1);
        (void)a;
    }
    
    benchmark_addition();
    benchmark_logic_operations();
    benchmark_comparisons();
    benchmark_memory_operations();
    
    printf("\n=== Benchmark Complete ===\n");
    return 0;
}

