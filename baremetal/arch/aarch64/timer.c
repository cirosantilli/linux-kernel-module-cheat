/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-exception-level */

#include <stdio.h>
#include <inttypes.h>

#define CNTV_CTL_ENABLE  (1 << 0)
#define CNTV_CTL_IMASK   (1 << 1)
#define CNTV_CTL_ISTATUS (1 << 2)

#define SYSREG_READ(type, name) \
    type name ## _read(void) { \
        type name; \
        __asm__ __volatile__("mrs %0, " #name : "=r" (name) : : ); \
        return name; \
    }

#define SYSREG_WRITE(type, name) \
    void name ## _write(type name) { \
        __asm__ __volatile__("msr " #name ", %0" : : "r" (name) : ); \
    }

#define SYSREG_READ_WRITE(name, type) \
    SYSREG_READ(name, type) \
    SYSREG_WRITE(name, type)

/* Frequency in Hz. ? */
SYSREG_READ_WRITE(uint64_t, cntfrq_el0)

/* Current virtual counter value. */
SYSREG_READ(uint64_t, cntvct_el0)

/* Compare value. See: cntv_ctl_el0_enable. */
SYSREG_READ_WRITE(uint64_t, cntv_cval_el0)

/* On write, set cntv_cval_el0 = (cntvct_el0 + cntv_tval_el0).
 * This means that the next interrupt will happen in cntv_tval_el0 cycles.
 */
SYSREG_READ_WRITE(uint64_t, cntv_tval_el0)

/* Control register. */
SYSREG_READ_WRITE(uint32_t, cntv_ctl_el0)

void cntv_ctl_el0_disable(void) {
    cntv_ctl_el0_write(cntv_ctl_el0_read() & ~CNTV_CTL_ENABLE);
}

/* If enabled, when: cntv_ctl > cntv_cval then:
 *
 * * if CNTV_CTL_IMASK is clear, raise an interrupt
 * * set CNTV_CTL_ISTATUS
 */
void cntv_ctl_el0_enable(void) {
    cntv_ctl_el0_write(cntv_ctl_el0_read() | CNTV_CTL_ENABLE);
}

int main(void) {
    /* Initial state. */
    printf("cntv_ctl_el0 0x%" PRIx32 "\n", cntv_ctl_el0_read());
    printf("cntfrq_el0 0x%" PRIx64 "\n", cntfrq_el0_read());
    printf("cntv_cval_el0 0x%" PRIx64 "\n", cntv_cval_el0_read());

    /* Get the counter value many times to watch the time pass. */
    printf("cntvct_el0 0x%" PRIx64 "\n", cntvct_el0_read());
    printf("cntvct_el0 0x%" PRIx64 "\n", cntvct_el0_read());
    printf("cntvct_el0 0x%" PRIx64 "\n", cntvct_el0_read());

#if 0
    /* TODO crashes gem5. */
    puts("cntfrq_el0 = 1");
    cntfrq_el0_write(1);
    printf("cntfrq_el0 0x%" PRIx64 "\n", cntfrq_el0_read());
#endif

    return 0;
}
