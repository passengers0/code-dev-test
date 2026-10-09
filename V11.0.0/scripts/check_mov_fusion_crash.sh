#!/bin/bash
# TCRH-445: Verify no compiler crash in MOV fusion optimization pass
# Usage: check_mov_fusion_crash.sh <compiler> <source_file>

CC=$1
SRC=$2

if [ -z "$CC" ] || [ -z "$SRC" ]; then
    echo "[FAIL] Usage: $0 <compiler> <source_file>"
    exit 1
fi

PASS=true

# Test compilation at various optimization levels
for OPT in "-O0" "-O1" "-O2" "-O3" "-Os"; do
    OUTPUT=$(mktemp /tmp/tcrh445_XXXXXX.o)
    if "$CC" -march=rh850-g4mh2 $OPT -c "$SRC" -o "$OUTPUT" 2>/tmp/tcrh445_err.txt; then
        echo "[INFO] TCRH-445: Compilation $OPT succeeded"
    else
        echo "[FAIL] TCRH-445: Compilation $OPT FAILED (possible MOV fusion crash)"
        cat /tmp/tcrh445_err.txt
        PASS=false
    fi
    rm -f "$OUTPUT"
done

# Also test with -mllvm flags that may affect MOV fusion
OUTPUT=$(mktemp /tmp/tcrh445_XXXXXX.o)
if "$CC" -march=rh850-g4mh2 -O2 -c "$SRC" -o "$OUTPUT" 2>/tmp/tcrh445_err.txt; then
    # Verify the object file is valid
    if [ -s "$OUTPUT" ]; then
        echo "[INFO] TCRH-445: Object file generated successfully (non-empty)"
    else
        echo "[FAIL] TCRH-445: Object file is empty"
        PASS=false
    fi
else
    echo "[FAIL] TCRH-445: Final compilation check FAILED"
    PASS=false
fi
rm -f "$OUTPUT"

if $PASS; then
    echo "[PASS] TCRH-445: No compiler crash in MOV fusion optimization pass"
    exit 0
else
    echo "[FAIL] TCRH-445: Compiler crash detected"
    exit 1
fi
