#include <assert.h>
#include <inttypes.h>

int main(void) {
    register uint32_t x0 __asm__ ("x0");
    __asm__ ("mov x0, #1;" : : : "%x0");
    assert(x0 == 1);
}
