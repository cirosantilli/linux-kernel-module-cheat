/* https://github.com/cirosantilli/linux-kernel-module-cheat#ring0 */

#include <linux/module.h>
#include <linux/kernel.h>

#include "ring0.h"

static int myinit(void)
{
#if defined(__x86_64__) || defined(__i386__)
	Ring0Regs ring0_regs;
	ring0_get_control_regs(&ring0_regs);
	pr_info("cr0 = 0x%8.8llX\n", (unsigned long long)ring0_regs.cr0);
	pr_info("cr2 = 0x%8.8llX\n", (unsigned long long)ring0_regs.cr2);
	pr_info("cr3 = 0x%8.8llX\n", (unsigned long long)ring0_regs.cr3);
#endif
    return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
