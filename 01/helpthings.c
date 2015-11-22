#include <linux/module.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/device.h>

void pointless_function(void)
{
	int num = 2;

	num++;
}

void print_things(void)
{
	void *tmp = kmalloc(1000, GFP_KERNEL);
	int num = 5;

	if (tmp)
		pr_info("Malloc variable address: %p\n", &tmp);
	kfree(tmp);
	pr_info("Stack variable address: %p\n", &num);
	pr_info("Jiffies address: %p\n", &jiffies);
	pr_info("My function address: %p\n", pointless_function);
	pr_info("Other function address: %p\n", bus_register);
	pr_info("__builtin_return_address(0): %pF\n",
		__builtin_return_address(0));
}

