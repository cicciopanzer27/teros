#!/usr/bin/env python3
"""
TEROS AI Code Generator
Use Ollama to generate code components from specifications
"""

import subprocess
import json
import sys
import os

def ollama_generate(prompt, model="codellama"):
    """Generate code using Ollama"""
    print(f"Generating code with {model}...")
    
    # Prepare prompt
    full_prompt = f"""You are a C programming expert creating code for TEROS operating system.
TEROS is a ternary operating system (uses trits instead of bits).

Requirements:
- Write clean, efficient C code
- Follow POSIX standards where applicable
- Handle NULL pointers safely
- Add proper error checking
- Include comments for clarity
- No memory leaks

{prompt}"""
    
    try:
        # Call Ollama
        result = subprocess.run(
            ["ollama", "run", model, full_prompt],
            capture_output=True,
            text=True,
            encoding='utf-8',
            errors='ignore',
            timeout=300
        )
        
        if result.returncode == 0:
            return result.stdout
        else:
            print(f"ERROR: {result.stderr}")
            return None
            
    except subprocess.TimeoutExpired:
        print("ERROR: Timeout generating code")
        return None
    except Exception as e:
        print(f"ERROR: {e}")
        return None

def generate_string_functions():
    """Generate libc string functions"""
    print("Generating string functions...")
    
    prompt = """
Implement these C string functions for TEROS libc in src/lib/libc/string.c:

Functions to implement:
1. size_t strlen(const char* str)
2. size_t strnlen(const char* str, size_t maxlen)
3. char* strcpy(char* dest, const char* src)
4. char* strncpy(char* dest, const char* src, size_t n)
5. char* strcat(char* dest, const char* src)
6. char* strncat(char* dest, const char* src, size_t n)
7. int strcmp(const char* s1, const char* s2)
8. int strncmp(const char* s1, const char* s2, size_t n)
9. const char* strchr(const char* s, int c)
10. const char* strrchr(const char* s, int c)

Requirements:
- Safe NULL pointer handling
- Proper bounds checking
- Always null-terminate strings
- Return appropriate values
- Clear comments
- Efficient implementation

Provide complete C code with all functions.
"""
    
    code = ollama_generate(prompt)
    
    if code:
        # Save to file
        os.makedirs("src/lib/libc", exist_ok=True)
        with open("src/lib/libc/string.c", "w") as f:
            f.write(code)
        print("SUCCESS: Generated src/lib/libc/string.c")
        return True
    return False

def generate_memory_functions():
    """Generate libc memory functions"""
    print("Generating memory functions...")
    
    prompt = """
Implement these C memory functions for TEROS libc in src/lib/libc/memory.c:

Functions to implement:
1. void* memset(void* dest, int value, size_t count)
2. void* memcpy(void* dest, const void* src, size_t count)
3. void* memmove(void* dest, const void* src, size_t count)
4. int memcmp(const void* ptr1, const void* ptr2, size_t count)
5. void* memchr(const void* ptr, int value, size_t count)

Requirements:
- Handle overlapping memory correctly (memmove)
- Safe NULL pointer handling
- Efficient implementation
- Clear comments

Provide complete C code with all functions.
"""
    
    code = ollama_generate(prompt)
    
    if code:
        os.makedirs("src/lib/libc", exist_ok=True)
        with open("src/lib/libc/memory.c", "w") as f:
            f.write(code)
        print("SUCCESS: Generated src/lib/libc/memory.c")
        return True
    return False

def generate_ls_command():
    """Generate ls command utility"""
    print("Generating ls command...")
    
    prompt = """
Implement the 'ls' command for TEROS in src/bin/ls.c.

Features:
- List directory contents
- Support for different file types (regular, directory, etc.)
- Basic formatting
- Call syscalls for directory operations

Requirements:
- Use syscalls (open, readdir, close)
- Proper error handling
- Clean output formatting
- Handle empty directories

Provide complete C code.
"""
    
    code = ollama_generate(prompt)
    
    if code:
        os.makedirs("src/bin", exist_ok=True)
        with open("src/bin/ls.c", "w") as f:
            f.write(code)
        print("SUCCESS: Generated src/bin/ls.c")
        return True
    return False

def generate_cat_command():
    """Generate cat command utility"""
    print("Generating cat command...")
    
    prompt = """
Implement the 'cat' command for TEROS in src/bin/cat.c.

Features:
- Display file contents
- Support reading from stdin
- Basic error handling

Requirements:
- Use syscalls (open, read, write, close)
- Handle file not found
- Read and write in chunks
- Proper resource cleanup

Provide complete C code.
"""
    
    code = ollama_generate(prompt)
    
    if code:
        os.makedirs("src/bin", exist_ok=True)
        with open("src/bin/cat.c", "w") as f:
            f.write(code)
        print("SUCCESS: Generated src/bin/cat.c")
        return True
    return False

def generate_echo_command():
    """Generate echo command utility"""
    print("Generating echo command...")
    
    prompt = """
Implement the 'echo' command for TEROS in src/bin/echo.c.

Features:
- Print arguments to stdout
- Support basic escape sequences

Requirements:
- Simple implementation
- Write to stdout
- Handle arguments correctly

Provide complete C code.
"""
    
    code = ollama_generate(prompt)
    
    if code:
        os.makedirs("src/bin", exist_ok=True)
        with open("src/bin/echo.c", "w") as f:
            f.write(code)
        print("SUCCESS: Generated src/bin/echo.c")
        return True
    return False

def generate_ps_command():
    """Generate ps command utility"""
    print("Generating ps command...")
    
    prompt = """
Implement the 'ps' command for TEROS in src/bin/ps.c.

Features:
- List running processes
- Display process information (PID, state, name)
- Call syscalls to get process list

Requirements:
- Use process syscalls
- Proper formatting
- Handle errors

Provide complete C code.
"""
    
    code = ollama_generate(prompt)
    
    if code:
        os.makedirs("src/bin", exist_ok=True)
        with open("src/bin/ps.c", "w") as f:
            f.write(code)
        print("SUCCESS: Generated src/bin/ps.c")
        return True
    return False

def generate_kill_command():
    """Generate kill command utility"""
    print("Generating kill command...")
    
    prompt = """
Implement the 'kill' command for TEROS in src/bin/kill.c.

Features:
- Send signals to processes
- Support different signal types

Requirements:
- Use signal syscalls
- Parse PID from arguments
- Error handling

Provide complete C code.
"""
    
    code = ollama_generate(prompt)
    
    if code:
        os.makedirs("src/bin", exist_ok=True)
        with open("src/bin/kill.c", "w") as f:
            f.write(code)
        print("SUCCESS: Generated src/bin/kill.c")
        return True
    return False

def generate_shell_command():
    """Generate basic shell"""
    print("Generating shell...")
    
    prompt = """
Implement a basic shell for TEROS in src/bin/sh.c.

Features:
- Command prompt
- Parse and execute commands
- Support built-in commands
- Support external commands via exec

Requirements:
- Read from stdin
- Parse command line
- Execute system commands
- Basic error handling

Provide complete C code.
"""
    
    code = ollama_generate(prompt)
    
    if code:
        os.makedirs("src/bin", exist_ok=True)
        with open("src/bin/sh.c", "w") as f:
            f.write(code)
        print("SUCCESS: Generated src/bin/sh.c")
        return True
    return False

def main():
    """Main generation function"""
    print("TEROS AI Code Generator")
    print("=" * 50)
    
    # Check if ollama is available
    try:
        subprocess.run(["ollama", "--version"], capture_output=True)
    except FileNotFoundError:
        print("ERROR: Ollama not found. Please install Ollama.")
        sys.exit(1)
    
    # Generate components
    components = [
        ("String Functions", generate_string_functions),
        ("Memory Functions", generate_memory_functions),
        ("ls command", generate_ls_command),
        ("cat command", generate_cat_command),
        ("echo command", generate_echo_command),
        ("ps command", generate_ps_command),
        ("kill command", generate_kill_command),
        ("shell", generate_shell_command),
    ]
    
    results = []
    for name, func in components:
        print(f"\nGenerating {name}...")
        success = func()
        results.append((name, success))
        if success:
            print(f"SUCCESS: {name} completed")
        else:
            print(f"FAILED: {name} failed")
    
    # Summary
    print("\n" + "=" * 50)
    print("Summary:")
    for name, success in results:
        status = "SUCCESS" if success else "FAILED"
        print(f"  {status}: {name}")
    
    print("\nGeneration complete!")

if __name__ == "__main__":
    main()

