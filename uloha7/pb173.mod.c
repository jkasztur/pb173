#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
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
	{ 0x151b2616, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x56199cbf, __VMLINUX_SYMBOL_STR(pci_unregister_driver) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x3f31c15, __VMLINUX_SYMBOL_STR(pci_dev_put) },
	{ 0x408e903b, __VMLINUX_SYMBOL_STR(__pci_register_driver) },
	{ 0xc65f199a, __VMLINUX_SYMBOL_STR(pci_dev_get) },
	{ 0xdd626e59, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xbe84a3b9, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbb5866a4, __VMLINUX_SYMBOL_STR(pci_get_device) },
	{ 0xa5e4fa, __VMLINUX_SYMBOL_STR(pci_disable_device) },
	{ 0xe1d8f494, __VMLINUX_SYMBOL_STR(pci_release_region) },
	{ 0x37141b3b, __VMLINUX_SYMBOL_STR(pci_request_region) },
	{ 0x6d35ca54, __VMLINUX_SYMBOL_STR(pci_enable_device) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v00008086d00009CA0sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "2917562F6F86C4F4C43ABF6");
