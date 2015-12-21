#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/gfp.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/mm.h>
void *temp1;
void *temp2;
void *temp3;
void *temp4;

int my_mmap(struct file *file, struct vm_area_struct *vma)
{
	pr_info("my_mmap starting");
	if((0 <= vma->vm_pgoff) && (vma->vm_pgoff < 1))
	{
		if((vma->vm_flags & (VM_WRITE | VM_READ)) != VM_READ)
		{
			pr_info("Bad flags, my comment");
			return -EINVAL;
		}
		return remap_vmalloc_range(vma, temp1, 0);
	}
	if((1 <= vma->vm_pgoff) && (vma->vm_pgoff < 2))
	{
		if((vma->vm_flags & (VM_WRITE | VM_READ)) != VM_READ)
		{
			pr_info("Bad flags, my comment");
			return -EINVAL;
		}
		return remap_vmalloc_range(vma, temp2, 0);
	}
	if((2 <= vma->vm_pgoff) && (vma->vm_pgoff < 3))
	{
		if((vma->vm_flags & (VM_READ | VM_WRITE)) != (VM_READ | VM_WRITE))
		{
			pr_info("not read and write, my_com");
			return -EINVAL;		
		}
		return remap_pfn_range(vma, vma->vm_start, page_to_pfn(virt_to_page(temp3)),
			PAGE_SIZE, vma->vm_page_prot);
	}
	if((3 <= vma->vm_pgoff) && (vma->vm_pgoff < 4))
	{
		if((vma->vm_flags & (VM_READ | VM_WRITE)) != (VM_READ | VM_WRITE))
		{
			pr_info("not read and write, my_com");
			return -EINVAL;		
		}
		return remap_pfn_range(vma, vma->vm_start + PAGE_SIZE,
			page_to_pfn(virt_to_page(temp4)),
			PAGE_SIZE, vma->vm_page_prot);
	}
	else
	{
		pr_info("out of range, mycom");
		return -EFAULT;

	}
	pr_info("mymap finished successfully");
	return 0;
}

const struct file_operations hw9_fops = {
	.owner = THIS_MODULE,
	.mmap = my_mmap,
};

struct miscdevice my_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "MYDEVICE",
	.fops = &hw9_fops,
};

static int my_init(void)
{
	pr_info("Adding module.");
	temp1 = vmalloc_user(PAGE_SIZE);
	temp2 = vmalloc_user(PAGE_SIZE);
	temp3 = (void*)__get_free_page(GFP_KERNEL);
	temp4 = (void*)__get_free_page(GFP_KERNEL);
	strcpy(temp1, "aaa");
	strcpy(temp2, "bbb");
	strcpy(temp3, "ccc");
	strcpy(temp4, "ddd");
	
	misc_register(&my_device);	
	return 0;
}

static void my_exit(void)
{
	vfree(temp1);
	vfree(temp2);
	free_page((unsigned long)temp3);
	free_page((unsigned long)temp4);
	
	misc_deregister(&my_device);
	pr_info("Removing module.");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
