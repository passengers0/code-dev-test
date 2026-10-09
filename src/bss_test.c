/*
 * CCRM-150 test: -fno-zero-initialized-in-bss-gcc
 *
 * Expected behavior (from HighTec User Guide 11.0.0):
 *   variable                 default        -fno-zero-initialized-in-bss    -fno-zero-initialized-in-bss-gcc
 *   int implicit_zero;       .bss           .data                            .bss
 *   int explicit_zero = 0;   .bss           .data                            .data
 */

int implicit_zero;          /* implicitly zero initialized */
int explicit_zero = 0;      /* explicitly zero initialized  */
int nonzero = 7;            /* nonzero -> always .data      */
