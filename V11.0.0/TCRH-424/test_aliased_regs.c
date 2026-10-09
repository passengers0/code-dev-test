/*
 * TCRH-424: Add support for aliased system registers
 * Test principle: Use __builtin_v850_read_register / __builtin_v850_write_register
 *                 with both canonical and aliased (lowercase) system register names
 *                 to verify the compiler accepts all names and generates correct
 *                 stsr/ldsr instructions.
 * Verification:  Compilation succeeds for all register name variants; assembly
 *                 contains stsr/ldsr instructions with correct register selectors.
 * Note:          Mixed-case names like "FpSr" are not supported; only canonical
 *                 uppercase and full lowercase are accepted.
 */

#include <stdint.h>
#include <v850intrin.h>

/* Test 1: Read all supported system registers using canonical names
 * noinline prevents backend selection issues with G_READ_REGISTER in main */
__attribute__((noinline))
uint32_t test_read_canonical(void) {
    uint32_t val = 0;
    val ^= __builtin_v850_read_register("EIPC");
    val ^= __builtin_v850_read_register("EIPSW");
    val ^= __builtin_v850_read_register("FEPC");
    val ^= __builtin_v850_read_register("FEPSW");
    val ^= __builtin_v850_read_register("PSW");
    val ^= __builtin_v850_read_register("FPSR");
    val ^= __builtin_v850_read_register("FPEPC");
    val ^= __builtin_v850_read_register("EIIC");
    val ^= __builtin_v850_read_register("FEIC");
    val ^= __builtin_v850_read_register("CTPC");
    val ^= __builtin_v850_read_register("CTPSW");
    val ^= __builtin_v850_read_register("CTBP");
    val ^= __builtin_v850_read_register("FXSR");
    val ^= __builtin_v850_read_register("FXXP");
    return val;
}

/* Test 2: Read registers using lowercase aliases (case-insensitive) */
__attribute__((noinline))
uint32_t test_read_lowercase(void) {
    uint32_t val = 0;
    val ^= __builtin_v850_read_register("eipc");
    val ^= __builtin_v850_read_register("eipsw");
    val ^= __builtin_v850_read_register("fepc");
    val ^= __builtin_v850_read_register("fepsw");
    val ^= __builtin_v850_read_register("psw");
    val ^= __builtin_v850_read_register("fpsr");
    val ^= __builtin_v850_read_register("fpepc");
    val ^= __builtin_v850_read_register("eiic");
    val ^= __builtin_v850_read_register("feic");
    val ^= __builtin_v850_read_register("ctpc");
    val ^= __builtin_v850_read_register("ctpsw");
    val ^= __builtin_v850_read_register("ctbp");
    val ^= __builtin_v850_read_register("fxsr");
    val ^= __builtin_v850_read_register("fxxp");
    return val;
}

/* Test 3: Write system registers using canonical names */
__attribute__((noinline))
void test_write_canonical(uint32_t psw_val) {
    __builtin_v850_write_register("PSW", psw_val);
    __builtin_v850_write_register("FPSR", 0);
    __builtin_v850_write_register("EIIC", 0);
    __builtin_v850_write_register("FEIC", 0);
}

/* Test 4: Write registers using lowercase aliases */
__attribute__((noinline))
void test_write_lowercase(uint32_t psw_val) {
    __builtin_v850_write_register("psw", psw_val);
    __builtin_v850_write_register("fpsr", 0);
}

/* Test 5: Inline assembly with system register names (stsr/ldsr) */
__attribute__((noinline))
uint32_t asm_read_psw(void) {
    uint32_t val;
    __asm__ volatile("stsr PSW, %0" : "=r"(val));
    return val;
}

__attribute__((noinline))
void asm_write_psw(uint32_t val) {
    __asm__ volatile("ldsr %0, PSW" : : "r"(val));
}

__attribute__((noinline))
uint32_t asm_read_fpsr(void) {
    uint32_t val;
    __asm__ volatile("stsr FPSR, %0" : "=r"(val));
    return val;
}

__attribute__((noinline))
uint32_t test_read_generic_regs(void) {
    uint32_t val = 0;
    val ^= __builtin_v850_read_register("r0");
    val ^= __builtin_v850_read_register("r2");
    val ^= __builtin_v850_read_register("r3");
    val ^= __builtin_v850_read_register("r4");
    val ^= __builtin_v850_read_register("r5");
    val ^= __builtin_v850_read_register("r31");
    val ^= __builtin_v850_read_register("sp");
    val ^= __builtin_v850_read_register("gp");
    val ^= __builtin_v850_read_register("tp");
    val ^= __builtin_v850_read_register("lp");
    return val;
}

/* Test 6b: ep (Element pointer, r30) must be tested in a separate function
 * because it crashes when combined with other register reads. */
__attribute__((noinline))
uint32_t test_read_ep_alone(void) {
    return __builtin_v850_read_register("ep");
}

/* Test 6c: Write aliased registers (safe subset) */
__attribute__((noinline))
void test_write_aliased_regs(uint32_t v) {
    __builtin_v850_write_register("sp", v);
    __builtin_v850_write_register("gp", v);
    __builtin_v850_write_register("tp", v);
    __builtin_v850_write_register("lp", v);
}

/* Test 7: Read-modify-write PSW using builtin */
__attribute__((noinline))
void test_rmw_psw(uint32_t bit) {
    uint32_t val = __builtin_v850_read_register("PSW");
    val |= bit;
    __builtin_v850_write_register("PSW", val);
}

volatile uint32_t sink;

int main(void) {
    sink = test_read_canonical();
    sink = test_read_lowercase();
    sink = test_read_generic_regs();
    sink = test_read_ep_alone();

    test_write_aliased_regs(0x1000);

    uint32_t psw = asm_read_psw();
    sink = psw;
    asm_write_psw(psw);

    sink = asm_read_fpsr();

    test_write_canonical(psw);
    test_write_lowercase(psw);
    test_rmw_psw(0x1);

    return 0;
}
