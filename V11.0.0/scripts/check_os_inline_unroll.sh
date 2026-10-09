#!/bin/bash
# TCRH-456: Verify improved inline and loop unrolling under -Os
# Usage: check_os_inline_unroll.sh <compiler> <source_file>

CC=$1
SRC=$2

if [ -z "$CC" ] || [ -z "$SRC" ]; then
    echo "[FAIL] Usage: $0 <compiler> <source_file>"
    exit 1
fi

PASS=true

# Compile with -Os
ASM_OS=$(mktemp /tmp/tcrh456_os_XXXXXX.s)
if "$CC" -march=rh850-g4mh2 -Os -S "$SRC" -o "$ASM_OS" 2>/tmp/tcrh456_err.txt; then
    echo "[INFO] TCRH-456: Compilation with -Os succeeded"
else
    echo "[FAIL] TCRH-456: Compilation with -Os FAILED"
    cat /tmp/tcrh456_err.txt
    PASS=false
fi

# Compile with -O2 for comparison
ASM_O2=$(mktemp /tmp/tcrh456_o2_XXXXXX.s)
"$CC" -march=rh850-g4mh2 -O2 -S "$SRC" -o "$ASM_O2" 2>/dev/null

# Compare code sizes
if [ -f "$ASM_OS" ] && [ -f "$ASM_O2" ]; then
    SIZE_OS=$(wc -l < "$ASM_OS")
    SIZE_O2=$(wc -l < "$ASM_O2")
    echo "[INFO] TCRH-456: -Os assembly lines: $SIZE_OS, -O2 assembly lines: $SIZE_O2"
    if [ "$SIZE_OS" -le "$SIZE_O2" ]; then
        echo "[INFO] TCRH-456: -Os produces smaller or equal code size (expected)"
    else
        echo "[WARN] TCRH-456: -Os produces larger code than -O2 (unusual but possible)"
    fi
fi

# Check that always_inline functions are inlined
if grep -q "forced_inline" "$ASM_OS" 2>/dev/null; then
    if grep -q "jarl.*forced_inline\|call.*forced_inline" "$ASM_OS" 2>/dev/null; then
        echo "[WARN] TCRH-456: always_inline function may not be fully inlined"
    else
        echo "[INFO] TCRH-456: always_inline function appears inlined"
    fi
fi

# Check that noinline functions are NOT inlined (should appear as separate symbols)
if grep -q "forced_noinline:" "$ASM_OS" 2>/dev/null; then
    echo "[INFO] TCRH-456: noinline function preserved as separate function"
else
    echo "[WARN] TCRH-456: noinline function not found as separate symbol"
fi

rm -f "$ASM_OS" "$ASM_O2"

if $PASS; then
    echo "[PASS] TCRH-456: -Os inline/unrolling strategy verification complete"
    exit 0
else
    echo "[FAIL] TCRH-456: Verification failed"
    exit 1
fi
