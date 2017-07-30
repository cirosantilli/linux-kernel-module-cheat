/*
Only tested in x86_64.

Adapted from: https://github.com/dwks/pagemap/blob/8a25747bc79d6080c8b94eac80807a4dceeda57a/pagemap2.c

- https://stackoverflow.com/questions/17021214/how-to-decode-proc-pid-pagemap-entries-in-linux/45126141#45126141
- https://stackoverflow.com/questions/5748492/is-there-any-api-for-determining-the-physical-address-from-virtual-address-in-li

Dump the page map of a given process PID.

Data sources: /proc/PIC/{map,pagemap}
*/

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h" /* pagemap_get_entry */

int main(int argc, char **argv) {
	char buffer[BUFSIZ];
	char maps_file[BUFSIZ];
	char pagemap_file[BUFSIZ];
	int maps_fd;
	int offset = 0;
	int pagemap_fd;
	pid_t pid;

	if (argc < 2) {
		printf("Usage: %s pid\n", argv[0]);
		return EXIT_FAILURE;
	}
	pid = strtoull(argv[1], NULL, 0);
	snprintf(maps_file, sizeof(maps_file), "/proc/%ju/maps", (uintmax_t)pid);
	snprintf(pagemap_file, sizeof(pagemap_file), "/proc/%ju/pagemap", (uintmax_t)pid);
	maps_fd = open(maps_file, O_RDONLY);
	if (maps_fd < 0) {
		perror("open maps");
		return EXIT_FAILURE;
	}
	pagemap_fd = open(pagemap_file, O_RDONLY);
	if (pagemap_fd < 0) {
		perror("open pagemap");
		return EXIT_FAILURE;
	}
	printf("addr pfn soft-dirty file/shared swapped present library\n");
	for (;;) {
		ssize_t length = read(maps_fd, buffer + offset, sizeof buffer - offset);
		if (length <= 0) break;
		length += offset;
		for (size_t i = offset; i < (size_t)length; i++) {
			uintptr_t low = 0, high = 0;
			if (buffer[i] == '\n' && i) {
				const char *lib_name;
				size_t y;
				/* Parse a line from maps. Each line contains a range that contains many pages. */
				{
					size_t x = i - 1;
					while (x && buffer[x] != '\n') x--;
					if (buffer[x] == '\n') x++;
					size_t beginning = x;
					while (buffer[x] != '-' && x < sizeof buffer) {
						char c = buffer[x++];
						low *= 16;
						if (c >= '0' && c <= '9') {
							low += c - '0';
						} else if (c >= 'a' && c <= 'f') {
							low += c - 'a' + 10;
						} else {
						    break;
						}
					}
					while (buffer[x] != '-' && x < sizeof buffer) x++;
					if (buffer[x] == '-') x++;
					while (buffer[x] != ' ' && x < sizeof buffer) {
						char c = buffer[x++];
						high *= 16;
						if (c >= '0' && c <= '9') {
							high += c - '0';
						} else if (c >= 'a' && c <= 'f') {
							high += c - 'a' + 10;
						} else {
							break;
						}
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
					PagemapEntry entry;
					for (uintptr_t addr = low; addr < high; addr += sysconf(_SC_PAGE_SIZE)) {
						/* TODO always fails for the last page (vsyscall), why? pread returns 0. */
						if (!pagemap_get_entry(&entry, pagemap_fd, addr)) {
							printf("%jx %jx %u %u %u %u %s\n",
								(uintmax_t)addr,
								(uintmax_t)entry.pfn,
								entry.soft_dirty,
								entry.file_page,
								entry.swapped,
								entry.present,
								lib_name
							);
						}
					}
				}
				buffer[y] = '\n';
			}
		}
	}
	close(maps_fd);
	close(pagemap_fd);
	return EXIT_SUCCESS;
}
