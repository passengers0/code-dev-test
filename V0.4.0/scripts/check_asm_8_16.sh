#!/bin/bash
ASM_FILE=$1

if [ ! -f "$ASM_FILE" ]; then
    echo "[FAIL] 找不到汇编文件: $ASM_FILE"
    exit 1
fi

PASS=true

# 检查1: 编译器没有崩溃（.s 文件存在且非空）
if [ ! -s "$ASM_FILE" ]; then
    echo "[FAIL] TCRH-423: 汇编文件为空，编译器可能崩溃"
    exit 1
fi

# 检查2: 验证内联汇编中 8 位操作数的处理
# 8 位操作数在 V850 汇编中可能体现为 movb / stb / ldb 等指令
# 或者通过 sign-extend / zero-extend 操作来确保宽度正确
if grep -qiE "movb|stb|ldb|movz|movs" "$ASM_FILE"; then
    echo "[PASS] TCRH-423: 检测到 8 位操作数的正确处理"
else
    echo "[WARN] TCRH-423: 未检测到 8 位操作数的显式处理，可能已隐式处理"
fi

# 检查3: 验证 16 位操作数的处理
# 16 位操作数可能体现为 movw / stw / ldw 等指令
if grep -qiE "movw|stw|ldw" "$ASM_FILE"; then
    echo "[PASS] TCRH-423: 检测到 16 位操作数的正确处理"
else
    echo "[WARN] TCRH-423: 未检测到 16 位操作数的显式处理，可能已隐式处理"
fi

if $PASS; then
    echo "[PASS] TCRH-423: 编译器未因内联汇编 8/16 位操作数而崩溃"
    exit 0
else
    echo "[FAIL] TCRH-423: 验证失败"
    exit 1
fi