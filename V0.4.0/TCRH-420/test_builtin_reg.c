/*
 * TCRH-420: 内置函数无法识别寄存器名称导致崩溃
 * 验证点：使用 RH850 寄存器名称的内置函数不再导致编译器崩溃
 */

#include <stdint.h>

/* RH850 寄存器相关内置函数测试 */

/* 测试1：使用寄存器名的内联约束 */
static inline uint32_t read_register_r0(void) {
    uint32_t val;
    __asm__ volatile(
        "mov r0, %0"
        : "=r"(val)
        :
        : "memory"
    );
    return val;
}

/* 测试2：使用多个寄存器  mov 指令源操作数不能是内存？*/
static inline void swap_regs(uint32_t *a, uint32_t *b) {
    __asm__ volatile(
        "mov %1, r1\n\t"
        "mov %2, r6\n\t"
        "mov r6, %0\n\t"
        "mov r1, %1"
        : "+r"(*a), "+r"(*b)
        : "r"(*b), "r"(*a)
        : "r1", "r6", "memory"
    );
}

/* 测试3：__builtin 函数中使用寄存器 */
static inline uint32_t test_builtin_clz(uint32_t x) {
    return __builtin_clz(x);
}

static inline uint32_t test_builtin_ctz(uint32_t x) {
    return __builtin_ctz(x);
}

/* 测试4：PSW 寄存器操作 */
static inline uint32_t read_psw(void) {
    uint32_t psw;
    __asm__ volatile(
        "stsr PSW, %0"
        : "=r"(psw)
        :
    );
    return psw;
}

static inline void write_psw(uint32_t val) {
    __asm__ volatile(
        "ldsr %0, PSW"
        :
        : "r"(val)
    );
}

/* 测试5：FEC/FEPC 寄存器 */
static inline uint32_t read_fepc(void) {
    uint32_t val;
    __asm__ volatile(
        "stsr FEPC, %0"
        : "=r"(val)
    );
    return val;
}

volatile uint32_t result;

int main(void) {
    result = read_register_r0();
    
    uint32_t a = 0x1234, b = 0x5678;
    swap_regs(&a, &b);
    
    result = test_builtin_clz(0x00000001);
    result = test_builtin_ctz(0x80000000);
    
    result = read_psw();
    write_psw(result);
    
    result = read_fepc();
    
    return 0;
}