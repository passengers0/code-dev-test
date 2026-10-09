# -*- coding: utf-8 -*-
"""v12 -> v13 in-place: mark 14 automotive extensions as partial (-) for HighTec,
keep all formatting; append reason analysis to '参考文档' sheet; backup first."""
import openpyxl, shutil, datetime, copy

SRC = r"C:\Users\yj202\DoubaoWork\chats\2026-09-07\new-chat\riscv_isa_comparison_v12.xlsx"
OUT = r"C:\Users\yj202\DoubaoWork\chats\2026-09-07\new-chat\riscv_isa_comparison_v13.xlsx"
BAK = r"C:\Users\yj202\DoubaoWork\chats\2026-09-07\new-chat\riscv_isa_comparison_v12_backup_20260914.xlsx"
shutil.copy2(SRC, BAK)
print("backup created:", BAK)

rows = [27, 65, 66, 85, 97, 98, 135, 137, 143, 147, 156, 157, 158, 173]

wb = openpyxl.load_workbook(SRC)
ws = wb["RISC-V ISA 全量对比"]
ref = wb["参考文档"]

# sample style of '-' cell
src_cell = ws["C20"]  # Zalrsc HighTec = '-'
style_template = copy.deepcopy(src_cell._style)

changed = []
for r in rows:
    cell = ws.cell(row=r, column=3)
    assert cell.value == "×", f"R{r}C value {cell.value!r} != ×"
    cell.value = "-"
    cell._style = copy.deepcopy(style_template)
    changed.append(r)
print("changed rows:", changed)

# ---- append reason analysis to 参考文档 sheet (keep original content) ----
analysis = [
    "▎2026-09-14 更新：HighTec v11.0 不支持的汽车标准扩展原因分析（状态已改为 - 部分支持，可通过库方式支持）",
    "",
    "[Zacas 比较并交换] 用途：多核无锁数据结构核心原语(原子CAS)。不支持原因：LLVM/Clang 21.1.0 后端未实现 amocas 指令选择。库方式：内联汇编封装，或 __atomic_compare_exchange 内建经 libatomic 以 lr/sc 序列软件模拟。",
    "[Zicfilp 着陆垫] 用途：控制流完整性(CFI)，间接跳转目标校验，防 ROP/JOP 攻击(功能安全/信息安全场景)。不支持原因：需编译器生成 landing pad 指令并全链路 CFI 支持，LLVM 21.1.0 未合入。库方式：函数指针/间接调用封装+汇编层着陆垫补丁(有限)。",
    "[Zicfiss 阴影栈] 用途：硬件阴影栈防返回地址篡改(ROP 防御)。不支持原因：需编译器在函数序言/尾声生成 ss push/pop 指令，LLVM 21.1.0 未实现。库方式：软件阴影栈库(入口保存返回地址、出口校验)。",
    "[Zks 标量密码学ShangMi套件] 用途：国密 SM3/SM4 标量加速(哈希/对称加密，国密合规)。不支持原因：LLVM 21.1.0 未实现 sm3p0/sm3p1/sm4ed/sm4ks 指令选择。库方式：mbedTLS/OpenSSL 国密软件库，纯库级支持。",
    "[Zvkng 向量加密NIST(GCM)] 用途：AES-GCM 向量加速(安全启动/OTA/V2X 加密)。不支持原因：LLVM 21.1.0 V 扩展密码学子集未实现 GCM 专用向量指令。库方式：软件加密库向量化 AES-GCM。",
    "[Zvksg 向量加密商密(GCM)] 用途：SM4-GCM 向量加速(国密合规)。不支持原因：同上，LLVM 未实现商密向量 GCM 指令。库方式：国密软件库。",
    "[Smdbltrp 双重陷阱] 用途：处理 trap 内再 trap 的嵌套异常，防止 trap 循环死锁(虚拟化/安全隔离)。不支持原因：硬件特权架构异常流程特性，编译器无指令可生成。库方式：机器态固件/启动代码实现双重陷阱处理例程。",
    "[Smcntrpmf 周期与特权模式过滤] 用途：性能计数器按特权级过滤与溢出中断，实时监控/故障诊断。不支持原因：CSR 硬件特性，工具链无指令需求。库方式：固件库配置 CSR。",
    "[Sspmp S态物理内存保护] 用途：S 态(OS/Hypervisor)物理内存隔离，多租户安全。不支持原因：硬件/架构特性，非指令扩展。库方式：OS/固件库配置 PMP CSR。",
    "[Sspm S态性能监控] 用途：S 态任务级性能监控。不支持原因：硬件特性。库方式：驱动/库代码。",
    "[Sv48 48位虚拟内存] 用途：256TiB 地址空间，多 OS/虚拟化场景。不支持原因：MMU 运行环境特性，编译器仅与 -mcmodel 关联，工具链未验证该配置。库方式：OS/Hypervisor 库与链接配置(-mcmodel=large)。",
    "[Sv57 57位虚拟内存] 用途：128PiB 地址空间(需物理地址>48位支持)。不支持原因：同上。库方式：同上。",
    "[Svvptc 虚拟页表缓存] 用途：虚拟化中 vCPU 页表缓存，降低 TLB 开销。不支持原因：硬件特性。库方式：Hypervisor 库代码。",
    "[Sdtrig 调试触发] 用途：硬件断点/观察点(trigger 模块)。不支持原因：调试器支持问题(调试协议/工具链调试栈)，非编译器指令。库方式：调试器库/驱动(OpenOCD 类)支持。",
]
start = 21
for i, line in enumerate(analysis):
    ref.cell(row=start + i, column=1).value = line

wb.save(OUT)
print("saved:", OUT)
