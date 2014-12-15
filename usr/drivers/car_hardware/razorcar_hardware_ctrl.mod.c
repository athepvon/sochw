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
	{ 0x207d03f8, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xadf42bd5, "__request_region" },
	{ 0x15692c87, "param_ops_int" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x788fe103, "iomem_resource" },
	{ 0x61d9cc82, "dev_set_drvdata" },
	{ 0xf087137d, "__dynamic_pr_debug" },
	{ 0xe9a93013, "__register_chrdev" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0xac3aab9f, "mutex_unlock" },
	{ 0xe707d823, "__aeabi_uidiv" },
	{ 0x2f2a02e5, "dev_err" },
	{ 0xf0c71d03, "mutex_lock_interruptible" },
	{ 0x8ecee198, "__mutex_init" },
	{ 0x27e1a049, "printk" },
	{ 0x78aa795, "platform_get_resource" },
	{ 0x868d84ea, "platform_driver_register" },
	{ 0xc2165d85, "__arm_iounmap" },
	{ 0x9bce482f, "__release_region" },
	{ 0xfb80f136, "remap_pfn_range" },
	{ 0x40a6f522, "__arm_ioremap" },
	{ 0x15ab4431, "platform_driver_unregister" },
	{ 0xb0d0edea, "dev_get_drvdata" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Cxlnx,razorcar-hardware-1.01.a*");
