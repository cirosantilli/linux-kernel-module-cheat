#ifndef LKMC_GICV3_H
#define LKMC_GICV3_H

#include <lkmc/aarch64.h>
#include <lkmc/board.h>

typedef int32_t irq_no;

#define GIC_GICD_BASE (GIC_BASE)
#define GIC_GICC_BASE (GIC_BASE + 0x10000)

#define GIC_GICD_INT_PER_REG            (32)
#define GIC_GICD_IPRIORITY_PER_REG      (4)
#define GIC_GICD_IPRIORITY_SIZE_PER_REG (8)
#define GIC_GICD_ITARGETSR_CORE0_TARGET_BMAP (0x01010101)
#define GIC_GICD_ITARGETSR_PER_REG      (4)
#define GIC_GICD_ITARGETSR_SIZE_PER_REG (8)
#define GIC_GICD_ICFGR_PER_REG          (16)
#define GIC_GICD_ICFGR_SIZE_PER_REG     (2)
#define GIC_GICD_ICENABLER_PER_REG      (32)
#define GIC_GICD_ISENABLER_PER_REG      (32)
#define GIC_GICD_ICPENDR_PER_REG        (32)
#define GIC_GICD_ISPENDR_PER_REG        (32)

/* 8.12 The GIC CPU interface register map */
#define GIC_GICC_CTLR     (GIC_GICC_BASE + 0x000) /* CPU Interface Control Register */
#define GIC_GICC_PMR      (GIC_GICC_BASE + 0x004) /* Interrupt Priority Mask Register */
#define GIC_GICC_BPR      (GIC_GICC_BASE + 0x008) /* Binary Point Register */
#define GIC_GICC_IAR      (GIC_GICC_BASE + 0x00C) /* Interrupt Acknowledge Register */
#define GIC_GICC_EOIR     (GIC_GICC_BASE + 0x010) /* End of Interrupt Register */
#define GIC_GICC_RPR      (GIC_GICC_BASE + 0x014) /* Running Priority Register */
#define GIC_GICC_HPIR     (GIC_GICC_BASE + 0x018) /* Highest Pending Interrupt Register */
#define GIC_GICC_ABPR     (GIC_GICC_BASE + 0x01C) /* Aliased Binary Point Register */
#define GIC_GICC_IIDR     (GIC_GICC_BASE + 0x0FC) /* CPU Interface Identification Register */

/* 8.13.7 GICC_CTLR, CPU Interface Control Register */
#define GICC_CTLR_ENABLE            (0x1)   /* Enable GICC */
#define GICC_CTLR_DISABLE           (0x0)   /* Disable GICC */

/* 8.13.14 GICC_PMR, CPU Interface Priority Mask Register */
#define GICC_PMR_PRIO_MIN           (0xff)  /* The lowest level mask */
#define GICC_PMR_PRIO_HIGH          (0x0)   /* The highest level mask */

/* 8.13.6 GICC_BPR, CPU Interface Binary Point Register */
/* In systems that support only one Security state, when GICC_CTLR.CBPR == 0,
this register determines only Group 0 interrupt preemption. */
#define GICC_BPR_NO_GROUP           (0x0)   /* handle all interrupts */

/* 8.13.11 GICC_IAR, CPU Interface Interrupt Acknowledge Register */
#define GICC_IAR_INTR_IDMASK        (0x3ff) /* 0-9 bits means Interrupt ID */
#define GICC_IAR_SPURIOUS_INTR      (0x3ff) /* 1023 means spurious interrupt */

/* 8.8 The GIC Distributor register map */
#define GIC_GICD_CTLR           (GIC_GICD_BASE + 0x000)                 /* Distributor Control Register */
#define GIC_GICD_TYPER          (GIC_GICD_BASE + 0x004)                 /* Interrupt Controller Type Register */
#define GIC_GICD_IIDR           (GIC_GICD_BASE + 0x008)                 /* Distributor Implementer Identification Register */
#define GIC_GICD_IGROUPR(n)     (GIC_GICD_BASE + 0x080 + ( (n) * 4 ) )  /* Interrupt Group Registers */
#define GIC_GICD_ISENABLER(n)   (GIC_GICD_BASE + 0x100 + ( (n) * 4 ) )  /* Interrupt Set-Enable Registers */
#define GIC_GICD_ICENABLER(n)   (GIC_GICD_BASE + 0x180 + ( (n) * 4 ) )  /* Interrupt Clear-Enable Registers */
#define GIC_GICD_ISPENDR(n)     (GIC_GICD_BASE + 0x200 + ( (n) * 4 ) )  /* Interrupt Set-Pending Registers */
#define GIC_GICD_ICPENDR(n)     (GIC_GICD_BASE + 0x280 + ( (n) * 4 ) )  /* Interrupt Clear-Pending Registers */
#define GIC_GICD_ISACTIVER(n)   (GIC_GICD_BASE + 0x300 + ( (n) * 4 ) )  /* Interrupt Set-Active Registers */
#define GIC_GICD_ICACTIVER(n)   (GIC_GICD_BASE + 0x380 + ( (n) * 4 ) )  /* Interrupt Clear-Active Registers */
#define GIC_GICD_IPRIORITYR(n)  (GIC_GICD_BASE + 0x400 + ( (n) * 4 ) )  /* Interrupt Priority Registers */
#define GIC_GICD_ITARGETSR(n)   (GIC_GICD_BASE + 0x800 + ( (n) * 4 ) )  /* Interrupt Processor Targets Registers */
#define GIC_GICD_ICFGR(n)       (GIC_GICD_BASE + 0xc00 + ( (n) * 4 ) )  /* Interrupt Configuration Registers */
#define GIC_GICD_NSCAR(n)       (GIC_GICD_BASE + 0xe00 + ( (n) * 4 ) )  /* Non-secure Access Control Registers */
#define GIC_GICD_SGIR           (GIC_GICD_BASE + 0xf00 )                /* Software Generated Interrupt Register */
#define GIC_GICD_CPENDSGIR(n)   (GIC_GICD_BASE + 0xf10 + ( (n) * 4 ) )  /* SGI Clear-Pending Registers */
#define GIC_GICD_SPENDSGIR(n)   (GIC_GICD_BASE + 0xf20 + ( (n) * 4 ) )  /* SGI Set-Pending Registers */

/* 8.9.4 GICD_CTLR, Distributor Control Register */
#define GIC_GICD_CTLR_ENABLE    (0x1)   /* Enable GICD */
#define GIC_GICD_CTLR_DISABLE   (0x0)   /* Disable GICD */

/* 8.9.7 GICD_ICFGR<n>, Interrupt Configuration Registers */
#define GIC_GICD_ICFGR_LEVEL    (0x0)   /* level-sensitive */
#define GIC_GICD_ICFGR_EDGE     (0x2)   /* edge-triggered */

/* Register access macros for GICC */
#define REG_GIC_GICC_CTLR          ((volatile uint32_t *)(uintptr_t)GIC_GICC_CTLR)
#define REG_GIC_GICC_PMR           ((volatile uint32_t *)(uintptr_t)GIC_GICC_PMR)
#define REG_GIC_GICC_BPR           ((volatile uint32_t *)(uintptr_t)GIC_GICC_BPR)
#define REG_GIC_GICC_IAR           ((volatile uint32_t *)(uintptr_t)GIC_GICC_IAR)
#define REG_GIC_GICC_EOIR          ((volatile uint32_t *)(uintptr_t)GIC_GICC_EOIR)
#define REG_GIC_GICC_RPR           ((volatile uint32_t *)(uintptr_t)GIC_GICC_RPR)
#define REG_GIC_GICC_HPIR          ((volatile uint32_t *)(uintptr_t)GIC_GICC_HPIR)
#define REG_GIC_GICC_ABPR          ((volatile uint32_t *)(uintptr_t)GIC_GICC_ABPR)
#define REG_GIC_GICC_IIDR          ((volatile uint32_t *)(uintptr_t)GIC_GICC_IIDR)

/* Register access macros for GICD */
#define REG_GIC_GICD_CTLR             ((volatile uint32_t *)(uintptr_t)GIC_GICD_CTLR)
#define REG_GIC_GICD_TYPE             ((volatile uint32_t *)(uintptr_t)GIC_GICD_TYPE)
#define REG_GIC_GICD_IIDR             ((volatile uint32_t *)(uintptr_t)GIC_GICD_IIDR)
#define REG_GIC_GICD_IGROUPR(n)       ((volatile uint32_t *)(uintptr_t)GIC_GICD_IGROUPR(n))
#define REG_GIC_GICD_ISENABLER(n)     ((volatile uint32_t *)(uintptr_t)GIC_GICD_ISENABLER(n))
#define REG_GIC_GICD_ICENABLER(n)     ((volatile uint32_t *)(uintptr_t)GIC_GICD_ICENABLER(n))
#define REG_GIC_GICD_ISPENDR(n)       ((volatile uint32_t *)(uintptr_t)GIC_GICD_ISPENDR(n))
#define REG_GIC_GICD_ICPENDR(n)       ((volatile uint32_t *)(uintptr_t)GIC_GICD_ICPENDR(n))
#define REG_GIC_GICD_ISACTIVER(n)     ((volatile uint32_t *)(uintptr_t)GIC_GICD_ISACTIVER(n))
#define REG_GIC_GICD_ICACTIVER(n)     ((volatile uint32_t *)(uintptr_t)GIC_GICD_ICACTIVER(n))
#define REG_GIC_GICD_IPRIORITYR(n)    ((volatile uint32_t *)(uintptr_t)GIC_GICD_IPRIORITYR(n))
#define REG_GIC_GICD_ITARGETSR(n)     ((volatile uint32_t *)(uintptr_t)GIC_GICD_ITARGETSR(n))
#define REG_GIC_GICD_ICFGR(n)         ((volatile uint32_t *)(uintptr_t)GIC_GICD_ICFGR(n))
#define REG_GIC_GICD_NSCAR(n)         ((volatile uint32_t *)(uintptr_t)GIC_GICD_NSCAR(n))
#define REG_GIC_GICD_SGIR             ((volatile uint32_t *)(uintptr_t)GIC_GICD_SGIR)
#define REG_GIC_GICD_CPENDSGIR(n)     ((volatile uint32_t *)(uintptr_t)GIC_GICD_CPENDSGIR(n))
#define REG_GIC_GICD_SPENDSGIR(n)     ((volatile uint32_t *)(uintptr_t)GIC_GICD_SPENDSGIR(n))

void gic_v3_initialize(void);
void gic_v3_eoi(irq_no irq);
int gic_v3_find_pending_irq(
    LkmcVectorExceptionFrame *exc __attribute__((unused)),
    irq_no *irqp
);
void gicd_disable_int(irq_no irq);
void gicd_enable_int(irq_no irq);
void gicd_clear_pending(irq_no irq);

/* Initialize GIC Controller */
static void init_gicc(void) {
    uint32_t pending_irq;

    /* Disable CPU interface */
    *REG_GIC_GICC_CTLR = GICC_CTLR_DISABLE;

    /* Set the priority level as the lowest priority.
     * Note: Higher priority corresponds to a lower Priority field value in the GIC_PMR.
     * In addition to this, writing 255 to the GICC_PMR always sets it to the
     * largest supported priority field value.
     */
    *REG_GIC_GICC_PMR = GICC_PMR_PRIO_MIN;

    /* Handle all of interrupts in a single group */
    *REG_GIC_GICC_BPR = GICC_BPR_NO_GROUP;

    /* Clear all of the active interrupts */
    for (
        pending_irq = (*REG_GIC_GICC_IAR & GICC_IAR_INTR_IDMASK);
        pending_irq != GICC_IAR_SPURIOUS_INTR;
        pending_irq = (*REG_GIC_GICC_IAR & GICC_IAR_INTR_IDMASK)
    )
        *REG_GIC_GICC_EOIR = *REG_GIC_GICC_IAR;

    /* Enable CPU interface */
    *REG_GIC_GICC_CTLR = GICC_CTLR_ENABLE;
}

static void init_gicd(void) {
    int32_t i, regs_nr;

    /* Diable distributor */
    *REG_GIC_GICD_CTLR = GIC_GICD_CTLR_DISABLE;

    /* Disable all IRQs */
    regs_nr = (GIC_INT_MAX + GIC_GICD_INT_PER_REG - 1) / GIC_GICD_INT_PER_REG;
    for (i = 0; regs_nr > i; ++i)
        *REG_GIC_GICD_ICENABLER(i) = ~((uint32_t)(0));

    /* Clear all pending IRQs */
    regs_nr = (GIC_INT_MAX + GIC_GICD_INT_PER_REG - 1) / GIC_GICD_INT_PER_REG;
    for (i = 0; regs_nr > i; ++i)
        *REG_GIC_GICD_ICPENDR(i) = ~((uint32_t)(0));

    /* Set all of interrupt priorities as the lowest priority */
    regs_nr = ( GIC_INT_MAX + GIC_GICD_IPRIORITY_PER_REG - 1) /
        GIC_GICD_IPRIORITY_PER_REG ;
    for (i = 0; regs_nr > i; i++)
        *REG_GIC_GICD_IPRIORITYR(i) = ~((uint32_t)(0));

    /* Set target of all of shared peripherals to processor 0 */
    for (i = GIC_INTNO_SPI0 / GIC_GICD_ITARGETSR_PER_REG;
         ( (GIC_INT_MAX + (GIC_GICD_ITARGETSR_PER_REG - 1) ) /
         GIC_GICD_ITARGETSR_PER_REG ) > i; ++i)
        *REG_GIC_GICD_ITARGETSR(i) =
            (uint32_t)GIC_GICD_ITARGETSR_CORE0_TARGET_BMAP;

    /* Set trigger type for all peripheral interrupts level triggered */
    for (i = GIC_INTNO_PPI0 / GIC_GICD_ICFGR_PER_REG;
         (GIC_INT_MAX + (GIC_GICD_ICFGR_PER_REG - 1)) / GIC_GICD_ICFGR_PER_REG > i; ++i)
        *REG_GIC_GICD_ICFGR(i) = GIC_GICD_ICFGR_LEVEL;

    /* Enable distributor */
    *REG_GIC_GICD_CTLR = GIC_GICD_CTLR_ENABLE;
}

/* Disable IRQ
 * @param[in] irq IRQ number
 */
void gicd_disable_int(irq_no irq) {
    *REG_GIC_GICD_ICENABLER( (irq / GIC_GICD_ICENABLER_PER_REG) ) =
        1U << ( irq % GIC_GICD_ICENABLER_PER_REG );
}

/* Enable IRQ
 * @param[in] irq IRQ number
 */
void gicd_enable_int(irq_no irq) {
    *REG_GIC_GICD_ISENABLER( (irq / GIC_GICD_ISENABLER_PER_REG) ) =
        1U << ( irq % GIC_GICD_ISENABLER_PER_REG );
}

/* Clear a pending interrupt
 * @param[in] irq IRQ number
 */
void gicd_clear_pending(irq_no irq) {
    *REG_GIC_GICD_ICPENDR( (irq / GIC_GICD_ICPENDR_PER_REG) ) =
        1U << ( irq % GIC_GICD_ICPENDR_PER_REG );
}


/* Probe pending interrupt
 * @param[in] irq IRQ number
 */
static int gicd_probe_pending(irq_no irq) {
    int is_pending;

    is_pending = ( *REG_GIC_GICD_ISPENDR( (irq / GIC_GICD_ISPENDR_PER_REG) ) &
        ( 1U << ( irq % GIC_GICD_ISPENDR_PER_REG ) ) );
    return ( is_pending != 0 );
}

/* Set an interrupt target processor
 * @param[in] irq IRQ number
 * @param[in] p   Target processor mask
 * 0x1 processor 0
 * 0x2 processor 1
 * 0x4 processor 2
 * 0x8 processor 3
 */
static void gicd_set_target(irq_no irq, uint32_t p){
    uint32_t  shift;
    uint32_t    reg;

    shift = (irq % GIC_GICD_ITARGETSR_PER_REG) * GIC_GICD_ITARGETSR_SIZE_PER_REG;
    reg = *REG_GIC_GICD_ITARGETSR(irq / GIC_GICD_ITARGETSR_PER_REG);
    reg &= ~( ((uint32_t)(0xff)) << shift);
    reg |= (p << shift);
    *REG_GIC_GICD_ITARGETSR(irq / GIC_GICD_ITARGETSR_PER_REG) = reg;
}

/* Set an interrupt priority
 * @param[in] irq  IRQ number
 * @param[in] prio Interrupt priority in Arm specific expression
 */
static void gicd_set_priority(irq_no irq, uint32_t prio){
    uint32_t  shift;
    uint32_t    reg;

    shift = (irq % GIC_GICD_IPRIORITY_PER_REG) * GIC_GICD_IPRIORITY_SIZE_PER_REG;
    reg = *REG_GIC_GICD_IPRIORITYR(irq / GIC_GICD_IPRIORITY_PER_REG);
    reg &= ~(((uint32_t)(0xff)) << shift);
    reg |= (prio << shift);
    *REG_GIC_GICD_IPRIORITYR(irq / GIC_GICD_IPRIORITY_PER_REG) = reg;
}

/* Configure IRQ
 * @param[in] irq     IRQ number
 * @param[in] config  Configuration value for GICD_ICFGR
 */
static void gicd_config(irq_no irq, unsigned int config) {
    uint32_t shift;
    uint32_t reg;

    shift = (irq % GIC_GICD_ICFGR_PER_REG) * GIC_GICD_ICFGR_SIZE_PER_REG; /* GICD_ICFGR has 16 fields, each field has 2bits. */
    reg = *REG_GIC_GICD_ICFGR( irq / GIC_GICD_ICFGR_PER_REG);
    reg &= ~( ( (uint32_t)(0x03) ) << shift );  /* Clear the field */
    reg |= ( ( (uint32_t)config ) << shift );  /* Set the value to the field correponding to irq */
    *REG_GIC_GICD_ICFGR( irq / GIC_GICD_ICFGR_PER_REG) = reg;
}

/* Send End of Interrupt to IRQ line for GIC
 * @param[in] ctrlr   IRQ controller information
 * @param[in] irq     IRQ number
 */
void gic_v3_eoi(irq_no irq) {
    gicd_clear_pending(irq);
}

/* Initialize GIC IRQ controller */
/* RyanYao: 2018/07/20
 * I supppose the current access is security, because GICD_CTLR.DS is 0b0 and
 * we can access.
 */
void gic_v3_initialize(void) {
    init_gicd();
    init_gicc();
    gicd_config(TIMER_IRQ, GIC_GICD_ICFGR_EDGE);
    gicd_set_priority(TIMER_IRQ, 0 << GIC_PRI_SHIFT );  /* Set priority */
    gicd_set_target(TIMER_IRQ, 0x1);  /* processor 0 */
    gicd_clear_pending(TIMER_IRQ);
    gicd_enable_int(TIMER_IRQ);
}


/* Find pending IRQ
 * @param[in]     exc  An exception frame
 * @param[in,out] irqp An IRQ number to be processed
 */
int gic_v3_find_pending_irq(
    LkmcVectorExceptionFrame *exc __attribute__((unused)),
    irq_no *irqp
) {
    int rc;
    irq_no i;
    for (i = 0; GIC_INT_MAX > i; ++i) {
        if (gicd_probe_pending(i)) {
            rc = 1;
            *irqp = i;
            goto found;
        }
    }
    rc = 0;
found:
    return rc;
}

#endif
