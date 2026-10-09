# RH850 Clang V0.4.0版本测试工程

本测试工程用于验证 RH850 新版本的 Bug 修复和改进：

| 编号 | 类型 | 描述 |
|------|------|------|
| TCRH-257 | Improvement | 尾调用优化改进 |
| TCRH-420 | Bug fix | 内置函数无法识别寄存器名称导致编译器崩溃 |
| TCRH-421 | Bug fix | 在无效配置下生成非法 `stv.dw` 指令 |
| TCRH-423 | Bug fix | 8位或16位操作数传入内联汇编导致编译器崩溃 |

## 快速开始

```git bash
# 方式一：使用 Make
make all
make test

# 方式二：使用 CMake
mkdir build && cd build
cmake ..
make
ctest

# 方式三：运行全部测试脚本
./scripts/run_all.sh