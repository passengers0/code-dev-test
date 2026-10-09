# RH850/V850 v0.4.0 TCRH Test Suite Makefile
# Compiler: HighTec RH850 Clang v0.4.0
# Compatible with Windows (cmd.exe) and GNU Make

# Toolchain configuration
CC = C:/HighTec/toolchains/v850/v0.4.0/bin/clang.exe
OBJDUMP = C:/HighTec/toolchains/v850/v0.4.0/bin/llvm-objdump.exe
CFLAGS = -march=rh850-g4mh2 -O2
CFLAGS_S = -march=rh850-g4mh2 -O2 -S
OBJDUMPFLAGS = -d

BUILD_DIR = build

# Test source files
SRCS = \
    TCRH-257/test_tailcall.c \
    TCRH-420/test_builtin_reg.c \
    TCRH-421/test_stv_dw.c \
    TCRH-423/test_inline_asm_8_16.c

# Generated targets
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))
ASMS = $(patsubst %.c,$(BUILD_DIR)/%.s,$(SRCS))
DUMPS = $(patsubst %.c,$(BUILD_DIR)/%.dump,$(SRCS))

# Build subdirectories
BUILD_SUBDIRS = $(addprefix $(BUILD_DIR)/,$(sort $(dir $(SRCS))))

.PHONY: all clean test compile asm dump

all: compile asm dump

# Create build directories (Windows compatible)
$(BUILD_DIR):
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@if not exist $(BUILD_DIR)\TCRH-257 mkdir $(BUILD_DIR)\TCRH-257
	@if not exist $(BUILD_DIR)\TCRH-420 mkdir $(BUILD_DIR)\TCRH-420
	@if not exist $(BUILD_DIR)\TCRH-421 mkdir $(BUILD_DIR)\TCRH-421
	@if not exist $(BUILD_DIR)\TCRH-423 mkdir $(BUILD_DIR)\TCRH-423
	@echo Build dirs ready

# Compile to .o
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo [CC] $< -^> $@
	@$(CC) $(CFLAGS) -c $< -o $@

# Generate assembly
$(BUILD_DIR)/%.s: %.c | $(BUILD_DIR)
	@echo [ASM] $< -^> $@
	@$(CC) $(CFLAGS_S) $< -o $@

# Generate disassembly
$(BUILD_DIR)/%.dump: $(BUILD_DIR)/%.o | $(BUILD_DIR)
	@echo [OBJDUMP] $< -^> $@
	@$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@

# Compile all
compile: $(OBJS)

# Generate all assembly files
asm: $(ASMS)

# Generate all disassembly files
dump: $(DUMPS)

# Run tests (requires bash)
test: compile asm dump
	@echo === run test ===
	@bash scripts/check_tailcall.sh $(BUILD_DIR)/TCRH-257/test_tailcall.s
	@bash scripts/check_stvdw.sh $(BUILD_DIR)/TCRH-421/test_stv_dw.s
	@bash scripts/check_reg.sh $(BUILD_DIR)/TCRH-420/test_builtin_reg.s
	@bash scripts/check_asm_8_16.sh $(BUILD_DIR)/TCRH-423/test_inline_asm_8_16.s
	@echo === Test done! ===

# Clean (Windows compatible)
clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@echo Cleaned $(BUILD_DIR)
