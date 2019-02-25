#include <assert.h>
#include <inttypes.h>

int main(void) {
	uint32_t in = 1;
	uint32_t out = 0;
	__asm__ (
		"mov %[in], %%eax;"
		"inc %%eax;"
		"mov %%eax, %[out]"
		: [out] "=g" (out)
		: [in] "g" (in)
		: "%eax"
	);
	assert(out == in + 1);
}
