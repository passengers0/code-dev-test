"""
TCRH-408: FXU vector __w128 type support
Test: Verify __w128 vector type compiles with pointer passing
      (V850 backend does not support vector as return value or value param).
"""
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import *

def main():
    print_header("TCRH-408", "FXU vector __w128 type")
    compiler = find_compiler("v11")
    if not check_compiler_exists(compiler):
        return print_result(False, "Compiler not found")
    print(f"Compiler: {compiler}")

    src = src_path("TCRH-408", "test_fxu_vector.c")
    obj = build_path("TCRH-408", "test_fxu_vector.o")
    flags = ["-march=rh850-g4mh2", "-O2", "-mfpu=fxu"]

    ok, out, err = compile_src(compiler, src, obj, flags)
    if not ok:
        print(f"  [FAIL] Compilation: {err.strip()[:300]}")
        return print_result(False, "Compilation failed")

    size = get_text_size(obj)
    print(f"  [OK] Compilation with -mfpu=fxu (text={size} bytes)" if size else "  [OK] Compilation")
    return print_result(True, f"text={size} bytes, -mfpu=fxu" if size else "compiled with -mfpu=fxu")

if __name__ == "__main__":
    sys.exit(main())
