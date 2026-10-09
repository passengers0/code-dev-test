/*
 * TCRH-341: Library improvements
 *   - Replaced some math library functions with improved implementation
 *   - Add hand-optimized memcpy and memmove library function implementations
 *   - Add support for dynamic rounding modes in compiler-rt software FP functions
 * Test principle: Exercise memcpy/memmove with various sizes/alignments, math
 *                 functions, and fesetround dynamic rounding mode changes.
 * Verification:  Compilation and linking succeed; disassembly shows optimized
 *                 memcpy/memmove calls; math functions resolve to improved libs.
 */

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <fenv.h>

/* Test 1: memcpy with various sizes (small, medium, large) */
void test_memcpy_variants(void) {
    uint8_t src[128], dst[128];

    /* Initialize source */
    for (int i = 0; i < 128; i++) src[i] = (uint8_t)(i * 7 + 3);

    /* Small copies (hand-optimized inline threshold) */
    memcpy(dst, src, 1);
    memcpy(dst, src, 2);
    memcpy(dst, src, 4);
    memcpy(dst, src, 8);
    memcpy(dst, src, 16);

    /* Medium copies */
    memcpy(dst, src, 32);
    memcpy(dst, src, 64);

    /* Large copy */
    memcpy(dst, src, 128);

    /* Unaligned source */
    memcpy(dst, src + 1, 32);
    memcpy(dst + 1, src, 32);
    memcpy(dst + 1, src + 3, 31);
}

/* Test 2: memmove with overlapping regions */
void test_memmove_overlap(void) {
    uint8_t buf[64];
    for (int i = 0; i < 64; i++) buf[i] = (uint8_t)i;

    /* Forward overlap (dst > src) */
    memmove(buf + 10, buf, 20);

    /* Backward overlap (dst < src) */
    memmove(buf, buf + 10, 20);

    /* Non-overlapping */
    uint8_t other[64];
    memmove(other, buf, 64);
}

/* Test 3: memset (related to memory function improvements) */
void test_memset_variants(void) {
    uint8_t buf[64];
    memset(buf, 0xAA, 1);
    memset(buf, 0x55, 4);
    memset(buf, 0x00, 16);
    memset(buf, 0xFF, 64);
}

/* Test 4: Math library functions - improved implementations */
float test_math_float(float x) {
    float result = 0.0f;
    result += sinf(x);
    result += cosf(x);
    result += sqrtf(x * x + 1.0f);
    result += fabsf(x - 2.0f);
    result += floorf(x);
    result += ceilf(x);
    result += truncf(x);
    result += roundf(x);
    result += fmodf(x, 3.0f);
    result += expf(x * 0.1f);
    result += logf(fabsf(x) + 1.0f);
    result += powf(fabsf(x) + 1.0f, 0.5f);
    return result;
}

double test_math_double(double x) {
    double result = 0.0;
    result += sin(x);
    result += cos(x);
    result += sqrt(x * x + 1.0);
    result += fabs(x - 2.0);
    result += floor(x);
    result += ceil(x);
    result += trunc(x);
    result += round(x);
    result += fmod(x, 3.0);
    result += exp(x * 0.1);
    result += log(fabs(x) + 1.0);
    result += pow(fabs(x) + 1.0, 0.5);
    return result;
}

/* Test 5: Dynamic rounding modes (compiler-rt software FP) */
float test_dynamic_rounding(float x, float y) {
    float result = 0.0f;

    /* Round to nearest (default) */
    fesetround(FE_TONEAREST);
    result += x / y;

    /* Round toward zero */
    fesetround(FE_TOWARDZERO);
    result += x / y;

    /* Round up (toward +infinity) */
    fesetround(FE_UPWARD);
    result += x / y;

    /* Round down (toward -infinity) */
    fesetround(FE_DOWNWARD);
    result += x / y;

    /* Restore default */
    fesetround(FE_TONEAREST);

    return result;
}

/* Test 6: nearbyint / rint / lrint respect rounding mode */
float test_rounding_functions(float x) {
    float result = 0.0f;

    fesetround(FE_TONEAREST);
    result += nearbyintf(x);

    fesetround(FE_UPWARD);
    result += nearbyintf(x);

    fesetround(FE_DOWNWARD);
    result += nearbyintf(x);

    fesetround(FE_TOWARDZERO);
    result += nearbyintf(x);

    fesetround(FE_TONEAREST);
    return result;
}

/* Test 7: String functions (strcpy, strlen, strcmp) */
int test_string_funcs(void) {
    const char *src = "Hello RH850 V11.0.0";
    char dst[64];
    strcpy(dst, src);
    int len = strlen(dst);
    int cmp = strcmp(dst, src);
    return len + cmp;
}

volatile uint8_t sink_buf[128];
volatile float sinkf;
volatile double sinkd;
volatile int sinkint;

int main(void) {
    test_memcpy_variants();
    test_memmove_overlap();
    test_memset_variants();

    sinkf = test_math_float(1.5f);
    sinkd = test_math_double(1.5);

    sinkf = test_dynamic_rounding(10.0f, 3.0f);
    sinkf = test_rounding_functions(2.5f);

    sinkint = test_string_funcs();

    /* Copy result to sink to prevent optimization */
    memcpy((void *)sink_buf, (const void *)sink_buf, 16);

    return 0;
}
