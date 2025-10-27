# String Functions - Delegate Specification

## Purpose
Implementare tutte le funzioni di manipolazione stringhe della C standard library per TEROS libc.

## API Interface
```c
// src/lib/libc/string.h

size_t strlen(const char* str);
size_t strnlen(const char* str, size_t maxlen);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
const char* strchr(const char* s, int c);
const char* strrchr(const char* s, int c);
const char* strstr(const char* haystack, const char* needle);
const char* strpbrk(const char* s, const char* accept);
size_t strspn(const char* s, const char* accept);
size_t strcspn(const char* s, const char* reject);
char* strtok(char* str, const char* delim);
char* strtok_r(char* str, const char* delim, char** saveptr);
int strcoll(const char* s1, const char* s2);
size_t strxfrm(char* dest, const char* src, size_t n);
char* strdup(const char* s);
char* strndup(const char* s, size_t n);
```

## Dependencies
- Requires: stddef.h, stdint.h
- Provides: Complete C string library
- No kernel dependencies

## Implementation Requirements
- [ ] All functions must handle NULL pointers (no crashes)
- [ ] Bounds checking for strncpy, strncat, etc.
- [ ] Null terminator always added
- [ ] No buffer overflows
- [ ] ASCII character handling only
- [ ] Optimized for common cases
- [ ] Thread-safe where applicable (strtok_r)

## Tests
### Unit Tests
```c
// tests/unit/test_string.c

void test_strlen() {
    assert(strlen("hello") == 5);
    assert(strlen("") == 0);
    assert(strlen(NULL) == 0);
}

void test_strcpy() {
    char buf[10];
    strcpy(buf, "hello");
    assert(strcmp(buf, "hello") == 0);
}

// ... more tests for each function
```

### Integration Tests
- Test string operations in real scenarios
- Memory safety tests
- Boundary condition tests

## Code Style
- TEROS style (see existing codebase)
- Header comments with function description
- No magic numbers
- Clear variable names
- Error handling consistent with other code

## References
- C11 Standard (ISO/IEC 9899:2011) - String handling
- Source code reference: musl libc, glibc

## Deliverables
- **File**: `src/lib/libc/string.c`
- **Header**: `src/lib/libc/string.h`
- **Tests**: `tests/unit/test_string.c`
- **Size**: ~1000-1500 lines

## Priority
**HIGH** - Needed for basic functionality

