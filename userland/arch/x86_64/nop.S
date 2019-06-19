/* https://github.com/cirosantilli/linux-kernel-module-cheat#nop-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    nop
    nop

    /* Other nops
     * http://stackoverflow.com/questions/11910501/why-did-gcc-generate-mov-eax-eax-and-what-does-it-mean
     */
    mov %rax, %rax
LKMC_EPILOGUE
