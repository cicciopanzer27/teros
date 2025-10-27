# TEROS Musl Integration Script
# This script copies and adapts musl libc files into TEROS

Write-Host "=== TEROS Musl Integration ===" -ForegroundColor Green

# Create directories first
New-Item -ItemType Directory -Path "src\lib\libc\musl_stdio" -Force | Out-Null
New-Item -ItemType Directory -Path "src\lib\libc\musl_stdlib" -Force | Out-Null
New-Item -ItemType Directory -Path "src\lib\libc\musl_string" -Force | Out-Null

# Copy stdio files
Write-Host "Copying stdio files..." -ForegroundColor Yellow
Copy-Item "integrations\musl\src\stdio\*.c" -Destination "src\lib\libc\musl_stdio\" -Force

# Copy stdlib files  
Write-Host "Copying stdlib files..." -ForegroundColor Yellow
Copy-Item "integrations\musl\src\stdlib\*.c" -Destination "src\lib\libc\musl_stdlib\" -Force

# Copy string files
Write-Host "Copying string files..." -ForegroundColor Yellow
Copy-Item "integrations\musl\src\string\*.c" -Destination "src\lib\libc\musl_string\" -Force

Write-Host "=== Integration Complete ===" -ForegroundColor Green
Write-Host "Files copied to src\lib\libc\musl_*" -ForegroundColor Cyan

