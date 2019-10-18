/* https://cirosantilli.com/linux-kernel-module-cheat#c */

#define _GNU_SOURCE
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    char buf[]  = { 'a', 'b', 'c', 'd' };
    char buf2[]  = { 'e', 'f', 'g', 'h' };
    int f, ret;
    size_t off;

    /* Create the temporary file and write to it. */
    f = open(".", O_TMPFILE | O_RDWR, S_IRUSR | S_IWUSR);
    ret = write(f, buf, sizeof(buf));

    /* Interactivelly check if anything changed on directory. It hasn't. */
    /*puts("hit enter to continue");*/
    /*getchar();*/

    /* Read from the temporary file, and assert that
     * we read the same as we wrote. */
    lseek(f, 0, SEEK_SET);
    off = 0;
    while ((ret = read(f, buf2 + off, sizeof(buf) - off))) {
        off += ret;
    }
    close(f);
    assert(!memcmp(buf, buf2, sizeof(buf)));

    /* Assert that the file is gone now that we removed it. */

    return EXIT_SUCCESS;
}
