/* x86_64 freestanding C inline assemby Linux hello world
 * https://github.com/cirosantilli/linux-kernel-module-cheat#linux-system-calls
 */

#define _XOPEN_SOURCE 700
#include <inttypes.h>
#include <sys/types.h>

ssize_t my_write(int fd, const void *buf, size_t size) {
    ssize_t ret;
    __asm__ __volatile__ (
        "syscall"
        : "=a" (ret)
        : "0" (1), "D" (fd), "S" (buf), "d" (size)
        : "cc", "rcx", "r11", "memory"
    );
    return ret;
}

void my_exit(int exit_status) {
    ssize_t ret;
    __asm__ __volatile__ (
        "syscall"
        : "=a" (ret)
        : "0" (60), "D" (exit_status)
        : "cc", "rcx", "r11", "memory"
    );
}

void _start(void) {
    char msg[] = "hello\n";
    my_exit(my_write(1, msg, sizeof(msg)) != sizeof(msg));
}
