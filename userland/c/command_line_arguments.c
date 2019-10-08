/* Print each command line argument received, one per line.
 *
 * Good sanity check for user mode:
 * https://cirosantilli.com/linux-kernel-module-cheat#qemu-user-mode-getting-started
 */

#include <stdio.h>

int main(int argc, char **argv) {
    size_t i;
    for (i = 0; i < (size_t)argc; ++i)
        printf("%s\n", argv[i]);
    return 0;
}
