#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ENABLED 1
#if defined(__aarch64__)
static void m5_checkpoint(uint64_t x, uint64_t y)
{
	__asm__ __volatile__ (".inst 0xff430110;":: "x0" (x), "x1" (y));
};
static void m5_dump_stats(uint64_t x, uint64_t y)
{
	__asm__ __volatile__ (".inst 0xff410110;":: "x0" (x), "x1" (y));
};
static void m5_exit(uint64_t x)
{
	__asm__ __volatile__ (".inst 0xff210110;":: "x0" (x));
};
static void m5_reset_stats(uint64_t x, uint64_t y)
{
	__asm__ __volatile__ (".inst 0xff400110;":: "x0" (x), "x1" (y));
};
#else
#undef ENABLED
#define ENABLED 0
#endif

int main(
#if ENABLED
int argc, char **argv
#else
void
#endif
)
{
#if defined(__aarch64__)
	char action;
	if (argc > 1) {
		action = argv[1][0];
	} else {
		action = 'e';
	}
	switch (action)
	{
        case 'c':
            m5_checkpoint(0, 0);
	        break;
        case 'd':
            m5_dump_stats(0, 0);
	        break;
        case 'e':
            m5_exit(0);
	        break;
        case 'r':
            m5_reset_stats(0, 0);
	        break;
	}
#endif
	return EXIT_SUCCESS;
}
