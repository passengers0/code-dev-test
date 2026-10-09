/*
 * TCRH-456: Improved function inline and loop unrolling strategy when optimizing for size
 * Test principle: Compare -O2 vs -Os code generation. At -O2, the compiler should
 *                 more aggressively inline medium-sized functions and unroll simple
 *                 loops. At -Os, these size-increasing optimizations should be
 *                 suppressed, resulting in smaller code.
 * Verification:  Both -O2 and -Os compile successfully; -Os generates smaller
 *                 .o text section than -O2 due to less inlining and loop unrolling.
 * Note:          Use llvm-size to compare text section sizes.
 */

#include <stdint.h>

/* Test 1: Medium-sized static function called multiple times.
 * At -O2 this may be inlined at all call sites (code duplication).
 * At -Os this should remain as a function call (smaller). */
static int process_value(int x, int y) {
    int t1 = x * y + x;
    int t2 = (t1 << 2) | (t1 >> 30);
    int t3 = t2 ^ (t2 >> 16);
    return t3 + y;
}

int test_inline_calls(int a, int b) {
    int r1 = process_value(a, b);
    int r2 = process_value(b, a);
    int r3 = process_value(a + 1, b - 1);
    int r4 = process_value(b + 2, a - 2);
    return r1 + r2 + r3 + r4;
}

/* Test 2: Simple loop with known trip count 8.
 * At -O2 this may be fully unrolled (8x load+add).
 * At -Os this should remain as a counted loop (much smaller). */
int test_loop_unroll(const int32_t *arr) {
    int32_t sum = 0;
    for (int i = 0; i < 8; i++) {
        sum += arr[i];
    }
    return sum;
}

/* Test 3: Loop with trip count 4 and simple body.
 * At -O2 this is likely unrolled; at -Os it may remain as loop. */
int test_loop_small(const int32_t *arr) {
    int32_t sum = 0;
    for (int i = 0; i < 4; i++) {
        sum += arr[i] * 3;
    }
    return sum;
}

/* Test 4: Another medium function for inlining test. */
static int transform(int v) {
    v = v * 1103515245 + 12345;
    v = (v >> 16) & 0x7FFF;
    return v;
}

int test_transform_array(int32_t *out, const int32_t *in, int n) {
    int total = 0;
    for (int i = 0; i < n; i++) {
        out[i] = transform(in[i]);
        total += out[i];
    }
    return total;
}

/* Test 5: always_inline - should be inlined at both levels. */
static inline __attribute__((always_inline))
int forced_inline_op(int a, int b) {
    return (a << 4) | (b & 0xF);
}

int test_always_inline(int x, int y) {
    return forced_inline_op(x, y) + forced_inline_op(y, x)
         + forced_inline_op(x + 1, y + 1);
}

/* Test 6: noinline - should not be inlined at either level. */
static __attribute__((noinline))
int forced_noinline_op(int a) {
    return a * a + a + 1;
}

int test_noinline(int x) {
    return forced_noinline_op(x) + forced_noinline_op(x + 1);
}

volatile int32_t sink;

int main(void) {
    int32_t arr[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int32_t out[8];

    sink = test_inline_calls(10, 20);
    sink = test_loop_unroll(arr);
    sink = test_loop_small(arr);
    sink = test_transform_array(out, arr, 8);
    sink = test_always_inline(0xAB, 0xCD);
    sink = test_noinline(7);

    return 0;
}
