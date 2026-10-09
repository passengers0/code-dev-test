#include <v850intrin.h>
typedef float __w128 __attribute__((__vector_size__(16), __aligned__(16)));
__w128 test_return(void) {
    __w128 v = {1.0f, 2.0f, 3.0f, 4.0f};
    return v;
}
