#include <linux/module.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/kfifo.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/gfp.h>
#include <linux/mm.h>

void *temp;
void *temp_page;
void *temp_one;
void *temp_two;
void *temp_three;
void *temp_four;

int my_mmap(struct file *filp, struct vm_area_struct *vma )
{
	pr_info("mymmap starting");
	if((0 <= vma->vm_pgoff) && (vma->vm_pgoff < 1))
	{
		if((vma->vm_flags & ( VM_WRITE | VM_READ ) ) != (VM_READ ))
			return -EINVAL;
		return remap_vmalloc_range(vma, temp_one, 0);	
	}
	if((1 <= vma->vm_pgoff) && (vma->vm_pgoff < 2))
	{
		if((vma->vm_flags & ( VM_WRITE | VM_READ ) ) != (VM_READ ))
			return -EINVAL;
		return remap_vmalloc_range(vma, temp_two, 0);		
	}
	if((2 <= vma->vm_pgoff) && (vma->vm_pgoff < 3))
	{
		if((vma->vm_flags & ( VM_WRITE & VM_READ) ) != (VM_WRITE & VM_READ))
			return -EINVAL;
		return remap_pfn_range(vma, vma->vm_start, page_to_pfn(virt_to_page(temp_three)),
			PAGE_SIZE, vma->vm_page_prot);
	}
	if((3 <= vma->vm_pgoff) && (vma->vm_pgoff < 4))
	{
		if((vma->vm_flags & ( VM_WRITE & VM_READ) ) != (VM_WRITE & VM_READ))
			return -EINVAL;
		return remap_pfn_range(vma, vma->vm_start + PAGE_SIZE,
			page_to_pfn(virt_to_page(temp_four)), PAGE_SIZE, vma->vm_page_prot);
	}
	else
		return -EFAULT;
	pr_info("mymmap finishing");
	return 0;



/*
	vma->private_data = (struct page*)[4];
	if((0 <= vma->vm_pgoff) && (vma->vm_pgoff < 1))
	{
		if((vma->vm_flags & ( VM_WRITE | VM_READ ) ) != (VM_READ ))
			return -EINVAL;
		vma->vm_private_data = temp_one;		
	}

	if((1 <= vma->vm_pgoff) && (vma->vm_pgoff < 2))
	{
		if((vma->vm_flags & ( VM_WRITE | VM_READ ) ) != (VM_READ ))
			return -EINVAL;
		vma->vm_private_data = temp_two;		
	}
 	if((2 <= vma->vm_pgoff) && (vma->vm_pgoff < 3))
	{
		if((vma->vm_flags & ( VM_WRITE & VM_READ) ) != (VM_WRITE & VM_READ))
			return -EINVAL;
		vma->vm_private_data = temp_three;
	}
 	if((3 <= vma->vm_pgoff) && (vma->vm_pgoff < 4))
	{
		if((vma->vm_flags & ( VM_WRITE & VM_READ) ) != (VM_WRITE & VM_READ))
			return -EINVAL;
		vma->vm_private_data = temp_four;
	}
	else
		return -EFAULT;
	return 0;
*/

}
/*
int my_open(struct inode* node, struct file* file)
{
	if((file = open("/home/vyvoj/repos/pb173/09/task.bin")) < 0)
		return -EIO;
	return 0;
}

int my_release(struct inode* node, struct file* file)
{
	
}*/
const struct file_operations hw6_fops = {
	.owner = THIS_MODULE,
	.mmap = my_mmap,
};

struct miscdevice my_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "test",
	.fops = &hw6_fops,
};

static int my_init(void)
{
//	temp = vmalloc_user(PAGE_SIZE * 2);
//	temp_page = (void*) __get_free_pages(GFP_KERNEL, 1);

	temp_one = vmalloc_user(PAGE_SIZE);
	temp_two = vmalloc_user(PAGE_SIZE);
	temp_three = (void*) __get_free_page(GFP_KERNEL);
	temp_four = (void*) __get_free_page(GFP_KERNEL);
//	strcpy(temp, "page12");
//	strcpy(temp + PAGE_SIZE, "page2");
//	strcpy(temp_page, "page34");
//	strcpy(temp_page + PAGE_SIZE, "page4");
	strcpy(temp_one, "page1");
	strcpy(temp_two, "page2");
	strcpy(temp_three, "page3");
	strcpy(temp_four, "page4");

	pr_info("Adding module\n");
//	kfifo_alloc(&my_fifo, 8 * 1024, GFP_KERNEL);
	misc_register(&my_misc_device);
	return 0;
}

static void my_exit(void)
{
//	kfifo_free(&my_fifo);
	vfree(temp_one);
	vfree(temp_two);
	//free_pages((long unsigned int) temp_three, 0);
	//free_pages((long unsigned int) temp_four, 0);
	misc_deregister(&my_misc_device);
	pr_info("Removing module\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
