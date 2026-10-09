/* Minimal cases to understand -Wsection-feature counting. */
__attribute__((section(".only_attr")))
int only_attr_var;                 /* only one section feature (attribute) */

__attribute__((section(".only_attr2")))
int only_attr_var_init = 7;        /* only one section feature (attribute) */

__attribute__((section(".only_func")))
int only_func(void) { return 1; }  /* only one section feature (attribute) */

int plain_var;                     /* no section feature at all */
int plain_init = 3;                /* no section feature at all */
