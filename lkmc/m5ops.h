/* https://github.com/cirosantilli/linux-kernel-module-cheat#m5ops-instructions */

#ifndef LKMC_M5OPS_H
#define LKMC_M5OPS_H

#if LKMC_M5OPS_ENABLE

#if defined(__arm__)

#define LKMC_M5OPS_CHECKPOINT_ASM mov r0, #0; mov r1, #0; mov r2, #0; mov r3, #0; .inst 0xEE000110 | (0x43 << 16)
#define LKMC_M5OPS_DUMPSTATS_ASM  mov r0, #0; mov r1, #0; mov r2, #0; mov r3, #0; .inst 0xEE000110 | (0x41 << 16)
#define LKMC_M5OPS_EXIT_ASM       mov r0, #0; mov r1, #0;                         .inst 0xEE000110 | (0x21 << 16)
#define LKMC_M5OPS_FAIL_1_ASM     mov r0, #0; mov r1, #0; mov r2, #1; mov r3, #0; .inst 0xEE000110 | (0x22 << 16)
#define LKMC_M5OPS_RESETSTATS_ASM mov r0, #0; mov r1, #0; mov r2, #0; mov r3, #0; .inst 0xEE000110 | (0x40 << 16)

#define LKMC_M5OPS_CHECKPOINT __asm__ __volatile__ ("mov r0, #0; mov r1, #0; mov r2, #0; mov r3, #0; .inst 0xEE000110 | (0x43 << 16);" : : : "r0", "r1", "r2", "r3")
#define LKMC_M5OPS_DUMPSTATS  __asm__ __volatile__ ("mov r0, #0; mov r1, #0; mov r2, #0; mov r3, #0; .inst 0xEE000110 | (0x41 << 16);" : : : "r0", "r1", "r2", "r3")
#define LKMC_M5OPS_EXIT       __asm__ __volatile__ ("mov r0, #0; mov r1, #0;                         .inst 0xEE000110 | (0x21 << 16);" : : : "r0", "r1"            )
#define LKMC_M5OPS_FAIL_1     __asm__ __volatile__ ("mov r0, #0; mov r1, #0; mov r2, #1; mov r3, #0; .inst 0xEE000110 | (0x22 << 16);" : : : "r0", "r1", "r2", "r3")
#define LKMC_M5OPS_RESETSTATS __asm__ __volatile__ ("mov r0, #0; mov r1, #0; mov r2, #0; mov r3, #0; .inst 0xEE000110 | (0x40 << 16);" : : : "r0", "r1", "r2", "r3")

#elif defined(__aarch64__)

#define LKMC_M5OPS_CHECKPOINT_ASM mov x0, #0; mov x1, #0; .inst 0xFF000110 | (0x43 << 16);
#define LKMC_M5OPS_DUMPSTATS_ASM  mov x0, #0; mov x1, #0; .inst 0xFF000110 | (0x41 << 16);
#define LKMC_M5OPS_EXIT_ASM       mov x0, #0;             .inst 0XFF000110 | (0x21 << 16);
#define LKMC_M5OPS_FAIL_1_ASM     mov x0, #0; mov x1, #1; .inst 0xFF000110 | (0x22 << 16);
#define LKMC_M5OPS_RESETSTATS_ASM mov x0, #0; mov x1, #0; .inst 0XFF000110 | (0x40 << 16);

#define LKMC_M5OPS_CHECKPOINT __asm__ __volatile__ ("mov x0, #0; mov x1, #0; .inst 0xFF000110 | (0x43 << 16);" : : : "x0", "x1")
#define LKMC_M5OPS_DUMPSTATS  __asm__ __volatile__ ("mov x0, #0; mov x1, #0; .inst 0xFF000110 | (0x41 << 16);" : : : "x0", "x1")
#define LKMC_M5OPS_EXIT       __asm__ __volatile__ ("mov x0, #0;             .inst 0XFF000110 | (0x21 << 16);" : : : "x0"      )
#define LKMC_M5OPS_FAIL_1     __asm__ __volatile__ ("mov x0, #0; mov x1, #1; .inst 0xFF000110 | (0x22 << 16);" : : : "x0", "x1")
#define LKMC_M5OPS_RESETSTATS __asm__ __volatile__ ("mov x0, #0; mov x1, #0; .inst 0XFF000110 | (0x40 << 16);" : : : "x0", "x1")

#else
#error m5ops not implemented for the current arch
#endif

#else

#define LKMC_M5OPS_CHECKPOINT_ASM
#define LKMC_M5OPS_DUMPSTATS_ASM
#define LKMC_M5OPS_EXIT_ASM
#define LKMC_M5OPS_FAIL_1_ASM
#define LKMC_M5OPS_RESETSTATS_ASM

#define LKMC_M5OPS_CHECKPOINT
#define LKMC_M5OPS_DUMPSTATS
#define LKMC_M5OPS_EXIT
#define LKMC_M5OPS_FAIL_1
#define LKMC_M5OPS_RESETSTATS

#endif

#endif
