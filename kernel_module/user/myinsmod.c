#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define init_module(mod, len, opts) syscall(__NR_init_module, mod, len, opts)

int main(int argc, char **argv) {
	const char *params;
	int fd;
	size_t image_size;
	struct stat st;
	void *image;

	if (argc < 2) {
		puts("Usage ./prog mymodule.ko [args]");
		return EXIT_FAILURE;
	}
	if (argc < 3) {
		params = "";
	} else {
		params = argv[2];
	}
	fd = open(argv[1], O_RDONLY);
	fstat(fd, &st);
	image_size = st.st_size;
	image = malloc(image_size);
	read(fd, image, image_size);
	close(fd);
	if (init_module(image, image_size, params) != 0) {
		perror("init_module");
		return EXIT_FAILURE;
	}
	free(image);
	return EXIT_SUCCESS;
}
