#include <stdio.h>
#include <inttypes.h>

#include <lkmc.h>

#define CNTV_CTL_ENABLE  (1 << 0)
#define CNTV_CTL_IMASK   (1 << 1)
#define CNTV_CTL_ISTATUS (1 << 2)

void cntv_ctl_el0_disable(void) {
    lkmc_sysreg_cntv_ctl_el0_write(lkmc_sysreg_cntv_ctl_el0_read() & ~CNTV_CTL_ENABLE);
}

/* If enabled, when: cntv_ctl > cntv_cval then:
 *
 * * if CNTV_CTL_IMASK is clear, raise an interrupt
 * * set CNTV_CTL_ISTATUS
 */
void cntv_ctl_el0_enable(void) {
    lkmc_sysreg_cntv_ctl_el0_write(lkmc_sysreg_cntv_ctl_el0_read() | CNTV_CTL_ENABLE);
}

int main(void) {
    /* Initial state. */
    printf("cntv_ctl_el0 0x%" PRIx32 "\n", lkmc_sysreg_cntv_ctl_el0_read());
    printf("cntfrq_el0 0x%" PRIx64 "\n", lkmc_sysreg_cntfrq_el0_read());
    printf("cntv_cval_el0 0x%" PRIx64 "\n", lkmc_sysreg_cntv_cval_el0_read());

    /* Get the counter value many times to watch the time pass. */
    printf("cntvct_el0 0x%" PRIx64 "\n", lkmc_sysreg_cntvct_el0_read());
    printf("cntvct_el0 0x%" PRIx64 "\n", lkmc_sysreg_cntvct_el0_read());
    printf("cntvct_el0 0x%" PRIx64 "\n", lkmc_sysreg_cntvct_el0_read());

#if 0
    /* TODO crashes gem5. */
    puts("cntfrq_el0 = 1");
    lkmc_sysreg_cntfrq_el0_write(1);
    printf("cntfrq_el0 0x%" PRIx64 "\n", lkmc_sysreg_cntfrq_el0_read());
#endif

    return 0;
}
