#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <common.h>
#include "common_aarch64.h"

void handle_svc() {
    exit(0);
}

int main(void) {
    /* View initial register values. */
    printf("daif 0x%" PRIx32 "\n", sysreg_daif_read());
    printf("spsel 0x%" PRIx32 "\n", sysreg_spsel_read());
    printf("vbar_el1 0x%" PRIx64 "\n", sysreg_vbar_el1_read());

    /* Set registers to the values that we need. */
    sysreg_daif_write(0);
    sysreg_vbar_el1_write(0);
    printf("daif 0x%" PRIx32 "\n", sysreg_daif_read());
    printf("spsel 0x%" PRIx32 "\n", sysreg_spsel_read());
    printf("vbar_el1 0x%" PRIx64 "\n", sysreg_vbar_el1_read());

    /* TODO this breaks execution because reading system registers that end
     * in ELx "trap", leading into an exception on the upper EL.
     */
    /*printf("sp_el1 0x%" PRIx64 "\n", sysreg_sp_el1_read());*/
    /*SVC(0);*/

    /* Should never be reached. */
    /*common_assert_fail();*/
    return 0;
}
