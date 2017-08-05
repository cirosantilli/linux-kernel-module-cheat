/*
Adapted from: https://yurovsky.github.io/2014/10/10/linux-uio-gpio-interrupt/

    modprobe uio_pdrv_genirq

TODO get working.

Handle interrupts from userland and print a message to stdout.
*/

#define _XOPEN_SOURCE 700
#include <fcntl.h> /* open */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> /* write */

int main(int argc, char **argv)
{

    char *dev;
    if (argc < 2) {
    }
    int fd = open("/dev/uio", O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    while (1) {
        uint32_t info = 1;
        size_t nb = write(fd, &info, sizeof(info));
        if (nb < sizeof(info)) {
            perror("write");
            close(fd);
            exit(EXIT_FAILURE);
        }
        nb = read(fd, &info, sizeof(info));
        if (nb == sizeof(info)) {
            printf("Interrupt #%u\n", info);
        }
    }
    close(fd);
    exit(EXIT_SUCCESS);
}
