/* https://cirosantilli.com/linux-kernel-module-cheat#dump-regs */

#include <stdio.h>
#include <inttypes.h>

#include <lkmc/aarch64_dump_regs.h>

int main(void) {
    lkmc_dump_system_regs();
    return 0;
}
