# -*- coding: utf-8 -*-
"""Dump riscv_isa_comparison_v12.xlsx structure for inspection."""
import openpyxl, json, sys

path = r"C:\Users\yj202\DoubaoWork\chats\2026-09-07\new-chat\riscv_isa_comparison_v12.xlsx"
wb = openpyxl.load_workbook(path, data_only=False)
print("sheets:", wb.sheetnames)
for sn in wb.sheetnames:
    ws = wb[sn]
    print(f"\n=== sheet '{sn}' dims={ws.dimensions} max_row={ws.max_row} max_col={ws.max_column}")
    print("merged:", [str(m) for m in list(ws.merged_cells.ranges)[:40]])
    # header rows: first 4 rows
    for r in range(1, min(6, ws.max_row + 1)):
        vals = []
        for c in range(1, ws.max_column + 1):
            v = ws.cell(row=r, column=c).value
            if v is not None:
                vals.append(f"{openpyxl.utils.get_column_letter(c)}{r}={v!r}")
        print("R%d: %s" % (r, " | ".join(vals)))
