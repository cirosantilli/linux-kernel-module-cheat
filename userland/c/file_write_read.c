/* https://cirosantilli.com/linux-kernel-module-cheat#c */

#include <lkmc.h>
#include <lkmc/file_io.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>

/* Same as `file_size`, but takes the path instead of a `FILE*`. */
long file_size(char *path) {
    FILE *fp;
    long return_value;
    fp = fopen(path, "r");
    if (fp == NULL) {
        return -1L;
    }
    return_value = lkmc_fget_file_size(fp);
    if (fclose(fp) == EOF) {
        return -1L;
    }
    return return_value;
}

/* Write a null terminated string to file
 *
 * Return -1 on failure, 0 on success.
 */
int file_write(char *path, char *write_string) {
    long len;
    FILE *fp;

    fp = fopen(path, "wb");
    if (fp == NULL) {
        return -1;
    }
    len = strlen(write_string);
    /* copy the file into the buffer: */
    if (fwrite(write_string, 1, len, fp) != (size_t)len) {
        return -1;
    }
    if (fclose(fp) == EOF) {
        return -1;
    }
    return 0;
}

int main(void) {
    char *path = LKMC_TMP_FILE;
    char *data = "asdf\nqwer\n";

    /* Write entire string to file at once. */
    if (file_write(path, data) == -1) {
        LKMC_IO_ERROR("file_write", path);
    }

    /* Read entire file at once to a string. */
    {
        char *output = lkmc_file_read(path);
        if (output == NULL) {
            LKMC_IO_ERROR("lkmc_file_read", path);
        }
        assert(strcmp(data, output) == 0);
        free(output);
    }

    /* Get file size. */
    {
        long size = file_size(path);
        if (size == -1) {
            LKMC_IO_ERROR("file_size", path);
        }
        assert((size_t)size == strlen(data));
    }

    return EXIT_SUCCESS;
}
