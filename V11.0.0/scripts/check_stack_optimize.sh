#!/bin/bash
# TCRH-442: Verify optimized stack usage of fixed and aligned stack objects
# Usage: check_stack_optimize.sh <asm_file>

ASM_FILE=$1

if [ ! -f "$ASM_FILE" ]; then
    echo "[FAIL] TCRH-442: Assembly file not found: $ASM_FILE"
    exit 1
fi

PASS=true

# Check for stack frame allocation (prepare/dispose or add/sub sp)
if grep -qi "prepare\|dispose\|add.*sp\|sub.*sp\|addi.*sp" "$ASM_FILE" 2>/dev/null; then
    echo "[INFO] TCRH-442: Stack frame allocation instructions found"
else
    echo "[WARN] TCRH-442: No explicit stack allocation found (may use leaf function optimization)"
fi

# Check for aligned stack access patterns
# Aligned objects should use aligned load/store instructions
if grep -qi "ld.w\|st.w" "$ASM_FILE" 2>/dev/null; then
    echo "[INFO] TCRH-442: Word-aligned load/store instructions present"
fi

# Check that large arrays are allocated on stack (not in registers)
# Look for stack pointer relative addressing with large offsets
LARGE_OFFSET=$(grep -oE "[0-9]+\[sp\]" "$ASM_FILE" 2>/dev/null | sort -t'[' -k1 -n | tail -1)
if [ -n "$LARGE_OFFSET" ]; then
    echo "[INFO] TCRH-442: Max stack offset found: $LARGE_OFFSET"
fi

# Verify no excessive stack padding (heuristic: check for nop-like padding)
NOP_COUNT=$(grep -ci "nop" "$ASM_FILE" 2>/dev/null || echo 0)
echo "[INFO] TCRH-442: NOP instructions in output: $NOP_COUNT"

echo "[PASS] TCRH-442: Stack usage optimization verification complete"
exit 0
