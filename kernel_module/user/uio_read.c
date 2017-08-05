/*
TODO get working. Currently I don't understand the behaviour.

TODO how to ACK interrupts? How to ensure that every interrupt gets handled separately?

TODO write to registers. Currently using /dev/mem and lspci.

Handle interrupts from userland and print a message to stdout.

- Userland driver
- https://stackoverflow.com/questions/15286772/userspace-vs-kernel-space-driver
- https://01.org/linuxgraphics/gfx-docs/drm/driver-api/uio-howto.html
- https://stackoverflow.com/questions/7986260/linux-interrupt-handling-in-user-space
- https://yurovsky.github.io/2014/10/10/linux-uio-gpio-interrupt/
- https://github.com/bmartini/zynq-axis/blob/65a3a448fda1f0ea4977adfba899eb487201853d/dev/axis.c
- https://yurovsky.github.io/2014/10/10/linux-uio-gpio-interrupt/
- http://nairobi-embedded.org/uio_example.html that website has QEMU examples for everything as usual. The example has a kernel-side which creates the memory mappings and is used by the user.

Userland driver stability questions:

- https://stackoverflow.com/questions/8030758/getting-kernel-version-from-linux-kernel-module-at-runtime/45430233#45430233
- https://stackoverflow.com/questions/37098482/how-to-build-a-linux-kernel-module-so-that-it-is-compatible-with-all-kernel-rele/45429681#45429681
- https://liquidat.wordpress.com/2007/07/21/linux-kernel-2623-to-have-stable-userspace-driver-api/
*/

#if 1

/* Adapted from: https://yurovsky.github.io/2014/10/10/linux-uio-gpio-interrupt */

#define _XOPEN_SOURCE 700
#include <fcntl.h> /* open */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> /* write */

#include <assert.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
    char *dev = "/dev/uio0";
    if (argc > 1) {
        dev = argv[1];
        exit(EXIT_FAILURE);
    }
    int fd = open(dev, O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* TODO not supported by this kernel module? */
	/*int *addr = mmap(NULL, sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);*/
	/*if (addr == MAP_FAILED) {*/
		/*perror("mmap");*/
		/*assert(0);*/
	/*}*/
	/**addr = 0x12345678;*/

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
            printf(__FILE__ " read = %u\n", info);
        }
    }
}

#else

/* Ripped from the kernel docs. */

#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unistd.h>

int main()
{
    int uiofd;
    int configfd;
    int err;
    int i;
    unsigned icount;
    unsigned char command_high;

    uiofd = open("/dev/uio0", O_RDONLY);
    if (uiofd < 0) {
        perror("uio open:");
        return errno;
    }
    configfd = open("/sys/class/uio/uio0/device/config", O_RDWR);
    if (configfd < 0) {
        perror("config open:");
        return errno;
    }
    err = pread(configfd, &command_high, 1, 5);
    if (err != 1) {
        perror("command config read:");
        return errno;
    }
    command_high &= ~0x4;
    for(i = 0;; ++i) {
        fprintf(stderr, "Interrupts: %d\n", icount);
        err = pwrite(configfd, &command_high, 1, 5);
        if (err != 1) {
            perror("config write:");
            break;
        }
        err = read(uiofd, &icount, 4);
        if (err != 4) {
            perror("uio read:");
            break;
        }

    }
    return errno;
}

#endif
