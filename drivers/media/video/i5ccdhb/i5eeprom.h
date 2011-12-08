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
 * @file i5eeprom.h
 *
 * @brief iroq5 eeprom data for IROQ5 CCD headboard
 *
 * @ingroup Camera
 */

#ifndef __I5EEPROM_H__
#define __I5EEPROM_H__

#include <linux/types.h>

struct i5ccdhb_eeprom_data {
  char tag[4];     /* always "CCD\0" */
  char version[3]; /* struct version "00 ... 99" */
  char serial[12]; /* serial number string */
  char sensor[12]; /* sensor type string */
  char afe[12];    /* AFE type string */
};

#define I5CCDHB_AT24_TAG_DEFAULT "CCD"
#define I5CCDHB_AT24_VERSION_0 "00"
#define I5CCDHB_AT24_SENSOR_ICX424AL "ICX424AL"
#define I5CCDHB_AT24_AFE_AD9923A "AD9923A"

#endif

