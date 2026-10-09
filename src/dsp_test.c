int main(void)
{
    unsigned long long result, a, b;
	// dstsa32
    asm volatile("dstsa32 %0, %1, %2" : "=r"(result) : "r"(a), "r"(b));
    return result;
}