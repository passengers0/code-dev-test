#!/bin/bash
# Run all V850 v11.0.0 TCRH test cases
# Usage: run_all.sh [compiler_path]

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
TESTS_DIR="$(dirname "$SCRIPT_DIR")"

# Default compiler path
CC="${1:-C:/HighTec/toolchains/v850/v11.0.0/bin/clang.exe}"
OBJDUMP="${CC%/clang.exe}/llvm-objdump.exe"

echo "========================================"
echo "  V850 v11.0.0 TCRH Test Suite"
echo "  Compiler: $CC"
echo "========================================"
echo ""

PASS_COUNT=0
FAIL_COUNT=0
SKIP_COUNT=0

run_test() {
    local name=$1
    local src=$2
    local checker=$3
    local extra_args=$4

    echo "--- $name ---"
    if [ -f "$src" ]; then
        if bash "$checker" "$CC" "$src" $extra_args 2>&1; then
            PASS_COUNT=$((PASS_COUNT + 1))
        else
            FAIL_COUNT=$((FAIL_COUNT + 1))
        fi
    else
        echo "[SKIP] $name: source not found"
        SKIP_COUNT=$((SKIP_COUNT + 1))
    fi
    echo ""
}

# TCRH-371: Multi-arch support
run_test "TCRH-371 (Multi-arch)" \
    "$TESTS_DIR/TCRH-371/test_multi_arch.c" \
    "$SCRIPT_DIR/check_multi_arch.sh"

# TCRH-453: U2B-E/U2B-EZ CPU
run_test "TCRH-453 (U2B-E/U2B-EZ)" \
    "$TESTS_DIR/TCRH-453/test_u2b_cpu.c" \
    "$SCRIPT_DIR/check_u2b_cpu.sh"

# TCRH-424: Aliased system registers
run_test "TCRH-424 (Aliased regs)" \
    "$TESTS_DIR/TCRH-424/test_aliased_regs.c" \
    "$SCRIPT_DIR/check_aliased_regs.sh"

# TCRH-341: Library improvements
run_test "TCRH-341 (Libs improve)" \
    "$TESTS_DIR/TCRH-341/test_libs_improve.c" \
    "$SCRIPT_DIR/check_libs_improve.sh"

# TCRH-408: FXU vector type
run_test "TCRH-408 (FXU vector type)" \
    "$TESTS_DIR/TCRH-408/test_fxu_vector.c" \
    "$SCRIPT_DIR/check_fxu_vector.sh"

# TCRH-409: FXU asm constraint
run_test "TCRH-409 (FXU asm constraint)" \
    "$TESTS_DIR/TCRH-409/test_fxu_asm_constraint.c" \
    "$SCRIPT_DIR/check_fxu_asm_constraint.sh"

# TCRH-456: -Os inline/unroll
run_test "TCRH-456 (-Os inline/unroll)" \
    "$TESTS_DIR/TCRH-456/test_os_inline_unroll.c" \
    "$SCRIPT_DIR/check_os_inline_unroll.sh"

# TCRH-442: Stack optimization
run_test "TCRH-442 (Stack optimize)" \
    "$TESTS_DIR/TCRH-442/test_stack_optimize.c" \
    "$SCRIPT_DIR/check_stack_optimize.sh"

# TCRH-443: Branch fusion
run_test "TCRH-443 (Branch fusion)" \
    "$TESTS_DIR/TCRH-443/test_branch_fusion.c" \
    "$SCRIPT_DIR/check_branch_fusion.sh"

# TCRH-445: MOV fusion crash fix
run_test "TCRH-445 (MOV fusion crash)" \
    "$TESTS_DIR/TCRH-445/test_mov_fusion_crash.c" \
    "$SCRIPT_DIR/check_mov_fusion_crash.sh"

# TCRH-448: Empty struct args
run_test "TCRH-448 (Empty struct)" \
    "$TESTS_DIR/TCRH-448/test_empty_struct.c" \
    "$SCRIPT_DIR/check_empty_struct.sh"

# TCRH-451: Interrupt funcptr crash
run_test "TCRH-451 (Interrupt funcptr)" \
    "$TESTS_DIR/TCRH-451/test_interrupt_funcptr.c" \
    "$SCRIPT_DIR/check_interrupt_funcptr.sh"

# TCRH-454: Inline asm sysreg crash
run_test "TCRH-454 (Inline asm sysreg)" \
    "$TESTS_DIR/TCRH-454/test_inline_asm_sysreg.c" \
    "$SCRIPT_DIR/check_inline_asm_sysreg.sh"

echo "========================================"
echo "  Test Summary"
echo "========================================"
echo "  PASS: $PASS_COUNT"
echo "  FAIL: $FAIL_COUNT"
echo "  SKIP: $SKIP_COUNT"
echo "  TOTAL: $((PASS_COUNT + FAIL_COUNT + SKIP_COUNT))"
echo "========================================"

if [ "$FAIL_COUNT" -gt 0 ]; then
    exit 1
fi
exit 0
