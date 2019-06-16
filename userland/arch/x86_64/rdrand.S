/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-random-number-generator-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
1:
    rdrand %rdi
    jnc 1b
    call lkmc_print_hex_64
    call lkmc_print_newline
LKMC_EPILOGUE
