/* https://cirosantilli.com/linux-kernel-module-cheat#x86-binary-arithmetic-instructions
 *
 * SIGFPE :-)
 *
 * Signal handlind discussed at:
 * https://stackoverflow.com/questions/39431879/c-handle-signal-sigfpe-and-continue-execution/39431923#39431923
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /* rdx:rax / 0 */
    mov $0, %rdx
    mov $1, %rax
    mov $0, %rbx
    div %rbx
LKMC_EPILOGUE
