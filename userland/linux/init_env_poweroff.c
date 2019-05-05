#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <sys/reboot.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int i;

    puts("args:");
    for (i = 0; i < argc; ++i)
        puts(argv[i]);
    puts("");

    puts("env:");
    extern char **environ;
    char **env = environ;
    while (*env) {
        printf("%s\n", *env);
        env++;
    }
    puts("");

    /* Poweroff. */
    reboot(RB_POWER_OFF);
}
