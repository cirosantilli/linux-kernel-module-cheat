#include <assert.h>
#include <inttypes.h>

int main(void) {
	uint32_t in = 1;
	uint32_t out = 0;
	__asm__ (
		"movl %1, %%eax;"
		"inc %%eax;"
		"movl %%eax, %0"
		: "=m" (out)
		: "m" (in)
		: "%eax"
	);
	assert(out == in + 1);
}
