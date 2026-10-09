/*
 * TCRH-257: 尾调用优化改进测试
 * 验证点：递归尾调用应被优化为跳转指令，不额外分配栈帧
 */

int factorial_tail(int n, int acc) {
    if (n <= 1)
        return acc;
    return factorial_tail(n - 1, acc * n);  // 尾调用
}

int fibonacci_tail(int n, int a, int b) {
    if (n == 0) return a;
    if (n == 1) return b;
    return fibonacci_tail(n - 1, b, a + b);  // 尾调用
}

/* 互递归尾调用 */
int is_even(int n);
int is_odd(int n);

int is_even(int n) {
    if (n == 0) return 1;
    return is_odd(n - 1);  // 尾调用
}

int is_odd(int n) {
    if (n == 0) return 0;
    return is_even(n - 1);  // 尾调用
}

/* 入口函数 */
int test_factorial(int n) {
    return factorial_tail(n, 1);
}

int test_fibonacci(int n) {
    return fibonacci_tail(n, 0, 1);
}

int test_even(int n) {
    return is_even(n);
}

/* 占位，防止被优化掉 */
volatile int sink;

int main(void) {
    sink = test_factorial(10);
    sink = test_fibonacci(10);
    sink = test_even(7);
    return 0;
}