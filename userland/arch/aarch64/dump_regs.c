/* https://cirosantilli.com/linux-kernel-module-cheat#dump-regs */

#include <stdio.h>
#include <stdlib.h>

#include <lkmc.h>
#include <lkmc/aarch64.h>

int main(void) {
    lkmc_sysreg_print_cntvct_el0();
    lkmc_sysreg_print_cntfrq_el0();
    return EXIT_SUCCESS;
}
