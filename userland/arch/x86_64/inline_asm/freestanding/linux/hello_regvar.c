/* Same as hello.c, but with explicit register variables, see:
 * https://cirosantilli.com/linux-kernel-module-cheat#linux-system-calls
 */

#define _XOPEN_SOURCE 700
#include <inttypes.h>
#include <sys/types.h>

ssize_t my_write(int fd, const void *buf, size_t size) {
    register int64_t rax __asm__ ("rax") = 1;
    register int rdi __asm__ ("rdi") = fd;
    register const void *rsi __asm__ ("rsi") = buf;
    register size_t rdx __asm__ ("rdx") = size;
    __asm__ __volatile__ (
        "syscall"
        : "+r" (rax)
        : "r" (rdi), "r" (rsi), "r" (rdx)
        : "cc", "rcx", "r11", "memory"
    );
    return rax;
}

void my_exit(int exit_status) {
    register int64_t rax __asm__ ("rax") = 60;
    register int rdi __asm__ ("rdi") = exit_status;
    __asm__ __volatile__ (
        "syscall"
        : "+r" (rax)
        : "r" (rdi)
        : "cc", "rcx", "r11", "memory"
    );
}

void _start(void) {
    char msg[] = "hello\n";
    my_exit(my_write(1, msg, sizeof(msg)) != sizeof(msg));
}
