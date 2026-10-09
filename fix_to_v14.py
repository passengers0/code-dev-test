# -*- coding: utf-8 -*-
"""Generate v14: correct the v13 mistake based on userguide evidence.
- Zks: × -> √  (userguide: Supported)
- Zvkng, Zvksg: × -> - (userguide: Assembly Support)
- other 11 candidates: keep × (not listed in userguide)
- rewrite analysis block in 参考文档 with verified definitions."""
import openpyxl, shutil, copy

SRC = r"C:\Users\yj202\DoubaoWork\chats\2026-09-07\new-chat\riscv_isa_comparison_v12.xlsx"
OUT = r"C:\Users\yj202\DoubaoWork\chats\2026-09-07\new-chat\riscv_isa_comparison_v14.xlsx"

wb = openpyxl.load_workbook(SRC)
ws = wb["RISC-V ISA 全量对比"]
ref = wb["参考文档"]

# style templates
sqrt_style = copy.deepcopy(ws["C5"]._style)    # √ green
dash_style = copy.deepcopy(ws["C20"]._style)   # - yellow

# updates: row -> (new_value, style)
updates = {
    85: ("√", sqrt_style),    # Zks
    97: ("-", dash_style),    # Zvkng
    98: ("-", dash_style),    # Zvksg
}
for r, (val, st) in updates.items():
    cell = ws.cell(row=r, column=3)
    print(f"R{r} {ws.cell(row=r,column=1).value[:25]:27s} {cell.value!r} -> {val!r}")
    cell.value = val
    cell._style = copy.deepcopy(st)

# verify the other 11 stay ×
keep = [27, 65, 66, 135, 137, 143, 147, 156, 157, 158, 173]
for r in keep:
    v = ws.cell(row=r, column=3).value
    assert v == "×", f"R{r} expected ×, got {v!r}"
    print(f"R{r} {ws.cell(row=r,column=1).value[:25]:27s} stays ×")

# ---- rewrite analysis block in 参考文档 (rows 21..) ----
analysis = [
    "▎2026-09-14 v14 修正：HighTec v11.0 不支持/受限的汽车标准扩展——官方定义与解释（以 userguide Tab.1 支持表为准，修正 v13 全量改-的错误）",
    "",
    "[Zks 标量密码学ShangMi套件] 定义：RISC-V Crypto Vol.I 标量国密套件(Zksh=SM3指令 sm3p0/sm3p1；Zksed=SM4指令 sm4ed/sm4ks)，Ratified v1.0.1。用于国密SM3/SM4加速。HighTec v11.0：userguide=Supported → 完全支持(√)。v12原标×有误，已修正。",
    "[Zvkng 向量加密NIST(GCM)] 定义：RISC-V Vector Crypto v1.0，AES-GCM 向量加速指令。HighTec v11.0：userguide=Assembly Support(-)，可用汇编/内联汇编，编译器不自动生成指令。",
    "[Zvksg 向量加密商密(GCM)] 定义：Vector Crypto v1.0 国密 SM4-GCM 向量加速。HighTec v11.0：userguide=Assembly Support(-)。",
    "[Zacas 比较并交换] 定义：AMOCAS.W/D/Q 原子CAS指令，Unpriv ISA Ratified v1.0，RVA23强制。用于多核无锁算法(比LR/SC扩展性好、可规避ABA)。HighTec v11.0：userguide未列出 → 编译器不支持(×)；LLVM 21.1.8未实现amocas指令选择。可用内联汇编或lr/sc软件模拟，但非指令级支持。",
    "[Zicfilp 着陆垫] 定义：CFI间接跳转着陆垫(x7标签寄存器+lpad指令)，防ROP/JOP。RVB23包含、Linux 6.7+已支持。HighTec v11.0：userguide未列出 → 编译器不支持(×)；需LLVM全链路CFI支持，库仅可汇编层有限补丁。",
    "[Zicfiss 阴影栈] 定义：CFI影子栈(ssp CSR+sspush/sspopchk/ssrdp/ssamoswap指令+SS页PTE编码)，防返回地址篡改。HighTec v11.0：userguide未列出 → 编译器不支持(×)；需编译器+特权页表配合。",
    "[Smdbltrp 双重陷阱] 定义：Privileged ISA Ratified，mstatus.MDT位控制trap处理中再遇trap的行为(防trap循环死锁)，用于虚拟化/安全隔离。HighTec v11.0：userguide未列出 → 不支持(×)；属硬件/固件特性，编译器无指令需求，固件可配置MDT(CSR内联汇编)。",
    "[Smcntrpmf 周期与特权模式过滤] 定义：Privileged ISA Ratified v1.0，mcyclecfg/minstretcfg CSR按特权级过滤cycle/instret计数，用于性能监控/诊断。HighTec v11.0：userguide未列出 → 不支持(×)；硬件CSR特性，固件/驱动可配置。",
    "[Sspmp S态物理内存保护] 定义：S-mode PMP(SPMP)，S态内存隔离，SPMP Task Group提案，当前为Draft(未批准)。HighTec v11.0：userguide未列出 → 不支持(×)；草案未定稿+硬件特性。",
    "[Sspm] 定义澄清：官方定义为 S态指针掩码(S-mode Pointer Masking, Pointer Masking Spec v1.0 Ratified，Smnpm/Smmpm/Sspm系列)，非性能监控——表格原注'S-mode Performance Monitor'有误。HighTec v11.0：userguide未列出 → 不支持(×)。",
    "[Sv48 48位虚拟内存] 定义：Privileged ISA Ratified，48位虚拟地址(256TiB，satp模式，需物理地址≥48)。HighTec v11.0：userguide未列出 → 不支持(×)；属OS/MMU运行环境特性，编译器仅与-mcmodel关联。",
    "[Sv57 57位虚拟内存] 定义：Privileged ISA Ratified，57位虚拟地址(128PiB)。HighTec v11.0：userguide未列出 → 不支持(×)；同上。",
    "[Svvptc 虚拟页表缓存] 定义：Privileged ISA Ratified v1.0，PTE invalid→valid转变在有界时间可见，免除显式memory-management fence，RVA23S64可选。HighTec v11.0：userguide未列出 → 不支持(×)；硬件/微架构行为特性，编译器无关，Linux内核可据此省略fence。",
    "[Sdtrig 调试触发] 定义：RISC-V Debug Spec v1.0 Trigger Module(TM，tselect/tdata1-3 CSR)，硬件断点/观察点/跟踪。HighTec v11.0：userguide未列出 → 不支持(×)；属调试器(gdb/OpenOCD/SBI DBTR扩展)支持范畴，非编译器指令。",
]
start = 21
for i, line in enumerate(analysis):
    ref.cell(row=start + i, column=1).value = line

wb.save(OUT)
print("saved:", OUT)
