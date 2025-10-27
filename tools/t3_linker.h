/**
 * @file t3_linker.h
 * @brief Ternary linker header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef T3_LINKER_H
#define T3_LINKER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Forward declarations
typedef struct linker_t linker_t;
typedef struct object_file_t object_file_t;

// Linker API
linker_t* t3_linker_create(void);
void t3_linker_destroy(linker_t* linker);

// Object file loading
object_file_t* t3_linker_load_object(linker_t* linker, const char* filename);

// Symbol resolution
int t3_linker_find_symbol(linker_t* linker, const char* name);
bool t3_linker_add_global_symbol(linker_t* linker, const char* name, uint32_t address, uint8_t type);

// Linking process
bool t3_linker_resolve_symbols(linker_t* linker);
bool t3_linker_apply_relocations(linker_t* linker);
bool t3_linker_generate_executable(linker_t* linker, const char* output_filename);

// Main API
bool t3_linker_link(linker_t* linker, const char* output_filename);

#endif // T3_LINKER_H

