#!/bin/bash
# TCRH-451: Verify no crash when initializing function pointer array with interrupt_handler
# Usage: check_interrupt_funcptr.sh <compiler> <source_file>

CC=$1
SRC=$2

if [ -z "$CC" ] || [ -z "$SRC" ]; then
    echo "[FAIL] Usage: $0 <compiler> <source_file>"
    exit 1
fi

PASS=true

# Test compilation at various optimization levels
for OPT in "-O0" "-O1" "-O2" "-Os"; do
    OUTPUT=$(mktemp /tmp/tcrh451_XXXXXX.o)
    if "$CC" -march=rh850-g4mh2 $OPT -c "$SRC" -o "$OUTPUT" 2>/tmp/tcrh451_err.txt; then
        echo "[INFO] TCRH-451: Compilation $OPT succeeded"
    else
        echo "[FAIL] TCRH-451: Compilation $OPT FAILED (interrupt handler funcptr crash)"
        cat /tmp/tcrh451_err.txt
        PASS=false
    fi
    rm -f "$OUTPUT"
done

# Generate assembly to verify interrupt handler prologue
ASM_FILE=$(mktemp /tmp/tcrh451_XXXXXX.s)
"$CC" -march=rh850-g4mh2 -O2 -S "$SRC" -o "$ASM_FILE" 2>/dev/null

# Check for interrupt handler attributes in assembly
if grep -q "eint_handler:" "$ASM_FILE" 2>/dev/null; then
    echo "[INFO] TCRH-451: eint_handler function present"
    # Check for interrupt prologue (dispose/prepare or register saves)
    if grep -qi "dispose\|prepare\|stsr\|ldsr" "$ASM_FILE" 2>/dev/null; then
        echo "[INFO] TCRH-451: Interrupt handler prologue/epilogue present"
    fi
fi

# Check that function pointer arrays are in the data section
if grep -q "interrupt_table" "$ASM_FILE" 2>/dev/null; then
    echo "[INFO] TCRH-451: interrupt_table symbol present"
fi

if grep -q "const_table" "$ASM_FILE" 2>/dev/null; then
    echo "[INFO] TCRH-451: const_table symbol present (read-only)"
fi

rm -f "$ASM_FILE"

if $PASS; then
    echo "[PASS] TCRH-451: No crash with interrupt_handler function pointer array"
    exit 0
else
    echo "[FAIL] TCRH-451: Compiler crash detected"
    exit 1
fi
