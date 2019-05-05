/* https://github.com/cirosantilli/linux-kernel-module-cheat#ring0 */

#include <stdio.h>
#include <stdlib.h>

#include <include/ring0.h>

int main(void) {
    LkmcRing0Regs ring0_regs;
    lkmc_ring0_get_control_regs(&ring0_regs);
    return EXIT_SUCCESS;
}
