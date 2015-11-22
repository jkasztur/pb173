#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/atomic.h>
#include <linux/bitops.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

//void *my_bitmap;
static int my_init(void)
{
	
	int num = -3;
	
	//DECLARE_BITMAP(my_bitmap,100);
	DEFINE_SPINLOCK(my_num_lock);
	
	spin_lock(&my_num_lock);
				//atomic_set(&num, -3);
	num++;			//atomic_inc(&num);
	pr_info("%d\n",num);	//pr_info("%d\n",atomic_read(&num));
	num = num + 3;		//atomic_add(3,&num);	
	num--;			//atomic_dec(&num);
	pr_info("%d\n",num);	//pr_info("%d\n",atomic_read(&num));
	spin_unlock(&my_num_lock);
	
	/*
	bitmap_zero(my_bitmap,100);
	bitmap_set(my_bitmap,2,1);
	bitmap_set(my_bitmap,63,1);
	bitmap_set(my_bitmap,76,1);
	
	pr_info("%lx\n",my_bitmap[BIT_WORD(63)]);
	bitmap_scnprintf(,100,my_bitmap,100)*/

	return num;	
}

static void my_exit(void)
{
	
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
