/* https://cirosantilli.com/linux-kernel-module-cheat#c */

#include <stdio.h>

int main(int argc, char **argv) {
    int i;
    printf("argc = %d\n", argc);
    for (i = 0; i < argc; ++i) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    return 0;
}
