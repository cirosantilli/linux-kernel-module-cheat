/*
Only works in x86_64.

- https://en.wikipedia.org/wiki/Time_Stamp_Counter
- https://stackoverflow.com/questions/9887839/clock-cycle-count-wth-gcc/9887979
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__i386__) || defined(__x86_64__)
#include <x86intrin.h>
#endif

int main(void) {
	uintmax_t val;
#if defined(__i386__) || defined(__x86_64__)
	/* https://stackoverflow.com/questions/9887839/clock-cycle-count-wth-gcc/9887979 */
	val = __rdtsc();
#else
	val = 0;
#endif
	printf("%jx\n", val);
	return EXIT_SUCCESS;
}
