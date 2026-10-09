/*
 * TCRH-454: Fixed compiler crash when inline assembly accessed a system register
 * Test principle: Use inline assembly with system register access (stsr/ldsr
 *                 instructions) in various contexts - functions, loops, conditionals,
 *                 with different register names and operand constraints. Verify the
 *                 compiler no longer crashes.
 * Verification:  Compilation succeeds without crash; assembly contains correct
 *                 stsr/ldsr instructions for system register access.
 */

#include <stdint.h>

/* Test 1: Basic system register read via inline asm */
static inline uint32_t read_psw(void) {
    uint32_t val;
    __asm__ volatile("stsr PSW, %0" : "=r"(val));
    return val;
}

/* Test 2: Basic system register write via inline asm */
static inline void write_psw(uint32_t val) {
    __asm__ volatile("ldsr %0, PSW" : : "r"(val));
}

/* Test 3: Read multiple system registers */
static inline void read_all_sysregs(uint32_t *out) {
    __asm__ volatile("stsr EIPC, %0" : "=r"(out[0]));
    __asm__ volatile("stsr EIPSW, %0" : "=r"(out[1]));
    __asm__ volatile("stsr FEPC, %0" : "=r"(out[2]));
    __asm__ volatile("stsr FEPSW, %0" : "=r"(out[3]));
    __asm__ volatile("stsr FPSR, %0" : "=r"(out[4]));
    __asm__ volatile("stsr FPEPC, %0" : "=r"(out[5]));
}

/* Test 4: System register access in a loop */
static inline uint32_t read_psw_loop(int count) {
    uint32_t sum = 0;
    for (int i = 0; i < count; i++) {
        uint32_t val;
        __asm__ volatile("stsr PSW, %0" : "=r"(val));
        sum += val;
    }
    return sum;
}

/* Test 5: System register access in conditional */
static inline uint32_t conditional_sysreg(int cond) {
    uint32_t val = 0;
    if (cond) {
        __asm__ volatile("stsr PSW, %0" : "=r"(val));
    } else {
        __asm__ volatile("stsr FPSR, %0" : "=r"(val));
    }
    return val;
}

/* Test 6: System register with read-modify-write pattern */
static inline void set_psw_bit(uint32_t bit) {
    uint32_t val;
    __asm__ volatile("stsr PSW, %0" : "=r"(val));
    val |= bit;
    __asm__ volatile("ldsr %0, PSW" : : "r"(val));
}

static inline void clear_psw_bit(uint32_t bit) {
    uint32_t val;
    __asm__ volatile("stsr PSW, %0" : "=r"(val));
    val &= ~bit;
    __asm__ volatile("ldsr %0, PSW" : : "r"(val));
}

/* Test 7: System register access with memory clobber */
static inline uint32_t read_psw_memory(void) {
    uint32_t val;
    __asm__ volatile("stsr PSW, %0" : "=r"(val) : : "memory");
    return val;
}

/* Test 8: Multiple system registers in single asm block */
static inline void read_pair(uint32_t *hi, uint32_t *lo) {
    __asm__ volatile(
        "stsr EIPC, %0\n\t"
        "stsr EIPSW, %1"
        : "=r"(*hi), "=r"(*lo)
    );
}

/* Test 9: System register access in inline function called many times */
static inline uint32_t get_psw_id_bit(void) {
    uint32_t val;
    __asm__ volatile("stsr PSW, %0" : "=r"(val));
    return (val >> 5) & 0x1;  /* ID bit position */
}

uint32_t test_multiple_calls(void) {
    uint32_t result = 0;
    result += get_psw_id_bit();
    result += get_psw_id_bit() << 1;
    result += get_psw_id_bit() << 2;
    return result;
}

/* Test 10: System register write with immediate value */
static inline void write_psw_immediate(void) {
    __asm__ volatile("ldsr %0, PSW" : : "r"(0x00000000));
}

/* Test 11: System register access in struct member function-like macro */
#define SYSREG_READ(name) ({ \
    uint32_t _val; \
    __asm__ volatile("stsr " #name ", %0" : "=r"(_val)); \
    _val; \
})

#define SYSREG_WRITE(name, val) do { \
    __asm__ volatile("ldsr %0, " #name : : "r"(val)); \
} while(0)

uint32_t test_macro_sysreg(void) {
    uint32_t psw = SYSREG_READ(PSW);
    uint32_t fpsr = SYSREG_READ(FPSR);
    SYSREG_WRITE(PSW, psw);
    return psw + fpsr;
}

/* Test 12: System register access with output tied to input (+r constraint) */
static inline void toggle_psw_bit(uint32_t mask) {
    uint32_t val;
    __asm__ volatile(
        "stsr PSW, %0\n\t"
        "xor %1, %0\n\t"
        "ldsr %0, PSW"
        : "=&r"(val)
        : "r"(mask)
        : "memory"
    );
}

/* Test 13: EIIC/FEIC system registers (interrupt control) */
static inline uint32_t read_eiic(void) {
    uint32_t val;
    __asm__ volatile("stsr EIIC, %0" : "=r"(val));
    return val;
}

static inline uint32_t read_feic(void) {
    uint32_t val;
    __asm__ volatile("stsr FEIC, %0" : "=r"(val));
    return val;
}

/* Test 14: CTPC/CTPSW/CTBP system registers */
static inline void read_ct_regs(uint32_t *out) {
    __asm__ volatile("stsr CTPC, %0" : "=r"(out[0]));
    __asm__ volatile("stsr CTPSW, %0" : "=r"(out[1]));
    __asm__ volatile("stsr CTBP, %0" : "=r"(out[2]));
}

/* Test 15: FXSR/FXXP system registers (FXU related) */
static inline uint32_t read_fxsr(void) {
    uint32_t val;
    __asm__ volatile("stsr FXSR, %0" : "=r"(val));
    return val;
}

static inline uint32_t read_fxxp(void) {
    uint32_t val;
    __asm__ volatile("stsr FXXP, %0" : "=r"(val));
    return val;
}

/* Test 16: System register access in interrupt handler context */
__attribute__((interrupt_handler("feint")))
void handler_with_sysreg(void) {
    uint32_t eipc, eipsw;
    __asm__ volatile("stsr EIPC, %0" : "=r"(eipc));
    __asm__ volatile("stsr EIPSW, %0" : "=r"(eipsw));
    (void)eipc;
    (void)eipsw;
}

volatile uint32_t sink;

int main(void) {
    uint32_t regs[6];

    sink = read_psw();
    write_psw(sink);
    read_all_sysregs(regs);
    sink = regs[0] + regs[4];

    sink = read_psw_loop(3);
    sink = conditional_sysreg(1);
    sink = conditional_sysreg(0);

    set_psw_bit(0x1);
    clear_psw_bit(0x1);

    sink = read_psw_memory();

    uint32_t hi, lo;
    read_pair(&hi, &lo);
    sink = hi + lo;

    sink = test_multiple_calls();
    write_psw_immediate();
    sink = test_macro_sysreg();

    toggle_psw_bit(0x10);

    sink = read_eiic();
    sink = read_feic();

    uint32_t ct[3];
    read_ct_regs(ct);
    sink = ct[0] + ct[1] + ct[2];

    sink = read_fxsr();
    sink = read_fxxp();

    /* Restore PSW */
    write_psw(0);

    return 0;
}
