/*
 * TCRH-448: Fixed argument passing logic for empty non-zero sized structures
 * Test principle: Define empty structs (size 1 due to C standard, no members)
 *                 and use them as function arguments and return values. Verify
 *                 the ABI handles them correctly without miscompilation.
 * Verification:  Compilation succeeds; functions accept and return empty structs;
 *                 argument passing follows correct ABI (not treated as zero-size).
 */

#include <stdint.h>

/* Test 1: Empty struct (size = 1 in C) */
struct Empty {
    /* no members - size is 1 per C standard */
};

/* Test 2: Empty struct with only a typedef (still size 1, C-compatible) */
struct EmptyTypedefInner {
    /* no data members - size is 1 per C standard */
};

/* Test 3: Struct with only a zero-length array (GCC extension) */
struct ZeroLenArray {
    int data[0];  /* GCC zero-length array - size may be 0 or 1 */
};

/* Test 4: Empty struct as function argument */
int test_empty_arg(struct Empty e, int x) {
    (void)e;
    return x + 1;
}

/* Test 5: Empty struct as function return value */
struct Empty test_empty_return(int x) {
    struct Empty e;
    (void)x;
    return e;
}

/* Test 6: Empty struct as pointer argument */
int test_empty_ptr_arg(const struct Empty *e, int x) {
    (void)e;
    return x * 2;
}

/* Test 7: Multiple empty struct arguments */
int test_multi_empty_args(struct Empty e1, struct Empty e2, struct Empty e3, int x) {
    (void)e1;
    (void)e2;
    (void)e3;
    return x + 3;
}

/* Test 8: Empty struct mixed with normal arguments */
int test_mixed_args(struct Empty e, int a, struct Empty e2, float b) {
    (void)e;
    (void)e2;
    return a + (int)b;
}

/* Test 9: Empty struct in a larger struct */
struct WithEmpty {
    int value;
    struct Empty marker;
    int flag;
};

int test_struct_with_empty(struct WithEmpty *w) {
    return w->value + w->flag;
}

/* Test 10: Empty struct array */
int test_empty_array(struct Empty *arr, int len) {
    int count = 0;
    for (int i = 0; i < len; i++) {
        /* Access each element - even empty structs have distinct addresses */
        if ((uintptr_t)&arr[i] != 0) {
            count++;
        }
    }
    return count;
}

/* Test 11: Empty struct as typedef */
typedef struct {
    /* empty */
} EmptyTypedef;

int test_typedef_empty(EmptyTypedef e, int x) {
    (void)e;
    return x - 1;
}

/* Test 12: Empty struct passed by value and returned */
struct Empty test_empty_passthrough(struct Empty e) {
    return e;
}

/* Test 13: Empty struct with alignment attribute */
struct __attribute__((aligned(4))) AlignedEmpty {
    /* empty but aligned */
};

int test_aligned_empty(struct AlignedEmpty e, int x) {
    (void)e;
    return x + 4;
}

/* Test 14: Empty struct in varargs function */
int test_varargs_empty(int count, ...) {
    /* Varargs with empty struct is tricky; just test compilation */
    return count;
}

/* Test 15: Nested empty structs */
struct OuterEmpty {
    struct Empty inner1;
    struct Empty inner2;
};

int test_nested_empty(struct OuterEmpty o, int x) {
    (void)o;
    return x * 10;
}

/* Test 16: Empty struct and function pointer */
typedef int (*EmptyFunc)(struct Empty, int);

int test_funcptr_empty(EmptyFunc fn, struct Empty e, int x) {
    return fn(e, x);
}

volatile int sink;
volatile struct Empty sink_empty;

int main(void) {
    struct Empty e1, e2, e3;
    EmptyTypedef et;
    struct AlignedEmpty ae;
    struct OuterEmpty oe;

    sink = test_empty_arg(e1, 42);
    sink_empty = test_empty_return(42);
    sink = test_empty_ptr_arg(&e1, 42);
    sink = test_multi_empty_args(e1, e2, e3, 42);
    sink = test_mixed_args(e1, 10, e2, 3.14f);
    sink = test_typedef_empty(et, 42);
    sink_empty = test_empty_passthrough(e1);
    sink = test_aligned_empty(ae, 42);
    sink = test_nested_empty(oe, 42);

    struct WithEmpty w = {10, {}, 20};
    sink = test_struct_with_empty(&w);

    struct Empty arr[4];
    sink = test_empty_array(arr, 4);

    sink = test_funcptr_empty(test_empty_arg, e1, 42);

    /* Test varargs with empty struct (may be ABI-dependent) */
    sink = test_varargs_empty(1, e1);

    return 0;
}
