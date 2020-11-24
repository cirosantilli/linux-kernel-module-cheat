/* https://cirosantilli.com/linux-kernel-module-cheat#vermagic */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/utsname.h>
#if 0
#include <linux/vermagic.h> /* VERMAGIC_STRING */
#endif

static int myinit(void)
{
    struct new_utsname *uts = init_utsname();
	pr_info(
        "sysname    = %s\n"
        "nodename   = %s\n"
        "release    = %s\n"
        "version    = %s\n"
        "machine    = %s\n"
        "domainname = %s\n",
        uts->sysname,
        uts->nodename,
        uts->release,
        uts->version,
        uts->machine,
        uts->domainname
    );

#if 0
    /* Possible before v5.8, but was buggy apparently, not sure why:
     * https://github.com/cirosantilli/linux/commit/51161bfc66a68d21f13d15a689b3ea7980457790 */
	pr_info("VERMAGIC_STRING = " VERMAGIC_STRING "\n");
#endif
	/* Nice try, but it is not a member. */
	/*pr_info("THIS_MODULE->vermagic = %s\n", THIS_MODULE->vermagic);*/
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
