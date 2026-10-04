/*
 * Clear the factory-cable flag an engineering bootloader leaves in SMEM
 * VENDOR1 before the modem reads it.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/init.h>
#include <linux/printk.h>
#include <linux/err.h>
#include <soc/qcom/smem.h>

/*
 * SMEM_ID_VENDOR1 is a 16-byte block the bootloader fills in for the LGE
 * modem firmware (lge_hw_smem_id1_type). The first word is the SBL cable
 * type; the last word is the factory-cable/QEM flag that the modem's
 * lge_is_factory_cable() tests on every data call. A stock bootloader
 * writes 0 there. The DirtySanta US996 engineering bootloader leaves it
 * at 0xffffffff, so the modem believes a factory cable is attached and
 * rejects every AP data call (QMI WDS START_NETWORK_INTERFACE fails with
 * QMI_ERR_UNKNOWN; the F3 log says "AP data call block under factory
 * cable connection"). LTE attach, IMS and ATCoP calls bypass that check,
 * which is why registration and VoLTE look fine while mobile data never
 * comes up.
 */
#define VENDOR1_FACTORY_CABLE_OFFSET	12
#define VENDOR1_MIN_SIZE		16

static int __init dirtysanta_fixup_vendor1(void)
{
	u32 *ptr;
	unsigned size = 0;

	ptr = smem_get_entry(SMEM_ID_VENDOR1, &size, 0, SMEM_ANY_HOST_FLAG);
	if (IS_ERR_OR_NULL(ptr)) {
		pr_info("DirtySanta: VENDOR1 not present in smem as of subsys_init\n");
		return -EFAULT;
	}

	if (size < VENDOR1_MIN_SIZE) {
		pr_err("DirtySanta: VENDOR1 area too small (%u)\n", size);
		return -ENOMEM;
	}

	ptr += VENDOR1_FACTORY_CABLE_OFFSET / sizeof(*ptr);

	if (*ptr == 0) {
		pr_info("DirtySanta: VENDOR1 factory-cable flag already clear\n");
		return 0;
	}

	pr_info("DirtySanta: clearing VENDOR1 factory-cable flag 0x%08x\n", *ptr);
	*ptr = 0;

	return 0;
}

/* smem is initialized at arch_initcall(); the modem is loaded much later */
subsys_initcall(dirtysanta_fixup_vendor1);
