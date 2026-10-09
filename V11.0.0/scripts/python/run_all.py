"""
Run all TCRH test scripts and report summary.
Usage: python run_all.py
"""
import sys
import os
import subprocess

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

TESTS = [
    "test_TCRH-371.py",
    "test_TCRH-453.py",
    "test_TCRH-424.py",
    "test_TCRH-341.py",
    "test_TCRH-408.py",
    "test_TCRH-409.py",
    "test_TCRH-456.py",
    "test_TCRH-442.py",
    "test_TCRH-443.py",
    "test_TCRH-445.py",
    "test_TCRH-448.py",
    "test_TCRH-451.py",
    "test_TCRH-454.py",
]

def main():
    results = []
    for script in TESTS:
        path = os.path.join(SCRIPT_DIR, script)
        if not os.path.exists(path):
            print(f"SKIP {script}: not found")
            results.append((script, "SKIP"))
            continue
        print(f"\n{'#'*60}")
        print(f"# Running {script}")
        print(f"{'#'*60}")
        rc = subprocess.call([sys.executable, path])
        status = "PASS" if rc == 0 else "FAIL"
        results.append((script, status))

    print("\n" + "=" * 60)
    print("  SUMMARY")
    print("=" * 60)
    passed = sum(1 for _, s in results if s == "PASS")
    failed = sum(1 for _, s in results if s == "FAIL")
    skipped = sum(1 for _, s in results if s == "SKIP")
    for script, status in results:
        print(f"  [{status}] {script}")
    print(f"\n  Total: {len(results)}, Pass: {passed}, Fail: {failed}, Skip: {skipped}")
    return 0 if failed == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
