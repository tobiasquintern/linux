/*
 * Copyright 2010 TQ Systems GmbH. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/*!
 * @file i5ccdhb_config.c
 *
 * @brief iroq5 ccd headboard camera driver default configuration data
 *
 * @ingroup Camera
 */

#include <linux/types.h>
#include <linux/ctype.h>

const u8 afe_default_regs[] = {
	0x30, 0x39, 0x30, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0xd, 0xa,
	0x30, 0x39, 0x31, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x33, 0xd, 0xa,
	0x34, 0x38, 0x66, 0x9, 0x30, 0x30, 0x30, 0x32, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x38, 0x65, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x38, 0x64, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x38, 0x63, 0x9, 0x30, 0x30, 0x31, 0x30, 0x30, 0x34, 0x31, 0xd, 0xa,
	0x34, 0x39, 0x33, 0x9, 0x30, 0x34, 0x31, 0x61, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x39, 0x32, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x39, 0x31, 0x9, 0x30, 0x30, 0x30, 0x30, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x39, 0x30, 0x9, 0x30, 0x33, 0x66, 0x38, 0x30, 0x30, 0x66, 0xd, 0xa,
	0x34, 0x39, 0x37, 0x9, 0x31, 0x66, 0x66, 0x65, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x39, 0x36, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x34, 0x30, 0xd, 0xa,
	0x34, 0x39, 0x35, 0x9, 0x30, 0x30, 0x61, 0x61, 0x38, 0x39, 0x66, 0xd, 0xa,
	0x34, 0x39, 0x34, 0x9, 0x31, 0x66, 0x66, 0x65, 0x33, 0x30, 0x63, 0xd, 0xa,
	0x34, 0x35, 0x33, 0x9, 0x30, 0x30, 0x30, 0x31, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x35, 0x32, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x35, 0x31, 0x9, 0x30, 0x30, 0x30, 0x38, 0x30, 0x30, 0x34, 0xd, 0xa,
	0x34, 0x35, 0x30, 0x9, 0x30, 0x36, 0x31, 0x38, 0x30, 0x30, 0x33, 0xd, 0xa,
	0x34, 0x35, 0x37, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x35, 0x36, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0xd, 0xa,
	0x34, 0x35, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x35, 0x34, 0x9, 0x30, 0x30, 0x62, 0x65, 0x30, 0x32, 0x33, 0xd, 0xa,
	0x34, 0x35, 0x62, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x35, 0x61, 0x9, 0x30, 0x30, 0x64, 0x36, 0x30, 0x32, 0x33, 0xd, 0xa,
	0x34, 0x35, 0x39, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x35, 0x38, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x35, 0x66, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x35, 0x65, 0x9, 0x30, 0x30, 0x33, 0x65, 0x30, 0x30, 0x62, 0xd, 0xa,
	0x34, 0x35, 0x64, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x35, 0x63, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x36, 0x33, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x36, 0x32, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x36, 0x31, 0x9, 0x30, 0x30, 0x30, 0x30, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x36, 0x30, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x36, 0x37, 0x9, 0x30, 0x30, 0x30, 0x31, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x36, 0x36, 0x9, 0x30, 0x30, 0x38, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x36, 0x35, 0x9, 0x30, 0x30, 0x30, 0x38, 0x30, 0x30, 0x34, 0xd, 0xa,
	0x34, 0x36, 0x34, 0x9, 0x30, 0x36, 0x31, 0x38, 0x30, 0x30, 0x33, 0xd, 0xa,
	0x34, 0x36, 0x62, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x36, 0x61, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0xd, 0xa,
	0x34, 0x36, 0x39, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x36, 0x38, 0x9, 0x30, 0x34, 0x62, 0x61, 0x30, 0x32, 0x33, 0xd, 0xa,
	0x34, 0x36, 0x66, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x36, 0x65, 0x9, 0x30, 0x30, 0x64, 0x36, 0x30, 0x32, 0x33, 0xd, 0xa,
	0x34, 0x36, 0x64, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x36, 0x63, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x37, 0x33, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x37, 0x32, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x37, 0x31, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x37, 0x30, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x37, 0x37, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x37, 0x36, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x37, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x37, 0x34, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x37, 0x62, 0x9, 0x30, 0x30, 0x30, 0x31, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x37, 0x61, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x37, 0x39, 0x9, 0x30, 0x30, 0x30, 0x38, 0x30, 0x30, 0x34, 0xd, 0xa,
	0x34, 0x37, 0x38, 0x9, 0x30, 0x36, 0x31, 0x38, 0x30, 0x30, 0x33, 0xd, 0xa,
	0x34, 0x37, 0x66, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x37, 0x65, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0xd, 0xa,
	0x34, 0x37, 0x64, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x37, 0x63, 0x9, 0x30, 0x30, 0x62, 0x65, 0x30, 0x32, 0x33, 0xd, 0xa,
	0x34, 0x38, 0x33, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x38, 0x32, 0x9, 0x30, 0x30, 0x64, 0x36, 0x30, 0x32, 0x33, 0xd, 0xa,
	0x34, 0x38, 0x31, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x38, 0x30, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x38, 0x37, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x38, 0x36, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x38, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x38, 0x34, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x38, 0x62, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x38, 0x61, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x38, 0x39, 0x9, 0x30, 0x30, 0x30, 0x30, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x38, 0x38, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x34, 0x30, 0x33, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x30, 0x32, 0x9, 0x30, 0x30, 0x34, 0x38, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x30, 0x31, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x30, 0x30, 0x9, 0x30, 0x30, 0x36, 0x30, 0x30, 0x30, 0x63, 0xd, 0xa,
	0x34, 0x30, 0x37, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x30, 0x36, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x30, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x30, 0x34, 0x9, 0x30, 0x30, 0x37, 0x38, 0x30, 0x31, 0x38, 0xd, 0xa,
	0x34, 0x30, 0x62, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x30, 0x61, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x30, 0x39, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x30, 0x38, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x30, 0x66, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x30, 0x65, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x30, 0x64, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x30, 0x63, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x33, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x32, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x31, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x30, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x37, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x36, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x34, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x62, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x61, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x39, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x38, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x66, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x65, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x64, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x31, 0x63, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x33, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x32, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x31, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x30, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x37, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x36, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x34, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x62, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x61, 0x9, 0x30, 0x30, 0x34, 0x38, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x39, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x38, 0x9, 0x30, 0x30, 0x36, 0x30, 0x30, 0x30, 0x63, 0xd, 0xa,
	0x34, 0x32, 0x66, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x65, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x64, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x32, 0x63, 0x9, 0x30, 0x34, 0x61, 0x61, 0x30, 0x31, 0x38, 0xd, 0xa,
	0x34, 0x33, 0x33, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x32, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x31, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x30, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x37, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x36, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x34, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x62, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x61, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x39, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x38, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x66, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x65, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x64, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x33, 0x63, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x33, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x32, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x31, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x30, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x37, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x36, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x34, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x62, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x61, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x39, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x38, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x66, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x65, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x64, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x34, 0x34, 0x63, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x34, 0x30, 0x9, 0x31, 0x66, 0x66, 0x65, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x30, 0x34, 0x31, 0x9, 0x31, 0x66, 0x66, 0x65, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x30, 0x34, 0x32, 0x9, 0x31, 0x66, 0x66, 0x65, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x30, 0x34, 0x33, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x34, 0x34, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x34, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x35, 0x30, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x33, 0xd, 0xa,
	0x30, 0x35, 0x31, 0x9, 0x30, 0x34, 0x38, 0x63, 0x32, 0x30, 0x38, 0xd, 0xa,
	0x30, 0x35, 0x32, 0x9, 0x30, 0x34, 0x38, 0x63, 0x32, 0x30, 0x38, 0xd, 0xa,
	0x30, 0x35, 0x33, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x35, 0x34, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x35, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x35, 0x36, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x35, 0x37, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x35, 0x38, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x36, 0x36, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0xd, 0xa,
	0x30, 0x36, 0x37, 0x9, 0x30, 0x30, 0x62, 0x65, 0x30, 0x34, 0x38, 0xd, 0xa,
	0x30, 0x36, 0x38, 0x9, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x30, 0x36, 0x64, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0xd, 0xa,
	0x30, 0x36, 0x65, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x34, 0xd, 0xa,
	0x30, 0x36, 0x66, 0x9, 0x30, 0x30, 0x39, 0x34, 0x30, 0x32, 0x66, 0xd, 0xa,
	0x30, 0x37, 0x30, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x34, 0xd, 0xa,
	0x30, 0x37, 0x31, 0x9, 0x31, 0x32, 0x63, 0x30, 0x30, 0x33, 0x30, 0xd, 0xa,
	0x30, 0x37, 0x32, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0xd, 0xa,
	0x30, 0x37, 0x33, 0x9, 0x30, 0x30, 0x30, 0x30, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x30, 0x37, 0x34, 0x9, 0x33, 0x66, 0x66, 0x65, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x30, 0x37, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x30, 0x37, 0x36, 0x9, 0x33, 0x66, 0x66, 0x65, 0x66, 0x66, 0x66, 0xd, 0xa,
	0x30, 0x37, 0x37, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x37, 0xd, 0xa,
	0x30, 0x37, 0x38, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x37, 0x62, 0xd, 0xa,
	0x30, 0x37, 0x39, 0x9, 0x30, 0x36, 0x32, 0x61, 0x31, 0x63, 0x38, 0xd, 0xa,
	0x30, 0x37, 0x61, 0x9, 0x30, 0x30, 0x30, 0x30, 0x31, 0x63, 0x38, 0xd, 0xa,
	0x30, 0x37, 0x62, 0x9, 0x30, 0x33, 0x39, 0x30, 0x33, 0x31, 0x35, 0xd, 0xa,
	0x30, 0x37, 0x63, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x37, 0xd, 0xa,
	0x30, 0x37, 0x64, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x37, 0x62, 0xd, 0xa,
	0x30, 0x37, 0x65, 0x9, 0x30, 0x36, 0x32, 0x61, 0x31, 0x63, 0x38, 0xd, 0xa,
	0x30, 0x37, 0x66, 0x9, 0x30, 0x30, 0x30, 0x30, 0x31, 0x63, 0x38, 0xd, 0xa,
	0x30, 0x38, 0x30, 0x9, 0x30, 0x33, 0x39, 0x30, 0x33, 0x31, 0x35, 0xd, 0xa,
	0x30, 0x30, 0x34, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x33, 0xd, 0xa,
	0x30, 0x30, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x66, 0xd, 0xa,
	0x30, 0x30, 0x36, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x34, 0x30, 0xd, 0xa,
	0x30, 0x36, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x36, 0x39, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x36, 0x61, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x36, 0x62, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x36, 0x63, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x32, 0x30, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0xd, 0xa,
	0x30, 0x32, 0x31, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x32, 0x32, 0x9, 0x30, 0x30, 0x30, 0x63, 0x30, 0x34, 0x65, 0xd, 0xa,
	0x30, 0x33, 0x30, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x33, 0x31, 0x9, 0x30, 0x30, 0x31, 0x32, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x33, 0x32, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x33, 0x33, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x33, 0x34, 0x9, 0x30, 0x30, 0x31, 0x31, 0x32, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x33, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x38, 0xd, 0xa,
	0x30, 0x33, 0x36, 0x9, 0x30, 0x33, 0x30, 0x30, 0x30, 0x33, 0x33, 0xd, 0xa,
	0x30, 0x33, 0x37, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x30, 0xd, 0xa,
	0x30, 0x33, 0x38, 0x9, 0x30, 0x30, 0x30, 0x30, 0x34, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x30, 0x30, 0x9, 0x30, 0x30, 0x30, 0x66, 0x30, 0x30, 0x34, 0xd, 0xa,
	0x30, 0x31, 0x36, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0xd, 0xa,
	0x30, 0x31, 0x35, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0xd, 0xa,
	0x30, 0x36, 0x31, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x66, 0xd, 0xa,
	0x30, 0x36, 0x32, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x36, 0x33, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x36, 0x34, 0x9, 0x30, 0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x31, 0x31, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0xd, 0xa,
	0x30, 0x36, 0x30, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x30, 0x31, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x61, 0xd, 0xa,
	0x30, 0x65, 0x61, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x36, 0x30, 0xd, 0xa,
	0x30, 0x31, 0x32, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0xd, 0xa,
	0x30, 0x31, 0x33, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
	0x30, 0x31, 0x33, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0xd, 0xa,
	0x38, 0x38, 0x30, 0x9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xd, 0xa,
};

const size_t afe_default_regs_bytes = sizeof(afe_default_regs);

