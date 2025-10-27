#!/bin/bash
# TEROS Launcher - Prova QEMU WSL, poi fallback a QEMU Windows

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo ""
echo "╔════════════════════════════════════════════════╗"
echo "║     TEROS - Ternary Operating System          ║"
echo "║            Terminal Launcher                   ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

# Verifica ISO
if [ ! -f "bin/teros.iso" ]; then
    echo "❌ ERRORE: bin/teros.iso non trovato!"
    echo ""
    echo "Esegui prima:"
    echo "  make iso"
    exit 1
fi

echo "✅ ISO trovata: bin/teros.iso ($(du -h bin/teros.iso | cut -f1))"
echo ""

# Prova 1: QEMU WSL
if command -v qemu-system-x86_64 &> /dev/null; then
    echo "Tentativo 1: QEMU nel WSL..."
    if qemu-system-x86_64 --version &> /dev/null; then
        echo "✅ QEMU WSL funzionante!"
        echo ""
        echo "🚀 Avvio TEROS con QEMU WSL..."
        echo "Per uscire: Ctrl+A, poi X"
        echo ""
        
        exec qemu-system-x86_64 \
            -cdrom bin/teros.iso \
            -m 512M \
            -serial stdio \
            -display none \
            -no-reboot
    else
        echo "⚠️  QEMU WSL ha errori (probabilmente glibc)"
        echo ""
    fi
fi

# Prova 2: QEMU Windows
echo "Tentativo 2: QEMU per Windows..."
QEMU_WIN=""
QEMU_PATHS=(
    "/mnt/c/Program Files/qemu/qemu-system-x86_64.exe"
    "/mnt/c/Program Files (x86)/qemu/qemu-system-x86_64.exe"
    "/mnt/c/qemu/qemu-system-x86_64.exe"
    "/mnt/c/qemu/qemu-system-x86_64w.exe"
)

for path in "${QEMU_PATHS[@]}"; do
    if [ -f "$path" ]; then
        QEMU_WIN="$path"
        break
    fi
done

if [ -n "$QEMU_WIN" ]; then
    echo "✅ QEMU Windows trovato: $(basename "$QEMU_WIN")"
    echo ""
    
    WIN_PATH=$(wslpath -w "$SCRIPT_DIR/bin/teros.iso")
    
    echo "🚀 Avvio TEROS con QEMU Windows..."
    echo "Per uscire: Chiudi finestra QEMU o Ctrl+C"
    echo ""
    
    exec "$QEMU_WIN" \
        -cdrom "$WIN_PATH" \
        -m 512M \
        -serial stdio
fi

# Nessuna opzione disponibile
echo "❌ Nessun emulatore disponibile da terminale!"
echo ""
echo "SOLUZIONI:"
echo ""
echo "1. INSTALLA QEMU PER WINDOWS:"
echo "   - Scarica: https://qemu.weilnetz.de/w64/"
echo "   - Installa in: C:\\Program Files\\qemu"
echo "   - Rilancia: ./run_teros.sh"
echo ""
echo "2. USA VIRTUALBOX (interfaccia grafica):"
echo "   - Guida completa: VIRTUALBOX_SETUP.md"
echo "   - Scarica: https://www.virtualbox.org"
echo ""
echo "3. AGGIORNA WSL/QEMU:"
echo "   sudo pacman -Syu"
echo "   sudo pacman -S qemu-system-x86"
echo ""

exit 1
