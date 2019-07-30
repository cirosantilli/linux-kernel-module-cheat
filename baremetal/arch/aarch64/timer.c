#include <stdio.h>
#include <inttypes.h>

#include <lkmc.h>
#include <lkmc/gicv3.h>

void lkmc_vector_trap_handler(LkmcVectorExceptionFrame *exception __attribute__((unused))) {
    printf("CNTVCT_EL0 0x%" PRIX64 "\n", lkmc_sysreg_cntvct_el0_read());
}

/* DAIF, Interrupt Mask Bits */
#define DAIF_DBG_BIT (1<<3) /* Debug mask bit */
#define DAIF_ABT_BIT (1<<2) /* Asynchronous abort mask bit */
#define DAIF_IRQ_BIT (1<<1) /* IRQ mask bit */
#define DAIF_FIQ_BIT (1<<0) /* FIQ mask bit */

#define wfi() __asm__ __volatile__ ("wfi" : : : "memory")

void enable_cntv(void) {
    lkmc_sysreg_cntv_ctl_el0_write(lkmc_sysreg_cntv_ctl_el0_read() | LKMC_CNTV_CTL_ENABLE);
}

void enable_irq(void) {
    __asm__ __volatile__ ("msr DAIFClr, %0" : : "i" (DAIF_IRQ_BIT)  : "memory");
}

int main(void) {
    /* Initial state. */
    printf("CNTV_CTL_EL0 0x%" PRIX32 "\n", lkmc_sysreg_cntv_ctl_el0_read());
    printf("CNTFRQ_EL0 0x%" PRIX64 "\n", lkmc_sysreg_cntfrq_el0_read());
    printf("CNTV_CVAL_EL0 0x%" PRIX64 "\n", lkmc_sysreg_cntv_cval_el0_read());

    /* Get the counter value many times to watch the time pass. */
    printf("CNTVCT_EL0 0x%" PRIX64 "\n", lkmc_sysreg_cntvct_el0_read());
    printf("CNTVCT_EL0 0x%" PRIX64 "\n", lkmc_sysreg_cntvct_el0_read());
    printf("CNTVCT_EL0 0x%" PRIX64 "\n", lkmc_sysreg_cntvct_el0_read());

    /**/
    gic_v3_initialize();
    {
        /*uint64_t ticks, current_cnt;*/
        /*uint32_t cntfrq;*/
        lkmc_sysreg_cntfrq_el0_write(1);
        /*ticks = cntfrq;*/
        /*current_cnt = lkmc_sysreg_cntvct_el0_read();*/
        /*lkmc_sysreg_cntv_cval_el0_write(current_cnt + ticks);*/
        enable_cntv();
        enable_irq();
    }
    /*while (1) {*/
        /*puts("qwer");*/
        /*current_cnt = raw_read_cntvct_el0();*/
        /*val = raw_read_cntv_ctl();*/
        /*printf("CNTVCT_EL0 = ");*/
        /*wfi();*/
    /*}*/

#if 0
    /* TODO crashes gem5. */
    puts("cntfrq_el0 = 1");
    lkmc_sysreg_cntfrq_el0_write(1);
    printf("cntfrq_el0 0x%" PRIX64 "\n", lkmc_sysreg_cntfrq_el0_read());
#endif

    return 0;
}
