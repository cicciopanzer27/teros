/**
 * @file t3_linker.c
 * @brief Ternary linker for symbol resolution and executable generation
 * @author TEROS Development Team
 * @date 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// STRUCTURES
// =============================================================================

typedef struct {
    char* name;
    uint32_t address;
    uint8_t type;  // 0=undefined, 1=defined, 2=external
} symbol_t;

typedef struct {
    char* section_name;
    uint8_t* data;
    size_t size;
    uint32_t base_address;
    uint32_t alignment;
} section_t;

typedef struct {
    uint32_t offset;
    uint32_t symbol_index;
    uint8_t relocation_type;
} relocation_entry_t;

typedef struct {
    char filename[256];
    symbol_t* symbols;
    size_t symbol_count;
    size_t symbol_capacity;
    section_t* sections;
    size_t section_count;
    size_t section_capacity;
    relocation_entry_t* relocations;
    size_t relocation_count;
    size_t relocation_capacity;
} object_file_t;

typedef struct {
    object_file_t* files;
    size_t file_count;
    size_t file_capacity;
    symbol_t* global_symbols;
    size_t global_symbol_count;
    size_t global_symbol_capacity;
    uint32_t base_address;
} linker_t;

// =============================================================================
// LINKER INITIALIZATION
// =============================================================================

linker_t* t3_linker_create(void) {
    linker_t* linker = malloc(sizeof(linker_t));
    if (linker == NULL) return NULL;
    
    linker->files = NULL;
    linker->file_count = 0;
    linker->file_capacity = 0;
    linker->global_symbols = NULL;
    linker->global_symbol_count = 0;
    linker->global_symbol_capacity = 0;
    linker->base_address = 0x100000;  // Default base address
    
    return linker;
}

void t3_linker_destroy(linker_t* linker) {
    if (linker == NULL) return;
    
    for (size_t i = 0; i < linker->file_count; i++) {
        object_file_t* obj = &linker->files[i];
        
        for (size_t j = 0; j < obj->symbol_count; j++) {
            if (obj->symbols[j].name) {
                free(obj->symbols[j].name);
            }
        }
        free(obj->symbols);
        
        for (size_t j = 0; j < obj->section_count; j++) {
            if (obj->sections[j].section_name) {
                free(obj->sections[j].section_name);
            }
            if (obj->sections[j].data) {
                free(obj->sections[j].data);
            }
        }
        free(obj->sections);
        free(obj->relocations);
    }
    free(linker->files);
    
    for (size_t i = 0; i < linker->global_symbol_count; i++) {
        if (linker->global_symbols[i].name) {
            free(linker->global_symbols[i].name);
        }
    }
    free(linker->global_symbols);
    
    free(linker);
}

// =============================================================================
// OBJECT FILE LOADING
// =============================================================================

object_file_t* t3_linker_load_object(linker_t* linker, const char* filename) {
    if (linker == NULL || filename == NULL) {
        return NULL;
    }
    
    // Resize files array
    if (linker->file_count >= linker->file_capacity) {
        size_t new_capacity = linker->file_capacity == 0 ? 8 : linker->file_capacity * 2;
        object_file_t* new_files = realloc(linker->files, new_capacity * sizeof(object_file_t));
        if (new_files == NULL) {
            return NULL;
        }
        linker->files = new_files;
        linker->file_capacity = new_capacity;
    }
    
    object_file_t* obj = &linker->files[linker->file_count];
    memset(obj, 0, sizeof(object_file_t));
    strncpy(obj->filename, filename, sizeof(obj->filename) - 1);
    
    obj->symbols = NULL;
    obj->symbol_count = 0;
    obj->symbol_capacity = 0;
    obj->sections = NULL;
    obj->section_count = 0;
    obj->section_capacity = 0;
    obj->relocations = NULL;
    obj->relocation_count = 0;
    obj->relocation_capacity = 0;
    
    linker->file_count++;
    return obj;
}

// =============================================================================
// SYMBOL RESOLUTION
// =============================================================================

int t3_linker_find_symbol(linker_t* linker, const char* name) {
    if (linker == NULL || name == NULL) {
        return -1;
    }
    
    // Search in global symbol table
    for (size_t i = 0; i < linker->global_symbol_count; i++) {
        if (strcmp(linker->global_symbols[i].name, name) == 0) {
            return (int)i;
        }
    }
    
    return -1;
}

bool t3_linker_add_global_symbol(linker_t* linker, const char* name, uint32_t address, uint8_t type) {
    if (linker == NULL || name == NULL) {
        return false;
    }
    
    // Check if symbol already exists
    int idx = t3_linker_find_symbol(linker, name);
    if (idx >= 0) {
        // Update existing symbol
        linker->global_symbols[idx].address = address;
        linker->global_symbols[idx].type = type;
        return true;
    }
    
    // Resize symbol array
    if (linker->global_symbol_count >= linker->global_symbol_capacity) {
        size_t new_capacity = linker->global_symbol_capacity == 0 ? 32 : linker->global_symbol_capacity * 2;
        symbol_t* new_symbols = realloc(linker->global_symbols, new_capacity * sizeof(symbol_t));
        if (new_symbols == NULL) {
            return false;
        }
        linker->global_symbols = new_symbols;
        linker->global_symbol_capacity = new_capacity;
    }
    
    // Add new symbol
    symbol_t* sym = &linker->global_symbols[linker->global_symbol_count];
    sym->name = malloc(strlen(name) + 1);
    if (sym->name == NULL) {
        return false;
    }
    strcpy(sym->name, name);
    sym->address = address;
    sym->type = type;
    linker->global_symbol_count++;
    
    return true;
}

// =============================================================================
// LINKING PROCESS
// =============================================================================

bool t3_linker_resolve_symbols(linker_t* linker) {
    if (linker == NULL) {
        return false;
    }
    
    // Two-pass resolution
    // Pass 1: Collect all symbols
    for (size_t i = 0; i < linker->file_count; i++) {
        object_file_t* obj = &linker->files[i];
        
        for (size_t j = 0; j < obj->symbol_count; j++) {
            symbol_t* sym = &obj->symbols[j];
            
            if (sym->type == 1) {  // Defined
                t3_linker_add_global_symbol(linker, sym->name, sym->address, sym->type);
            }
        }
    }
    
    // Pass 2: Resolve undefined symbols
    for (size_t i = 0; i < linker->file_count; i++) {
        object_file_t* obj = &linker->files[i];
        
        for (size_t j = 0; j < obj->symbol_count; j++) {
            symbol_t* sym = &obj->symbols[j];
            
            if (sym->type == 0) {  // Undefined
                int global_idx = t3_linker_find_symbol(linker, sym->name);
                if (global_idx < 0) {
                    fprintf(stderr, "Unresolved symbol: %s\n", sym->name);
                    return false;
                }
                sym->address = linker->global_symbols[global_idx].address;
                sym->type = 1;  // Now resolved
            }
        }
    }
    
    return true;
}

bool t3_linker_apply_relocations(linker_t* linker) {
    if (linker == NULL) {
        return false;
    }
    
    // Apply relocations to sections
    for (size_t i = 0; i < linker->file_count; i++) {
        object_file_t* obj = &linker->files[i];
        
        for (size_t j = 0; j < obj->relocation_count; j++) {
            relocation_entry_t* reloc = &obj->relocations[j];
            symbol_t* sym = &obj->symbols[reloc->symbol_index];
            
            // Find target section
            for (size_t k = 0; k < obj->section_count; k++) {
                section_t* sect = &obj->sections[k];
                
                // Check if offset is within section
                uint32_t local_offset = reloc->offset - sect->base_address;
                if (local_offset < sect->size) {
                    // Apply relocation (simplified - just write address)
                    uint32_t target_addr = sym->address;
                    
                    if (sect->data != NULL) {
                        memcpy(&sect->data[local_offset], &target_addr, sizeof(uint32_t));
                    }
                    break;
                }
            }
        }
    }
    
    return true;
}

// =============================================================================
// EXECUTABLE GENERATION
// =============================================================================

bool tyl_linker_generate_executable(linker_t* linker, const char* output_filename) {
    if (linker == NULL || output_filename == NULL) {
        return false;
    }
    
    FILE* f = fopen(output_filename, "wb");
    if (f == NULL) {
        return false;
    }
    
    // Write header (simplified)
    uint32_t magic = 0x5445524F;  // "TERO"
    fwrite(&magic, sizeof(uint32_t), 1, f);
    
    uint32_t entry_point = linker->base_address;
    fwrite(&entry_point, sizeof(uint32_t), 1, f);
    
    // Write sections
    for (size_t i = 0; i < linker->file_count; i++) {
        object_file_t* obj = &linker->files[i];
        
        for (size_t j = 0; j < obj->section_count; j++) {
            section_t* sect = &obj->sections[j];
            
            if (sect->data != NULL && sect->size > 0) {
                fwrite(sect->data, sect->size, 1, f);
            }
        }
    }
    
    fclose(f);
    return true;
}

// =============================================================================
// MAIN LINKER API
// =============================================================================

bool t3_linker_link(linker_t* linker, const char* output_filename) {
    if (linker == NULL || output_filename == NULL) {
        return false;
    }
    
    printf("Linking...\n");
    
    // Step 1: Resolve symbols
    if (!t3_linker_resolve_symbols(linker)) {
        fprintf(stderr, "Symbol resolution failed\n");
        return false;
    }
    
    printf("Resolved %zu symbols\n", linker->global_symbol_count);
    
    // Step 2: Apply relocations
    if (!t3_linker_apply_relocations(linker)) {
        fprintf(stderr, "Relocation failed\n");
        return false;
    }
    
    printf("Applied relocations\n");
    
    // Step 3: Generate executable
    if (!t3_linker_generate_executable(linker, output_filename)) {
        fprintf(stderr, "Executable generation failed\n");
        return false;
    }
    
    printf("Generated executable: %s\n", output_filename);
    return true;
}

// =============================================================================
// MAIN ENTRY POINT
// =============================================================================

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <output> <input1> [input2] ...\n", argv[0]);
        return 1;
    }
    
    const char* output = argv[1];
    
    linker_t* linker = t3_linker_create();
    if (linker == NULL) {
        fprintf(stderr, "Failed to create linker\n");
        return 1;
    }
    
    // Load object files
    for (int i = 2; i < argc; i++) {
        object_file_t* obj = t3_linker_load_object(linker, argv[i]);
        if (obj == NULL) {
            fprintf(stderr, "Failed to load object file: %s\n", argv[i]);
            t3_linker_destroy(linker);
            return 1;
        }
        printf("Loaded: %s\n", argv[i]);
    }
    
    // Link
    if (!t3_linker_link(linker, output)) {
        t3_linker_destroy(linker);
        return 1;
    }
    
    t3_linker_destroy(linker);
    return 0;
}

