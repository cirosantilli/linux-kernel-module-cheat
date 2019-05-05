/* This is the main entry point for all .S examples.
 * https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly-c-standard-library
 */

#include "stdio.h"
#include "stdint.h"

#include "lkmc.h"

int asm_main(uint32_t *line);

int main(void) {
    uint32_t ret, line;
    ret = asm_main(&line);
    if (ret) {
        printf("error %d at line %d\n", ret, line);
    }
    return ret;
}
