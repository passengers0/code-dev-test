"""
TCRH-341: Math library / memcpy / dynamic rounding improvements
Test: Verify compilation with math functions and memcpy.
"""
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import *

def main():
    print_header("TCRH-341", "Math library / memcpy / dynamic rounding")
    compiler = find_compiler("v11")
    if not check_compiler_exists(compiler):
        return print_result(False, "Compiler not found")
    print(f"Compiler: {compiler}")

    src = src_path("TCRH-341", "test_libs_improve.c")
    obj = build_path("TCRH-341", "test_libs_improve.o")
    flags = ["-march=rh850-g4mh2", "-O2"]

    ok, out, err = compile_src(compiler, src, obj, flags)
    if not ok:
        print(f"  [FAIL] Compilation: {err.strip()[:300]}")
        return print_result(False, "Compilation failed")

    size = get_text_size(obj)
    print(f"  [OK] Compilation (text={size} bytes)" if size else "  [OK] Compilation")
    return print_result(True, f"text={size} bytes" if size else "compiled")

if __name__ == "__main__":
    sys.exit(main())
