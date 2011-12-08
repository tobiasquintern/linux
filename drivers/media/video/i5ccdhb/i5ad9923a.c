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
 * @file i5ad9923a.c
 *
 * @brief iroq5 spi protocol driver for Analog devices AD 9923 A AFE driver 
 *
 * @ingroup Camera
 */
/*
 #define DEBUG 123
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/bitrev.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/spi/spi.h>

#include "i5ad9923a.h"

#if 0
#define afe_trace dev_dbg
#else
#define afe_trace(dev, format, arg...) do { (void)(dev); } while (0)
#endif

enum i5ad9923a_const
{
	i5ad9923a_xfer_size = 5,
/*	i5ad9923a_bits_per_word = 8 * i5ad9923a_xfer_size, */
	i5ad9923a_bits_per_word = 8,
};

struct i5ad9923a_priv_data {
	struct spi_message message;
	struct spi_transfer transfer;
	u8 tx_buffer[i5ad9923a_xfer_size];
	u8 rx_buffer[i5ad9923a_xfer_size];
	struct spi_device *spi;
	struct mutex lock;
#if defined(DEBUG)
	u32 ptr;
#endif
};

/*!
	\brief fill transmit buffer with address (12 Bit) and data (28 Bit)
	
	buffer is all 40 Bits, organized 
	- bytewise
	- address LSB first
	- no dummy / dead bits
	- data LSB first
*/
static inline void i5ad9923a_to_buffer(u8 *buf, u16 address, u32 data)
{
#if 1
	buf[0] = bitrev8((u8)(address & 0xff));
	buf[1] = (u8)(((address & 0x0f00) >> 8) & 0x0f);
	buf[1] |= (u8)(((data & 0x0f) << 4) & 0x0f0);
	buf[1] = bitrev8(buf[1]);
	buf[2] = bitrev8((u8)((data >> 4) & 0xff));
	buf[3] = bitrev8((u8)((data >> 12) & 0xff));
	buf[4] = bitrev8((u8)((data >> 20) & 0xff));
#else
	buf[0] = (u8)(address >> 4);
	buf[1] = (u8)((address << 4) & 0xf0) | (u8)((data >> 24) & 0x0f);
	buf[2] = (u8)((data >> 16) & 0xff);
	buf[3] = (u8)((data >> 8) & 0xff);
	buf[4] = (u8)(data & 0xff);
#endif
}

extern int i5ad9923a_write_reg(struct spi_device *sdev, u16 address, u32 data)
{
	int result = -ENODEV;
	
	struct i5ad9923a_priv_data* priv = spi_get_drvdata(sdev);
	if (NULL != priv) {
		result = mutex_lock_interruptible(&priv->lock);
		if (0 == result) {
			afe_trace(&sdev->dev, "%s >>>\n", __func__);
			i5ad9923a_to_buffer(priv->tx_buffer, address, data);
			afe_trace(&sdev->dev, "%s: tx %#02x %#02x %#02x %#02x %#02x\n", __func__,
				priv->tx_buffer[0], priv->tx_buffer[1],priv->tx_buffer[2],
				priv->tx_buffer[3], priv->tx_buffer[4] );
			result = spi_sync(sdev, &priv->message);
			afe_trace(&sdev->dev, "%s <<<\n", __func__);
			mutex_unlock(&priv->lock);
		}
	}
	return result;
}

static int i5ad9923a_init_private(struct spi_device *sdev)
{
	int result = -1;
	struct i5ad9923a_priv_data *priv = kzalloc(sizeof(struct i5ad9923a_priv_data), 
						GFP_KERNEL);
	if (NULL != priv) {
#if defined(DEBUG)
		priv->ptr = 0;
#endif
		mutex_init(&priv->lock);
		result = mutex_lock_interruptible(&priv->lock);
		spi_message_init(&priv->message);
		priv->transfer.tx_buf = priv->tx_buffer;
		priv->transfer.rx_buf = priv->rx_buffer;
		priv->transfer.len = ARRAY_SIZE(priv->tx_buffer);
		priv->transfer.bits_per_word = i5ad9923a_bits_per_word;
		spi_message_add_tail(&priv->transfer, &priv->message);
		spi_set_drvdata(sdev, priv);
		mutex_unlock(&priv->lock);
		result = 0;
	}
	else {
		dev_err(&sdev->dev, "%s: alloc failed\n", __func__);
		result = -ENOMEM;
	}
	
	return result;
}

static int i5ad9923a_cleanup_private(struct spi_device *sdev)
{
	int result = -ENODEV;
	struct i5ad9923a_priv_data* priv = spi_get_drvdata(sdev);
	if (NULL != priv) {
		result = mutex_lock_interruptible(&priv->lock);
		if (0 == result) {
			spi_set_drvdata(sdev, NULL);
			mutex_unlock(&priv->lock);
			kfree(priv);
		}
	}
	return result;
}

#if defined DEBUG
static ssize_t i5ad9923a_show_ptr(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	struct spi_device *sdev = to_spi_device(dev);
	struct i5ad9923a_priv_data* priv = spi_get_drvdata(sdev);
	return snprintf(buf, PAGE_SIZE, "%u\n", priv->ptr);
}

static ssize_t i5ad9923a_store_ptr(struct device *dev, struct device_attribute *da,
			 const char *buf, size_t count)
{
	unsigned value;
	struct spi_device *sdev = to_spi_device(dev);
	struct i5ad9923a_priv_data* priv = spi_get_drvdata(sdev);
	sscanf(buf, "%u", &value);
	priv->ptr = value;
	return strnlen(buf, PAGE_SIZE);
}

static ssize_t i5ad9923a_show_reg(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	return 0;
}

static ssize_t i5ad9923a_store_reg(struct device *dev, struct device_attribute *da,
			 const char *buf, size_t count)
{
	struct spi_device *sdev = to_spi_device(dev);
	struct i5ad9923a_priv_data* priv = spi_get_drvdata(sdev);
	unsigned value;
	sscanf(buf, "%u", &value);
	i5ad9923a_write_reg(sdev, priv->ptr, value);
	return strnlen(buf, PAGE_SIZE);
}

static DEVICE_ATTR(ptr, S_IRUGO | S_IWUGO, i5ad9923a_show_ptr, i5ad9923a_store_ptr);
static DEVICE_ATTR(reg, S_IWUGO, i5ad9923a_show_reg, i5ad9923a_store_reg);

#endif

static void i5ad9923a_cleanup(struct spi_device *spi)
{
	i5ad9923a_cleanup_private(spi);
}

static int __devinit i5ad9923a_probe(struct spi_device *spi)
{
	int result = -1;

	dev_info(&spi->dev, "%s >>>\n", __func__);

	/* the driver requires no board-specific data: */
/*
	pdata = spi->dev.platform_data;
	if (!pdata) {
		dev_info(&spi->dev, "%s no platform data\n", __func__);
		return -ENODEV;
	}
*/
	result = i5ad9923a_init_private(spi);
	if (0 == result) {
		spi->mode = SPI_MODE_0;
		spi->bits_per_word = i5ad9923a_bits_per_word;
		result = spi_setup(spi);
		dev_info(&spi->dev, "%s - spi setup mode = %u, bpw = %u, cs = %u\n", 
			__func__, (unsigned)spi->mode, (unsigned)spi->bits_per_word, 
			(unsigned)spi->chip_select);
	}
#if defined DEBUG
	afe_trace(&spi->dev, "%s create sysfs stuff\n", __func__);
	if (0 == result) {
		result = device_create_file(&spi->dev, &dev_attr_ptr);
	}
	if (0 == result) {
		result = device_create_file(&spi->dev, &dev_attr_reg);
	}
#endif
	if (0 != result) {
		i5ad9923a_cleanup(spi);
		afe_trace(&spi->dev, "%s error cleanup\n", __func__);
	}

	dev_info(&spi->dev, "%s <<<\n", __func__);
	return result;
}

static int i5ad9923a_remove(struct spi_device *spi)
{
	afe_trace(&spi->dev, "%s >>>\n", __func__);
#if defined DEBUG
	device_remove_file(&spi->dev, &dev_attr_ptr);
	device_remove_file(&spi->dev, &dev_attr_reg);
#endif
	i5ad9923a_cleanup(spi);
	afe_trace(&spi->dev, "%s <<<\n", __func__);
	
	return 0;
}

static int i5ad9923a_suspend(struct spi_device *spi, pm_message_t mesg)
{
	int result = 0; 
	afe_trace(&spi->dev, "%s [%#04x]>>>\n", __func__, mesg.event);
	dev_warn(&spi->dev, "%s not impl.\n", __func__);
	afe_trace(&spi->dev, "%s <<<\n", __func__);
	return result;
}

static int i5ad9923a_resume(struct spi_device *spi)
{
	int result = 0;
	afe_trace(&spi->dev, "%s >>>\n", __func__);
	dev_warn(&spi->dev, "%s not impl.\n", __func__);
	afe_trace(&spi->dev, "%s <<<\n", __func__);
	return result;
}

static struct spi_driver i5ad9923a_driver = {
		.driver = {
			.name = "i5ad9923a",
			.owner = THIS_MODULE,
			.bus = &spi_bus_type,
		},
		.probe		= i5ad9923a_probe,
		.remove		= __devexit_p(i5ad9923a_remove),
		.suspend	= i5ad9923a_suspend,
		.resume		= i5ad9923a_resume,
};


/*!
 * i5ad9923a init function
 * Called by insmod i5ccdhb_camera.ko.
 *
 * @return  Error code indicating success or failure
 */
static __init int i5ad9923a_init(void)
{
	u8 err;

	pr_info("%s >>>\n", __func__);

	err = spi_register_driver(&i5ad9923a_driver);
	if (err != 0)
		pr_err("%s: driver registration failed, error=%d \n",
			__func__, err);

	pr_info("%s <<<\n", __func__);

	return err;
}

/*!
 * i5ad9923a cleanup function
 * Called on rmmod i5ad9923a.ko
 *
 * @return  Error code indicating success or failure
 */
static void __exit i5ad9923a_clean(void)
{
	pr_info("%s >>>\n", __func__);
	spi_unregister_driver(&i5ad9923a_driver);
	pr_info("%s <<<\n", __func__);
}

/* force init before users */
module_init(i5ad9923a_init);
//subsys_initcall_sync(i5ad9923a_init); //-- prev!!
module_exit(i5ad9923a_clean);

EXPORT_SYMBOL(i5ad9923a_write_reg);

MODULE_AUTHOR("TQ Systems GmbH");
MODULE_DESCRIPTION("IROQ5 AFE SPI Protocol Driver");
MODULE_LICENSE("GPL");

