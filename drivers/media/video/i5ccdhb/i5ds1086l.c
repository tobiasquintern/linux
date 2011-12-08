/*
 * LM73 Sensor driver
 * Based on LM75
 *
 * Copyright (C) 2007, CenoSYS (www.cenosys.com).
 * Copyright (C) 2009, Bollore telecom (www.bolloretelecom.eu).
 *
 * Guillaume Ligneul <guillaume.ligneul@gmail.com>
 * Adrien Demarez <adrien.demarez@bolloretelecom.eu>
 * Jeremy Laine <jeremy.laine@bolloretelecom.eu>
 *
 * This software program is licensed subject to the GNU General Public License
 * (GPL).Version 2,June 1991, available at
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/err.h>

#include "i5ds1086l.h"
/* DS1086L registers */
/*
#define DS1086L_DEVICE_BASEADDR   ((u8)0xB0)
#define DS1086L_V_CFGADDR_MIN     ((u8)0x00)
#define DS1086L_V_CFGADDR_MAX     ((u8)0x07)
#define DS1086L_M_CFGADDR         ((u8)0x0E)
#define DS1086L_S_CFGADDR         ((u8)0x01)
*/

#define DS1086L_REG_PRESCALER     ((u8)0x02)
#define DS1086L_REG_DAC           ((u8)0x08)
#define DS1086L_REG_OFFSET        ((u8)0x0E)
#define DS1086L_REG_ADDR          ((u8)0x0D)
#define DS1086L_REG_RANGE         ((u8)0x37)
#define DS1086L_REG_WRITE_EE      ((u8)0x3F)

#define DS1086L_PRESCALER_M_JRATE   ((u16)0xC000)
#define DS1086L_PRESCALER_M_JPCTG   ((u16)0x3800)
#define DS1086L_PRESCALER_M_LOHIZ   ((u16)0x0400)

#define DS1086L_PRESCALER_M_DIVIDER     ((u16)0x03C0)
#define DS1086L_PRESCALER_S_DIVIDER     ((u16)6)
#define DS1086L_PRESCALER_V_DIVIDER_MAX 8

#define DS1086L_DAC_M_DAC             ((u16)0xFFC0)
#define DS1086L_DAC_S_DAC             ((u16)6)

#define DS1086L_OFFSET_M_OFFSET       ((u8)0x1F)

#define DS1086L_RANGE_M_RANGE         ((u8)0x1F)

#define DS1086L_ADDR_M_WC             ((u8)0x08)
#define DS1086L_ADDR_M_A              ((u8)0x07)

#define DS1086L_WRITE_EE_V_WRITE      ((u8)0xFF)

#define DS1086L_OSC_RANGES            13



/*! \brief oscilator range definition */
struct ds1086l_osc_range {
	u32 min; /*!< lower limit */
	u32 max; /*!< upper limit */
	u32 os; 
};

static const struct ds1086l_osc_range ds1086l_osc_ranges[DS1086L_OSC_RANGES] = {
	{ 30740000, 35860000, -6 },
	{ 33300000, 38420000, -5 },
	{ 35860000, 40980000, -4 },
	{ 38420000, 43540000, -3 },
	{ 40980000, 46100000, -2 },
	{ 43540000, 48660000, -1 },
	{ 46100000, 51220000,  0 },
	{ 48660000, 53780000,  1 },
	{ 51220000, 56340000,  2 },
	{ 53780000, 58900000,  3 },
	{ 56340000, 61460000,  4 },
	{ 58900000, 64020000,  5 },
	{ 61460000, 66580000,  6 },
};


/* Addresses scanned */
static const unsigned short normal_i2c[] = { 0x58, 0x59, 0x5a, 0x5b, 0x5c,
					0x5d, 0x5e, 0x5f, I2C_CLIENT_END };

/*-----------------------------------------------------------------------*/
struct i5ds1086l_priv_data {
	struct i2c_client *client;
	struct mutex lock;
};

static inline int i5ds1086l_lock(struct i2c_client *client)
{
	int result = -1;
	struct i5ds1086l_priv_data *priv = i2c_get_clientdata(client);
	if (priv) {
		result = mutex_lock_interruptible(&priv->lock);
	}
	return result;
}

static inline void i5ds1086l_unlock(struct i2c_client *client)
{
	struct i5ds1086l_priv_data *priv = i2c_get_clientdata(client);
	if (priv) {
		mutex_unlock(&priv->lock);
	}
}

/*-----------------------------------------------------------------------*/

/* TODO: address change */
extern int i5ds1086l_set_address(struct i2c_client *client, u8 address)
{
	u8 regval;
	s32 val = -EINVAL;
	
	if ((DS1086L_ADDR_M_A & address) == address) {
		val = i5ds1086l_lock(client);
		if (0 == val) {
			val = i2c_smbus_read_byte_data(client, 
						DS1086L_REG_ADDR);
			if (0 <= val) {
				regval = (u8)val;
				regval &= ~DS1086L_ADDR_M_A;
				regval |= address;
				val = i2c_smbus_write_byte_data(client, 
						DS1086L_REG_ADDR, regval);
			}
			i5ds1086l_unlock(client);
		}
	}
	return (int)val;
}


extern int i5ds1086l_set_write_control(struct i2c_client *client, int enabled)
{
	s32 val = -EINVAL;
	val = i5ds1086l_lock(client);
	if (0 == val) {
		val = i2c_smbus_read_byte_data(client, DS1086L_REG_ADDR);
		if(0 <= val) {
			u8 regval = (u8)val;
			regval = (enabled) ? (regval | DS1086L_ADDR_M_WC) : 
						(regval & ~DS1086L_ADDR_M_WC);
			val = i2c_smbus_write_byte_data(client, DS1086L_REG_ADDR, 
							regval);
		}
		i5ds1086l_unlock(client);
	}
	return (int)val;
}

extern int i5ds1086l_get_write_control(struct i2c_client *client)
{
	s32 val = -EINVAL;
	val = i5ds1086l_lock(client);
	if (0 == val) {
		s32 val = i2c_smbus_read_byte_data(client, DS1086L_REG_ADDR);
		if (0 <= val) {
			u8 regval = (u8)val;
			val = (regval & DS1086L_ADDR_M_WC) ? 1 : 0;
		}
		i5ds1086l_unlock(client);
	}
	return val;
}

extern int i5ds1086l_set_frequency(struct i2c_client *client, u32 frequency)
{
	s32 val = -EINVAL;
	val = i5ds1086l_lock(client);
	if (0 == val) {
		val = i2c_smbus_read_byte_data(client, DS1086L_REG_RANGE);
		if(0 <= val) {
			u8 range_val = (u8)val;
			u64 min = (u64)ds1086l_osc_ranges[0].min;
			u64 max = (u64)ds1086l_osc_ranges[DS1086L_OSC_RANGES - 1].max;
			u32 pre_scaler = 0;
			u32 offset;
			u32 dac;
			u64 helper;
			u64 scaled_freq;
			u16 i;
			int found = 0;

			range_val &= DS1086L_RANGE_M_RANGE;

			for (i = 0; i <= DS1086L_PRESCALER_V_DIVIDER_MAX && !found; 
				++i) {
				helper = (u64)((u32)1 << (u32)i);
				scaled_freq = (u64)frequency * helper;
				if ((min < scaled_freq) && (max > scaled_freq)) {
					found     = 1;
					pre_scaler = (u32)i;
				}
			}
			dev_dbg(&client->dev, "%s .range = %u\n", __func__, (unsigned)range_val);
			if (found) {
				u16 i;
				offset = 0xffffffff;
				min    = 0xffffffffffffffffULL;

				for (i = 0; i < DS1086L_OSC_RANGES; ++i) {
					helper = ((u64)ds1086l_osc_ranges[i].max + 
						(u64)ds1086l_osc_ranges[i].min);
					helper += (u64)1;
					helper >>= 1;
					if (helper > scaled_freq) {
						helper = helper - scaled_freq;
					}
					else {
						helper = scaled_freq - helper;
					}
					if (helper < min) {
						min = helper;
						offset = (u32)i;
					}
				}
				found = (offset < DS1086L_OSC_RANGES);
			}
			if (found) {
				u16 scaler_val;
				u16 dac_val;
				u8 offset_val;
			
				min = scaled_freq - (u64)ds1086l_osc_ranges[offset].min;
				min += (u64)2500;
				if (0xffffffffULL < min) {
					panic("u32 overflow before divide (%s::%d)\n",
						__FILE__, __LINE__);
				}
				dac = (u32)(min) / (u32)5000;
				offset = (u32)((u32)range_val + ds1086l_osc_ranges[offset].os);
				scaler_val = (u16)(pre_scaler << DS1086L_PRESCALER_S_DIVIDER);
				scaler_val &= DS1086L_PRESCALER_M_DIVIDER;
				dac_val =  (u16)(dac << DS1086L_DAC_S_DAC);
				dac_val &= DS1086L_DAC_M_DAC;
				offset_val = (u8)offset & DS1086L_OFFSET_M_OFFSET;

				dev_dbg(&client->dev, "%s .range = %u\n", __func__, (unsigned)range_val);
				dev_dbg(&client->dev, "%s .off = %u\n", __func__,   (unsigned)offset_val);
				dev_dbg(&client->dev, "%s .scaler = %u\n", __func__, (unsigned)scaler_val);
				dev_dbg(&client->dev, "%s .dac = %u\n", __func__, (unsigned)dac_val);

				scaler_val = swab16(scaler_val);
				dac_val = swab16(dac_val);
				val = i2c_smbus_write_word_data(client, 
						DS1086L_REG_DAC, dac_val );
				if(0 == val) {
					val = i2c_smbus_write_word_data(client, 
							DS1086L_REG_PRESCALER, 
							scaler_val );
				}
				if(0 == val) {
					val = i2c_smbus_write_byte_data(client, 
							DS1086L_REG_OFFSET, offset_val);
				}
			}
			else {
				val = -1;
			}
		}
		i5ds1086l_unlock(client);
	}
	return (int)val;
}

extern s32 i5ds1086l_get_frequency(struct i2c_client *client)
{
	u8 range_val;
	u8 off_val;
	u16 scaler_val;
	u16 dac_val;
	s32 val = -EINVAL;
	val = i5ds1086l_lock(client);
	if (0 == val) {
		val = i2c_smbus_read_byte_data( client, DS1086L_REG_RANGE );
		if (0 <= val) {
			range_val = (u8)val;
			val = i2c_smbus_read_word_data( client, DS1086L_REG_DAC );
		}
		if (0 <= val) {
			dac_val = (u16)val;
			dac_val = swab16(dac_val);
			val = i2c_smbus_read_word_data( client, 
				DS1086L_REG_PRESCALER );
		}
		if (0 <= val) {
			scaler_val = (u16)val;
			scaler_val = swab16(scaler_val);
			val = i2c_smbus_read_byte_data( client, 
				DS1086L_REG_OFFSET );
		}
		if (0 <= val) {
			s32 offset;
		
			off_val = (u8)val;
			range_val &=  DS1086L_RANGE_M_RANGE;
			off_val &=  DS1086L_OFFSET_M_OFFSET;
			scaler_val >>= DS1086L_PRESCALER_S_DIVIDER;
			dac_val >>= DS1086L_DAC_S_DAC;

			dev_dbg(&client->dev, "%s .range = %u\n", __func__, (unsigned)range_val);
			dev_dbg(&client->dev, "%s .off = %u\n", __func__,   (unsigned)off_val);
			dev_dbg(&client->dev, "%s .scaler = %u\n", __func__, (unsigned)scaler_val);
			dev_dbg(&client->dev, "%s .dac = %u\n", __func__, (unsigned)dac_val);

			offset = (s32)off_val - (s32)range_val;
			offset -= (s32)ds1086l_osc_ranges[0].os;
			dev_dbg(&client->dev, "%s .calc offset = %d\n", __func__, offset);
			if (0 <= offset) {
				u32 base_freq = ds1086l_osc_ranges[offset].min;
				u32 tmp;
				dev_dbg(&client->dev, "%s .base freq (tbl) = %u\n", __func__, base_freq);
				tmp = (u32)dac_val * 5000;
				if (0xffffffff - tmp < base_freq) {
					panic("u32 overflow (add) (%s::%d)\n",
						__FILE__, __LINE__);
				}
				base_freq += tmp;
				dev_dbg(&client->dev, "%s .base freq (dac) = %u\n", __func__, base_freq);
				tmp = (1 << (u32)scaler_val);
				if (0xffffffff - (tmp / 2) < base_freq) {
					panic("u32 overflow (round) (%s::%d)\n",
						__FILE__, __LINE__);
				}
				base_freq += (tmp / 2);
				dev_dbg(&client->dev, "%s .base freq (rnd) = %u\n", __func__, base_freq);
				if (tmp > base_freq) {
					panic("u32 underflow (divide) (%s::%d)\n",
						__FILE__, __LINE__);
				}
				base_freq /= tmp;
				dev_dbg(&client->dev, "%s .base freq = %u\n", __func__, base_freq);
				if (0x7fffffff < base_freq) {
					panic("s32 overflow (assign) (%s::%d)\n",
						__FILE__, __LINE__);
				}
				val = (s32)base_freq;
				dev_dbg(&client->dev, "%s .calc freq = %d\n", __func__, val);
			}
			else {
				val = -1;
			}
		}
		i5ds1086l_unlock(client);
	}
	return val;
}

extern int i5ds1086l_write_eeprom(struct i2c_client *client)
{
	s32 val = -EINVAL;
	val = i5ds1086l_lock(client);
	if (0 == val) {
		val = i2c_smbus_write_byte_data( client, DS1086L_REG_WRITE_EE,
					DS1086L_WRITE_EE_V_WRITE);
		i5ds1086l_unlock(client);
	}
	return (int)val;
}



/*-----------------------------------------------------------------------*/
/* sysfs attributes */

static ssize_t i5ds1086l_frequency_show(struct device *dev, 
				struct device_attribute *attr, char *buf)
{
	s32 data = 0;
	struct i2c_client *idev = to_i2c_client(dev);
	data = i5ds1086l_get_frequency(idev);
	return snprintf(buf, PAGE_SIZE, "%d\n", data);
}

static ssize_t i5ds1086l_frequency_store(struct device *dev, 
				struct device_attribute *attr, const char *buf, 
				size_t count)
{
	u32 frequency;
	struct i2c_client *idev = to_i2c_client(dev);
	sscanf(buf, "%u", &frequency);
	i5ds1086l_set_frequency(idev, frequency);
	return strnlen(buf, PAGE_SIZE);

	return 0;
}

DEVICE_ATTR(frequency, S_IRUGO | S_IWUSR, i5ds1086l_frequency_show, 
		i5ds1086l_frequency_store);

/*-----------------------------------------------------------------------*/

/* device probe and removal */

static int i5ds1086l_probe(struct i2c_client *client, 
				const struct i2c_device_id *id)
{
	struct i5ds1086l_priv_data *priv = 0;
	int result = -EINVAL;
	dev_dbg(&client->dev, "%s: >>>", __func__);
	priv = kzalloc(sizeof(struct i5ds1086l_priv_data), GFP_KERNEL);
	if (priv == NULL) {
		dev_err(&client->dev, "%s alloc failed\n", __func__);
		result = -ENOMEM;
		goto exit_nofree;
	}
	mutex_init(&priv->lock);
	result = mutex_lock_interruptible(&priv->lock);
	if (0 != result) {
		dev_err(&client->dev, "%s locking failed\n", __func__);
		goto exit_free;
		result = -1;
	}
	result = device_create_file(&client->dev, &dev_attr_frequency);
	if (0 != result) {
		dev_err(&client->dev, "%s sysfs reg failed\n", __func__);
		goto exit_unlock;
		result = -1;
	}
	
	i2c_set_clientdata(client, priv);
	priv->client = client;
	
	mutex_unlock(&priv->lock);
	dev_dbg(&client->dev, "%s <<<\n", __func__);

	return 0;
exit_unlock:
	mutex_unlock(&priv->lock);
exit_free:
	kfree(priv);
exit_nofree:

	return result;
}

static int i5ds1086l_remove(struct i2c_client *client)
{
	struct i5ds1086l_priv_data *priv = i2c_get_clientdata(client);
	int result = mutex_lock_interruptible(&priv->lock);
	if (0 == result) {
		i2c_set_clientdata(client, NULL);
		device_remove_file(&client->dev, &dev_attr_frequency);
		mutex_unlock(&priv->lock);
		kfree(priv);
	}
    
	return result;
}

static const struct i2c_device_id i5ds1086l_ids[] = {
	{ "i5ds1086l", 0 },
	{ /* LIST END */ }
};
MODULE_DEVICE_TABLE(i2c, i5ds1086l_ids);


static struct i2c_driver i5ds1086l_driver = {
	.class = I2C_CLASS_SPD,
	.driver = {
		.name = "i5ds1086l",
	},
	.probe = i5ds1086l_probe,
	.remove = i5ds1086l_remove,
	.id_table = i5ds1086l_ids,
};

/* module glue */

static int __init i5ds1086l_init(void)
{
pr_info("%s: >>>", __func__);
	return i2c_add_driver(&i5ds1086l_driver);
}

static void __exit i5ds1086l_exit(void)
{
pr_info("%s: >>>", __func__);
	i2c_del_driver(&i5ds1086l_driver);
}

MODULE_AUTHOR("Markus Niebel <markus.niebel@tq-group.com>");
MODULE_DESCRIPTION("DS1086L driver");
MODULE_LICENSE("GPL");

EXPORT_SYMBOL_GPL(i5ds1086l_set_address);
EXPORT_SYMBOL_GPL(i5ds1086l_set_write_control);
EXPORT_SYMBOL_GPL(i5ds1086l_get_write_control);
EXPORT_SYMBOL_GPL(i5ds1086l_set_frequency);
EXPORT_SYMBOL_GPL(i5ds1086l_get_frequency);
EXPORT_SYMBOL_GPL(i5ds1086l_write_eeprom);

/* BUGBUG: use subsys_initcall_sync to make sure we're here before our users */
module_init(i5ds1086l_init);
//subsys_initcall_sync(i5ds1086l_init); -- prev
module_exit(i5ds1086l_exit);

