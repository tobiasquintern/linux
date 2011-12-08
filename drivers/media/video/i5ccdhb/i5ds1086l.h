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

#ifndef __I5DS1086L_H__
#define __I5DS1086L_H__


#include <linux/types.h>
#include <linux/i2c.h>

/* struct i5ds1086l_priv_data; */

extern int i5ds1086l_set_address(struct i2c_client *, u8 address);
extern int i5ds1086l_set_write_control(struct i2c_client *, int enabled);
extern int get_write_control(struct i2c_client *);
extern int i5ds1086l_set_frequency(struct i2c_client *, u32 frequency);
extern s32 i5ds1086l_get_frequency(struct i2c_client *);
extern int ds1086l_write_eeprom(struct i2c_client *);


#endif /* __I5DS1086L_H__ */

