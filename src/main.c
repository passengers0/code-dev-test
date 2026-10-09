/*
 * End-to-end smoke test: compile + link a full C program with
 * picolibc on the HighTec RISC-V 11.0.0 toolchain.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int called = 0;

int add(int a, int b)
{
    called++;
    return a + b;
}

int main(void)
{
    int r = add(20, 22);
    char buf[32];
    memcpy(buf, "Hello RISC-V 11.0.0", 20);
    buf[20] = 0;
    printf("%s -> %d (called=%d)\n", buf, r, called);
    return 0;
}
