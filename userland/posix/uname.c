/* https://github.com/cirosantilli/linux-kernel-module-cheat#fatal-kernel-too-old */

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>

int main(void) {
    struct utsname info;
    if (uname(&info) == -1) {
        perror("uname");
        exit(EXIT_FAILURE);
    }
    printf("sysname  = %s\n", info.sysname );
    printf("nodename = %s\n", info.nodename);
    printf("release  = %s\n", info.release );
    printf("version  = %s\n", info.version );
    printf("machine  = %s\n", info.machine );
    return EXIT_SUCCESS;
}
