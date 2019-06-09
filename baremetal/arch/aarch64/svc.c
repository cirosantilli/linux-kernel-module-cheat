/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-svc-instruction */

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <lkmc.h>

int myvar = 0;

void lkmc_vector_trap_handler(LkmcVectorExceptionFrame *exception) {
    puts("lkmc_vector_trap_handler");
    printf("exc_type 0x%" PRIx64 "\n", exception->exc_type);
    if (exception->exc_type == LKMC_VECTOR_SYNC_SPX) {
        puts("exc_type is LKMC_VECTOR_SYNC_SPX");
    }
    printf("ESR 0x%" PRIx64 "\n", exception->exc_esr);
    printf("SP 0x%" PRIx64 "\n", exception->exc_sp);
    printf("ELR 0x%" PRIx64 "\n", exception->exc_elr);
    printf("SPSR 0x%" PRIx64 "\n", exception->exc_spsr);
    printf("x0 0x%" PRIx64 "\n", exception->x0);
    printf("x1 0x%" PRIx64 "\n", exception->x1);
    printf("x2 0x%" PRIx64 "\n", exception->x2);
    printf("x3 0x%" PRIx64 "\n", exception->x3);
    printf("x4 0x%" PRIx64 "\n", exception->x4);
    printf("x5 0x%" PRIx64 "\n", exception->x5);
    printf("x6 0x%" PRIx64 "\n", exception->x6);
    printf("x7 0x%" PRIx64 "\n", exception->x7);
    printf("x8 0x%" PRIx64 "\n", exception->x8);
    printf("x9 0x%" PRIx64 "\n", exception->x9);
    printf("x10 0x%" PRIx64 "\n", exception->x10);
    printf("x11 0x%" PRIx64 "\n", exception->x11);
    printf("x12 0x%" PRIx64 "\n", exception->x12);
    printf("x13 0x%" PRIx64 "\n", exception->x13);
    printf("x14 0x%" PRIx64 "\n", exception->x14);
    printf("x15 0x%" PRIx64 "\n", exception->x15);
    printf("x16 0x%" PRIx64 "\n", exception->x16);
    printf("x17 0x%" PRIx64 "\n", exception->x17);
    printf("x18 0x%" PRIx64 "\n", exception->x18);
    printf("x19 0x%" PRIx64 "\n", exception->x19);
    printf("x20 0x%" PRIx64 "\n", exception->x20);
    printf("x21 0x%" PRIx64 "\n", exception->x21);
    printf("x22 0x%" PRIx64 "\n", exception->x22);
    printf("x23 0x%" PRIx64 "\n", exception->x23);
    printf("x24 0x%" PRIx64 "\n", exception->x24);
    printf("x25 0x%" PRIx64 "\n", exception->x25);
    printf("x26 0x%" PRIx64 "\n", exception->x26);
    printf("x27 0x%" PRIx64 "\n", exception->x27);
    printf("x28 0x%" PRIx64 "\n", exception->x28);
    printf("x29 0x%" PRIx64 "\n", exception->x29);
    printf("x30 0x%" PRIx64 "\n", exception->x30);
    myvar = 1;
}

int main(void) {
    /* View initial relevant register values. */
    printf("daif 0x%" PRIx32 "\n", lkmc_sysreg_daif_read());
    printf("spsel 0x%" PRIx32 "\n", lkmc_sysreg_spsel_read());
    printf("vbar_el1 0x%" PRIx64 "\n", lkmc_sysreg_vbar_el1_read());
    assert(myvar == 0);
    LKMC_SVC(0x42);
    assert(myvar == 1);
    return 0;
}
