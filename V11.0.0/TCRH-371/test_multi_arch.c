/*
 * TCRH-371: Support for generating code for different core versions from G3K to G4MH2
 * Test principle: Compile the same source with different -march values and verify
 *                 that each target architecture produces valid code without errors.
 * Verification:  Compilation succeeds for all -march variants; generated assembly
 *                 contains architecture-appropriate instructions (e.g. FXU on g4mh2).
 */

#include <stdint.h>

/* Test 1: Basic arithmetic - should compile on all cores */
int32_t test_add(int32_t a, int32_t b) {
    return a + b;
}

int32_t test_multiply(int32_t a, int32_t b) {
    return a * b;
}

/* Test 2: 64-bit operations */
int64_t test_add64(int64_t a, int64_t b) {
    return a + b;
}

uint64_t test_shift64(uint64_t val, int shift) {
    return val << shift;
}

/* Test 3: Floating-point (single precision - all cores support spfpu) */
float test_fadd(float a, float b) {
    return a + b;
}

float test_fmul(float a, float b) {
    return a * b;
}

/* Test 4: Memory operations */
void test_memcpy_local(void *dst, const void *src, uint32_t n) {
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    for (uint32_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
}

/* Test 5: Bit manipulation */
uint32_t test_bitops(uint32_t x) {
    return __builtin_clz(x) + __builtin_ctz(x) + __builtin_popcount(x);
}

/* Test 6: Loop with conditional */
int32_t test_sum_array(const int32_t *arr, int len) {
    int32_t sum = 0;
    for (int i = 0; i < len; i++) {
        if (arr[i] > 0) {
            sum += arr[i];
        }
    }
    return sum;
}

volatile int32_t sink32;
volatile int64_t sink64;
volatile float sinkf;

int main(void) {
    sink32 = test_add(10, 20);
    sink32 = test_multiply(6, 7);
    sink64 = test_add64(0x100000000LL, 0x200000000LL);
    sink64 = test_shift64(0xFF, 8);
    sinkf = test_fadd(1.5f, 2.5f);
    sinkf = test_fmul(3.0f, 4.0f);
    sink32 = test_bitops(0x80000001);

    int32_t arr[5] = {1, -2, 3, -4, 5};
    sink32 = test_sum_array(arr, 5);

    uint8_t src[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t dst[4] = {0};
    test_memcpy_local(dst, src, 4);
    sink32 = dst[0] | (dst[1] << 8) | (dst[2] << 16) | (dst[3] << 24);

    return 0;
}
