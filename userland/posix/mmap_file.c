/* https://cirosantilli.com/linux-kernel-module-cheat#mmap-file */

#define _XOPEN_SOURCE 700
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <lkmc.h>

int main(void) {
    char *filepath = LKMC_TMP_FILE;
    enum Constexpr { NUMINTS = 4 };
    size_t filesize = NUMINTS * sizeof(int);

    int i;
    int fd;
    int result;
    /* mmapped array of int's */
    int *map;

    /* Write to file with mmap. */
    {
        /* `O_WRONLY` is not sufficient when mmaping, need `O_RDWR`.*/
        fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        /* Set fd position and write to it to strech the file. */
        result = lseek(fd, filesize - 1, SEEK_SET);
        if (result == -1) {
            close(fd);
            perror("lseek");
            exit(EXIT_FAILURE);
        }

        /* Write something to the file to actually strech it. */
        result = write(fd, "", 1);
        if (result != 1) {
            close(fd);
            perror("write");
            exit(EXIT_FAILURE);
        }

        /* Do the actual mapping call. */
        map = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED) {
            close(fd);
            perror("mmap");
            exit(EXIT_FAILURE);
        }

        /* Write int's to the file as if it were memory because MAP_SHARED was used. */
        for (i = 0; i < NUMINTS; ++i) {
            map[i] = i;
        }

        /* Free mmapped memory. */
        if (munmap(map, filesize) == -1) {
            perror("munmap");
            exit(EXIT_FAILURE);
        }

        /* Un-mmaping doesn't close the file, so we still need to do that. */
        if (close(fd) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }
    }

    /* Read result back in. */
    {
        fd = open(filepath, O_RDONLY, 0);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        map = mmap(0, filesize, PROT_READ, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED) {
            close(fd);
            perror("mmap");
            exit(EXIT_FAILURE);
        }

        assert(map[1] == 1);

        /* Segmentation fault because no `PROT_WRITE`: */
        /*map[1] = 2;*/

        if (munmap(map, filesize) == -1) {
            perror("munmap");
            exit(EXIT_FAILURE);
        }

        if (close(fd) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
