#!/bin/bash
set -e

echo "=== 清理旧构建 ==="
make clean

echo "=== 编译所有测试用例 ==="
make compile

echo "=== 生成汇编文件 ==="
make asm

echo "=== 运行验证脚本 ==="
make test

echo ""
echo "========================================"
echo "  所有测试完成！"
echo "========================================"