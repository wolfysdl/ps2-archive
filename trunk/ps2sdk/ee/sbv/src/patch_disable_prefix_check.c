/* patch_disable_prefix_check.c - Disable the MODLOAD check for .elf's or .irx's
 *                                on certain devices.
 *
 * Copyright (c) 2003  Marcus R. Brown <mrbrown@0xd6.org>
 *
 * This code is licensed under the Academic Free License v2.0.
 * See the file LICENSE included with this distribution for licensing terms.
 */

#include "tamtypes.h"

#include "smem.h"
#include "slib.h"

int sbv_patch_disable_prefix_check()
{
	u8 buf[512];
	u32 patch[2];
	slib_exp_lib_t *modload_lib = (slib_exp_lib_t *)buf;

	patch[0] = 0x03e00008;	/* jr $ra */
	patch[1] = 0x00001021;	/* addiu $v0, $0, 0 */

	if (!slib_get_exp_lib("modload", modload_lib))
		return -1;

	smem_write(modload_lib->exports[15], patch, sizeof patch);
	return 0;
}
