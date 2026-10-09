/*
 * CCRM-110 test: -Wsection-feature
 *
 * Warns if a variable or function uses more than one clang section
 * feature, i.e. more than one of __attribute__((section)) or
 * #pragma clang section.
 *
 * Test cases are taken from the HighTec User Guide.
 */

#pragma clang section bss=".test1"
__attribute__((section(".test11")))
int test1; /* warning: section attribute overrides '#pragma clang section' */
#pragma clang section bss=""

#pragma clang section text=".test2"
__attribute__((section(".test22")))
void test2(void) {} /* warning: section attribute overrides '#pragma clang section' */
#pragma clang section text=""

#pragma clang section bss=".test3"
#pragma clang section bss=".test33" /* warning: pragma overrides previous active section */
int test3;
#pragma clang section bss=""

/* No warning: only one section feature used. */
__attribute__((section(".single")))
int ok_single;

#pragma clang section data=".only_pragma"
int ok_pragma;
#pragma clang section data=""
