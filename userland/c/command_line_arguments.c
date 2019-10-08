/* https://cirosantilli.com/linux-kernel-module-cheat#c */

#include <stdio.h>

int main(int argc, char **argv) {
    size_t i;
    for (i = 0; i < (size_t)argc; ++i)
        printf("%s\n", argv[i]);
    return 0;
}
