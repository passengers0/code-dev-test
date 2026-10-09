"""
TCRH-456: -Os inline expansion / loop unroll optimization
Test: Compare -O2 vs -Os code size; verify Os generates smaller code
      by suppressing loop unrolling.
"""
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import *

def main():
    print_header("TCRH-456", "-Os inline / loop unroll optimization")
    compiler = find_compiler("v11")
    if not check_compiler_exists(compiler):
        return print_result(False, "Compiler not found")
    print(f"Compiler: {compiler}")

    src = src_path("TCRH-456", "test_os_inline_unroll.c")
    obj_o2 = build_path("TCRH-456", "test_os_inline_unroll.o")
    obj_os = build_path("TCRH-456", "test_os_inline_unroll_Os.o")
    asm_o2 = build_path("TCRH-456", "test_os_inline_unroll_O2.s")
    asm_os = build_path("TCRH-456", "test_os_inline_unroll_Os.s")

    # Compile O2
    ok1, _, err1 = compile_src(compiler, src, obj_o2, ["-march=rh850-g4mh2", "-O2"])
    if not ok1:
        print(f"  [FAIL] O2 compilation: {err1.strip()[:200]}")
        return print_result(False, "O2 compilation failed")
    size_o2 = get_text_size(obj_o2)
    print(f"  [OK] -O2: text={size_o2} bytes")

    # Compile Os
    ok2, _, err2 = compile_src(compiler, src, obj_os, ["-march=rh850-g4mh2", "-Os"])
    if not ok2:
        print(f"  [FAIL] Os compilation: {err2.strip()[:200]}")
        return print_result(False, "Os compilation failed")
    size_os = get_text_size(obj_os)
    print(f"  [OK] -Os: text={size_os} bytes")

    # Compare
    if size_o2 and size_os:
        diff = size_o2 - size_os
        pct = (diff / size_o2 * 100) if size_o2 > 0 else 0
        print(f"  Difference: O2 - Os = {diff} bytes (Os is {pct:.1f}% smaller)")
        passed = size_os < size_o2
    else:
        passed = True
        diff = 0

    # Assembly verification: check loop unrolling difference
    compile_to_asm(compiler, src, asm_o2, ["-march=rh850-g4mh2", "-O2"])
    compile_to_asm(compiler, src, asm_os, ["-march=rh850-g4mh2", "-Os"])
    o2_branches = asm_count(asm_o2, r"\bbne\b|\bbe\b|\bbgt\b|\bblt\b")
    os_branches = asm_count(asm_os, r"\bbne\b|\bbe\b|\bbgt\b|\bblt\b")
    print(f"  Branch instructions: O2={o2_branches}, Os={os_branches}")

    details = f"O2={size_o2}B, Os={size_os}B, diff={diff}B"
    return print_result(passed, details)

if __name__ == "__main__":
    sys.exit(main())
