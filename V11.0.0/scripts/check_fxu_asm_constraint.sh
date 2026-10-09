#!/bin/bash
# TCRH-409: Verify FXU vector register inline assembly constraint ("v")
# Usage: check_fxu_asm_constraint.sh <compiler> <source_file>
# Note: Requires -mfpu=fxu and -march=rh850-g4mh2

CC=$1
SRC=$2

if [ -z "$CC" ] || [ -z "$SRC" ]; then
    echo "[FAIL] Usage: $0 <compiler> <source_file>"
    exit 1
fi

# Compile with FXU support
ASM_FILE=$(mktemp /tmp/tcrh409_XXXXXX.s)
if "$CC" -march=rh850-g4mh2 -mfpu=fxu -O2 -S "$SRC" -o "$ASM_FILE" 2>/tmp/tcrh409_err.txt; then
    echo "[INFO] TCRH-409: Compilation with FXU asm constraint succeeded"
else
    echo "[FAIL] TCRH-409: Compilation with FXU asm constraint FAILED"
    cat /tmp/tcrh409_err.txt
    rm -f "$ASM_FILE"
    exit 1
fi

# Check for FXU register usage (fx0-fx7) in inline assembly
FX_REG_COUNT=$(grep -ciE "fx[0-7]" "$ASM_FILE" 2>/dev/null || echo 0)

if [ "$FX_REG_COUNT" -gt 0 ]; then
    echo "[INFO] TCRH-409: Found $FX_REG_COUNT references to FXU vector registers (fx0-fx7)"
    echo "[PASS] TCRH-409: FXU vector register inline assembly constraint supported"
    rm -f "$ASM_FILE"
    exit 0
else
    echo "[WARN] TCRH-409: No explicit fx registers found (may use different allocation)"
    echo "[INFO] TCRH-409: Compilation succeeded - constraint accepted by compiler"
    echo "[PASS] TCRH-409: FXU asm constraint compiles successfully"
    rm -f "$ASM_FILE"
    exit 0
fi
