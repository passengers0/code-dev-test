#!/bin/bash
ASM_FILE=$1

if [ ! -f "$ASM_FILE" ]; then
    echo "[FAIL] 找不到汇编文件: $ASM_FILE"
    exit 1
fi

# 检查是否生成了非法的 stv.dw 指令
if grep -qiE "\bstv\.dw\b" "$ASM_FILE"; then
    echo "[FAIL] TCRH-421: 检测到非法的 stv.dw 指令"
    exit 1
else
    echo "[PASS] TCRH-421: 未生成非法 stv.dw 指令"
    exit 0
fi