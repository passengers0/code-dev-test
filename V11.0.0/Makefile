# RH850/V850 v11.0.0 TCRH Test Suite Makefile
# Compiler: HighTec RH850 Clang v11.0.0
# Compatible with Windows (cmd.exe) and GNU Make

# Toolchain configuration
TOOLCHAIN = C:/HighTec/toolchains/v850/v11.0.0
CC = $(TOOLCHAIN)/bin/clang.exe
OBJDUMP = $(TOOLCHAIN)/bin/llvm-objdump.exe

# Default architecture
ARCH = rh850-g4mh2
CFLAGS = -march=$(ARCH) -O2 -Wall
CFLAGS_S = -march=$(ARCH) -O2 -S
OBJDUMPFLAGS = -d

BUILD_DIR = build

# Test source files (all 13 TCRH items)
SRCS = \
    TCRH-371/test_multi_arch.c \
    TCRH-453/test_u2b_cpu.c \
    TCRH-424/test_aliased_regs.c \
    TCRH-341/test_libs_improve.c \
    TCRH-408/test_fxu_vector.c \
    TCRH-409/test_fxu_asm_constraint.c \
    TCRH-456/test_os_inline_unroll.c \
    TCRH-442/test_stack_optimize.c \
    TCRH-443/test_branch_fusion.c \
    TCRH-445/test_mov_fusion_crash.c \
    TCRH-448/test_empty_struct.c \
    TCRH-451/test_interrupt_funcptr.c \
    TCRH-454/test_inline_asm_sysreg.c

# FXU test sources (need -mfpu=fxu)
FXU_SRCS = TCRH-408/test_fxu_vector.c TCRH-409/test_fxu_asm_constraint.c

# Generated targets
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))
# Extra: U2B-EZ variant of TCRH-453
U2B_G3KH_OBJ = $(BUILD_DIR)/TCRH-453/test_u2b_cpu_g3kh.o
# Extra: -Os variant of TCRH-456 (for size comparison with -O2)
OS_456_OBJ = $(BUILD_DIR)/TCRH-456/test_os_inline_unroll_Os.o
ASMS = $(patsubst %.c,$(BUILD_DIR)/%.s,$(SRCS))
DUMPS = $(patsubst %.c,$(BUILD_DIR)/%.dump,$(SRCS))

# Build subdirectories
BUILD_SUBDIRS = $(addprefix $(BUILD_DIR)/,$(sort $(dir $(SRCS))))

.PHONY: all clean test compile asm dump

all: compile asm dump

# Create build directories (Windows compatible)
$(BUILD_DIR):
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@if not exist $(BUILD_DIR)\TCRH-371 mkdir $(BUILD_DIR)\TCRH-371
	@if not exist $(BUILD_DIR)\TCRH-453 mkdir $(BUILD_DIR)\TCRH-453
	@if not exist $(BUILD_DIR)\TCRH-424 mkdir $(BUILD_DIR)\TCRH-424
	@if not exist $(BUILD_DIR)\TCRH-341 mkdir $(BUILD_DIR)\TCRH-341
	@if not exist $(BUILD_DIR)\TCRH-408 mkdir $(BUILD_DIR)\TCRH-408
	@if not exist $(BUILD_DIR)\TCRH-409 mkdir $(BUILD_DIR)\TCRH-409
	@if not exist $(BUILD_DIR)\TCRH-456 mkdir $(BUILD_DIR)\TCRH-456
	@if not exist $(BUILD_DIR)\TCRH-442 mkdir $(BUILD_DIR)\TCRH-442
	@if not exist $(BUILD_DIR)\TCRH-443 mkdir $(BUILD_DIR)\TCRH-443
	@if not exist $(BUILD_DIR)\TCRH-445 mkdir $(BUILD_DIR)\TCRH-445
	@if not exist $(BUILD_DIR)\TCRH-448 mkdir $(BUILD_DIR)\TCRH-448
	@if not exist $(BUILD_DIR)\TCRH-451 mkdir $(BUILD_DIR)\TCRH-451
	@if not exist $(BUILD_DIR)\TCRH-454 mkdir $(BUILD_DIR)\TCRH-454
	@echo Build dirs ready

# Compile to .o (verifies no compiler crash)
# FXU tests get extra -mfpu=fxu flag
$(BUILD_DIR)/TCRH-408/%.o: TCRH-408/%.c | $(BUILD_DIR)
	@echo [CC] $< -^> $@
	@$(CC) $(CFLAGS) -mfpu=fxu -c $< -o $@

$(BUILD_DIR)/TCRH-409/%.o: TCRH-409/%.c | $(BUILD_DIR)
	@echo [CC] $< -^> $@
	@$(CC) $(CFLAGS) -mfpu=fxu -c $< -o $@

# U2B-E (u2b-eva): DPFPU + FXU + HV, no -march (mcpu implies arch)
$(BUILD_DIR)/TCRH-453/%.o: TCRH-453/%.c | $(BUILD_DIR)
	@echo [CC-u2b-eva] $< -^> $@
	@$(CC) -mcpu=u2b-eva -O2 -Wall -c $< -o $@

# U2B-EZ (u2b-eva-g3kh): SPFPU only, G3KH core
$(BUILD_DIR)/TCRH-453/%_g3kh.o: TCRH-453/%.c | $(BUILD_DIR)
	@echo [CC-u2b-eva-g3kh] $< -^> $@
	@$(CC) -mcpu=u2b-eva-g3kh -O2 -Wall -c $< -o $@

# TCRH-456 -Os variant (size optimization comparison)
$(BUILD_DIR)/TCRH-456/%_Os.o: TCRH-456/%.c | $(BUILD_DIR)
	@echo [CC-Os] $< -^> $@
	@$(CC) -march=$(ARCH) -Os -Wall -c $< -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo [CC] $< -^> $@
	@$(CC) $(CFLAGS) -c $< -o $@

# Generate assembly (for instruction verification)
$(BUILD_DIR)/TCRH-408/%.s: TCRH-408/%.c | $(BUILD_DIR)
	@echo [ASM] $< -^> $@
	@$(CC) $(CFLAGS_S) -mfpu=fxu $< -o $@

$(BUILD_DIR)/TCRH-409/%.s: TCRH-409/%.c | $(BUILD_DIR)
	@echo [ASM] $< -^> $@
	@$(CC) $(CFLAGS_S) -mfpu=fxu $< -o $@

# U2B-E assembly
$(BUILD_DIR)/TCRH-453/%.s: TCRH-453/%.c | $(BUILD_DIR)
	@echo [ASM-u2b-eva] $< -^> $@
	@$(CC) -mcpu=u2b-eva -O2 -S $< -o $@

$(BUILD_DIR)/%.s: %.c | $(BUILD_DIR)
	@echo [ASM] $< -^> $@
	@$(CC) $(CFLAGS_S) $< -o $@

# Generate disassembly
$(BUILD_DIR)/%.dump: $(BUILD_DIR)/%.o | $(BUILD_DIR)
	@echo [OBJDUMP] $< -^> $@
	@$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@

# Compile all (verification: no crash)
compile: $(OBJS) $(U2B_G3KH_OBJ) $(OS_456_OBJ)

# Generate all assembly files
asm: $(ASMS)

# Generate all disassembly files
dump: $(DUMPS)

# Run all verification scripts (requires bash)
test: compile asm dump
	@echo === Running V850 v11.0.0 TCRH Tests ===
	@bash scripts/run_all.sh $(CC)
	@echo === All tests completed ===

# Clean (Windows compatible: rmdir /s /q)
clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@echo Cleaned $(BUILD_DIR)
