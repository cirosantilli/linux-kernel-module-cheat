/* I want to move el and all other "what's the initial value of such system register" into here. */

#include <stdio.h>
#include <inttypes.h>

int main(void) {
    uint32_t cpsr;
    /*uint32_t mvfr1;*/
    __asm__ ("mrs %0, cpsr" : "=r" (cpsr) : :);
    /* TODO this is blowing up an exception, how to I read from it? */
    /*__asm__ ("vmrs %0, mvfr1" : "=r" (mvfr1) : :);*/
    printf("cpsr  %" PRIx32 "\n", cpsr);
    /*printf("mvfr1 %" PRIx32 "\n", mvfr1);*/
    return 0;
}
