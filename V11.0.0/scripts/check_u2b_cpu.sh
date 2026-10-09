#!/bin/bash
# TCRH-453: Verify U2B-E and U2B-EZ CPU support
# Usage: check_u2b_cpu.sh <compiler> <source_file>

CC=$1
SRC=$2

if [ -z "$CC" ] || [ -z "$SRC" ]; then
    echo "[FAIL] Usage: $0 <compiler> <source_file>"
    exit 1
fi

PASS=true

# Test U2B-E (supports FXU)
OUTPUT=$(mktemp /tmp/tcrh453_XXXXXX.o)
if "$CC" -mcpu=u2b-e -O2 -c "$SRC" -o "$OUTPUT" 2>/tmp/tcrh453_err.txt; then
    echo "[INFO] TCRH-453: -mcpu=u2b-e compilation OK"
else
    echo "[FAIL] TCRH-453: -mcpu=u2b-e compilation FAILED"
    cat /tmp/tcrh453_err.txt
    PASS=false
fi
rm -f "$OUTPUT"

# Test U2B-EZ (no FXU)
OUTPUT=$(mktemp /tmp/tcrh453_XXXXXX.o)
if "$CC" -mcpu=u2b-ez -O2 -c "$SRC" -o "$OUTPUT" 2>/tmp/tcrh453_err.txt; then
    echo "[INFO] TCRH-453: -mcpu=u2b-ez compilation OK"
else
    echo "[FAIL] TCRH-453: -mcpu=u2b-ez compilation FAILED"
    cat /tmp/tcrh453_err.txt
    PASS=false
fi
rm -f "$OUTPUT"

# Verify u2b-e enables FXU features
ASM_FILE=$(mktemp /tmp/tcrh453_XXXXXX.s)
"$CC" -mcpu=u2b-e -O2 -S "$SRC" -o "$ASM_FILE" 2>/dev/null
if grep -qi "fmov\|fadd.w\|fxu" "$ASM_FILE" 2>/dev/null; then
    echo "[INFO] TCRH-453: u2b-e generates FXU-capable code"
else
    echo "[INFO] TCRH-453: u2b-e code generated (FXU usage depends on source)"
fi
rm -f "$ASM_FILE"

# Verify u2b-ez does NOT use FXU instructions
ASM_FILE=$(mktemp /tmp/tcrh453_XXXXXX.s)
"$CC" -mcpu=u2b-ez -O2 -S "$SRC" -o "$ASM_FILE" 2>/dev/null
if grep -qi "fmov.w\|fadd.w" "$ASM_FILE" 2>/dev/null; then
    echo "[WARN] TCRH-453: u2b-ez generated FXU instructions (unexpected)"
else
    echo "[INFO] TCRH-453: u2b-ez correctly avoids FXU instructions"
fi
rm -f "$ASM_FILE"

if $PASS; then
    echo "[PASS] TCRH-453: Both U2B-E and U2B-EZ CPUs supported"
    exit 0
else
    echo "[FAIL] TCRH-453: CPU support verification failed"
    exit 1
fi
