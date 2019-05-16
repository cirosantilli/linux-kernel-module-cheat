/* aarch64 freestanding C inline assemby Linux hello world
 * https://github.com/cirosantilli/linux-kernel-module-cheat#linux-system-calls
 */

#include <inttypes.h>

void _start(void) {
    uint64_t exit_status;

    /* write */
    {
        char msg[] = "hello\n";
        uint64_t syscall_return;
        register uint64_t x0 __asm__ ("x0") = 1; /* stdout */
        register char *x1 __asm__ ("x1") = msg;
        register uint64_t x2 __asm__ ("x2") = sizeof(msg);
        register uint64_t x8 __asm__ ("x8") = 64; /* syscall number */
        __asm__ __volatile__ (
            "svc 0;"
            : "+r" (x0)
            : "r" (x1), "r" (x2), "r" (x8)
            : "memory"
        );
        syscall_return = x0;
        exit_status = (syscall_return != sizeof(msg));
    }

    /* exit */
    {
        register uint64_t x0 __asm__ ("x0") = exit_status;
        register uint64_t x8 __asm__ ("x8") = 93;
        __asm__ __volatile__ (
            "svc 0;"
            : "+r" (x0)
            : "r" (x8)
            :
        );
    }
}
