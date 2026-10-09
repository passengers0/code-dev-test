# -*- coding: utf-8 -*-
"""Scan v14: find P-extension rows and all rows where B(draft) is non-empty and C(Hightec) == x."""
import openpyxl

path = r"C:\Users\yj202\DoubaoWork\chats\2026-09-07\new-chat\riscv_isa_comparison_v14.xlsx"
wb = openpyxl.load_workbook(path, data_only=False)
ws = wb["RISC-V ISA 全量对比"]

print("sheets:", wb.sheetnames)
print("\n-- P extension rows (A col contains P ) --")
for r in range(4, 178):
    a = ws.cell(row=r, column=1).value
    if a and isinstance(a, str) and (a.startswith("P") or a.startswith("P ")):
        print(f"R{r}: A={a!r} B={ws.cell(row=r,column=2).value!r} C={ws.cell(row=r,column=3).value!r}")

print("\n-- draft-required (B in {√,-}) but HighTec unsupported (C=x) --")
for r in range(5, 178):
    b = ws.cell(row=r, column=2).value
    c = ws.cell(row=r, column=3).value
    if b in ("√", "-") and c == "×":
        print(f"R{r}: A={ws.cell(row=r,column=1).value!r} B={b!r} C={c!r}")

print("\n-- 参考文档 sheet current rows 1..40 (A) --")
ref = wb["参考文档"]
for r in range(1, 41):
    v = ref.cell(row=r, column=1).value
    if v:
        print(f"R{r}: {str(v)[:90]}")
