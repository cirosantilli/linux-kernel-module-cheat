/*
This illustrates operations which are only possible in ring 0.
https://stackoverflow.com/questions/7415515/how-to-access-the-control-registers-cr0-cr2-cr3-from-a-program-getting-segmenta/7419306#7419306

It only works for x86_64.

Then try to run this on userland and see the process be killed:

    /ring0.out
*/

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
