/* Exit with status 1 like the POSIX false utility:
 * http://pubs.opengroup.org/onlinepubs/9699919799/utilities/false.html
 *
 * Can be uesd to test that emulators forward the exit status properly.
 * https://github.com/cirosantilli/linux-kernel-module-cheat#gem5-syscall-emulation-exit-status
 */

#include <stdlib.h>

int main(int argc, char **argv) {
    int ret;
    if (argc <= 1) {
        ret = 1;
    } else {
        ret = strtoull(argv[1], NULL, 0);
    }
    return ret;
}
