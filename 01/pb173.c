#include <linux/module.h>
#include <linux/slab.h>
#include "helpthings.h"


static int my_init(void)
{
	pr_info("HELLO WORLD\n");
	print_things();
	return 0;
}

static void my_exit(void)
{
	void *mem = kmalloc(1000, GFP_KERNEL);

	if (mem) {
		strcpy(mem, "Bye");
		pr_info("%s", (char *)mem);
	}
	kfree(mem);
}


module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");

