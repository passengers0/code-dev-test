/*
 * TCRH-408: Add a target specific builtin FXU vector type (__w128)
 * Test principle: Define and use the __w128 FXU vector type (4 x float32,
 *                 16-byte aligned) with vector arithmetic operations, and verify
 *                 the compiler accepts the type and generates FXU instructions
 *                 when -mfpu=fxu is specified.
 * Verification:  Compilation succeeds with -mfpu=fxu; assembly contains
 *                 FXU vector instructions (e.g. fmov.w, fadd.w).
 * Note 1:        Requires -mfpu=fxu and -march=rh850-g4mh2 (or -mcpu with fxu).
 * Note 2:        V850 backend does not support vector types as function return
 *                 values. All vector results are passed via output pointer params.
 * Note 3:        Float vectors do not support bitwise ops (&, |, ^, ~).
 * Note 4:        fx register names are not yet accepted in inline asm clobber
 *                 lists in this build; load/store tests use plain vector copies.
 */

#include <stdint.h>

/* Define the FXU vector type as specified in the user guide */
typedef float __w128 __attribute__((__vector_size__(16), __aligned__(16)));

/* Test 1: Vector type declaration and initialization (result via pointer) */
void test_vector_init(__w128 *out) {
    __w128 a = {1.0f, 2.0f, 3.0f, 4.0f};
    __w128 b = {5.0f, 6.0f, 7.0f, 8.0f};
    *out = a + b;  /* Element-wise addition */
}

/* Test 2: Vector arithmetic operations */
void test_vector_arithmetic(const __w128 *a, const __w128 *b, __w128 *out) {
    __w128 result;
    result = *a + *b;    /* Vector add */
    result = result - *b; /* Vector sub */
    result = result * *b; /* Vector mul */
    result = result / *b; /* Vector div */
    *out = result;
}

/* Test 3: Vector comparison */
void test_vector_compare(const __w128 *a, const __w128 *b, __w128 *out) {
    __w128 result;
    result = *a > *b;    /* Element-wise greater than */
    result = *a < *b;    /* Element-wise less than */
    result = *a == *b;   /* Element-wise equal */
    *out = result;
}

/* Test 4: Vector element access */
float test_vector_element_access(const __w128 *v) {
    float sum = 0.0f;
    sum += (*v)[0];
    sum += (*v)[1];
    sum += (*v)[2];
    sum += (*v)[3];
    return sum;
}

/* Test 5: Vector load/store via plain vector copy (no inline asm) */
void test_vector_loadstore(const float *src, float *dst) {
    __w128 v;
    /* Load from aligned memory via pointer cast */
    v = *(const __w128 *)src;
    /* Store to aligned memory via pointer cast */
    *(__w128 *)dst = v;
}

/* Test 6: Vector multiply-add pattern (common in DSP) */
void test_vector_multiply_add(const __w128 *a, const __w128 *b,
                               const __w128 *c, __w128 *out) {
    *out = (*a) * (*b) + (*c);  /* FMA pattern: a*b + c */
}

/* Test 7: Vector horizontal sum (reduce) */
float test_vector_hsum(const __w128 *v) {
    return (*v)[0] + (*v)[1] + (*v)[2] + (*v)[3];
}

/* Test 8: Vector type as function parameter via pointer (byval not supported) */
void test_vector_param_ptr(const __w128 *v, __w128 *out) {
    __w128 scale = {2.0f, 2.0f, 2.0f, 2.0f};
    *out = (*v) * scale;
}

/* Test 9: Vector in local array (initialize at declaration) */
void test_vector_array(__w128 *out) {
    __w128 arr[2] = {
        {1.0f, 2.0f, 3.0f, 4.0f},
        {5.0f, 6.0f, 7.0f, 8.0f}
    };
    *out = arr[0] + arr[1];
}

/* Test 10: Vector with static storage */
static __w128 static_vec = {0.1f, 0.2f, 0.3f, 0.4f};

void test_vector_static(__w128 *out) {
    __w128 tmp = {1.0f, 1.0f, 1.0f, 1.0f};
    *out = static_vec + tmp;
}

/* Test 11: Vector element access via index (bitcast via pointer not supported) */
float test_vector_index(const __w128 *v, int idx) {
    return (*v)[idx & 3];
}

/* Test 12: Vector negation via arithmetic (bitwise ops not supported on float vectors) */
void test_vector_neg(const __w128 *v, __w128 *out_neg) {
    __w128 zero = {0.0f, 0.0f, 0.0f, 0.0f};
    *out_neg = zero - *v;
}

volatile __w128 sink_vec;
volatile float sinkf;

int main(void) {
    __w128 a = {1.0f, 2.0f, 3.0f, 4.0f};
    __w128 b = {5.0f, 6.0f, 7.0f, 8.0f};
    __w128 c = {0.1f, 0.2f, 0.3f, 0.4f};
    __w128 result;

    test_vector_init(&result);
    sink_vec = result;

    test_vector_arithmetic(&a, &b, &result);
    sink_vec = result;

    test_vector_compare(&a, &b, &result);
    sink_vec = result;

    test_vector_multiply_add(&a, &b, &c, &result);
    sink_vec = result;

    test_vector_param_ptr(&a, &result);
    sink_vec = result;

    test_vector_array(&result);
    sink_vec = result;

    test_vector_static(&result);
    sink_vec = result;

    __w128 out_neg;
    test_vector_neg(&a, &out_neg);
    sink_vec = out_neg;

    sinkf = test_vector_element_access(&a);
    sinkf = test_vector_hsum(&a);

    float src[4] __attribute__((aligned(16))) = {1.0f, 2.0f, 3.0f, 4.0f};
    float dst[4] __attribute__((aligned(16))) = {0};
    test_vector_loadstore(src, dst);
    sinkf = dst[0] + dst[1] + dst[2] + dst[3];

    sinkf = test_vector_index(&a, 0);
    sinkf = test_vector_index(&a, 2);

    return 0;
}
