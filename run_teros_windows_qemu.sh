#!/bin/bash
# TEROS Launcher usando QEMU di Windows dal WSL

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo ""
echo "╔════════════════════════════════════════════════╗"
echo "║  TEROS - Avvio con QEMU Windows dal WSL        ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

# Verifica ISO
if [ ! -f "bin/teros.iso" ]; then
    echo "❌ ERRORE: bin/teros.iso non trovato!"
    echo "Esegui prima: make iso"
    exit 1
fi

echo "✅ ISO trovata: bin/teros.iso"
echo ""

# Cerca QEMU di Windows
QEMU_WIN=""
QEMU_PATHS=(
    "/mnt/c/Program Files/qemu/qemu-system-x86_64.exe"
    "/mnt/c/Program Files (x86)/qemu/qemu-system-x86_64.exe"
    "/mnt/c/qemu/qemu-system-x86_64.exe"
)

for path in "${QEMU_PATHS[@]}"; do
    if [ -f "$path" ]; then
        QEMU_WIN="$path"
        break
    fi
done

if [ -z "$QEMU_WIN" ]; then
    echo "❌ QEMU per Windows non trovato!"
    echo ""
    echo "OPZIONE 1: Installa QEMU per Windows"
    echo "  1. Scarica: https://qemu.weilnetz.de/w64/"
    echo "  2. Installa in: C:\\Program Files\\qemu"
    echo "  3. Rilancia questo script"
    echo ""
    echo "OPZIONE 2: Usa VirtualBox (vedi VIRTUALBOX_SETUP.md)"
    echo ""
    exit 1
fi

echo "✅ QEMU trovato: $QEMU_WIN"
echo ""

# Converti path Windows per ISO
WIN_PATH=$(wslpath -w "$SCRIPT_DIR/bin/teros.iso")

echo "🚀 Avvio TEROS..."
echo ""
echo "Opzioni:"
echo "  -cdrom: $WIN_PATH"
echo "  -m: 512M RAM"
echo "  -serial: stdio"
echo ""
echo "Per uscire da QEMU: Premi Ctrl+C in questo terminale"
echo ""
echo "Premere Enter per continuare..."
read

echo ""
echo "═══════════════════════════════════════════════"
echo " TEROS BOOTING..."
echo "═══════════════════════════════════════════════"
echo ""

# Avvia QEMU Windows
"$QEMU_WIN" \
    -cdrom "$WIN_PATH" \
    -m 512M \
    -serial stdio \
    2>&1 || {
    EXIT_CODE=$?
    echo ""
    echo "═══════════════════════════════════════════════"
    echo " QEMU terminato (exit code: $EXIT_CODE)"
    echo "═══════════════════════════════════════════════"
    exit $EXIT_CODE
}

echo ""
echo "✅ TEROS terminato."

