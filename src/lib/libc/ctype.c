/**
 * @file ctype.c
 * @brief Character classification and conversion functions
 * @note Adapted from musl libc (MIT License)
 * @author TEROS Development Team
 * @date 2025
 */

#include "ctype.h"

int isalnum(int c) {
    return isalpha(c) || isdigit(c);
}

int isalpha(int c) {
    return ((unsigned)c - 'A' < 26) || ((unsigned)c - 'a' < 26);
}

int isblank(int c) {
    return c == ' ' || c == '\t';
}

int iscntrl(int c) {
    return (unsigned)c < 0x20 || c == 0x7f;
}

int isdigit(int c) {
    return (unsigned)c - '0' < 10;
}

int isgraph(int c) {
    return (unsigned)c - 0x21 < 0x5e;
}

int islower(int c) {
    return (unsigned)c - 'a' < 26;
}

int isprint(int c) {
    return (unsigned)c - 0x20 < 0x5f;
}

int ispunct(int c) {
    return isgraph(c) && !isalnum(c);
}

int isspace(int c) {
    return c == ' ' || (unsigned)c - '\t' < 5;
}

int isupper(int c) {
    return (unsigned)c - 'A' < 26;
}

int isxdigit(int c) {
    return isdigit(c) || ((unsigned)c | 32) - 'a' < 6;
}

int tolower(int c) {
    if (isupper(c)) return c | 32;
    return c;
}

int toupper(int c) {
    if (islower(c)) return c & 0x5f;
    return c;
}

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

