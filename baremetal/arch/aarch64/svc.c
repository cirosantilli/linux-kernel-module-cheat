#include <stdio.h>
#include <inttypes.h>

#include "common_aarch64.h"

/* Masks each of the 4 exception types: Synchronous, System error,
 * IRQ and FIQ.
 */
SYSREG_READ_WRITE(uint32_t, daif)

/* Determines if we use SP0 or SPx. Default: SP0.
 * See also: https://stackoverflow.com/questions/29393677/armv8-exception-vector-significance-of-el0-sp
 */
SYSREG_READ_WRITE(uint32_t, spsel)

/* Jump to this SP if spsel == SPx. */
SYSREG_READ_WRITE(uint64_t, sp_el1)

int main(void) {
    printf("daif 0x%" PRIx32 "\n", sysreg_daif_read());
    printf("spsel 0x%" PRIx32 "\n", sysreg_spsel_read());
    /* TODO this breaks execution because reading system registers that end
     * in ELx "trap", leading into an exception on the upper EL.
     */
    /*printf("sp_el1 0x%" PRIx64 "\n", sysreg_sp_el1_read());*/
    /*SVC(0);*/
    return 0;
}
