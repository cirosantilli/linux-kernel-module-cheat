/* https://github.com/cirosantilli/linux-kernel-module-cheat#rdtsc */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__i386__) || defined(__x86_64__)
#include <x86intrin.h>
#endif

int main(void) {
	uintmax_t val;
#if defined(__i386__) || defined(__x86_64__)
	val = __rdtsc();
#else
	val = 0;
#endif
	printf("%ju\n", val);
	return EXIT_SUCCESS;
}
