/* https://github.com/cirosantilli/linux-kernel-module-cheat#anonymous-inode */

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> /* sleep */

#include <lkmc/anonymous_inode.h>

int main(int argc, char **argv) {
    char buf[1024];
    int fd_ioctl, fd_ioctl_anon, ret;
    size_t i, nreads;

    if (argc < 2) {
        puts("Usage: ./prog <ioctl-file> [<nreads>]");
        return EXIT_FAILURE;
    } else if (argc > 2) {
        nreads = strtol(argv[2], NULL, 10);
    } else {
        nreads = 3;
    }
    fd_ioctl = open(argv[1], O_RDONLY);
    if (fd_ioctl == -1) {
        perror("open");
        return EXIT_FAILURE;
    }
    ret = ioctl(fd_ioctl, LKMC_ANONYMOUS_INODE_GET_FD, &fd_ioctl_anon);
    if (ret == -1) {
        perror("ioctl");
        return EXIT_FAILURE;
    }
    for (i = 0; i < nreads; ++i) {
        ret = read(fd_ioctl_anon, buf, sizeof(buf));
        printf("%.*s\n", ret, buf);
    }
    close(fd_ioctl_anon);
    close(fd_ioctl);
    return EXIT_SUCCESS;
}
