/* https://cirosantilli.com/linux-kernel-module-cheat#rand_check-out */

#include <inttypes.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int bss = 0;
int data = 1;

int main(__attribute__((unused)) int argc, char **argv) {
    int i, *ip;
    uint64_t uint64;
    FILE *fp;

    /* Loaded addresses. */
    printf("&i         = %p\n", (void *)&i);
    printf("&argv[0]   = %p\n", (void *)&argv[0]);
    printf("&main      = %p\n", (void *)(intptr_t)main);
    printf("&bss       = %p\n", (void *)&bss);
    printf("&data      = %p\n", (void *)&data);

    /* Misc syscalls. */
    printf("time(NULL) = %ju\n", (uintmax_t)time(NULL));
    printf("pid        = %ju\n", (uintmax_t)getpid());

    /* malloc */
    ip = malloc(sizeof(*ip));
    printf("&malloc    = %p\n", (void *)ip);
    free(ip);

    /* /dev/urandom */
    fp = fopen("/dev/urandom", "rb");
    fread(&uint64, sizeof(uint64), 1, fp);
    printf("/dev/urandom = %" PRIX64 "\n", uint64);
    fclose(fp);
}
