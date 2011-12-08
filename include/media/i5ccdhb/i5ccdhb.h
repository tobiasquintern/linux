/*
 * Copyright (C) 2010 TQ Systems GmbH 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef __I5CCDHB_H__
#define __I5CCDHB_H__

#include <linux/types.h>
#include <media/v4l2-subdev.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-device.h>

struct device;

/*!
 * platform data for the IROQ5 CCD headboard device
 */
struct soc_i5ccdhb_platform_data {
	/* struct soc_camera_link* camera_link; */
	struct device *dev; /* pointer to soc_camera_device */
	int i2c_adapter_id; /* i2c adapter for our i2c subdevices */
	u16 afe_spi_master_id; /* spi master for our spi subdevices */
	u16 afe_spi_chipselect; /* spi chipselect for afe subdevice */
	u32 afe_spi_maxspeed; /* spi speed for afe subdevice */

  u16 vdr_en_gpio; /* GPIO to set VDR_EN */
  
//  struct v4l2_subdev v4l2_subdev;
  
  

//  struct media_pad pad;  

  
  
/*
	void *device_data;
	int (*active) (int id);
	void (*inactive) (int id);
*/
};

struct i5fpga_platform_data {
	/* TODO: used to detect correct fpga */
	u32 id;
};

#endif /* __I5CCDHB_H__ */

