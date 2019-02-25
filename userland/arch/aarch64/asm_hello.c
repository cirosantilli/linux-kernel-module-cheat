#include <assert.h>
#include <inttypes.h>

int main(void) {
    uint32_t myvar = 1;
    __asm__ (
        "add %[myvar], %[myvar], 1;"
        : [myvar] "=r" (myvar)
        :
        :
    );
    assert(myvar == 2);
}
