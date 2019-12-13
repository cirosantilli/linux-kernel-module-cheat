#ifndef LKMC_FUTEX_H
#define LKMC_FUTEX_H

static int
lkmc_futex(int *uaddr, int futex_op, int val,
        const struct timespec *timeout, int *uaddr2, int val3)
{
    (void)uaddr2;
    return syscall(SYS_futex, uaddr, futex_op, val,
                    timeout, uaddr, val3);
}

#endif
