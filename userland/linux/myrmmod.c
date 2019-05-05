/* https://github.com/cirosantilli/linux-kernel-module-cheat#myinsmod */

#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define delete_module(name, flags) syscall(__NR_delete_module, name, flags)

int main(int argc, char **argv) {
    if (argc != 2) {
        puts("Usage ./prog mymodule");
        return EXIT_FAILURE;
    }
    if (delete_module(argv[1], O_NONBLOCK) != 0) {
        perror("delete_module");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
