/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-beq-instruction */

#include "common.h"

ENTRY
    /* cbz == 0 */
    mov x0, 0
    cbz x0, 1f
    FAIL
1:

    /* cbz != 0 */
    mov x0, 1
    cbz x0, 1f
    b 2f
1:
    FAIL
2:

    /* cbnz != 0 */
    mov x0, 1
    cbnz x0, 1f
    FAIL
1:

    /* cbnz == 0 */
    mov x0, 0
    cbnz x0, 1f
    b 2f
1:
    FAIL
2:
EXIT
