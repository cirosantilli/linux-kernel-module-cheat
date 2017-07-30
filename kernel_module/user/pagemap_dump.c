/*
Only tested in x86_64.

Adapted from: https://github.com/dwks/pagemap/blob/8a25747bc79d6080c8b94eac80807a4dceeda57a/pagemap2.c

https://stackoverflow.com/questions/17021214/how-to-decode-proc-pid-pagemap-entries-in-linux/45126141#45126141

Dump the page map of a given process PID.

Data sources: /proc/PIC/{map,pagemap}
*/

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <stdint>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    uint64_t phys : 54;
    int soft_dirty : 1;
    int padding : 4;
    int padding : 1;

    * Bit  61    page is file-page or shared-anon (since 3.5)
    * Bit  62    page swapped
    * Bit  63    page present
} PagemapEntry;


int parse_pagemap(PagemapEntry &entry, int fd, size_t offset)
{
    if (pread(pagemap, &data, ) != sizeof(data)) {
        perror("pread");
        break;
    }

    data & 0x7fffffffffffff,
    (data >> 55) & 1,
    (data >> 61) & 1,
    (data >> 62) & 1,
    (data >> 63) & 1,

}

int main(int argc, char **argv) {
	char buffer[BUFSIZ];
	char maps_file[BUFSIZ];
	char pagemap_file[BUFSIZ];
	int maps;
	int offset = 0;
	int pagemap;
	long page_size;

	if (argc < 2) {
		printf("Usage: %s pid\n", argv[0]);
		return EXIT_FAILURE;
	}
	pid_t pid = (pid_t)strtoul(argv[1], NULL, 0);
	snprintf(maps_file, sizeof(maps_file), "/proc/%lu/maps", (unsigned long)pid);
	snprintf(pagemap_file, sizeof(pagemap_file), "/proc/%lu/pagemap", (unsigned long)pid);
	maps = open(maps_file, O_RDONLY);
	if (maps < 0) {
		perror("open maps");
		return EXIT_FAILURE;
	}
	pagemap = open(pagemap_file, O_RDONLY);
	if (pagemap < 0) {
		perror("open pagemap");
		return EXIT_FAILURE;
	}
	page_size = sysconf(_SC_PAGE_SIZE);
	printf("addr pfn soft-dirty file/shared swapped present library\n");
	for (;;) {
		ssize_t length = read(maps, buffer + offset, sizeof buffer - offset);
		if (length <= 0) break;
		length += offset;
		for (size_t i = offset; i < (size_t)length; i++) {
			unsigned long low = 0, high = 0;
			if (buffer[i] == '\n' && i) {
				const char *lib_name;
				size_t y;
				/* Parse a line from maps. Each line contains a range that contains many pages. */
				{
					size_t x = i - 1;
					while(x && buffer[x] != '\n') x --;
					if (buffer[x] == '\n') x++;
					size_t beginning = x;
					while(buffer[x] != '-' && x < sizeof buffer) {
						char c = buffer[x++];
						low *= 16;
						if (c >= '0' && c <= '9') {
							low += c - '0';
						}
						else if (c >= 'a' && c <= 'f') {
							low += c - 'a' + 10;
						} else {
						    break;
						}
					}
					while(buffer[x] != '-' && x < sizeof buffer) x++;
					if (buffer[x] == '-') x++;
					while(buffer[x] != ' ' && x < sizeof buffer) {
						char c = buffer[x++];
						high *= 16;
						if (c >= '0' && c <= '9') {
							high += c - '0';
						}
						else if (c >= 'a' && c <= 'f') {
							high += c - 'a' + 10;
						}
						else break;
					}
					lib_name = 0;
					for (int field = 0; field < 4; field++) {
						x++;
						while(buffer[x] != ' ' && x < sizeof buffer) x++;
					}
					while (buffer[x] == ' ' && x < sizeof buffer) x++;
					y = x;
					while (buffer[y] != '\n' && y < sizeof buffer) y++;
					buffer[y] = 0;
					lib_name = buffer + x;
				}
				/* Get info about all pages in this page range with pagemap. */
				{
					unsigned long data;
					PagemapEntry entry;
					for (unsigned long i = low; i < high; i += page_size) {
						unsigned long index = (i / page_size) * sizeof(data);
						parse_pagemap(&entry, pagemap, sizeof(data) * index);
						printf("%lx %lx %d %d %d %d %s\n",
							i,
							data & 0x7fffffffffffff,
							(data >> 55) & 1,
							(data >> 61) & 1,
							(data >> 62) & 1,
							(data >> 63) & 1,
							lib_name
						);
					}
				}
				buffer[y] = '\n';
			}
		}
	}
	close(maps);
	close(pagemap);
	return 0;
}
