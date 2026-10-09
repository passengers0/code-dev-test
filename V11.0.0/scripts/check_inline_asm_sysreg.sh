#!/bin/bash
# TCRH-454: Verify no crash when inline assembly accesses system registers
# Usage: check_inline_asm_sysreg.sh <compiler> <source_file>

CC=$1
SRC=$2

if [ -z "$CC" ] || [ -z "$SRC" ]; then
    echo "[FAIL] Usage: $0 <compiler> <source_file>"
    exit 1
fi

PASS=true

# Test compilation at various optimization levels
for OPT in "-O0" "-O1" "-O2" "-O3" "-Os"; do
    OUTPUT=$(mktemp /tmp/tcrh454_XXXXXX.o)
    if "$CC" -march=rh850-g4mh2 $OPT -c "$SRC" -o "$OUTPUT" 2>/tmp/tcrh454_err.txt; then
        echo "[INFO] TCRH-454: Compilation $OPT succeeded"
    else
        echo "[FAIL] TCRH-454: Compilation $OPT FAILED (inline asm sysreg crash)"
        cat /tmp/tcrh454_err.txt
        PASS=false
    fi
    rm -f "$OUTPUT"
done

# Generate assembly to verify system register instructions
ASM_FILE=$(mktemp /tmp/tcrh454_XXXXXX.s)
"$CC" -march=rh850-g4mh2 -O2 -S "$SRC" -o "$ASM_FILE" 2>/dev/null

# Count stsr/ldsr instructions
STS_COUNT=$(grep -ci "stsr" "$ASM_FILE" 2>/dev/null || echo 0)
LDS_COUNT=$(grep -ci "ldsr" "$ASM_FILE" 2>/dev/null || echo 0)

echo "[INFO] TCRH-454: stsr instructions: $STS_COUNT"
echo "[INFO] TCRH-454: ldsr instructions: $LDS_COUNT"

if [ "$STS_COUNT" -gt 0 ]; then
    echo "[INFO] TCRH-454: System register reads (stsr) generated correctly"
else
    echo "[WARN] TCRH-454: No stsr instructions found (may be optimized out)"
fi

# Check for various system registers
for reg in PSW FPSR EIPC FEPC EIIC FEIC CTPC CTPSW FXSR FXXP; do
    if grep -qi "$reg" "$ASM_FILE" 2>/dev/null; then
        echo "[INFO] TCRH-454: System register $reg accessed in inline asm"
    fi
done

rm -f "$ASM_FILE"

if $PASS; then
    echo "[PASS] TCRH-454: No crash with inline assembly system register access"
    exit 0
else
    echo "[FAIL] TCRH-454: Compiler crash detected"
    exit 1
fi
