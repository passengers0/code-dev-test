# -*- coding: utf-8 -*-
"""Generate v16: color-code rows in '汽车标准不支持组合及原因' by reason category.
Fix: unmerge note row A15:H15 first, then place legend at R14-18, note moves to R19."""
import openpyxl
from openpyxl.styles import PatternFill, Font, Alignment, Border, Side

SRC = r"C:\Users\yj202\DoubaoWork\chats\2026-09-07\new-chat\riscv_isa_comparison_v15.xlsx"
OUT = r"C:\Users\yj202\DoubaoWork\chats\2026-09-07\new-chat\riscv_isa_comparison_v16.xlsx"

wb = openpyxl.load_workbook(SRC)
ns = wb["汽车标准不支持组合及原因"]

thin = Side(style="thin", color="FFBFBFBF")
border = Border(left=thin, right=thin, top=thin, bottom=thin)
wrap = Alignment(vertical="top", wrap_text=True)
center = Alignment(horizontal="center", vertical="center", wrap_text=True)

# 1. free R15: unmerge and take note text
note_text = None
if "A15:H15" in [str(m) for m in ns.merged_cells.ranges]:
    ns.unmerge_cells("A15:H15")
note_text = ns.cell(row=15, column=1).value
for c in range(1, 9):
    ns.cell(row=15, column=c).value = None
print("note text captured:", (note_text or "")[:40], "...")

# 2. color data rows 3..13 by category
CAT = {
    3:  ("编译器指令实现缺失（LLVM 未实现指令选择）", "FFF4CCCC"),   # Zacas
    4:  ("编译器指令实现缺失（LLVM 未实现指令选择）", "FFF4CCCC"),   # Zicfilp
    5:  ("编译器指令实现缺失（LLVM 未实现指令选择）", "FFF4CCCC"),   # Zicfiss
    6:  ("硬件/微架构行为特性（固件/硬件 CSR 配置）", "FFD6E4F0"),   # Smdbltrp
    7:  ("硬件/微架构行为特性（固件/硬件 CSR 配置）", "FFD6E4F0"),   # Smcntrpmf
    8:  ("安全/内存保护扩展（草案或 OS+硬件协同）", "FFFCE4D6"),     # Sspmp
    9:  ("安全/内存保护扩展（草案或 OS+硬件协同）", "FFFCE4D6"),     # Sspm
    10: ("虚拟内存/OS 运行环境（MMU 模式，OS 配置）", "FFD9EAD3"),   # Sv48
    11: ("虚拟内存/OS 运行环境（MMU 模式，OS 配置）", "FFD9EAD3"),   # Sv57
    12: ("硬件/微架构行为特性（固件/硬件 CSR 配置）", "FFD6E4F0"),   # Svvptc
    13: ("调试硬件接口（调试器/调试工具链范畴）", "FFE1D5E7"),       # Sdtrig
}
for r, (cat, fill) in CAT.items():
    fillobj = PatternFill("solid", fgColor=fill)
    for c in range(1, 9):
        cell = ns.cell(row=r, column=c)
        cell.fill = fillobj
        cell.border = border
        cell.alignment = wrap if c >= 5 else center
    ns.cell(row=r, column=4).font = Font(color="FF9C0006", bold=True)
    print(f"R{r} {ns.cell(row=r,column=2).value[:22]:24s} -> {cat[:30]}")

# 3. legend at R14-18
legend = [
    ("FFF4CCCC", "编译器指令实现缺失（LLVM 未实现指令选择）：Zacas / Zicfilp / Zicfiss —— 需编译器生成指令，库只能模拟或汇编补丁"),
    ("FFD6E4F0", "硬件/微架构行为特性（固件/硬件 CSR 配置，编译器无指令需求）：Smdbltrp / Smcntrpmf / Svvptc"),
    ("FFFCE4D6", "安全/内存保护扩展（OS+硬件协同，其中 Sspmp 为 Draft 未批准）：Sspmp / Sspm"),
    ("FFD9EAD3", "虚拟内存/OS 运行环境（MMU 模式，OS 配置范畴）：Sv48 / Sv57"),
    ("FFE1D5E7", "调试硬件接口（调试器/调试工具链范畴）：Sdtrig"),
]
row = 14
for fill, text in legend:
    a = ns.cell(row=row, column=1)
    a.fill = PatternFill("solid", fgColor=fill)
    a.border = border
    b = ns.cell(row=row, column=2, value=text)
    b.alignment = Alignment(vertical="center", wrap_text=True)
    b.font = Font(size=9)
    ns.merge_cells(start_row=row, start_column=2, end_row=row, end_column=8)
    for c in range(2, 9):
        ns.cell(row=row, column=c).border = border
    ns.row_dimensions[row].height = 24
    row += 1

# 4. note to R19
t = ns.cell(row=19, column=1, value=note_text)
t.font = Font(italic=True, size=9, color="FF595959")
t.alignment = wrap
ns.merge_cells("A19:H19")
ns.row_dimensions[19].height = 30

wb.save(OUT)
print("saved:", OUT)
