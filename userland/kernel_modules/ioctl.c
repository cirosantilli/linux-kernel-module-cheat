/* https://cirosantilli.com/linux-kernel-module-cheat#ioctl */

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <lkmc/ioctl.h>

int main(int argc, char **argv) {
    char *ioctl_path;
    int fd, request, arg0, arg1, arg_int, ret;
    lkmc_ioctl_struct arg_struct;

    if (argc < 2) {
        puts("Usage: ./prog <ioctl-file> <request> [<arg>...]");
        return EXIT_FAILURE;
    }
    ioctl_path = argv[1];
    request = strtol(argv[2], NULL, 10);
    if (argc > 3) {
        arg0 = strtol(argv[3], NULL, 10);
    } else {
        arg0 = 0;
    }
    if (argc > 4) {
        arg1 = strtol(argv[4], NULL, 10);
    } else {
        arg1 = 0;
    }

    fd = open(ioctl_path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }
    switch (request)
    {
        case 0:
            arg_int = arg0;
            ret = ioctl(fd, LKMC_IOCTL_INC, &arg_int);
            if (ret != -1) {
                printf("%d\n", arg_int);
            }
            break;
        case 1:
            arg_struct.i = arg0;
            arg_struct.j = arg1;
            ret = ioctl(fd, LKMC_IOCTL_INC_DEC, &arg_struct);
            if (ret != -1) {
                printf("%d %d\n", arg_struct.i, arg_struct.j);
            }
            break;
        default:
            puts("error: unknown request");
            return EXIT_FAILURE;
    }
    if (ret == -1) {
        perror("ioctl");
        printf("errno = %d\n", errno);
        return EXIT_FAILURE;
    }
    close(fd);
    return EXIT_SUCCESS;
}
