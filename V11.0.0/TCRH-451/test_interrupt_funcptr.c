/*
 * TCRH-451: Fixed compiler crash on initializing array of function pointers with
 *           a function having interrupt_handler attribute
 * Test principle: Declare functions with __attribute__((interrupt_handler)) and
 *                 use them to initialize arrays of function pointers. Verify the
 *                 compiler no longer crashes during this initialization.
 * Verification:  Compilation succeeds without crash; the interrupt handler functions
 *                 have correct prologue/epilogue; function pointer arrays initialized.
 * Note 1:        Valid interrupt types in v11.0.0 are "eiint", "feint" and "fenmi".
 *                 eiint = EI-level, feint = FE-level, fenmi = Non-maskable FE-level.
 * Note 2:        Interrupt handler function pointers require explicit cast to
 *                 ordinary function pointer type due to ABI differences.
 */

#include <stdint.h>

/* Test 1: interrupt_handler function used in function pointer array */
__attribute__((interrupt_handler("eiint")))
void eiint_handler1(void) {
    volatile uint32_t *reg = (volatile uint32_t *)0xFFFFF000;
    *reg |= 0x1;
}

__attribute__((interrupt_handler("eiint")))
void eiint_handler2(void) {
    volatile uint32_t *reg = (volatile uint32_t *)0xFFFFF004;
    *reg |= 0x2;
}

__attribute__((interrupt_handler("feint")))
void feint_handler1(void) {
    volatile uint32_t *reg = (volatile uint32_t *)0xFFFFF010;
    *reg |= 0x1;
}

__attribute__((interrupt_handler("feint")))
void feint_handler2(void) {
    volatile uint32_t *reg = (volatile uint32_t *)0xFFFFF014;
    *reg |= 0x2;
}

__attribute__((interrupt_handler("fenmi")))
void fenmi_handler(void) {
    volatile uint32_t *reg = (volatile uint32_t *)0xFFFFF020;
    *reg |= 0x4;
}

/* Array of function pointers initialized with interrupt handlers (with casts) */
typedef void (*handler_fn)(void);

handler_fn interrupt_table[6] = {
    (handler_fn)eiint_handler1,
    (handler_fn)eiint_handler2,
    (handler_fn)feint_handler1,
    (handler_fn)feint_handler2,
    (handler_fn)fenmi_handler,
    0  /* NULL terminator */
};

/* Global funcptr array initialized DIRECTLY with interrupt_handler functions
 * WITHOUT casts. This exact pattern crashes V0.4.0 with
 * "UNREACHABLE executed at TypePrinter.cpp:2014". V11.0.0 fixes this. */
void (*raw_handler_table[])(void) = {
    eiint_handler1,
    feint_handler1,
    fenmi_handler,
    (void *)0
};

/* Test 2: interrupt_handler with argument (FEIC value) */
__attribute__((interrupt_handler("feint")))
void feint_handler_with_arg(unsigned int cause) {
    volatile uint32_t *reg = (volatile uint32_t *)0xFFFFF010;
    *reg = cause;
}

/* Test 3: Function pointer array with mixed handler types */
typedef void (*int_handler_fn)(unsigned int);

int_handler_fn int_handler_table[3] = {
    (int_handler_fn)feint_handler_with_arg,
    (int_handler_fn)feint_handler2,
    0
};

/* Test 4: Static function pointer array with interrupt handler */
static handler_fn static_table[2] = {
    (handler_fn)feint_handler1,
    (handler_fn)feint_handler2
};

/* Test 5: Const function pointer array (flash-resident) */
const handler_fn const_table[3] = {
    (handler_fn)feint_handler1,
    (handler_fn)feint_handler2,
    (handler_fn)fenmi_handler
};

/* Test 6: Interrupt handler referenced via struct */
struct VectorTable {
    handler_fn feint1;
    handler_fn feint2;
    handler_fn fenmi;
};

struct VectorTable vectors = {
    .feint1 = (handler_fn)feint_handler1,
    .feint2 = (handler_fn)feint_handler2,
    .fenmi = (handler_fn)fenmi_handler
};

/* Test 7: Interrupt handler function pointer assigned at runtime */
handler_fn runtime_table[2];

void init_runtime_table(void) {
    runtime_table[0] = (handler_fn)feint_handler1;
    runtime_table[1] = (handler_fn)feint_handler2;
}

/* Test 8: Nested array of function pointers with handlers */
handler_fn nested_table[2][2] = {
    { (handler_fn)feint_handler1, (handler_fn)feint_handler2 },
    { (handler_fn)fenmi_handler, 0 }
};

/* Test 9: Interrupt handler with more complex logic */
volatile uint32_t interrupt_count = 0;
volatile uint32_t interrupt_status = 0;

__attribute__((interrupt_handler("feint")))
void complex_feint_handler(void) {
    interrupt_count++;
    interrupt_status = 0xDEAD;
    volatile uint32_t *icr = (volatile uint32_t *)0xFFFFF100;
    *icr = 0;
}

/* Test 10: Function pointer to interrupt handler passed as argument */
void register_handler(handler_fn *table, int index, handler_fn fn) {
    if (index >= 0 && index < 4) {
        table[index] = fn;
    }
}

/* Test 11: Array initialized with designated initializers */
handler_fn designated_table[8] = {
    [0] = (handler_fn)feint_handler1,
    [3] = (handler_fn)feint_handler2,
    [7] = (handler_fn)complex_feint_handler
};

/* Test 12: Interrupt handler address taken via & operator */
void test_address_of_handler(void) {
    handler_fn p = (handler_fn)&feint_handler1;
    handler_fn q = (handler_fn)&complex_feint_handler;
    if (p != 0 && q != 0) {
        interrupt_status = (uint32_t)(uintptr_t)p ^ (uint32_t)(uintptr_t)q;
    }
}

volatile uint32_t sink;

int main(void) {
    sink = (uint32_t)(uintptr_t)interrupt_table[0];
    sink = (uint32_t)(uintptr_t)interrupt_table[1];
    sink = (uint32_t)(uintptr_t)interrupt_table[2];

    sink = (uint32_t)(uintptr_t)static_table[0];
    sink = (uint32_t)(uintptr_t)const_table[0];

    sink = (uint32_t)(uintptr_t)vectors.feint1;
    sink = (uint32_t)(uintptr_t)vectors.feint2;

    init_runtime_table();
    sink = (uint32_t)(uintptr_t)runtime_table[0];

    sink = (uint32_t)(uintptr_t)nested_table[0][0];
    sink = (uint32_t)(uintptr_t)nested_table[1][0];

    sink = (uint32_t)(uintptr_t)int_handler_table[0];

    sink = (uint32_t)(uintptr_t)designated_table[0];
    sink = (uint32_t)(uintptr_t)designated_table[3];
    sink = (uint32_t)(uintptr_t)designated_table[7];

    register_handler(runtime_table, 2, (handler_fn)complex_feint_handler);
    sink = (uint32_t)(uintptr_t)runtime_table[2];

    test_address_of_handler();

    return 0;
}
