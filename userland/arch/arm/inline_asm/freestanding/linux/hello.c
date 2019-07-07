/* https://cirosantilli.com/linux-kernel-module-cheat#linux-system-calls
 *
 * arm freestanding C inline assemby Linux hello world.
 */

#include <inttypes.h>

void _start(void) {
    uint32_t exit_status;

    /* write */
    {
        char msg[] = "hello\n";
        uint32_t syscall_return;
        register uint32_t r0 __asm__ ("r0") = 1; /* stdout */
        register char *r1 __asm__ ("r1") = msg;
        register uint32_t r2 __asm__ ("r2") = sizeof(msg);
        register uint32_t r8 __asm__ ("r7") = 4; /* syscall number */
        __asm__ __volatile__ (
            "svc 0;"
            : "+r" (r0)
            : "r" (r1), "r" (r2), "r" (r8)
            : "memory"
        );
        syscall_return = r0;
        exit_status = (syscall_return != sizeof(msg));
    }

    /* exit */
    {
        register uint32_t r0 __asm__ ("r0") = exit_status;
        register uint32_t r7 __asm__ ("r7") = 1;
        __asm__ __volatile__ (
            "svc 0;"
            : "+r" (r0)
            : "r" (r7)
            :
        );
    }
}
