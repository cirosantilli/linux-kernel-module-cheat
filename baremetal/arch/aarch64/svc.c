#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <lkmc.h>

int myvar = 0;

void lkmc_vector_trap_handler(LkmcVectorExceptionFrame *exception) {
    puts("trap handler");
    myvar = 1;
}

int main(void) {
    /* View initial relevant register values. */
    printf("daif 0x%" PRIx32 "\n", lkmc_sysreg_daif_read());
    printf("spsel 0x%" PRIx32 "\n", lkmc_sysreg_spsel_read());
    printf("vbar_el1 0x%" PRIx64 "\n", lkmc_sysreg_vbar_el1_read());
    lkmc_assert(myvar == 0);
    LKMC_SVC(0);
    lkmc_assert(myvar == 1);
    return 0;
}
