/*
 * TCRH-443: Improved code generation to fuse more instructions with conditional
 *           branch instructions
 * Test principle: Create code patterns with compare + conditional branch sequences
 *                 and verify the compiler fuses them into compare-and-branch
 *                 instructions (e.g. bnc, bne, beq with embedded compare).
 * Verification:  Compilation succeeds with -O2; assembly shows fused compare-branch
 *                 patterns rather than separate cmp + branch instruction pairs.
 */

#include <stdint.h>

/* Test 1: Simple if-else with integer comparison */
int test_simple_branch(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

/* Test 2: Equality comparison */
int test_equality_branch(int a, int b) {
    if (a == b) {
        return 1;
    }
    return 0;
}

/* Test 3: Inequality comparison */
int test_inequality_branch(int a, int b) {
    if (a != b) {
        return a - b;
    }
    return 0;
}

/* Test 4: Less-than comparison */
int test_less_than(int a, int b) {
    if (a < b) {
        return b - a;
    }
    return a - b;
}

/* Test 5: Unsigned comparison */
int test_unsigned_branch(uint32_t a, uint32_t b) {
    if (a < b) {  /* Unsigned less than */
        return 1;
    }
    return 0;
}

/* Test 6: Comparison with immediate */
int test_compare_immediate(int a) {
    if (a > 100) {
        return a - 100;
    } else if (a < 0) {
        return -a;
    }
    return a;
}

/* Test 7: Loop with condition (counted loop) */
int test_loop_branch(const int *arr, int len) {
    int sum = 0;
    for (int i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum;
}

/* Test 8: While loop with complex condition */
int test_while_branch(int *arr, int len) {
    int i = 0;
    int sum = 0;
    while (i < len && arr[i] != 0) {
        sum += arr[i];
        i++;
    }
    return sum;
}

/* Test 9: Switch statement (branch table / jump chain) */
int test_switch_branch(int x) {
    switch (x) {
        case 0: return 10;
        case 1: return 20;
        case 2: return 30;
        case 3: return 40;
        default: return -1;
    }
}

/* Test 10: Logical AND/OR short-circuit branches */
int test_logical_branch(int a, int b, int c) {
    if (a > 0 && b > 0 && c > 0) {
        return a + b + c;
    }
    if (a < 0 || b < 0) {
        return -1;
    }
    return 0;
}

/* Test 11: Ternary operator (conditional move or branch) */
int test_ternary(int a, int b) {
    return (a > b) ? a : b;
}

/* Test 12: Bit test and branch */
int test_bit_test(uint32_t flags) {
    if (flags & 0x1) {
        return 1;
    }
    if (flags & 0x2) {
        return 2;
    }
    if (flags & 0x4) {
        return 3;
    }
    return 0;
}

/* Test 13: Pointer comparison (NULL check) */
int test_null_check(const int *p) {
    if (p == 0) {
        return -1;
    }
    return *p;
}

/* Test 14: Character comparison */
int test_char_branch(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 1;
    }
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 1;
    }
    return 0;
}

/* Test 15: Do-while loop */
int test_dowhile_branch(int n) {
    int sum = 0;
    int i = 0;
    do {
        sum += i;
        i++;
    } while (i < n);
    return sum;
}

volatile int sink;

int main(void) {
    int arr[8] = {1, 2, 3, 4, 5, 6, 7, 8};

    sink = test_simple_branch(10, 20);
    sink = test_equality_branch(10, 10);
    sink = test_inequality_branch(10, 20);
    sink = test_less_than(10, 20);
    sink = test_unsigned_branch(10u, 20u);
    sink = test_compare_immediate(150);
    sink = test_loop_branch(arr, 8);
    sink = test_while_branch(arr, 8);
    sink = test_switch_branch(2);
    sink = test_logical_branch(1, 2, 3);
    sink = test_ternary(10, 20);
    sink = test_bit_test(0x5);
    sink = test_null_check(arr);
    sink = test_null_check(0);
    sink = test_char_branch('m');
    sink = test_dowhile_branch(10);

    return 0;
}
