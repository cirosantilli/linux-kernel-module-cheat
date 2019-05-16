/* https://github.com/cirosantilli/linux-kernel-module-cheat#ring0 */

#ifndef LKMC_RING0_H
#define LKMC_RING0_H
#if defined(__x86_64__) || defined(__i386__)
#ifdef THIS_MODULE
#include <linux/kernel.h>
#if defined(__x86_64__)
typedef u64 LkmcRing0RegsType;
#elif defined(__i386__)
typedef u32 LkmcRing0RegsType;
#endif
#else
#include <stdint.h>
#if defined(__x86_64__)
typedef uint64_t LkmcRing0RegsType;
#elif defined(__i386__)
typedef uint32_t LkmcRing0RegsType;
#endif
#endif

typedef struct {
    LkmcRing0RegsType cr0;
    LkmcRing0RegsType cr2;
    LkmcRing0RegsType cr3;
} LkmcRing0Regs;

void lkmc_ring0_get_control_regs(LkmcRing0Regs *ring0_regs) {
#if defined(__x86_64__)
    __asm__ __volatile__ (
        "mov %%cr0, %%rax;"
        "mov %%eax, %[cr0];"
        : [cr0] "=m" (ring0_regs->cr0)
        :
        : "rax"
    );
    __asm__ __volatile__ (
        "mov %%cr2, %%rax;"
        "mov %%eax, %[cr2];"
        : [cr2] "=m" (ring0_regs->cr2)
        :
        : "rax"
    );
    __asm__ __volatile__ (
        "mov %%cr3, %%rax;"
        "mov %%eax, %[cr3];"
        : [cr3] "=m" (ring0_regs->cr3)
        :
        : "rax"
    );
#elif defined(__i386__)
    __asm__ __volatile__ (
        "mov %%cr0, %%eax;"
        "mov %%eax, %[cr0];"
        : [cr0] "=m" (ring0_regs->cr0)
        :
        : "eax"
    );
    __asm__ __volatile__ (
        "mov %%cr2, %%eax;"
        "mov %%eax, %[cr2];"
        : [cr2] "=m" (ring0_regs->cr2)
        :
        : "eax"
    );
    __asm__ __volatile__ (
        "mov %%cr3, %%eax;"
        "mov %%eax, %[cr3];"
        : [cr3] "=m" (ring0_regs->cr3)
        :
        : "eax"
    );
#endif
}
#endif
#endif
