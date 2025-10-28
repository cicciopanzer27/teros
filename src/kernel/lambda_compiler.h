/**
 * @file lambda_compiler.h
 * @brief Lambda³ to T3 Compiler Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef LAMBDA_COMPILER_H
#define LAMBDA_COMPILER_H

#include "lambda_engine.h"
#include <stdint.h>

/**
 * Compile lambda term to T3 bytecode
 * 
 * @param term Lambda term to compile
 * @param bytecode Output buffer for bytecode
 * @param capacity Size of bytecode buffer
 * @param output_size Pointer to receive actual size (can be NULL)
 * @return 0 on success, -1 on error
 */
int32_t lambda_compile_to_t3(LambdaTerm* term, uint8_t* bytecode, 
                              int32_t capacity, int32_t* output_size);

/**
 * Optimize lambda term
 * 
 * @param term Term to optimize
 * @return Optimized term (new allocation)
 */
LambdaTerm* lambda_optimize_term(LambdaTerm* term);

#endif // LAMBDA_COMPILER_H

