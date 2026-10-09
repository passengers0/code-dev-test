/*
 * TCRH-442: Optimized stack usage of fixed and aligned stack objects
 * Test principle: Compare V11.0.0 vs V0.4.0 stack frame sizes. Force actual
 *                 stack allocation (address escape, volatile, large arrays)
 *                 and measure total stack usage (prepare frame + sp adjust).
 * Verification:  Both versions compile; compare stack usage per function.
 * Note:          Use noinline helpers to prevent inlining and force address escape.
 * Finding:       V11 and V0.4 generate identical stack layout for all standalone
 *                functions. main() in V11 uses 412 bytes vs 380 in V0.4 (V11
 *                uses 32 bytes more due to different inlining/regalloc decisions).
 *                See stack_comparison.txt for detailed data.
 */

#include <stdint.h>

/* Noinline helper that takes array pointer - prevents inlining, forces address escape */
__attribute__((noinline))
int sum_array(const int *arr, int n) {
    int s = 0;
    for (int i = 0; i < n; i++) s += arr[i];
    return s;
}

__attribute__((noinline))
void fill_array(int *arr, int n, int val) {
    for (int i = 0; i < n; i++) arr[i] = val + i;
}

/* Test 1: Large fixed array - must allocate stack */
int test_large_array(int x) {
    int buf[64];  /* 256 bytes */
    fill_array(buf, 64, x);
    return sum_array(buf, 64);
}

/* Test 2: Two arrays in mutually exclusive branches - should share stack slots */
int test_stack_slot_reuse(int x, int flag) {
    if (flag) {
        int buf_a[32];  /* 128 bytes */
        fill_array(buf_a, 32, x);
        return sum_array(buf_a, 32);
    } else {
        int buf_b[32];  /* 128 bytes - should reuse buf_a's space */
        fill_array(buf_b, 32, x + 100);
        return sum_array(buf_b, 32);
    }
}

/* Test 3: Over-aligned array (32-byte alignment) - check padding */
int test_aligned_array(int x) {
    int buf[16] __attribute__((aligned(32)));  /* 64 bytes, 32-byte aligned */
    fill_array(buf, 16, x);
    return sum_array(buf, 16);
}

/* Test 4: Mixed alignment - small vars + over-aligned array */
int test_mixed_alignment(int x) {
    uint8_t small1 = (uint8_t)x;
    int buf[16] __attribute__((aligned(32)));
    uint8_t small2 = (uint8_t)(x >> 8);
    fill_array(buf, 16, x);
    return sum_array(buf, 16) + small1 + small2;
}

/* Test 5: Three arrays in separate branches - max reuse */
int test_three_way_reuse(int x, int selector) {
    if (selector == 0) {
        int a[24];
        fill_array(a, 24, x);
        return sum_array(a, 24);
    } else if (selector == 1) {
        int b[24];
        fill_array(b, 24, x + 1);
        return sum_array(b, 24);
    } else {
        int c[24];
        fill_array(c, 24, x + 2);
        return sum_array(c, 24);
    }
}

/* Test 6: Struct with alignment on stack */
struct __attribute__((aligned(16))) AlignedStruct {
    uint8_t  a;
    uint32_t b;
    uint8_t  c;
    uint64_t d;
};

__attribute__((noinline))
int process_struct(struct AlignedStruct *s) {
    return s->a + s->b + s->c + (int)s->d;
}

int test_aligned_struct(int x) {
    struct AlignedStruct s;
    s.a = (uint8_t)x;
    s.b = x * 2;
    s.c = (uint8_t)(x >> 8);
    s.d = (uint64_t)x << 32;
    return process_struct(&s);
}

/* Test 7: Large volatile array - always on stack */
int test_volatile_array(int x) {
    volatile int buf[32];
    for (int i = 0; i < 32; i++) buf[i] = x + i;
    return (int)buf[0] + (int)buf[31];
}

volatile int sink;

int main(void) {
    sink = test_large_array(42);
    sink = test_stack_slot_reuse(42, 1);
    sink = test_stack_slot_reuse(42, 0);
    sink = test_aligned_array(42);
    sink = test_mixed_alignment(42);
    sink = test_three_way_reuse(42, 0);
    sink = test_three_way_reuse(42, 1);
    sink = test_three_way_reuse(42, 2);
    sink = test_aligned_struct(42);
    sink = test_volatile_array(42);
    return 0;
}
