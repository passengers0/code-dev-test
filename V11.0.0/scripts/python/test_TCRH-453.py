"""
TCRH-453: U2B-E / U2B-EZ CPU support
Test: Verify u2b-eva (U2B-E) and u2b-eva-g3kh (U2B-EZ) compile.
Note: -mcpu cannot be used together with -march for V850.
"""
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import *

CPUS = [
    ("u2b-eva", "U2B-E", ["-mcpu=u2b-eva", "-O2"]),
    ("u2b-eva-g3kh", "U2B-EZ", ["-mcpu=u2b-eva-g3kh", "-O2"]),
]

def main():
    print_header("TCRH-453", "U2B-E / U2B-EZ CPU support")
    compiler = find_compiler("v11")
    if not check_compiler_exists(compiler):
        return print_result(False, "Compiler not found")
    print(f"Compiler: {compiler}")

    src = src_path("TCRH-453", "test_u2b_cpu.c")
    all_pass = True
    details = []

    for cpu, label, flags in CPUS:
        obj = build_path("TCRH-453", f"test_{cpu}.o")
        ok, out, err = compile_src(compiler, src, obj, flags)
        status = "OK" if ok else "FAIL"
        print(f"  [{status}] -mcpu={cpu} ({label})")
        if not ok:
            all_pass = False
            details.append(f"{cpu}: {err.strip()[:200]}")
        else:
            size = get_text_size(obj)
            if size:
                details.append(f"{cpu}: text={size} bytes")

    return print_result(all_pass, "; ".join(details))

if __name__ == "__main__":
    sys.exit(main())
