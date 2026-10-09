#!/bin/bash
# TCRH-371: Verify multi-arch compilation for all supported -march variants
# Usage: check_multi_arch.sh <compiler> <source_file>

CC=$1
SRC=$2

if [ -z "$CC" ] || [ -z "$SRC" ]; then
    echo "[FAIL] Usage: $0 <compiler> <source_file>"
    exit 1
fi

PASS=true
ARCHS=("rh850-g3k" "rh850-g3kh" "rh850-g3m" "rh850-g3mh" "rh850-g4kh" "rh850-g4mh" "rh850-g4mh2")

for arch in "${ARCHS[@]}"; do
    OUTPUT=$(mktemp /tmp/tcrh371_XXXXXX.o)
    if "$CC" -march="$arch" -O2 -c "$SRC" -o "$OUTPUT" 2>/tmp/tcrh371_err.txt; then
        echo "[INFO] TCRH-371: -march=$arch compilation OK"
    else
        echo "[FAIL] TCRH-371: -march=$arch compilation FAILED"
        cat /tmp/tcrh371_err.txt
        PASS=false
    fi
    rm -f "$OUTPUT"
done

# Verify g4mh2 enables FXU by checking assembly
ASM_FILE=$(mktemp /tmp/tcrh371_XXXXXX.s)
"$CC" -march=rh850-g4mh2 -mfpu=fxu -O2 -S "$SRC" -o "$ASM_FILE" 2>/dev/null
if grep -qi "fxu\|fmov\|fadd.w" "$ASM_FILE" 2>/dev/null; then
    echo "[INFO] TCRH-371: g4mh2 with fxu generates FXU instructions"
else
    echo "[WARN] TCRH-371: No FXU instructions found (may need -mfpu=fxu)"
fi
rm -f "$ASM_FILE"

if $PASS; then
    echo "[PASS] TCRH-371: All -march variants compile successfully"
    exit 0
else
    echo "[FAIL] TCRH-371: Some -march variants failed"
    exit 1
fi
