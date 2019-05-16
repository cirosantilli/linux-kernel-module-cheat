// https://stackoverflow.com/questions/3666013/how-to-write-multiline-inline-assembly-code-in-gcc-c/54575948#54575948

#include <assert.h>
#include <inttypes.h>

int main(void) {
    uint64_t io = 0;
    __asm__ (
        R"(
add %[io], %[io], #1
add %[io], %[io], #1
)"
        : [io] "+r" (io)
        :
        :
    );
    assert(io == 2);
}
