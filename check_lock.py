# -*- coding: utf-8 -*-
"""Diagnose whether v12 xlsx is locked by another process."""
import os

p = r"C:\Users\yj202\DoubaoWork\chats\2026-09-07\new-chat\riscv_isa_comparison_v12.xlsx"
print("exists:", os.path.exists(p))
try:
    f = open(p, "a+b")
    f.close()
    print("WRITABLE: no lock detected")
except PermissionError as e:
    print("LOCKED:", e)

# also check for lock files / office temp files in the directory
d = os.path.dirname(p)
for name in os.listdir(d):
    if name.startswith("~$") or "riscv_isa" in name.lower():
        print("dir entry:", name, os.path.getsize(os.path.join(d, name)) if os.path.exists(os.path.join(d, name)) else "")
