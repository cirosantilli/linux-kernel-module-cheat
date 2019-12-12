/* https://cirosantilli.com/linux-kernel-module-cheat#dump-regs */

#include <linux/module.h>
#include <linux/kernel.h>

#define LKMC_DUMP_SYSTEM_REGS_PRINTF pr_info
#if defined(__aarch64__)
#include <lkmc/aarch64_dump_regs.h>
#else
#define LKMC_DO_NOTHING
#endif

static int myinit(void)
{
#if !defined(LKMC_DO_NOTHING)
    LKMC_DUMP_SYSTEM_REGS;
#endif
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
