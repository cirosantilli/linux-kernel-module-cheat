#include <assert.h>
#include <inttypes.h>

int main(void) {
    uint64_t io = 1;
    __asm__ (
        "add %[io], %[io], 1;"
        : [io] "+r" (io)
        :
        :
    );
    assert(io == 2);
}
