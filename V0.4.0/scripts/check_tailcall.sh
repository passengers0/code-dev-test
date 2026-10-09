#!/bin/bash
ASM_FILE=$1

if [ ! -f "$ASM_FILE" ]; then
    echo "[FAIL] 找不到汇编文件: $ASM_FILE"
    exit 1
fi

PASS=true

# 检查1: 不应存在对递归函数的 jarl 调用（TCO 生效的标志）
for func in factorial_tail fibonacci_tail is_even is_odd; do
    if grep -qiE "jarl.*_${func}\b" "$ASM_FILE"; then
        echo "[FAIL] TCRH-257: 检测到对 ${func} 的函数调用(jarl)，TCO 未生效"
        PASS=false
    fi
done

# 检查2: 应存在循环跳转（TCO 将递归转为循环的证据）
if grep -qiE "bnc|bne|be.*\.LBB" "$ASM_FILE"; then
    echo "[INFO] TCRH-257: 检测到循环跳转指令，递归已转换为循环"
else
    echo "[WARN] TCRH-257: 未检测到预期的循环跳转模式"
fi

if $PASS; then
    echo "[PASS] TCRH-257: 尾调用优化生效，递归已被转换为循环"
    exit 0
else
    echo "[FAIL] TCRH-257: 尾调用优化未生效"
    exit 1
fi