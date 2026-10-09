"""
TCRH-371: Multi-core architecture support (G3K ~ G4MH2)
Test: Verify all 7 RH850 architecture variants compile successfully.
"""
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import *

ARCHS = [
    "rh850-g3k", "rh850-g3kh", "rh850-g3m", "rh850-g3mh",
    "rh850-g4kh", "rh850-g4mh", "rh850-g4mh2",
]

def main():
    print_header("TCRH-371", "Multi-core architecture support")
    compiler = find_compiler("v11")
    if not check_compiler_exists(compiler):
        return print_result(False, "Compiler not found")
    print(f"Compiler: {compiler}")

    src = src_path("TCRH-371", "test_multi_arch.c")
    all_pass = True
    details = []

    for arch in ARCHS:
        obj = build_path("TCRH-371", f"test_{arch}.o")
        flags = [f"-march={arch}", "-O2"]
        ok, out, err = compile_src(compiler, src, obj, flags)
        status = "OK" if ok else "FAIL"
        print(f"  [{status}] -march={arch}")
        if not ok:
            all_pass = False
            details.append(f"{arch}: {err.strip()[:200]}")
        else:
            size = get_text_size(obj)
            if size:
                details.append(f"{arch}: text={size} bytes")

    return print_result(all_pass, "; ".join(details))

if __name__ == "__main__":
    sys.exit(main())
