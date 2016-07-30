/*
http://stackoverflow.com/questions/5947286/how-can-linux-kernel-modules-be-loaded-from-c-code/38606527#38606527
*/

#define _GNU_SOURCE
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define init_module(mod, len, opts) syscall(__NR_init_module, mod, len, opts)
#define delete_module(name, flags) syscall(__NR_delete_module, name, flags)

int main(void) {
    int fd = open("hello.ko", O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    size_t image_size = st.st_size;
    void *image = malloc(image_size);
    read(fd, image, image_size);
    close(fd);
    if (init_module(image, image_size, "") != 0) {
        perror("init_module");
        return EXIT_FAILURE;
    }
    free(image);
    if (delete_module("hello", O_NONBLOCK) != 0) {
        perror("delete_modul");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
