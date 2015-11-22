#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/atomic.h>
#include <linux/bitops.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

char hw4_buffer[128];
dev_t misc_dev_one;
dev_t misc_dev_two;

const struct file_operations fops_one = {
	.owner = THIS_MODULE,
	.read = my_read,
};

const struct file_operations fops_two = {
	.owner = THIS_MODULE,
	.write = my_write,
};

static int my_init(void)
{
	
}

static void my_exit(void)
{
	
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
