/*
 * CCRM-176 test (part 2): lld garbage collection of unreferenced
 * sections is enabled by default.
 *
 * used_func / used_global are referenced from _start -> must survive.
 * unused_func / unused_global are unreferenced -> must be GC'd by default.
 */

int used_global = 42;
int unused_global = 99;

int used_func(int x)
{
    return x + used_global;
}

int unused_func(int x)
{
    return x * unused_global;
}

void _start(void)
{
    (void)used_func(1);
    for (;;)
        ;
}
