/**
 * @file ctype.h
 * @brief Character classification and conversion header
 * @note Adapted from musl libc (MIT License)
 */

#ifndef CTYPE_H
#define CTYPE_H

#include <stdint.h>

int isalnum(int c);
int isalpha(int c);
int isblank(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);

int tolower(int c);
int toupper(int c);

#endif // CTYPE_H

