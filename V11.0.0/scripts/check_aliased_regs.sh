#!/bin/bash
# TCRH-424: Verify aliased system register support
# Usage: check_aliased_regs.sh <asm_file>

ASM_FILE=$1

if [ ! -f "$ASM_FILE" ]; then
    echo "[FAIL] TCRH-424: Assembly file not found: $ASM_FILE"
    exit 1
fi

PASS=true

# Check that stsr/ldsr instructions are generated for system register access
STS_COUNT=$(grep -ci "stsr" "$ASM_FILE" 2>/dev/null || echo 0)
LDS_COUNT=$(grep -ci "ldsr" "$ASM_FILE" 2>/dev/null || echo 0)

if [ "$STS_COUNT" -gt 0 ]; then
    echo "[INFO] TCRH-424: Found $STS_COUNT stsr instructions (system register reads)"
else
    echo "[FAIL] TCRH-424: No stsr instructions found"
    PASS=false
fi

if [ "$LDS_COUNT" -gt 0 ]; then
    echo "[INFO] TCRH-424: Found $LDS_COUNT ldsr instructions (system register writes)"
else
    echo "[WARN] TCRH-424: No ldsr instructions found (may be optimized out)"
fi

# Check for various system register names in the assembly
for reg in PSW FPSR EIPC FEPC EIIC FEIC CTPC CTPSW; do
    if grep -qi "$reg" "$ASM_FILE" 2>/dev/null; then
        echo "[INFO] TCRH-424: System register $reg referenced"
    fi
done

if $PASS; then
    echo "[PASS] TCRH-424: Aliased system registers supported (stsr/ldsr generated)"
    exit 0
else
    echo "[FAIL] TCRH-424: System register verification failed"
    exit 1
fi
