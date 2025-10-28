@echo off
REM Build and Test Script for TEROS
REM This script builds the kernel and tests it in QEMU

echo ========================================
echo TEROS Build and Test Script
echo ========================================
echo.

REM Check if WSL is available
where wsl >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: WSL not found. Please install WSL2.
    pause
    exit /b 1
)

echo [1/5] Cleaning previous build...
wsl bash -c "cd /mnt/c/Users/jecho/Documents/GitHub/teros && make clean"

echo.
echo [2/5] Building kernel...
wsl bash -c "cd /mnt/c/Users/jecho/Documents/GitHub/teros && make"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo.
echo [3/5] Creating bootable ISO...
wsl bash -c "cd /mnt/c/Users/jecho/Documents/GitHub/teros && make iso"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: ISO creation failed!
    pause
    exit /b 1
)

echo.
echo [4/5] Verifying kernel binary...
if not exist "bin\teros.bin" (
    echo ERROR: Kernel binary not found!
    pause
    exit /b 1
)

dir bin\teros.bin

echo.
echo [5/5] Starting QEMU...
echo Press Ctrl+A then X to exit QEMU
echo.

REM Run QEMU with the ISO
wsl bash -c "cd /mnt/c/Users/jecho/Documents/GitHub/teros && qemu-system-x86_64 -cdrom bin/teros.iso -serial stdio -m 512M -display none"

echo.
echo Test complete.
pause

