#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x14522340, "module_layout" },
	{ 0x42e80c19, "cdev_del" },
	{ 0xc45a9f63, "cdev_init" },
	{ 0x5a5e7ea3, "simple_read_from_buffer" },
	{ 0x3eba92, "debugfs_create_dir" },
	{ 0x2bd43d13, "dynamic_debug_enabled2" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0xced11b05, "debugfs_create_file" },
	{ 0xf0909ad6, "debugfs_remove_recursive" },
	{ 0x3acac6d7, "misc_register" },
	{ 0xea147363, "printk" },
	{ 0x85f8a266, "copy_to_user" },
	{ 0xb4390f9a, "mcount" },
	{ 0xa6d1bdca, "cdev_add" },
	{ 0xead58fb9, "print_hex_dump" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x9cb480f4, "dynamic_debug_enabled" },
	{ 0x14bb2460, "debugfs_create_u16" },
	{ 0x3302b500, "copy_from_user" },
	{ 0x16c0b339, "misc_deregister" },
	{ 0x29537c9e, "alloc_chrdev_region" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "3B85A64BD7736C8717F1738");

static const struct rheldata _rheldata __used
__attribute__((section(".rheldata"))) = {
	.rhel_major = 6,
	.rhel_minor = 7,
	.rhel_release = 572,
};
