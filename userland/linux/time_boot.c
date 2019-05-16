/* https://github.com/cirosantilli/linux-kernel-module-cheat#time_boot-out */

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <unistd.h>

int main(void) {
    FILE *fp;
    fp = fopen("/dev/kmsg", "w");
    fputs(__FILE__ "\n", fp);
    fclose(fp);
    while (1)
        sleep(0xFFFFFFFF);
}
