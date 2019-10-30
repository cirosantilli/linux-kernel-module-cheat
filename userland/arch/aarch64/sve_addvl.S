/* https://cirosantilli.com/linux-kernel-module-cheat#arm-sve-vaddl-instruction */

#include <lkmc.h>

LKMC_PROLOGUE
    mov x0, 0
    addvl x0, x0, 8
    bl lkmc_print_hex_64
    bl lkmc_print_newline
LKMC_EPILOGUE
