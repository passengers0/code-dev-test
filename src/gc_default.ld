/* Minimal linker script for the GC default test. */
OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 64K
}

SECTIONS
{
    .text : { *(.text*) } > RAM
    .data : { *(.data*) *(.rodata*) } > RAM
    .bss  : { *(.bss*) *(COMMON) } > RAM
}
