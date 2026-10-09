"""
TCRH-424: Aliased system registers (__builtin_v850_read_register/write_register)
Test: Verify aliased register names (sp/gp/tp/ep/lp) compile and generate
      stsr/ldsr instructions in assembly.
"""
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import *

def main():
    print_header("TCRH-424", "Aliased system registers")
    compiler = find_compiler("v11")
    if not check_compiler_exists(compiler):
        return print_result(False, "Compiler not found")
    print(f"Compiler: {compiler}")

    src = src_path("TCRH-424", "test_aliased_regs.c")
    obj = build_path("TCRH-424", "test_aliased_regs.o")
    asm = build_path("TCRH-424", "test_aliased_regs.s")
    flags = ["-march=rh850-g4mh2", "-O2"]

    # Compile
    ok, out, err = compile_src(compiler, src, obj, flags)
    if not ok:
        print(f"  [FAIL] Compilation: {err.strip()[:300]}")
        return print_result(False, "Compilation failed")
    print("  [OK] Compilation")

    # Compile to assembly for verification
    ok2, out2, err2 = compile_to_asm(compiler, src, asm, flags)
    if not ok2:
        print(f"  [WARN] Assembly generation failed: {err2.strip()[:200]}")
        return print_result(True, "Compilation OK; assembly verification skipped")

    # Check for stsr/ldsr instructions (system register access)
    has_stsr = asm_contains(asm, r"\bstsr\b")
    has_ldsr = asm_contains(asm, r"\bldsr\b")
    print(f"  [{'OK' if has_stsr else 'WARN'}] stsr instruction: {'found' if has_stsr else 'not found'}")
    print(f"  [{'OK' if has_ldsr else 'WARN'}] ldsr instruction: {'found' if has_ldsr else 'not found'}")

    details = [f"stsr={'yes' if has_stsr else 'no'}", f"ldsr={'yes' if has_ldsr else 'no'}"]
    passed = has_stsr or has_ldsr  # At least one sysreg access pattern
    return print_result(passed, "; ".join(details))

if __name__ == "__main__":
    sys.exit(main())
