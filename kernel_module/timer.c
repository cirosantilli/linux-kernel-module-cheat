/*
Timers are callbacks that run when an interrupt happens, from the interrupt context itself.

Therefore they produce more accurate timing than thread scheduling, which is more complex,
but you can't do too much work inside of them.

See also:

- http://stackoverflow.com/questions/10812858/timers-in-linux-device-drivers
- https://gist.github.com/yagihiro/310149
*/

#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");

static void callback(unsigned long data);
static unsigned long onesec;

DEFINE_TIMER(mytimer, callback, 0, 0);

static void callback(unsigned long data)
{
	pr_info("%u\n", (unsigned)jiffies);
	mod_timer(&mytimer, jiffies + onesec);
}

int init_module(void)
{
	onesec = msecs_to_jiffies(1000);
	mod_timer(&mytimer, jiffies + onesec);
	return 0;
}

void cleanup_module(void)
{
	del_timer(&mytimer);
}
