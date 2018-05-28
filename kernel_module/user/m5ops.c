#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ENABLED 1
#if defined(__aarch64__)
static void m5_checkpoint()
{
	__asm__ __volatile__ ("mov x0, 0; mov x1, 0; .inst 0xff000110 | (0x43 << 16);");
};
static void m5_dump_stats()
{
	__asm__ __volatile__ ("mov x0, 0; mov x1, 0; .inst 0xff000110 | (0x41 << 16);");
};
static void m5_exit()
{
	__asm__ __volatile__ ("mov x0, 0; .inst 0xff000110 | (0x21 << 16);");
};
static void m5_reset_stats()
{
	__asm__ __volatile__ ("mov x0, 0; mov x1, 0; .inst 0xff000110 | (0x40 << 16);");
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
			m5_exit();
			break;
		case 'r':
			m5_reset_stats();
			break;
	}
#endif
	return EXIT_SUCCESS;
}
