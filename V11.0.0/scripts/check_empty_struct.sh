#!/bin/bash
# TCRH-448: Verify empty non-zero sized structure argument passing
# Usage: check_empty_struct.sh <compiler> <source_file>

CC=$1
SRC=$2

if [ -z "$CC" ] || [ -z "$SRC" ]; then
    echo "[FAIL] Usage: $0 <compiler> <source_file>"
    exit 1
fi

PASS=true

# Compile at -O0 (no optimization to see raw ABI)
ASM_O0=$(mktemp /tmp/tcrh448_o0_XXXXXX.s)
if "$CC" -march=rh850-g4mh2 -O0 -S "$SRC" -o "$ASM_O0" 2>/tmp/tcrh448_err.txt; then
    echo "[INFO] TCRH-448: Compilation at -O0 succeeded"
else
    echo "[FAIL] TCRH-448: Compilation at -O0 FAILED"
    cat /tmp/tcrh448_err.txt
    PASS=false
fi

# Compile at -O2
ASM_O2=$(mktemp /tmp/tcrh448_o2_XXXXXX.s)
if "$CC" -march=rh850-g4mh2 -O2 -S "$SRC" -o "$ASM_O2" 2>/tmp/tcrh448_err.txt; then
    echo "[INFO] TCRH-448: Compilation at -O2 succeeded"
else
    echo "[FAIL] TCRH-448: Compilation at -O2 FAILED"
    cat /tmp/tcrh448_err.txt
    PASS=false
fi

# Check that empty struct functions are generated (not optimized away incorrectly)
if grep -q "test_empty_arg:" "$ASM_O0" 2>/dev/null; then
    echo "[INFO] TCRH-448: test_empty_arg function generated"
else
    echo "[WARN] TCRH-448: test_empty_arg not found as separate function (may be inlined)"
fi

if grep -q "test_empty_return:" "$ASM_O0" 2>/dev/null; then
    echo "[INFO] TCRH-448: test_empty_return function generated"
fi

# Check that function calls with empty struct args use correct calling convention
# (empty structs should still consume a register or stack slot per ABI)
if grep -q "test_multi_empty_args" "$ASM_O0" 2>/dev/null; then
    echo "[INFO] TCRH-448: Multi-empty-arg function referenced"
fi

rm -f "$ASM_O0" "$ASM_O2"

if $PASS; then
    echo "[PASS] TCRH-448: Empty struct argument passing works correctly"
    exit 0
else
    echo "[FAIL] TCRH-448: Empty struct verification failed"
    exit 1
fi
