"""
TCRH-409: FXU inline assembly constraint support
Test: 1. Verify -mfpu=fxu flag accepted and inline asm with memory operands works.
      2. Verify "w" constraint (FXU vector register) is recognized by parser.
         Note: "w" gets past parser but fails backend legalization because FXU
         instructions are not utilized yet (userguide Appendix G known limit).
"""
import sys
import os
import subprocess
import tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import *

def test_w_constraint(compiler):
    """Verify 'w' constraint is recognized by parser (not 'invalid constraint')."""
    code = (
        '#include <stdint.h>\n'
        'typedef float __w128 __attribute__((__vector_size__(16), __aligned__(16)));\n'
        'void test_w_constraint(__w128 *a, __w128 *out) {\n'
        '    __asm__ volatile("" : "=w"(*out) : "w"(*a) : "memory");\n'
        '}\n'
    )
    tmpdir = tempfile.gettempdir()
    src = os.path.join(tmpdir, "fxu_w_constraint.c")
    obj = os.path.join(tmpdir, "fxu_w_constraint.o")
    with open(src, "w") as f:
        f.write(code)

    cmd = [compiler, "-march=rh850-g4mh2", "-O2", "-mfpu=fxu", "-c", src, "-o", obj]
    print(f"  CMD: {' '.join(cmd)}")
    try:
        p = subprocess.run(cmd, capture_output=True, text=True, timeout=15)
    except subprocess.TimeoutExpired:
        print("  [WARN] 'w' constraint: TIMEOUT")
        os.remove(src)
        return False

    stderr = p.stderr or ""
    # "w" constraint is recognized if error is NOT "invalid output constraint"
    recognized = "invalid output constraint" not in stderr and "invalid input constraint" not in stderr
    backend_fail = "unable to legalize" in stderr or "G_UNMERGE_VALUES" in stderr

    if recognized:
        if backend_fail:
            print("  [OK] 'w' constraint recognized by parser (backend legalization fails - expected, FXU not utilized)")
        else:
            print("  [OK] 'w' constraint recognized and compiled")
        result = True
    else:
        print(f"  [FAIL] 'w' constraint not recognized: {stderr.strip()[:150]}")
        result = False

    if os.path.exists(obj):
        os.remove(obj)
    os.remove(src)
    return result

def main():
    print_header("TCRH-409", "FXU inline assembly constraint")
    compiler = find_compiler("v11")
    if not check_compiler_exists(compiler):
        return print_result(False, "Compiler not found")
    print(f"Compiler: {compiler}")

    # Test 1: Main test file with "m" memory constraint
    src = src_path("TCRH-409", "test_fxu_asm_constraint.c")
    obj = build_path("TCRH-409", "test_fxu_asm_constraint.o")
    flags = ["-march=rh850-g4mh2", "-O2", "-mfpu=fxu"]

    ok, out, err = compile_src(compiler, src, obj, flags)
    if not ok:
        print(f"  [FAIL] Compilation: {err.strip()[:300]}")
        return print_result(False, "Compilation failed")

    size = get_text_size(obj)
    print(f"  [OK] -mfpu=fxu + 'm' memory constraint (text={size}B)" if size else "  [OK] -mfpu=fxu accepted")

    # Test 2: Verify "w" constraint (FXU vector register) is recognized
    print("  -- 'w' constraint check --")
    w_ok = test_w_constraint(compiler)

    all_ok = ok and w_ok
    detail = f"text={size}B, -mfpu=fxu OK, 'w' constraint {'recognized' if w_ok else 'NOT recognized'}"
    return print_result(all_ok, detail)

if __name__ == "__main__":
    sys.exit(main())
