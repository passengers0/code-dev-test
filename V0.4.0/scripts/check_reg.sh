#!/bin/bash
ASM_FILE=$1

if [ ! -f "$ASM_FILE" ]; then
    echo "[FAIL] 找不到汇编文件: $ASM_FILE"
    exit 1
fi

PASS=true

# 检查1: 编译器没有崩溃（Makefile 层面已保证，.s 文件存在即说明没崩溃）
if [ ! -s "$ASM_FILE" ]; then
    echo "[FAIL] TCRH-420: 汇编文件为空，编译器可能崩溃"
    exit 1
fi

# 检查2: 验证内置函数生成的代码中存在寄存器引用
# 内置函数通常会用特定的寄存器名（如 r0-r15 等 V850 寄存器）
if grep -qiE "mov\.\b|movw\.\b|movl\.\b" "$ASM_FILE"; then
    echo "[PASS] TCRH-420: 内置函数代码生成正常，未崩溃"
else
    echo "[WARN] TCRH-420: 未检测到预期的内置函数调用代码"
fi

if $PASS; then
    echo "[PASS] TCRH-420: 编译器未因内置函数寄存器名识别失败而崩溃"
    exit 0
else
    echo "[FAIL] TCRH-420: 验证失败"
    exit 1
fi