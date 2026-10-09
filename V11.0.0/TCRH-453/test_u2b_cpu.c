/*
 * TCRH-453: Add support for U2B-E and U2B-EZ CPUs
 * Test principle: Compile with -mcpu=u2b-eva (U2B-E) and -mcpu=u2b-eva-g3kh (U2B-EZ) to verify the
 *                 toolchain recognizes these new CPU targets and generates
 *                 correct code with appropriate feature sets.
 * Verification:  Compilation succeeds for both -mcpu=u2b-eva (U2B-E) and -mcpu=u2b-eva-g3kh (U2B-EZ);
 *                 u2b-eva (U2B-E) enables DPFPU+FXU+HV+G4 ops; u2b-eva-g3kh (U2B-EZ) has SPFPU only, G3KH core.
 */

#include <stdint.h>

/* Test 1: Core arithmetic - valid on both U2B-E and U2B-EZ */
int32_t test_arithmetic(int32_t a, int32_t b, int32_t c) {
    return (a * b) + c - (a >> 2);
}

/* Test 2: Single-precision float - both support spfpu */
float test_spfpu(float x, float y) {
    return x * y + x;
}

/* Test 3: Double-precision float - both support dfpu */
double test_dfpu(double x, double y) {
    return x / y + 1.0;
}

/* Test 4: Integer division and modulo */
int32_t test_divmod(int32_t a, int32_t b) {
    if (b == 0) return -1;
    return (a / b) * (a % b);
}

/* Test 5: Hardware loop / branch intensive code */
int32_t test_find_max(const int32_t *data, int len) {
    int32_t max = data[0];
    for (int i = 1; i < len; i++) {
        if (data[i] > max) {
            max = data[i];
        }
    }
    return max;
}

/* Test 6: Bit field operations */
typedef struct {
    uint32_t flag1 : 1;
    uint32_t flag2 : 3;
    uint32_t value : 12;
    uint32_t reserved : 16;
} BitFields;

uint32_t test_bitfield(BitFields *bf) {
    bf->flag1 = 1;
    bf->flag2 = 5;
    bf->value = 0xABC;
    return bf->flag1 | (bf->flag2 << 1) | (bf->value << 4);
}

/* Test 7: 64-bit atomic-like read (non-atomic, tests 64-bit access pattern) */
uint64_t test_read64(const volatile uint64_t *p) {
    return *p;
}

void test_write64(volatile uint64_t *p, uint64_t val) {
    *p = val;
}

volatile int32_t sink32;
volatile float sinkf;
volatile double sinkd;
volatile uint64_t sink64;

int main(void) {
    sink32 = test_arithmetic(10, 3, 5);
    sinkf = test_spfpu(2.0f, 3.0f);
    sinkd = test_dfpu(10.0, 3.0);
    sink32 = test_divmod(17, 5);

    int32_t data[6] = {3, 7, 2, 9, 4, 1};
    sink32 = test_find_max(data, 6);

    BitFields bf = {0};
    sink32 = test_bitfield(&bf);

    volatile uint64_t val = 0x123456789ABCDEF0ULL;
    sink64 = test_read64(&val);
    test_write64(&val, 0xFEDCBA9876543210ULL);
    sink64 = test_read64(&val);

    return 0;
}
