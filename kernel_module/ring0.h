#if defined(__x86_64__) || defined(__i386__)

#ifdef THIS_MODULE
#include <linux/kernel.h>
#if defined(__x86_64__)
typedef u64 T;
#elif defined(__i386__)
typedef u32 T;
#endif
#else
#include <stdint.h>
#if defined(__x86_64__)
typedef uint64_t T;
#elif defined(__i386__)
typedef uint32_t T;
#endif
#endif

typedef struct {
    T cr0;
    T cr2;
    T cr3;
} Ring0Regs;

void ring0_get_control_regs(Ring0Regs *ring0_regs)
{
#if defined(__x86_64__)
    __asm__ __volatile__ (
        "mov %%cr0, %%rax;"
        "mov %%eax, %0;"
        "mov %%cr2, %%rax;"
        "mov %%eax, %1;"
        "mov %%cr3, %%rax;"
        "mov %%eax, %2;"
        : "=m" (ring0_regs->cr0),
          "=m" (ring0_regs->cr2),
          "=m" (ring0_regs->cr3)
        :
        : "%rax"
    );
#elif defined(__i386__)
    __asm__ __volatile__ (
        "mov %%cr0, %%eax;"
        "mov %%eax, %0;"
        "mov %%cr2, %%eax;"
        "mov %%eax, %1;"
        "mov %%cr3, %%eax;"
        "mov %%eax, %2;"
        : "=m" (ring0_regs->cr0),
          "=m" (ring0_regs->cr2),
          "=m" (ring0_regs->cr3)
        :
        : "%eax"
    );
#endif
}

#endif
