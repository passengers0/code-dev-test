/*
 * CCRM-176 test: functions and static allocated variables
 * are emitted into unique sections by default.
 *
 * Compile WITHOUT -ffunction-sections / -fdata-sections and inspect
 * section names in the object file:
 *   - non-static function  -> .text.<funcname>
 *   - static function      -> .text.<funcname>
 *   - non-static data      -> .data.<name> / .bss.<name>
 *   - static allocated var -> .data.<name> / .bss.<name>
 */

int global_init = 5;          /* initialized non-static global -> .data.global_init */
static int static_init = 6;   /* initialized static global    -> .data.static_init  */
int global_zero;              /* zero-initialized global       -> .bss.global_zero   */
static int static_zero;       /* zero-initialized static       -> .bss.static_zero   */

int func_a(int x)
{
    return x + global_init;
}

static int helper(int x)
{
    return x * 2;
}

int func_b(int x)
{
    return helper(x) + static_init + global_zero + static_zero;
}
