/*
Adapted from: https://github.com/jethrogb/ptdump

TODO: use kernel constants instead of hard-coding everything to make this
more platform independent.
*/

#error

#include <asm/io.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define pte_address(p) ((p)&0x000ffffffffff000UL)
typedef unsigned long long ullong;

static void output(void* p, void* data, ullong len) {
	seq_write((struct seq_file *)p, data, len);
}

static ullong * read_page(void* p, ullong address) {
	return (ullong*)phys_to_virt(address);
}

static ullong* ptdump_dump(void *p, ullong address) {
	ullong* page = read_page(p, address);
	if (page) {
		output(p, &address, 8);
		output(p, page, 4096);
	}
	return page;
}

static void ptdump_page(void *p, ullong address, int levels_remaining);

static void ptdump_recurse(void *p, ullong* page, int levels_remaining) {
	int i;
	for (i = 0; i < 512; i++) {
		if ((page[i] & 1) && !(page[i] & (1 << 7)))
			ptdump_page(p, pte_address(page[i]), levels_remaining - 1);
	}
}

static void ptdump_page(void *p, ullong address, int levels_remaining) {
	ullong* page = ptdump_dump(p, address);
	if (!page) return;
	if (levels_remaining > 0)
		ptdump_recurse(p, page, levels_remaining);
}

static ullong get_base(void* p) {
	return read_cr3();
}

static void ptdump(void *p, int levels) {
	ptdump_page(p, get_base(p), levels);
}

static int do_ptdump(struct seq_file *m, void* v) {
	ptdump(m, (int)(unsigned long)(m->private));
	return 0;
}

static int ptdump_open(struct inode *inode, struct file *fp) {
	return single_open(fp, do_ptdump, PDE_DATA(inode));
}

static const struct file_operations ptdump_fops = {
	.open    = ptdump_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

static struct proc_dir_entry *proc_page_table0;
static struct proc_dir_entry *proc_page_table1;
static struct proc_dir_entry *proc_page_table2;
static struct proc_dir_entry *proc_page_table3;

static int __init ptdump_module_init(void) {
	proc_page_table0 = proc_create_data("page_table_0", 0444, NULL, &ptdump_fops,(void*)0);
	proc_page_table1 = proc_create_data("page_table_1", 0444, NULL, &ptdump_fops,(void*)1);
	proc_page_table2 = proc_create_data("page_table_2", 0444, NULL, &ptdump_fops,(void*)2);
	proc_page_table3 = proc_create_data("page_table_3", 0444, NULL, &ptdump_fops,(void*)3);
	return 0;
}

static void __exit ptdump_module_exit(void) {
	proc_remove(proc_page_table0);
	proc_remove(proc_page_table1);
	proc_remove(proc_page_table2);
	proc_remove(proc_page_table3);
}

module_init(ptdump_module_init);
module_exit(ptdump_module_exit);
MODULE_LICENSE("GPL");
