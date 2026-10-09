# -*- coding: utf-8 -*-
"""Generate v15:
1. P extension (R17) HighTec col: √ -> - (library/asm support), update note R186
2. New sheet '汽车标准不支持组合及原因' with 11 draft-required but unsupported ISAs + analysis
3. 参考文档: remove analysis rows 21-36 (restore to original links only)
"""
import openpyxl, copy

SRC = r"C:\Users\yj202\DoubaoWork\chats\2026-09-07\new-chat\riscv_isa_comparison_v14.xlsx"
OUT = r"C:\Users\yj202\DoubaoWork\chats\2026-09-07\new-chat\riscv_isa_comparison_v15.xlsx"

wb = openpyxl.load_workbook(SRC)
ws = wb["RISC-V ISA 全量对比"]
ref = wb["参考文档"]

# ---- 1. P extension ----
r17 = ws.cell(row=17, column=3)
dash_style = copy.deepcopy(ws["C20"]._style)  # yellow '-'
print(f"P R17: C {r17.value!r} -> '-'")
r17.value = "-"
r17._style = copy.deepcopy(dash_style)

# update note R186 (P extension note)
note = ("7. P扩展(Packed-SIMD DSP)：草案未纳入(CARV-RT26/M26/A26未提及)；"
        "HighTec v11.0编译器的LLVM/Clang 21.1.0未实现P指令自动生成，"
        "但指令集可由汇编/内联汇编或DSP软件库方式使用，故标注为部分支持(-)。"
        "RISC-V P扩展(Packed SIMD, v0.9草案)用于8/16位打包数据的DSP运算(饱和算术、点积、移位等)，汽车电机控制/音频等实时信号处理场景。")
ws["A186"] = note
print("note A186 updated")

# ---- 3. 参考文档: clear analysis rows 21-36 ----
cleared = 0
for r in range(21, 37):
    if ref.cell(row=r, column=1).value is not None:
        ref.cell(row=r, column=1).value = None
        cleared += 1
print(f"参考文档 analysis rows cleared: {cleared}")

# ---- 2. new sheet ----
if "汽车标准不支持组合及原因" in wb.sheetnames:
    del wb["汽车标准不支持组合及原因"]
ns = wb.create_sheet("汽车标准不支持组合及原因")

from openpyxl.styles import Font, PatternFill, Alignment, Border, Side
from openpyxl.utils import get_column_letter

hdr_font = Font(bold=True, color="FFFFFFFF")
hdr_fill = PatternFill("solid", fgColor="FF4472C4")
title_font = Font(bold=True, size=14, color="FF1F3864")
red_font = Font(color="FF9C0006", bold=True)
thin = Side(style="thin", color="FFBFBFBF")
border = Border(left=thin, right=thin, top=thin, bottom=thin)
wrap = Alignment(vertical="top", wrap_text=True)
center = Alignment(horizontal="center", vertical="center", wrap_text=True)

ns["A1"] = "汽车标准不支持组合及原因（草案要求但 HighTec v11.0 编译器不支持的 ISA 扩展）"
ns["A1"].font = title_font
ns.merge_cells("A1:H1")
ns["A1"].alignment = Alignment(vertical="center")
ns.row_dimensions[1].height = 28

headers = ["序号", "扩展", "草案要求", "HighTec v11.0", "官方定义", "规范状态", "应用场景（用在什么地方）", "不支持原因及理论依据 / 库方式可行性"]
for c, h in enumerate(headers, 1):
    cell = ns.cell(row=2, column=c, value=h)
    cell.font = hdr_font
    cell.fill = hdr_fill
    cell.alignment = center
    cell.border = border
ns.row_dimensions[2].height = 30

rows = [
    ("Zacas — 比较并交换指令", "-", "×",
     "AMOCAS.W/D/Q 原子比较并交换（Compare-and-Swap）指令：读旧值-比较-条件写入单指令完成，CAS 语义原子化",
     "Ratified v1.0（Unpriv ISA 附录，RVA23 强制）",
     "多核无锁算法（并发队列/栈/引用计数），比 LR/SC 循环扩展性更好、可规避 ABA 问题；汽车多核 ECU 共享内存同步",
     "userguide Tab.1 未列出 → LLVM/Clang 21.1.0 未实现 amocas 指令选择，编译器不生成该指令。库方式：可用内联汇编封装或用 LR/SC 软件模拟，但非指令级原生支持"),
    ("Zicfilp — 着陆垫(CFI Landing Pad)", "-", "×",
     "CFI 间接跳转着陆垫：x7 标签寄存器 + lpad 指令，间接跳转目标必须落在合法着陆垫上，否则触发异常",
     "Ratified v1.0（CFI 规范，RVB23 包含，Linux 6.7+ 支持）",
     "防 ROP/JOP 攻击的代码指针完整性保护；汽车功能安全（ISO 26262 ASIL）与网络安全（ISO/SAE 21434）要求的控制流完整性",
     "userguide 未列出 → 编译器无 CFI 全链路支持（插桩/标签校验），库只能在汇编层做有限补丁，无法替代编译器级保护"),
    ("Zicfiss — 阴影栈(CFI Shadow Stack)", "-", "×",
     "CFI 影子栈：ssp CSR + sspush/sspopchk/ssrdp/ssamoswap 指令 + SS 页 PTE 编码，函数返回地址压入影子栈并在返回时校验",
     "Ratified v1.0（CFI 规范，RVB23 包含）",
     "防返回地址篡改（返回导向编程）；汽车电子控制单元安全启动/可信执行，与 Zicfilp 组合构成完整 CFI",
     "userguide 未列出 → 需编译器序言/尾声插桩 + 特权态页表（SS 页）配合，LLVM 未实现；库无法单独提供，需工具链+OS 协同"),
    ("Smdbltrp — 双重陷阱", "-", "×",
     "mstatus.MDT 位控制 trap 处理过程中再遇 trap 的行为（进入死循环 trap 时触发更高层处理），防止 trap 递归死锁",
     "Ratified（Privileged ISA）",
     "虚拟化/安全隔离中 trap 处理鲁棒性，防止恶意或错误代码导致 trap 循环；汽车虚拟化（混合关键性）场景",
     "userguide 未列出 → 属硬件/固件特性（M 态 CSR 行为），编译器无指令需求，工具链不涉及；固件可配置 MDT（CSR 内联汇编），与编译器支持无关"),
    ("Smcntrpmf — 周期与特权模式过滤", "-", "×",
     "mcyclecfg/minstretcfg CSR 按特权级过滤 cycle/instret 计数器更新，限制低特权级对计时/指令计数的观测",
     "Ratified v1.0（Privileged ISA）",
     "性能监控与侧信道防护：S/U 态无法读取受保护计数器，用于安全环境（TEE）与功能安全诊断",
     "userguide 未列出 → 硬件 CSR 特性，编译器无指令需求；固件/驱动可配置，与编译工具链支持无关"),
    ("Sspmp — S态物理内存保护(SPMP)", "-", "×",
     "S-mode PMP：为 S 态（操作系统/管理程序）提供物理内存隔离，弥补传统 PMP 仅覆盖 M 态的限制",
     "Draft（SPMP Task Group 提案，未批准）",
     "S 态内存隔离/安全容器、虚拟机隔离、嵌入式安全；汽车 ECU 中 OS 与安全关键任务的内存隔离",
     "userguide 未列出 → 规范为草案未定稿 + 硬件特性，编译器不涉及；即便硬件支持，也由 OS/固件配置，非编译工具链范畴"),
    ("Sspm — S态指针掩码(Pointer Masking)", "-", "×",
     "S 态指针掩码：senvcfg 配置 PME，硬件将地址高位与掩码比较/替换，实现对象粒度内存标记与防护（注：官方定义为 Pointer Masking，非性能监控；表格A列原注有误）",
     "Ratified v1.0（Pointer Masking 规范，Smnpm/Smmpm/Sspm 系列）",
     "内存安全/指针压缩/标签：CFI 与内存标记辅助，JIT 与沙箱地址隔离；汽车安全关键软件内存防护",
     "userguide 未列出 → 编译器未实现 S 态指针掩码的地址生成/检查支持；需硬件+OS 配合，库仅可做软件指针掩码（性能受限）"),
    ("Sv48 — 48位虚拟内存系统", "-", "×",
     "satp 模式 Sv48：48 位虚拟地址（256 TiB），四级页表，需物理地址≥48 位",
     "Ratified（Privileged ISA）",
     "大内存 Linux/多应用虚拟化：汽车智能座舱/ADAS 域控大地址空间与多虚拟机隔离",
     "userguide 未列出 → 属 OS/MMU 运行环境特性，编译器仅与 -mcmodel（大地址代码模型）间接关联；工具链本身不'支持'该模式，由内核配置"),
    ("Sv57 — 57位虚拟内存系统", "-", "×",
     "satp 模式 Sv57：57 位虚拟地址（128 PiB），五级页表",
     "Ratified（Privileged ISA）",
     "超大地址空间（数据库/内存计算）；车规高端 SoC 未来扩展，当前应用少",
     "userguide 未列出 → 同 Sv48：OS/MMU 特性，编译器无关；x86 下需 5 级页表，RISC-V 侧由内核使能"),
    ("Svvptc — 虚拟页表缓存", "-", "×",
     "PTE invalid→valid 转变在有界时间内对所有观察者可见，免除显式 memory-management fence（sfence.vma 可省）",
     "Ratified v1.0（Privileged ISA，RVA23S64 可选）",
     "页表更新性能优化：热路径缺页处理省掉 fence；汽车 Linux/QNX 内核调度与内存管理性能",
     "userguide 未列出 → 硬件/微架构行为特性（页表缓存实现保证），编译器无关；Linux 内核据此省略 fence，与工具链无关"),
    ("Sdtrig — 调试触发(Debug Trigger)", "-", "×",
     "RISC-V Debug Spec v1.0 Trigger Module：tselect/tdata1-3 CSR 定义硬件断点/观察点/跟踪触发（如链式条件触发）",
     "Ratified v1.0（Debug Spec）",
     "硬件调试/性能分析：gdb 断点、跟踪器（Trace）、运行时监控；汽车 ECU 现场调试与失效分析",
     "userguide 未列出 → 属调试器/调试硬件接口（gdb/OpenOCD/JTAG），非编译器指令；SBI DBTR 扩展提供 S 态抽象，库/固件可实现，与编译工具链无关"),
]

from openpyxl.styles import Font as F
r = 3
for i, (name, draft, htc, definition, status, usage, reason) in enumerate(rows, 1):
    vals = [i, name, draft, htc, definition, status, usage, reason]
    for c, v in enumerate(vals, 1):
        cell = ns.cell(row=r, column=c, value=v)
        cell.border = border
        cell.alignment = wrap if c >= 5 else center
        if c == 4:
            cell.font = red_font
    ns.row_dimensions[r].height = 90
    r += 1

widths = [6, 22, 9, 10, 38, 20, 34, 42]
for c, w in enumerate(widths, 1):
    ns.column_dimensions[get_column_letter(c)].width = w

# reference legend on the new sheet
lr = r + 1
ns.cell(row=lr, column=1, value="说明：草案要求=B列(√强制/-可选)；HighTec v11.0=userguide Tab.1 支持表(√ Supported / - Assembly Support / × 未列出=不支持)；本表仅收录草案要求但 HighTec 标 × 的扩展，分析依据为 HighTec userguide 与 RISC-V 官方规范。").font = F(italic=True, size=9, color="FF595959")
ns.merge_cells(start_row=lr, start_column=1, end_row=lr, end_column=8)
ns.cell(row=lr, column=1).alignment = wrap
ns.row_dimensions[lr].height = 30

# move new sheet after 参考文档
wb.move_sheet("汽车标准不支持组合及原因", offset=1)

wb.save(OUT)
print("saved:", OUT)
print("sheets:", wb.sheetnames)
