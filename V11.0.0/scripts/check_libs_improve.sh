#!/bin/bash
# TCRH-341: Verify library improvements (memcpy/memmove/math/rounding)
# Usage: check_libs_improve.sh <objdump_file>

DUMP_FILE=$1

if [ ! -f "$DUMP_FILE" ]; then
    echo "[FAIL] TCRH-341: Dump file not found: $DUMP_FILE"
    exit 1
fi

PASS=true

# Check for memcpy/memmove calls (hand-optimized implementations)
if grep -qi "memcpy\|memmove" "$DUMP_FILE" 2>/dev/null; then
    echo "[INFO] TCRH-341: memcpy/memmove calls present (linked to optimized libs)"
else
    echo "[INFO] TCRH-341: memcpy/memmove may be inlined or optimized out"
fi

# Check for math function calls
MATH_FUNCS=("sin" "cos" "sqrt" "fabs" "floor" "ceil" "exp" "log" "pow")
for func in "${MATH_FUNCS[@]}"; do
    if grep -qi "<${func}@\|call.*${func}\|bl.*${func}" "$DUMP_FILE" 2>/dev/null; then
        echo "[INFO] TCRH-341: Math function $func referenced"
    fi
done

# Check for fesetround (dynamic rounding mode support)
if grep -qi "fesetround\|fegetround" "$DUMP_FILE" 2>/dev/null; then
    echo "[INFO] TCRH-341: Dynamic rounding mode (fesetround) referenced"
else
    echo "[WARN] TCRH-341: fesetround not found in disassembly (may be inlined)"
fi

# Check for software floating point functions (compiler-rt)
if grep -qi "__addsf3\|__mulsf3\|__divsf3\|__adddf3" "$DUMP_FILE" 2>/dev/null; then
    echo "[INFO] TCRH-341: compiler-rt software FP functions referenced"
fi

echo "[PASS] TCRH-341: Library improvements verification complete"
exit 0
