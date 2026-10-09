# -*- coding: utf-8 -*-
"""Check P row in v12 backup and legend/notes rows 178-186."""
import openpyxl

for path, tag in [
    (r"C:\Users\yj202\DoubaoWork\chats\2026-09-07\new-chat\riscv_isa_comparison_v12_backup_20260914.xlsx", "v12 backup"),
    (r"C:\Users\yj202\DoubaoWork\chats\2026-09-07\new-chat\riscv_isa_comparison_v14.xlsx", "v14"),
]:
    wb = openpyxl.load_workbook(path, data_only=False)
    ws = wb["RISC-V ISA 全量对比"]
    print(f"\n===== {tag} =====")
    # find all rows whose A mentions Packed or exactly 'P'
    for r in range(4, 178):
        a = ws.cell(row=r, column=1).value
        if a and isinstance(a, str) and ("Packed" in a or a.strip() in ("P", "P ")):
            print(f"R{r}: A={a!r} B={ws.cell(row=r,column=2).value!r} C={ws.cell(row=r,column=3).value!r}")
    print("-- legend/notes rows 177-186 --")
    for r in range(177, 187):
        vals = [ws.cell(row=r, column=c).value for c in range(1, 6)]
        if any(v is not None for v in vals):
            print(f"R{r}: {vals}")
