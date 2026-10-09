"""
TCRH-445: MOV fusion crash fix
Test: V11 compiles without crash; V0.4 comparison (expected to crash/assert).
"""
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import *

def compile_with_timeout(compiler, src, out, flags, timeout=30):
    """Compile with timeout, returns (success, stderr, timed_out)."""
    if flags is None:
        flags = []
    cmd = [compiler] + flags + ["-c", src, "-o", out]
    print(f"  CMD: {' '.join(cmd)}")
    try:
        p = subprocess.run(
            cmd, capture_output=True, text=True, timeout=timeout
        )
        return p.returncode == 0, p.stderr, False
    except subprocess.TimeoutExpired:
        return False, "TIMEOUT (process likely crashed/hung)", True
    except Exception as e:
        return False, str(e), False

def main():
    print_header("TCRH-445", "MOV fusion crash fix (V11 vs V0.4)")
    v11 = find_compiler("v11")
    v04 = find_compiler("v04")
    if not check_compiler_exists(v11):
        return print_result(False, "V11 compiler not found")
    print(f"V11: {v11}")
    print(f"V04: {v04 if os.path.exists(v04) else 'NOT FOUND'}")

    src = src_path("TCRH-445", "test_mov_fusion_crash.c")
    flags = ["-march=rh850-g4mh2", "-O2"]

    # V11 compilation
    obj_v11 = build_path("TCRH-445", "test_v11.o")
    ok11, err11, _ = compile_with_timeout(v11, src, obj_v11, flags)
    if ok11:
        size = get_text_size(obj_v11)
        print(f"  [OK] V11.0.0: compiled (text={size}B)" if size else "  [OK] V11.0.0: compiled")
    else:
        print(f"  [FAIL] V11.0.0: {err11.strip()[:200]}")
        return print_result(False, "V11 compilation failed")

    # V0.4 compilation (expected to crash/assert)
    if os.path.exists(v04):
        obj_v04 = build_path("TCRH-445", "test_v04.o")
        ok04, err04, timed04 = compile_with_timeout(v04, src, obj_v04, flags)
        has_assert = "Assertion" in err04 or "assertion" in err04
        if ok04:
            print(f"  [INFO] V0.4.0: compiled (bug not reproduced by this test case)")
            v04_status = "compiled"
        elif timed04:
            print(f"  [OK] V0.4.0: TIMEOUT/hang (bug confirmed - crash)")
            v04_status = "crashed/hung"
        elif has_assert:
            print(f"  [OK] V0.4.0: Assertion failure (bug confirmed)")
            v04_status = "assertion"
        else:
            print(f"  [INFO] V0.4.0: compile error: {err04.strip()[:150]}")
            v04_status = "error"
    else:
        v04_status = "not available"

    details = f"V11=PASS, V0.4={v04_status}"
    return print_result(True, details)

if __name__ == "__main__":
    sys.exit(main())
