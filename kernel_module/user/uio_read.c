/*
Adapted from: https://yurovsky.github.io/2014/10/10/linux-uio-gpio-interrupt/

    modprobe uio_pci_generic
    echo '1234 11e9' > /sys/bus/pci/drivers/uio_pci_generic/new_id
    /uio_read.out &

TODO get working. The problem now is how to generate IRQs to test our IRQ handling:

-   a synchronous IRQ needs regwrites to be kicked off, but mmap does not seem to work with this driver
-   a custom PCI periodic timer device would also need to be initiazed by some regwrite, otherwise
    early interrupts before the OS is setup would crash everything?

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
            printf("Interrupt #%u\n", info);
        }
    }
    close(fd);
    exit(EXIT_SUCCESS);
}
