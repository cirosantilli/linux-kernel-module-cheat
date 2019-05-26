#include <stdio.h>
#include <inttypes.h>

int main(void) {
    uint32_t cpsr;
    __asm__ ("mrs %0, cpsr" : "=r" (cpsr) : :);
    printf("cpsr  %" PRIx32 "\n", cpsr);

    /* TODO this is blowing up an exception, how to I read from it? */
    /*uint32_t mvfr1;*/
    /*__asm__ ("vmrs %0, mvfr1" : "=r" (mvfr1) : :);*/
    /*printf("mvfr1 %" PRIx32 "\n", mvfr1);*/

    return 0;
}
