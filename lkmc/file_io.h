#ifndef LKMC_FILE_IO_H
#define LKMC_FILE_IO_H

/* Returns the size of the given open `FILE*`.
 *
 * If an error occurs, returns `-1L`.
 *
 * Does not work for pipes.
 */
long lkmc_fget_file_size(FILE *fp) {
    long oldpos;
    long return_value;
    oldpos = ftell(fp);
    if (oldpos == -1L) {
        return -1L;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        return -1L;
    }
    return_value = ftell(fp);
    if (return_value == -1L) {
        return -1L;
    }
    /* Restore the old position. */
    if (fseek(fp, oldpos , SEEK_SET) != 0) {
        return -1L;
    }
    return return_value;
}

/* Read the entire file to a char[] dynamically allocated inside this function.
 *
 * Returns a pointer to the start of that array.
 *
 * In case of any error, returns NULL.
 *
 * http://stackoverflow.com/questions/174531/easiest-way-to-get-files-contents-in-c
 */
char *lkmc_file_read(char *path) {
    FILE *fp;
    char *buffer;
    long fsize;

    fp = fopen(path , "rb");
    if (fp == NULL) {
        return NULL;
    }
    fsize = lkmc_fget_file_size(fp);
    if (fsize < 0){
        fprintf(stderr, "could not determine lenght of:\n%s\n", path);
        return NULL;
    }
    buffer = (char*)malloc(fsize);
    if (buffer == NULL) {
        return NULL;
    }
    if (fread(buffer, 1, fsize, fp) != (size_t)fsize) {
        return NULL;
    }
    if (fclose(fp) == EOF){
        return NULL;
    }
    return buffer;
}

#endif
