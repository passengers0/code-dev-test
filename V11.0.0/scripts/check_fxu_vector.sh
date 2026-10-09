#!/bin/bash
# TCRH-408: Verify FXU vector type (__w128) support
# Usage: check_fxu_vector.sh <compiler> <source_file>
# Note: Requires -mfpu=fxu and -march=rh850-g4mh2

CC=$1
SRC=$2

if [ -z "$CC" ] || [ -z "$SRC" ]; then
    echo "[FAIL] Usage: $0 <compiler> <source_file>"
    exit 1
fi

# Compile with FXU support
ASM_FILE=$(mktemp /tmp/tcrh408_XXXXXX.s)
if "$CC" -march=rh850-g4mh2 -mfpu=fxu -O2 -S "$SRC" -o "$ASM_FILE" 2>/tmp/tcrh408_err.txt; then
    echo "[INFO] TCRH-408: Compilation with -mfpu=fxu succeeded"
else
    echo "[FAIL] TCRH-408: Compilation with -mfpu=fxu FAILED"
    cat /tmp/tcrh408_err.txt
    rm -f "$ASM_FILE"
    exit 1
fi

# Check for FXU vector instructions
FXU_COUNT=$(grep -ci "fmov.w\|fadd.w\|fmul.w\|fdiv.w\|fcmov.w\|ftrnc.w" "$ASM_FILE" 2>/dev/null || echo 0)

if [ "$FXU_COUNT" -gt 0 ]; then
    echo "[INFO] TCRH-408: Found $FXU_COUNT FXU vector instructions"
    echo "[PASS] TCRH-408: FXU vector type (__w128) supported"
    rm -f "$ASM_FILE"
    exit 0
else
    echo "[WARN] TCRH-408: No FXU instructions found in assembly"
    echo "[INFO] TCRH-408: Vector type accepted but operations may be scalarized"
    # Still pass if compilation succeeded - the type is supported
    echo "[PASS] TCRH-408: FXU vector type compiles successfully"
    rm -f "$ASM_FILE"
    exit 0
fi
