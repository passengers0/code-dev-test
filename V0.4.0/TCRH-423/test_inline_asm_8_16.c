/*
 * TCRH-423: 8位或16位操作数传入内联汇编导致编译器崩溃
 * 验证点：8位和16位操作数在内联汇编中不再导致编译器崩溃
 */

#include <stdint.h>

/* 测试1：8位输入操作数 */
static inline uint8_t asm_add_8bit(uint8_t a, uint8_t b) {

    __asm__ volatile(
        "add %1, %0"
        : "+r"(a)
        : "r"(b)
    );
    return a;
}

/* 测试2：8位输出操作数 */
static inline uint8_t asm_load_8bit(const uint8_t *ptr) {
    uint8_t val;
    __asm__ volatile(
        "ld.b 0[%1], %0"
        : "=r"(val)
        : "r"(ptr)
    );
    return val;
}

/* 测试3：8位立即数操作 */
static inline uint8_t asm_set_8bit(void) {
    uint8_t val;
    __asm__ volatile(
        "mov 0xFF, %0"
        : "=r"(val)
    );
    return val;
}

/* 测试4：16位输入操作数 */
static inline uint16_t asm_add_16bit(uint16_t a, uint16_t b) {
    uint16_t result;
    __asm__ volatile(
        "add %1, %0"
        : "+r"(result)
        : "r"(b)
    );
    return result;
}

/* 测试5：16位输出操作数 */
static inline uint16_t asm_load_16bit(const uint16_t *ptr) {
    uint16_t val;
    __asm__ volatile(
        "ld.h 0[%1], %0"
        : "=r"(val)
        : "r"(ptr)
    );
    return val;
}

/* 测试6：混合8位和16位操作数 */
static inline uint32_t asm_mixed_8_16(uint8_t a, uint16_t b) {
    uint32_t result;
    uint32_t temp = 0; // 用作内存暂存

    __asm__ volatile(
        "st.b %1, %0\n\t"    // 将 a 的低8位存入 temp 内存
        "st.h %2, %0"        // 将 b 的低16位存入 temp 内存（注意：sld.h 会写入2字节）
        : "=m"(temp)          // %0 约束为内存，编译器会自动展开为 0[reg] 格式
        : "r"(a), "r"(b)      // %1, %2 为寄存器
    );
    result = temp;
    return result;
}

/* 测试7：8位条件操作 */
static inline int asm_cmp_8bit(uint8_t a, uint8_t b) {
    int result;
    __asm__ volatile(
        "cmp %1, %2\n\t"
        "setf Z, %0"
        : "=r"(result)
        : "r"(a), "r"(b)
    );
    return result;
}

/* 测试8：16位条件操作 */
static inline int asm_cmp_16bit(uint16_t a, uint16_t b) {
    int result;
    __asm__ volatile(
        "cmp %1, %2\n\t"
        "setf Z, %0"
        : "=r"(result)
        : "r"(a), "r"(b)
    );
    return result;
}

/* 测试9：8位内联汇编在循环中 */
uint8_t sum_array_8bit(const uint8_t *arr, int len) {
    uint8_t sum = 0;
    for (int i = 0; i < len; i++) {
        __asm__ volatile(
            "add %1, %0"
            : "+r"(sum)
            : "r"(arr[i])
        );
    }
    return sum;
}

/* 测试10：16位内联汇编在循环中 */
uint16_t sum_array_16bit(const uint16_t *arr, int len) {
    uint16_t sum = 0;
    for (int i = 0; i < len; i++) {
        __asm__ volatile(
            "add %1, %0"
            : "+r"(sum)
            : "r"(arr[i])
        );
    }
    return sum;
}

volatile uint8_t sink8;
volatile uint16_t sink16;
volatile uint32_t sink32;
volatile int sink_int;

int main(void) {
    uint8_t a8 = 0x12, b8 = 0x34;
    uint16_t a16 = 0x1234, b16 = 0x5678;
    
    sink8  = asm_add_8bit(a8, b8);
    sink8  = asm_load_8bit(&a8);
    sink8  = asm_set_8bit();
    
    sink16 = asm_add_16bit(a16, b16);
    sink16 = asm_load_16bit(&a16);
    
    sink32 = asm_mixed_8_16(a8, b16);
    
    sink_int = asm_cmp_8bit(a8, b8);
    sink_int = asm_cmp_16bit(a16, b16);
    
    uint8_t arr8[4] = {1, 2, 3, 4};
    uint16_t arr16[4] = {10, 20, 30, 40};
    
    sink8  = sum_array_8bit(arr8, 4);
    sink16 = sum_array_16bit(arr16, 4);
    
    return 0;
}