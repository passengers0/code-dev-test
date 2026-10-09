"""
TCRH-451: interrupt_handler function pointer array fix
Test: V11 compiles (with -Wno-incompatible-pointer-types); V0.4 crashes with
      UNREACHABLE assertion when initializing global funcptr array with
      interrupt_handler functions without casts.
Bug: V0.4 crashes at TypePrinter.cpp:2014 ("non-type attribute attached to type")
Fix: V11 handles this gracefully (clean error or successful compile).
"""
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import *

def compile_with_timeout(compiler, src, out, flags, timeout=30):
    if flags is None:
        flags = []
    cmd = [compiler] + flags + ["-c", src, "-o", out]
    print(f"  CMD: {' '.join(cmd)}")
    try:
        p = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
        return p.returncode == 0, p.stderr, False
    except subprocess.TimeoutExpired:
        return False, "TIMEOUT (process likely crashed/hung)", True
    except Exception as e:
        return False, str(e), False

def main():
    print_header("TCRH-451", "interrupt_handler funcptr array (V11 vs V0.4)")
    v11 = find_compiler("v11")
    v04 = find_compiler("v04")
    if not check_compiler_exists(v11):
        return print_result(False, "V11 compiler not found")
    print(f"V11: {v11}")
    print(f"V04: {v04 if os.path.exists(v04) else 'NOT FOUND'}")

    src = src_path("TCRH-451", "test_interrupt_funcptr.c")
    asm = build_path("TCRH-451", "test_v11.s")
    # -Wno-incompatible-pointer-types allows raw interrupt_handler in funcptr array
    flags_v11 = ["-march=rh850-g4mh2", "-O2", "-Wno-incompatible-pointer-types"]
    flags_v04 = ["-march=rh850-g4mh2", "-O2"]

    # V11 compilation
    obj_v11 = build_path("TCRH-451", "test_v11.o")
    ok11, err11, _ = compile_with_timeout(v11, src, obj_v11, flags_v11)
    if ok11:
        size = get_text_size(obj_v11)
        print(f"  [OK] V11.0.0: compiled (text={size}B)" if size else "  [OK] V11.0.0: compiled")
        compile_to_asm(v11, src, asm, flags_v11)
        has_eipc = asm_contains(asm, r"EIPC|eipc")
        print(f"  [INFO] EIPC prologue: {'found' if has_eipc else 'not found'}")
    else:
        print(f"  [FAIL] V11.0.0: {err11.strip()[:200]}")
        return print_result(False, "V11 compilation failed")

    # V0.4 compilation (expected to CRASH with UNREACHABLE)
    if os.path.exists(v04):
        obj_v04 = build_path("TCRH-451", "test_v04.o")
        ok04, err04, timed04 = compile_with_timeout(v04, src, obj_v04, flags_v04)
        has_unreachable = "UNREACHABLE" in err04 or "Assertion" in err04 or "assertion" in err04
        if ok04:
            print(f"  [WARN] V0.4.0: compiled (bug not reproduced)")
            v04_status = "compiled"
        elif timed04:
            print(f"  [OK] V0.4.0: TIMEOUT/hang (bug confirmed - crash)")
            v04_status = "crashed/hung"
        elif has_unreachable:
            print(f"  [OK] V0.4.0: UNREACHABLE/Assertion crash (bug confirmed)")
            v04_status = "UNREACHABLE crash"
            # Print the crash message
            for line in err04.strip().splitlines()[:3]:
                print(f"         {line.strip()}")
        else:
            print(f"  [INFO] V0.4.0: error: {err04.strip()[:150]}")
            v04_status = "error"
    else:
        v04_status = "not available"

    return print_result(True, f"V11=PASS, V0.4={v04_status}")

if __name__ == "__main__":
    sys.exit(main())
