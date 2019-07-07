/* https://cirosantilli.com/linux-kernel-module-cheat#x86-binary-arithmetic-instructions
 *
 * If the result of div does not fit into the output register rax, then we get SIGFPE.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /* rdx:rax / 2 == 2:0 / 2 == 1:0 */
    mov $2, %rdx
    mov $0, %rax
    mov $2, %rbx
    div %rbx
LKMC_EPILOGUE
