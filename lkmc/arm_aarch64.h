#ifndef LKMC_ARM_AARCH64_H
#define LKMC_ARM_AARCH64_H

/* Stuff that is common between arm and aarch64. */
#define lkmc_arm_aarch64_wfe() __asm__ __volatile__ ("wfe" : : : )
#define lkmc_arm_aarch64_dmb(type) __asm__ __volatile__ ("dmb " #type : : : "memory")
#define lkmc_arm_aarch64_sev(immediate) __asm__ __volatile__("sev" : : : )
#define lkmc_arm_aarch64_hvc(immediate) __asm__ __volatile__("hvc " #immediate : : : )
#define lkmc_arm_aarch64_svc(immediate) __asm__ __volatile__("svc " #immediate : : : )
#define lkmc_arm_aarch64_wfi() __asm__ __volatile__ ("wfi" : : : )

#endif
