/*
 * TCRH-421: 编译器在无效配置下生成非法 stv.dw 指令
 * 验证点：编译器不应在无效配置下生成 stv.dw 指令
 */

#include <stdint.h>

/* 双字（64位）数据操作测试 */

typedef struct {
    uint64_t val;
    uint32_t flag;
} Data64;

/* 测试1：64位全局变量访问 */
volatile uint64_t global_u64 = 0;

void write_global_u64(uint64_t val) {
    global_u64 = val;
}

uint64_t read_global_u64(void) {
    return global_u64;
}

/* 测试2：64位结构体成员访问 */
void write_struct_val(Data64 *p, uint64_t val) {
    p->val = val;
}

uint64_t read_struct_val(const Data64 *p) {
    return p->val;
}

/* 测试3：64位数组操作 */
volatile uint64_t u64_array[4] = {0};

void init_array(void) {
    for (int i = 0; i < 4; i++) {
        u64_array[i] = (uint64_t)i << 32 | i;
    }
}

uint64_t get_array_elem(int idx) {
    if (idx >= 0 && idx < 4)
        return u64_array[idx];
    return 0;
}

/* 测试4：64位参数传递 */
uint64_t add_u64(uint64_t a, uint64_t b) {
    return a + b;
}

/* 测试5：64位局部变量 */
uint64_t compute_local(void) {
    uint64_t x = 0x123456789ABCDEF0ULL;
    uint64_t y = 0x0FEDCBA987654321ULL;
    return x * y + 1;
}

volatile uint32_t sink32;
volatile uint64_t sink64;

int main(void) {
    write_global_u64(0xDEADBEEFCAFEBABEULL);
    sink64 = read_global_u64();
    
    Data64 d;
    write_struct_val(&d, 0x1111222233334444ULL);
    sink64 = read_struct_val(&d);
    
    init_array();
    sink64 = get_array_elem(2);
    
    sink64 = add_u64(0xFFFFFFFFULL, 1);
    
    sink64 = compute_local();
    
    return 0;
}