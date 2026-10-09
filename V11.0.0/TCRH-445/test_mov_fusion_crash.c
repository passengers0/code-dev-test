/*
 * TCRH-445: Fixed compiler crash in optimization pass where MOV instructions are fused
 * Test principle: Create code patterns that trigger the MOV instruction fusion
 *                 optimization pass (redundant MOV elimination, MOV sinking,
 *                 register allocation with MOV chains) and verify the compiler
 *                 no longer crashes.
 * Verification:  Compilation succeeds at -O2 without crash or internal error.
 */

#include <stdint.h>

/* Test 1: Redundant MOV via intermediate variable (MOV fusion candidate) */
int test_mov_chain(int a, int b) {
    int tmp1 = a;
    int tmp2 = tmp1;
    int tmp3 = tmp2;
    return tmp3 + b;
}

/* Test 2: MOV sinking - moves after conditional */
int test_mov_sinking(int a, int b, int c) {
    int result;
    if (a > b) {
        result = a;
    } else {
        result = b;
    }
    /* This MOV should be sunk / fused */
    int final = result;
    return final + c;
}

/* Test 3: Multiple assignments to same variable (dead MOV elimination) */
int test_dead_mov(int a) {
    int x = a;
    x = a + 1;
    x = a + 2;
    x = a + 3;
    return x;
}

/* Test 4: MOV between different register classes / types */
uint32_t test_mov_types(uint8_t a, uint16_t b) {
    uint32_t x = a;
    uint32_t y = b;
    uint32_t z = x;
    z = y;
    return z + x;
}

/* Test 5: Struct copy (MEM/MOV fusion) */
struct Small {
    int a;
    int b;
};

int test_struct_copy(int x, int y) {
    struct Small s1 = {x, y};
    struct Small s2 = s1;  /* Struct copy via MOVs */
    struct Small s3 = s2;
    return s3.a + s3.b;
}

/* Test 6: Array element copy chain */
int test_array_copy(int x) {
    int arr[4];
    arr[0] = x;
    arr[1] = arr[0];
    arr[2] = arr[1];
    arr[3] = arr[2];
    return arr[0] + arr[3];
}

/* Test 7: Function return value MOV chain */
static int helper(int x) {
    int y = x;
    int z = y;
    return z;
}

int test_return_mov_chain(int a) {
    int r1 = helper(a);
    int r2 = r1;
    int r3 = r2;
    return r3;
}

/* Test 8: Conditional MOV (CMOV pattern) */
int test_cond_mov(int a, int b, int cond) {
    int result;
    if (cond) {
        result = a;
    } else {
        result = b;
    }
    return result;
}

/* Test 9: Loop invariant MOV */
int test_loop_invariant_mov(int n, int x) {
    int invariant = x * 2;
    int sum = 0;
    for (int i = 0; i < n; i++) {
        int tmp = invariant;  /* Loop-invariant MOV */
        sum += tmp + i;
    }
    return sum;
}

/* Test 10: Pointer aliasing with MOV */
int test_pointer_mov(int *a, int *b) {
    int *p = a;
    int *q = p;
    int *r = q;
    *r = 42;
    return *b;
}

/* Test 11: 64-bit MOV chain (two 32-bit MOVs) */
uint64_t test_mov64(uint64_t x) {
    uint64_t y = x;
    uint64_t z = y;
    return z + 1;
}

/* Test 12: Bitfield assignment with MOV */
struct Bitfield {
    uint32_t a : 4;
    uint32_t b : 8;
    uint32_t c : 20;
};

uint32_t test_bitfield_mov(uint32_t x) {
    struct Bitfield bf;
    bf.a = (x & 0xF);
    bf.b = (x >> 4) & 0xFF;
    bf.c = (x >> 12) & 0xFFFFF;
    struct Bitfield bf2 = bf;
    return bf2.a | (bf2.b << 4) | (bf2.c << 12);
}

/* Test 13: Complex expression with many temporaries */
int test_complex_temporaries(int a, int b, int c, int d) {
    int t1 = a + b;
    int t2 = c + d;
    int t3 = t1;
    int t4 = t2;
    int t5 = t3 + t4;
    int t6 = t5;
    return t6;
}

/* Test 14: Volatile variable access with MOV */
volatile int g_volatile;

int test_volatile_mov(int x) {
    g_volatile = x;
    int tmp = g_volatile;
    int tmp2 = tmp;
    g_volatile = tmp2;
    return g_volatile;
}

/* Test 15: Nested function calls with MOV */
static int square(int x) { return x * x; }

int test_nested_call_mov(int a) {
    int r1 = square(a);
    int r2 = square(r1);
    int r3 = r2;
    return r3;
}

volatile int sink;

int main(void) {
    sink = test_mov_chain(10, 20);
    sink = test_mov_sinking(10, 20, 30);
    sink = test_dead_mov(10);
    sink = test_mov_types(0xAB, 0xCDEF);
    sink = test_struct_copy(10, 20);
    sink = test_array_copy(10);
    sink = test_return_mov_chain(10);
    sink = test_cond_mov(10, 20, 1);
    sink = test_loop_invariant_mov(10, 5);

    int a = 1, b = 2;
    sink = test_pointer_mov(&a, &b);
    sink = (int)test_mov64(0x123456789ABCDEF0ULL);
    sink = test_bitfield_mov(0xABCDEF);
    sink = test_complex_temporaries(1, 2, 3, 4);
    sink = test_volatile_mov(42);
    sink = test_nested_call_mov(3);

    return 0;
}
