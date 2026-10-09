"""
Common utilities for TCRH test scripts.
Provides compilation, assembly inspection, and result reporting helpers.
"""
import os
import subprocess
import re
import sys

# Default paths
DEFAULT_V11 = r"C:\HighTec\toolchains\v850\v11.0.0\bin\clang.exe"
DEFAULT_V04 = r"C:\HighTec\toolchains\v850\v0.4.0\bin\clang.exe"
DEFAULT_LLVM_SIZE = r"C:\HighTec\toolchains\v850\v11.0.0\bin\llvm-size.exe"
DEFAULT_LLVM_OBJDUMP = r"C:\HighTec\toolchains\v850\v11.0.0\bin\llvm-objdump.exe"

# Test root (parent of scripts/python/)
TEST_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def find_compiler(version="v11"):
    """Find compiler by version label."""
    if version == "v11":
        return DEFAULT_V11
    elif version == "v04":
        return DEFAULT_V04
    return DEFAULT_V11


def src_path(tcrh, filename):
    """Get absolute path to a test source file."""
    return os.path.join(TEST_ROOT, tcrh, filename)


def build_path(tcrh, filename):
    """Get absolute path in build directory."""
    d = os.path.join(TEST_ROOT, "build", tcrh)
    os.makedirs(d, exist_ok=True)
    return os.path.join(d, filename)


def run_cmd(cmd, timeout=60):
    """Run a command, return (returncode, stdout, stderr)."""
    try:
        p = subprocess.run(
            cmd, capture_output=True, text=True, timeout=timeout
        )
        return p.returncode, p.stdout, p.stderr
    except FileNotFoundError as e:
        return -1, "", str(e)
    except subprocess.TimeoutExpired:
        return -1, "", "TIMEOUT"


def compile_src(compiler, src, out, flags=None, timeout=60):
    """
    Compile a source file.
    Returns (success: bool, stdout, stderr).
    """
    if flags is None:
        flags = []
    cmd = [compiler] + flags + ["-c", src, "-o", out]
    print(f"  CMD: {' '.join(cmd)}")
    rc, out_s, err = run_cmd(cmd, timeout)
    return rc == 0, out_s, err


def compile_to_asm(compiler, src, out, flags=None, timeout=60):
    """Compile to assembly (-S)."""
    if flags is None:
        flags = []
    cmd = [compiler] + flags + ["-S", src, "-o", out]
    print(f"  CMD: {' '.join(cmd)}")
    rc, out_s, err = run_cmd(cmd, timeout)
    return rc == 0, out_s, err


def get_text_size(obj, llvm_size=None):
    """Get text section size from object file."""
    if llvm_size is None:
        llvm_size = DEFAULT_LLVM_SIZE
    rc, out, err = run_cmd([llvm_size, obj])
    if rc != 0:
        return None
    # Parse: text data bss dec hex filename
    lines = out.strip().splitlines()
    if len(lines) >= 2:
        parts = lines[1].split()
        if len(parts) >= 1:
            try:
                return int(parts[0])
            except ValueError:
                return None
    return None


def asm_contains(asm_file, pattern):
    """Check if regex pattern exists in assembly file."""
    if not os.path.exists(asm_file):
        return False
    with open(asm_file, "r", errors="replace") as f:
        content = f.read()
    return re.search(pattern, content) is not None


def asm_count(asm_file, pattern):
    """Count occurrences of regex pattern in assembly file."""
    if not os.path.exists(asm_file):
        return 0
    with open(asm_file, "r", errors="replace") as f:
        content = f.read()
    return len(re.findall(pattern, content))


def get_prepare_frames(asm_file):
    """
    Extract function -> prepare frame size from assembly.
    Returns dict {func_name: frame_size}.
    """
    frames = {}
    if not os.path.exists(asm_file):
        return frames
    current = None
    with open(asm_file, "r", errors="replace") as f:
        for line in f:
            m = re.match(r"^_(\w+):", line)
            if m:
                current = m.group(1)
            m = re.search(r"prepare\s+.+,\s+(\d+)", line)
            if m and current:
                frames[current] = int(m.group(1))
    return frames


def get_sp_adjustments(asm_file):
    """
    Extract total sp adjustment per function (sum of movea -N,r3,r3).
    Returns dict {func_name: total_bytes}.
    """
    adjust = {}
    if not os.path.exists(asm_file):
        return adjust
    current = None
    with open(asm_file, "r", errors="replace") as f:
        for line in f:
            m = re.match(r"^_(\w+):", line)
            if m:
                current = m.group(1)
                if current not in adjust:
                    adjust[current] = 0
            m = re.search(r"movea\s+-(\d+),\s+r3,\s+r3", line)
            if m and current:
                adjust[current] = adjust.get(current, 0) + int(m.group(1))
    return adjust


def print_header(tcrh, title):
    """Print test header."""
    print("=" * 60)
    print(f"  {tcrh}: {title}")
    print("=" * 60)


def print_result(passed, details=""):
    """Print test result."""
    status = "PASS" if passed else "FAIL"
    print(f"\nResult: {status}")
    if details:
        print(f"Details: {details}")
    print()
    return 0 if passed else 1


def check_compiler_exists(compiler):
    """Check if compiler exists, print error if not."""
    if not os.path.exists(compiler):
        print(f"ERROR: Compiler not found: {compiler}")
        return False
    return True
