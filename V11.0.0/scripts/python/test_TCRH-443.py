"""
TCRH-443: Branch fusion / conditional move optimization
Test: Verify compilation and check for cmov/setf instructions in assembly
      (branchless code generation for simple conditionals).
"""
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import *

def main():
    print_header("TCRH-443", "Branch fusion / conditional move")
    compiler = find_compiler("v11")
    if not check_compiler_exists(compiler):
        return print_result(False, "Compiler not found")
    print(f"Compiler: {compiler}")

    src = src_path("TCRH-443", "test_branch_fusion.c")
    obj = build_path("TCRH-443", "test_branch_fusion.o")
    asm = build_path("TCRH-443", "test_branch_fusion.s")
    flags = ["-march=rh850-g4mh2", "-O2"]

    ok, _, err = compile_src(compiler, src, obj, flags)
    if not ok:
        print(f"  [FAIL] Compilation: {err.strip()[:300]}")
        return print_result(False, "Compilation failed")
    print("  [OK] Compilation")

    compile_to_asm(compiler, src, asm, flags)
    has_cmov = asm_contains(asm, r"\bcmov\b")
    has_setf = asm_contains(asm, r"\bsetf\b")
    print(f"  [{'OK' if has_cmov else 'INFO'}] cmov instruction: {'found' if has_cmov else 'not found'}")
    print(f"  [{'OK' if has_setf else 'INFO'}] setf instruction: {'found' if has_setf else 'not found'}")

    size = get_text_size(obj)
    details = [f"text={size}B" if size else "compiled"]
    details.append(f"cmov={'yes' if has_cmov else 'no'}")
    details.append(f"setf={'yes' if has_setf else 'no'}")
    return print_result(True, "; ".join(details))

if __name__ == "__main__":
    sys.exit(main())
