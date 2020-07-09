#ifndef LKMC_FUTEX_H
#define LKMC_FUTEX_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

static int
lkmc_futex(int *uaddr, int futex_op, int val,
        const struct timespec *timeout, int *uaddr2, int val3)
{
#if defined(__aarch64__)
    register uint64_t x0 __asm__ ("x0") = (uint64_t)uaddr;
    register uint64_t x1 __asm__ ("x1") = futex_op;
    register uint64_t x2 __asm__ ("x2") = val;
    register const struct timespec *x3 __asm__ ("x3") = timeout;
    register int *x4 __asm__ ("x4") = uaddr2;
    register uint64_t x5 __asm__ ("x5") = val3;
    register uint64_t x8 __asm__ ("x8") = SYS_futex; /* syscall number */
    __asm__ __volatile__ (
        "svc 0;"
        : "+r" (x0)
        : "r" (x1), "r" (x2), "r" (x3), "r" (x4), "r" (x5), "r" (x8)
        : "memory"
    );
    return x0;
#else
    (void)uaddr2;
    return syscall(SYS_futex, uaddr, futex_op, val,
                    timeout, uaddr, val3);
#endif
}

#endif
