/* https://cirosantilli.com/linux-kernel-module-cheat#arm-thumb-encoding */

#include <lkmc.h>

.thumb_func
LKMC_PROLOGUE

    /* TODO: #if 0 something that is not thumb encodable. */
    cbz r1, 1f
    1:

LKMC_EPILOGUE
