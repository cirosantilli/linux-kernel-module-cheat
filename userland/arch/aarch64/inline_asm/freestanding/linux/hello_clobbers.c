/* Like hello.c trying to do it without named register variables.
 * The code is more complicated, and I was not able to get as efficient,
 * so better just stick to named register variables.
 *
 * https://cirosantilli.com/linux-kernel-module-cheat#linux-system-calls
 */

#include <inttypes.h>

void _start(void) {
    uint64_t exit_status;

    /* write */
    {
        char msg[] = "hello\n";
        uint64_t syscall_return;
        __asm__ (
            "mov x0, 1;" /* stdout */
            "mov x1, %[msg];"
            "mov x2, %[len];"
            "mov x8, 64;" /* syscall number */
            "svc 0;"
            "mov %[syscall_return], x0;"
            : [syscall_return] "=r" (syscall_return)
            : [msg] "p" (msg),
            [len] "i" (sizeof(msg))
            : "x0", "x1", "x2", "x8", "memory"
        );
        exit_status = (syscall_return != sizeof(msg));
    }

    /* exit */
    __asm__ (
        "mov x0, %[exit_status];"
        "mov x8, 93;" /* syscall number */
        "svc 0;"
        :
        : [exit_status] "r" (exit_status)
        : "x0", "x8"
    );
}

