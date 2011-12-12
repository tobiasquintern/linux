/*
 * Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
 * @file i5ccdhb.c
 *
 * @brief iroq5 ccd headboard camera driver functions
 *
 * @ingroup Camera
 */
//#define DEBUG 123

//#define GPIO_144_VDR_EN 144
 
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/i2c/at24.h>
#include <linux/spi/spi.h>
#include <linux/gpio.h>

#include <media/i5ccdhb/i5ccdhb.h>
#include <linux/videodev2.h>
#include <media/v4l2-subdev.h>
#include <media/soc_camera.h>

#include "i5ad9923a.h"
#include "i5eeprom.h"
//#include "i5fpga.h"
#include "i5ds1086l.h"
#include "i5ccdhb_v4l.h"
#include "i5ccdhb_internal.h"
#include "../omap3isp/isp.h"


#if 0
#define hb_trace dev_info
#define hb_ptrace pr_info
#else
/*#undef dev_info(dev, fmt, arg...)
  #define dev_info(dev, fmt, arg...) do { (void)(dev); } while (0)*/
#define hb_trace(dev, format, arg...) do { (void)(dev); } while (0)

//#define pr_ptrace(fmt, ...) ({ if (0) printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__); 0; })
#define pr_ptrace(fmt, ...)do { } while(0)
#endif

#define SOC_I5CCDHB_BUS_PARAM (SOCAM_PCLK_SAMPLE_RISING | \
				SOCAM_HSYNC_ACTIVE_HIGH | \
				SOCAM_VSYNC_ACTIVE_HIGH | \
				SOCAM_DATA_ACTIVE_HIGH | \
				SOCAM_MASTER | \
				SOCAM_DATAWIDTH_8 | SOCAM_DATAWIDTH_10 | \
				SOCAM_DATAWIDTH_15 \
				)





static const struct v4l2_queryctrl i5ccdhb_qc_gain_template = {
	.id = V4L2_CID_GAIN,
	.type = V4L2_CTRL_TYPE_INTEGER,
	.name = "Gain in 1/1000",
	.minimum = 2000,
	.maximum = 10000,
	.step = 100,
	.default_value = 2000,
};

static const struct v4l2_queryctrl i5ccdhb_qc_contrast_template = {
	.id = V4L2_CID_CONTRAST,
	.type = V4L2_CTRL_TYPE_INTEGER,
	.name = "Contrast / CDS gain",
	.minimum = afe_cdsgain_min,
	.maximum = afe_cdsgain_max,
	.step = 1,
	.default_value = afe_cdsgain_def,
};

static const struct v4l2_queryctrl i5ccdhb_qc_brightness_template = {
	.id = V4L2_CID_BRIGHTNESS,
	.type = V4L2_CTRL_TYPE_INTEGER,
	.name = "Brightness / clamp level",
	.minimum = afe_clamp_min,
	.maximum = afe_clamp_max,
	.step = 1,
	.default_value = afe_clamp_def,
};

static const struct v4l2_queryctrl i5ccdhb_qc_exposure_template = {
	.id = V4L2_CID_EXPOSURE,
	.type = V4L2_CTRL_TYPE_INTEGER,
	.name = "Exposure in usec",
	.minimum = 1,
	.maximum = 50000000,
	.step = 1,
	.default_value = 16000,
};


static long i5ccdhb_dummy_op(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
  printk("_________%s________\r\n",__func__);
  return 0;
}


/*
 * preview_link_setup - Setup previewer connections.
 * @entity : Pointer to media entity structure
 * @local  : Pointer to local pad array
 * @remote : Pointer to remote pad array
 * @flags  : Link flags
 * return -EINVAL or zero on success
 */
static int i5ccdhb_link_setup(struct media_entity *entity,
			      const struct media_pad *local,
			      const struct media_pad *remote, u32 flags)
{
  //printk("_________%s________\r\n",__func__);
  return 0;
}

/* media operations */
static const struct media_entity_operations i5ccdhb_media_ops = {
	.link_setup = i5ccdhb_link_setup,
};

static inline struct i5ccdhb_data *get_priv(struct platform_device *pdev)
{
//	struct v4l2_subdev *subdev = platform_get_drvdata(pdev);
	return platform_get_drvdata(pdev); //container_of(subdev, struct i5ccdhb_data, subdev);
}



static inline struct i5ccdhb_data* device_to_i5ccdhb(struct device* dev)
{
	return get_priv(to_platform_device(dev));
}

static inline struct device* i5ccdhb_to_device(struct i5ccdhb_data *data) 
{
	return &data->pdevice->dev;
}


static inline int i5ccdhb_lock(struct i5ccdhb_data *data)
{
	int result = result = mutex_lock_interruptible(&data->lock);
	return result;
}

static inline void i5ccdhb_unlock(struct i5ccdhb_data *data)
{
	mutex_unlock(&data->lock);
}

/* list of image formats supported by this sensor */
/*
const static struct v4l2_fmtdesc i5ccdhb_formats[] = {
	{
		.description = "YUYV (YUV 4:2:2), packed",
		.pixelformat = V4L2_PIX_FMT_UYVY,
	},
};
 */

/* ------------ FPGA communication ------------------------------------------*/
/*!
  \brief set / reset VDREN signal to AFE
  \return non null for error
  \param enabled - switch on if non zero
   
  \pre i5fpga driver (spi client device was loaded)
  
*/
void i5ccdhb_set_vdren( struct i5ccdhb_data* data , unsigned enable)
{
  struct soc_i5ccdhb_platform_data *plat_data = 
		data->pdevice->dev.platform_data;
  
	gpio_set_value( plat_data->vdr_en_gpio, enable);	
}

void i5ccdhb_set_shen( struct i5ccdhb_data* data , unsigned enable)
{
  struct soc_i5ccdhb_platform_data *plat_data = 
		data->pdevice->dev.platform_data;
  
	gpio_set_value( plat_data->sh_en_gpio, enable);	
  //gpio_set_value( plat_data->sh_en_gpio, 1);	
}

int i5ccdhb_get_shrdy( struct i5ccdhb_data* data )
{
  struct soc_i5ccdhb_platform_data *plat_data = 
		data->pdevice->dev.platform_data;
  
	return gpio_get_value( plat_data->sh_rdy_gpio );	
}


/*!
  \brief check if headboard is powered
  \return non null for error
  
  - read I5FPGA_STATUS_REG
  - check field I5FPGA_STAT_M_SHRDY
  
  \pre i5fpga driver (spi client device was loaded)
  
*/
int i5ccdhb_is_powered( struct i5ccdhb_data* data )
{
	
  int result = i5ccdhb_get_shrdy( data );

  printk("%s:::::::: %i\r\n",
         __func__ , result );
#if 0
  if( 0 == result )
  {
    return 0;
  }
  return -1;
#else
  if( 0 == result )
  {
    return -1;
  }
  return 0;
#endif  
#if 0 /* FIXME!!! */
  u32 regval = 0xffffffff;
	int result = i5fpga_read_headboard_reg(sdev, I5FPGA_STATUS_REG, 
					&regval);
	if (0 == result) {
		if (!(regval & I5FPGA_STAT_M_SHRDY)) {
			result = -1;
			pr_info("%s: i5ccdhb !SHRDY\n", __func__);
		}
	}
  return result;	
#endif 
  return 0;
	
}

/*!
  \brief power up headboard
  \return non null for error
  
  - check state
  /TODO 
  - set SHEN flag
  
  \pre i5fpga driver (spi client device was loaded)
  
*/
int i5ccdhb_power_up( struct i5ccdhb_data* data )
{
#define SH_RDY_WAIT_TIMEOUT 10
	int result = i5ccdhb_is_powered( data );
  
  int sh_rdy_timeout = 0;

  //printk("_____________%s__res: %i__________\r\n",__func__,result);
  
  if( 0 != result )
  {
    /* Set SH_EN flag */
    i5ccdhb_set_shen( data , 1 );
    

    /* Wait for SH_RDY flag */
    do
    {
      schedule_timeout_uninterruptible(msecs_to_jiffies(100));
      result = i5ccdhb_is_powered( data );
      sh_rdy_timeout++;
    }
    while( ( SH_RDY_WAIT_TIMEOUT > sh_rdy_timeout ) &&
           ( 0 != result ) );
    
    if( 0 != result )
    {
      printk("_________%s TIMEOUT!______-\r\n",
             __func__);
    }
    
  }
/*
  if (data->ad9923a) 
  {
    i5ccdhb_set_vdren(data,1);
    }*/
  
  return result;

#if 0 /* FIXME */
	if (0 != result) {
		u32 regval = 0xffffffff;

		result = i5fpga_read_headboard_reg(sdev, I5FPGA_CONTROL_REG, 
							&regval);

/* TODO: 
	- set SHEN flag
	- write back
	- poll for i5ccdhb_is_powered()
*/
	}
#endif
}

/*!
  \brief power down headboard
  \return non null for error
   
  \pre i5fpga driver (spi client device was loaded)
  
*/
static int i5ccdhb_power_down(struct i5ccdhb_data* data)
{
  

	int result = 0;
  int sh_rdy_timeout = 0;
  /* FIXME!! */

  result = i5ccdhb_is_powered( data );
  
  //printk("_____________%s__res: %i__________\r\n",__func__,result);
  result = 0;
  
	if (0 == result) {
		if (data->ad9923a) 
    {
      i5ccdhb_set_vdren(data,0);
/*
  TODO:
  - read control reg
  - clear SHEN
  - write control reg
*/
		}
    /* Unset SH_EN flag */
    i5ccdhb_set_shen( data , 0 );
    
    /* Wait for SH_RDY flag */
    do
    {
      schedule_timeout_uninterruptible(msecs_to_jiffies(100));
      result = i5ccdhb_is_powered( data );
      printk("___%s %i___\r\n",
             __func__ , sh_rdy_timeout );
      sh_rdy_timeout++;
    }
    while( ( SH_RDY_WAIT_TIMEOUT > sh_rdy_timeout ) &&
           ( 0 == result ) );

    if( 0 == result )
    {
      printk("_________%s TIMEOUT!______-\r\n",
             __func__);
      result = -1;
    }
    else
    {
      result = 0;
    }

	}
	else
  {
		result = 0;
	}

	return result;

}

/* -------------------i2c subdevice management -------------------------------*/
static void i5ccdhb_at24_setup(struct memory_accessor *macc, void *context)
{
	struct i5ccdhb_data* data = (struct i5ccdhb_data*)context;
	if (NULL != data) {
/* BUGBUG: must store pointer, driver uses struct to get containing struct! */
		data->at24_macc = macc;
		pr_info("%s: macc = %p, rd = %p / wr = %p\n", __func__, 
			data->at24_macc, data->at24_macc->read, data->at24_macc->write);
	}
	else {
		data->at24_macc = 0;
	}
}

static struct at24_platform_data i5ccdhb_at24_platform_data = {
	.byte_len = 256, /* size (sum of all addr) => 256 for 24lc02 */
	.page_size = 8, /* for writes => 8 for 24lc02*/
	.flags = AT24_FLAG_READONLY, /* AT24_FLAG_READONLY */
	.setup = i5ccdhb_at24_setup,
};

static struct i2c_board_info i5ccdhb_i2c_lm73_info = {
	.type = "lm73",
	.addr = 0x48,
};

static struct i2c_board_info i5ccdhb_i2c_at24_info = {
	.type = "at24",
	.addr = 0x50,
	.platform_data = (void *)&i5ccdhb_at24_platform_data,
};

static struct i2c_board_info i5ccdhb_i2c_i5ds1086l_info = {
	.type = "i5ds1086l",
	.addr = 0x58,
};

static struct i2c_client *i5ccdhb_i2c_check_addr_probed(struct device *dev, void *addrp)
{
  struct i2c_client       *client = i2c_verify_client(dev);
  int                     addr = *(int *)addrp;
  
  if (client && client->addr == addr)
  {
    /* I2C Client alredy probed. Return i2c_client data */
    return client;
  }
  /* I2C Client not found. Return NULL */
  return NULL;
}

/*!
  \brief probe one i2c device on headboard
  \return NULL if error
*/
static struct i2c_client *i5ccdhb_probe_i2c_dev(struct i2c_adapter *i2c_adap,
					struct i2c_board_info const *info)
{			

	struct i2c_client *client = NULL;
  struct i2c_client *result = NULL;
  int addr = info->addr; //0x48;

  /* Check, if i2c device with given addres has already been probed. 
   * If device has already been probed, no further action has to be taken. 
   */
  result = device_for_each_child(&i2c_adap->dev, (void*)&addr,
                                 i5ccdhb_i2c_check_addr_probed );
  
  if( NULL == result )
  {
    client = i2c_new_device(i2c_adap, info);
  }
  else
  {
    client = result;
  }
  

	return client;
}

/*!
  \brief probe i2c devices on headboard
  \return non null on error
   
  \pre 
  - i5fpga driver (spi client device was loaded)
  - SHEN was set (headboard powered) 
*/
static int i5ccdhb_probe_i2c_devs(struct i5ccdhb_data* data)
{
	int result; 
	struct soc_i5ccdhb_platform_data *plat_data = 
		data->pdevice->dev.platform_data;
	struct i2c_adapter *i2c_adap = 0;

  dev_info(i5ccdhb_to_device(data),"Probing I2C devices\r\n");
  
	 /* lock */	
	i2c_adap = i2c_get_adapter(plat_data->i2c_adapter_id);
	dev_info(i5ccdhb_to_device(data), "%s: lock i2c adapter %d %p\n", __func__, 
			(unsigned)plat_data->i2c_adapter_id, i2c_adap);
	result = (0 != i2c_adap) ? 0 : -ENODEV;
	if (0 == result) {
		data->lm73 = i5ccdhb_probe_i2c_dev(i2c_adap, 
						&i5ccdhb_i2c_lm73_info);
		dev_info(i5ccdhb_to_device(data), "%s: probed lm73\n", __func__);
		
		i5ccdhb_at24_platform_data.context = data;
		data->at24 = i5ccdhb_probe_i2c_dev(i2c_adap, 
						&i5ccdhb_i2c_at24_info);
		dev_info(i5ccdhb_to_device(data), "%s: probed at24\n", __func__);

		data->ds1086l = i5ccdhb_probe_i2c_dev(i2c_adap, 
						&i5ccdhb_i2c_i5ds1086l_info);
		dev_info(i5ccdhb_to_device(data), "%s: probed ds1086l\n", __func__);

		i2c_put_adapter(i2c_adap); /* unlock */
		
		result = ((0 != data->lm73) && (0 != data->at24) &&
				(0 != data->ds1086l)) ? 0 : -ENODEV;
		dev_info(i5ccdhb_to_device(data), "%s: finished\n", __func__);
	}
	return result;
}

/*!
  \brief remove i2c device on headboard
   
  \pre 
  - device was loaded 
*/
static void i5ccdhb_remove_i2c_dev(struct i2c_client *client)
{
	if (client) {
		i2c_unregister_device(client);
	}
}

/*!
  \brief remove i2c devices on headboard
   
  \pre 
  - i5fpga driver (spi client device was loaded)
*/
static void i5ccdhb_remove_i2c_devs(struct i5ccdhb_data* data)
{
	i5ccdhb_remove_i2c_dev(data->lm73);
	data->lm73 = 0;
	i5ccdhb_remove_i2c_dev(data->at24);
	data->at24 = 0;
	i5ccdhb_remove_i2c_dev(data->ds1086l);
	data->ds1086l = 0;
}

static int i5ccdhb_verify_headboard(struct i5ccdhb_data* data)
{
	struct i5ccdhb_eeprom_data* hbdata = NULL;
	
	int result = (0 != data->at24_macc->read) ? 0 : -1;
	if (0 == result) {
		data->at24_macc->read(data->at24_macc, data->at24_data,
			 		0, sizeof(data->at24_data));
	}
	if (0 == result) {
		hbdata = (struct i5ccdhb_eeprom_data*)data->at24_data;
		result = strncmp(hbdata->tag, I5CCDHB_AT24_TAG_DEFAULT, 
			ARRAY_SIZE(hbdata->tag));
		dev_info(i5ccdhb_to_device(data), "%s: at24 tag %s / %s\n", 
			__func__, I5CCDHB_AT24_TAG_DEFAULT, hbdata->tag);
	}
	if (0 == result) {
		result = strncmp(hbdata->sensor, I5CCDHB_AT24_SENSOR_ICX424AL, 
			ARRAY_SIZE(hbdata->sensor));
		dev_info(i5ccdhb_to_device(data), "%s: at24 sensor %s / %s\n", 
			__func__, I5CCDHB_AT24_SENSOR_ICX424AL, hbdata->sensor);
	}
	if (0 == result) {
		result = strncmp(hbdata->afe, I5CCDHB_AT24_AFE_AD9923A, 
			ARRAY_SIZE(hbdata->afe));
		dev_info(i5ccdhb_to_device(data), "%s: at24 afe %s / %s\n", 
			__func__, I5CCDHB_AT24_AFE_AD9923A, hbdata->afe);
	}
	if (0 == result) {
		/* TODO version check ??? */
		dev_info(i5ccdhb_to_device(data), "%s: at24 version %s\n", 
			__func__, hbdata->version);
	}
	return result;
}

/* -------------------spi subdevice management -------------------------------*/
static struct spi_board_info i5ccdhb_spi_ad9923a_info = {
	.modalias = "i5ad9923a",
	.max_speed_hz = 1000000,	/* max spi SCK clock speed in HZ */
	.chip_select = 1,
};

/*!
  \brief probe one spi device on headboard
  \return non null for error
*/   
static struct spi_device *i5ccdhb_probe_spi_dev(struct spi_master *master,
					struct spi_board_info *info)
{			
	struct spi_device *device = spi_new_device(master, info);
	pr_info("%s: create dev %p\n", __func__, device);
	return device;
}


/*!
  \brief probe spi devices on headboard
  \return non null for error
   
  \pre 
  - i5fpga driver (spi client device was loaded)
  - SHEN was set (headboard powered) 
*/
static int i5ccdhb_probe_spi_devs(struct i5ccdhb_data* data)
{
	int result; 
	struct spi_master *spi_mst = 0;
	 /* lock */	
	struct soc_i5ccdhb_platform_data *plat_data = 
		data->pdevice->dev.platform_data;
	spi_mst = spi_busnum_to_master(plat_data->afe_spi_master_id);
	dev_info(i5ccdhb_to_device(data), "%s: lock spi master %d %p\n", __func__, 
			(unsigned)plat_data->afe_spi_master_id, spi_mst);
	result = (0 != spi_mst) ? 0 : -ENODEV;
	if (0 == result) {
		i5ccdhb_spi_ad9923a_info.chip_select = plat_data->afe_spi_chipselect;
		i5ccdhb_spi_ad9923a_info.max_speed_hz = plat_data->afe_spi_maxspeed;
		data->ad9923a = i5ccdhb_probe_spi_dev(spi_mst, 
						&i5ccdhb_spi_ad9923a_info);
		dev_info(i5ccdhb_to_device(data), "%s: probed ad9923a %p\n", __func__, data->ad9923a);
		spi_master_put(spi_mst);
		result = ((0 != data->ad9923a)) ? 0 : -ENODEV;
	}
	
	return result;
}

/*!
  \brief remove spi device on headboard
   
  \pre 
  - device was loaded 
*/
static void i5ccdhb_remove_spi_dev(struct spi_device *device)
{
	if (device) {
		spi_unregister_device(device);
	}
}

/*!
  \brief remove spi devices on headboard
   
  \pre 
  - i5fpga driver (spi client device was loaded)
*/
static void i5ccdhb_remove_spi_devs(struct i5ccdhb_data* data)
{
	if (data->ad9923a) {
		i5ccdhb_remove_spi_dev(data->ad9923a);
		data->ad9923a = 0;
	}
}

/* # reset headboard */
static int i5ccdhb_headboard_reset(struct i5ccdhb_data* data)
{
	int result;
/* # disable VDR */
	i5ccdhb_set_vdren( data ,0);
  
/* # afe soft reset */
	result = i5ad9923a_write_reg( data->ad9923a, afe_swreset_reg, 0x01 );
/* # reset to default */
	afe_reset_config(&data->regcfg);
	return result;
}

int i5ccdhb_parse_setup(struct i5ccdhb_data* data)
{
	int result = afe_parse_regs(afe_default_regs, afe_default_regs_bytes, 
			data->afe_regs, ARRAY_SIZE(data->afe_regs));
	data->regcnt = (0 < result) ? result : 0;
	result = (data->regcnt) ? 0 : -EINVAL;
	dev_info(i5ccdhb_to_device(data), "%s: afe_parse_regs (%d, %u)\n", __func__, result, data->regcnt);
	
	if (0 == result) {
		result = afe_get_setup(data->afe_regs, data->regcnt,
			&data->regcfg);
		dev_info(i5ccdhb_to_device(data), "%s: afe_get_setup (%d)\n", __func__, result);
	}
	return result;
}

int i5ccdhb_setup_afe(struct i5ccdhb_data* data)
{
	size_t i;
	int trigger_done = 0;
	int vdr_done = 0;

	int result = 0;
	for (i = 0; (i < data->regcnt) && (0 == result); ++i) {
		result = i5ad9923a_write_reg(data->ad9923a, data->afe_regs[i].address,
			data->afe_regs[i].value);
			hb_trace(i5ccdhb_to_device(data), "%s: @%x -> %x\n", __func__, 
			data->afe_regs[i].address, data->afe_regs[i].value);
		if (0 == result) {
			if (vdr_trigger == data->afe_regs[i].address) {
				hb_trace(i5ccdhb_to_device(data), "%s: vdr_trigger @reg %x\n", __func__, data->afe_regs[i].address);
				i5ccdhb_set_vdren( data, 1);
				schedule_timeout_uninterruptible(msecs_to_jiffies(500));
				vdr_done = 1;
				hb_trace(i5ccdhb_to_device(data), "%s: vdr_trigger pause %lu jiffies\n", __func__, msecs_to_jiffies(500));
			}
			if (afe_sync_reg == data->afe_regs[i].address) {
				hb_trace(i5ccdhb_to_device(data), "%s: afe_sync_reg %x\n", __func__, data->afe_regs[i].address);
				if (vdr_done && !trigger_done) {
					schedule_timeout_uninterruptible(msecs_to_jiffies(10));
					trigger_done = 1;
					hb_trace(i5ccdhb_to_device(data), "%s: afe_sync_reg %lu jiffies\n", __func__, msecs_to_jiffies(10));
				}
			}
		}
	}
	return result;
}

static int i5ccdhb_headboard_init(struct i5ccdhb_data* data)
{
	int result = 0;
	
	dev_info(i5ccdhb_to_device(data), "%s >>>\n", __func__);

	result = i5ccdhb_headboard_reset(data);
	if (0 == result) {
		if(data->requested_pixel_clock != data->current_pixel_clock) {
			i5ds1086l_set_frequency(data->ds1086l, 
				data->requested_pixel_clock);
			data->current_pixel_clock = 
				i5ds1086l_get_frequency(data->ds1086l);
			dev_info(i5ccdhb_to_device(data), 
				"%s: update clk: %u -> %u\n", __func__, 
				data->current_pixel_clock,
				data->requested_pixel_clock);
			data->requested_pixel_clock = data->current_pixel_clock;
		}
	}
	if (0 == result) {
		result = i5ccdhb_parse_setup(data);
		dev_info(i5ccdhb_to_device(data), "%s: i5ccdhb_parse_setup (%d)\n", __func__, result);
	}
	if (0 == result) {
		result = i5ccdhb_setup_afe(data);
		dev_info(i5ccdhb_to_device(data), "%s: i5ccdhb_setup_afe (%d)\n", __func__, result);
	}
	if (0 == result) {
		result = afe_adapt_to_pixelclk( data->ad9923a, data->current_pixel_clock ); 
		dev_info(i5ccdhb_to_device(data), "%s: afe_adapt_to_pixelclk (%d)\n", __func__, result);
	}
	if (0 == result) {
		u32 num, frac, nsec;
		afe_get_fps( &data->regcfg, data->current_pixel_clock,
			&num, &frac );
		data->streamcap.timeperframe.numerator = 1000;
		data->streamcap.timeperframe.denominator = num * 1000 + frac % 1000;
		dev_info(i5ccdhb_to_device(data), "%s: fps (%u.%.3u)\n", __func__, num, frac);
		dev_info(i5ccdhb_to_device(data), "%s: tpf num %u  tpf denom %u)\n", 
				__func__, data->streamcap.timeperframe.numerator, 
				data->streamcap.timeperframe.denominator);
		
		memcpy(&data->controls[qc_gain], &i5ccdhb_qc_gain_template, 
				sizeof(data->controls[qc_gain]));
		memcpy(&data->controls[qc_contrast], &i5ccdhb_qc_contrast_template, 
				sizeof(data->controls[qc_contrast]));
		memcpy(&data->controls[qc_brightness], &i5ccdhb_qc_brightness_template, 
				sizeof(data->controls[qc_brightness]));
		memcpy(&data->controls[qc_exposure], &i5ccdhb_qc_exposure_template, 
				sizeof(data->controls[qc_exposure]));
		
		afe_get_gain_min(&data->regcfg, &num, &frac);
		data->controls[qc_gain].minimum = (s32)(num * 1000 + frac % 1000);
		afe_get_gain_max(&data->regcfg, &num, &frac);
		data->controls[qc_gain].maximum = (s32)(num * 1000 + frac % 1000);

		nsec = afe_get_exposure_min_nsec( &data->regcfg, data->current_pixel_clock );
		data->controls[qc_exposure].minimum = (s32)((nsec + 500) / 1000);
		nsec = afe_get_exposure_max_nsec( &data->regcfg, data->current_pixel_clock );
		data->controls[qc_exposure].maximum = (s32)((nsec + 500) / 1000);
	}
	dev_info(i5ccdhb_to_device(data), "%s <<<\n", __func__);
	
	return result;
}


/* ------------------- device attributes -------------------------------------*/
static ssize_t i5ccdhb_store_empty(struct device *dev, struct device_attribute *da,
			 const char *buf, size_t count)
{
	return 0;
}

static ssize_t i5ccdhb_show_empty(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	return 0;
}

static ssize_t i5ccdhb_show_e2tag(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	struct i5ccdhb_eeprom_data *hbdata = 
			(struct i5ccdhb_eeprom_data*)data->at24_data;
	return snprintf(buf, PAGE_SIZE, "%s\n", hbdata->tag);
}

static ssize_t i5ccdhb_show_e2sensor(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	struct i5ccdhb_eeprom_data *hbdata = 
			(struct i5ccdhb_eeprom_data*)data->at24_data;
	ssize_t result;
	result = snprintf(buf, PAGE_SIZE, "%s\n", hbdata->sensor);
	return result;
}

static ssize_t i5ccdhb_show_e2afe(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	struct i5ccdhb_eeprom_data *hbdata = 
			(struct i5ccdhb_eeprom_data*)data->at24_data;
	return snprintf(buf, PAGE_SIZE, "%s\n", hbdata->afe);
}

static ssize_t i5ccdhb_show_e2version(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	struct i5ccdhb_eeprom_data *hbdata = 
			(struct i5ccdhb_eeprom_data*)data->at24_data;
	return snprintf(buf, PAGE_SIZE, "%s\n", hbdata->version);
}

static ssize_t i5ccdhb_show_e2serial(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	struct i5ccdhb_eeprom_data *hbdata = 
			(struct i5ccdhb_eeprom_data*)data->at24_data;
	ssize_t result = snprintf(buf, PAGE_SIZE, "%s\n", hbdata->serial);
	dev_info(dev, "%s: %s / %s (%d)\n", __func__, buf, hbdata->serial, result);
	return result;
}

static DEVICE_ATTR(e2tag, S_IRUGO, i5ccdhb_show_e2tag, i5ccdhb_store_empty);
static DEVICE_ATTR(e2version, S_IRUGO, i5ccdhb_show_e2version, i5ccdhb_store_empty);
static DEVICE_ATTR(e2serial, S_IRUGO, i5ccdhb_show_e2serial, i5ccdhb_store_empty);
static DEVICE_ATTR(e2sensor, S_IRUGO, i5ccdhb_show_e2sensor, i5ccdhb_store_empty);
static DEVICE_ATTR(e2afe, S_IRUGO, i5ccdhb_show_e2afe, i5ccdhb_store_empty);

static ssize_t i5ccdhb_show_exposure(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	ssize_t result = 0;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	u32 exposure = afe_get_exposure_nsec( &data->regcfg, data->current_pixel_clock );
	u32 msec = exposure / (1000 * 1000);
	u32 usec = exposure % (1000 * 1000);
	usec = (usec + 500) / 1000;
	result = snprintf(buf, PAGE_SIZE, "%u.%.3u\n", msec, usec);
	return result;
}

static ssize_t i5ccdhb_store_exposure(struct device *dev, struct device_attribute *da,
			 const char *buf, size_t count)
{
	unsigned usec, msec;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	sscanf(buf, "%u%u", &msec, &usec);
	dev_info(dev, "%s (%u) >>>\n", __func__, usec);
	if (0 == i5ccdhb_lock(data)) {
		u32 nsec = msec * 1000 + usec % 1000;
		nsec *= 1000;
		afe_set_exposure_nsec( data->ad9923a, &data->regcfg, data->current_pixel_clock,
			nsec );
		i5ccdhb_unlock(data);
	}
	return strnlen(buf, PAGE_SIZE);
}

static ssize_t i5ccdhb_show_fps(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	ssize_t result = 0;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	u32 num, frac;
	afe_get_fps( &data->regcfg, data->current_pixel_clock, &num, &frac );
	result = snprintf(buf, PAGE_SIZE, "%u.%.3u\n", num, frac);
	return result;
}

static ssize_t i5ccdhb_store_fps(struct device *dev, struct device_attribute *da,
			 const char *buf, size_t count)
{
	unsigned mfps_frac, mfps_num;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	sscanf(buf, "%u%u", &mfps_num, &mfps_frac);
	if (0 == i5ccdhb_lock(data)) {
		afe_set_fps( data->ad9923a, &data->regcfg, data->current_pixel_clock,
			mfps_num, mfps_frac );
		afe_get_fps( &data->regcfg, data->current_pixel_clock,
			&mfps_num, &mfps_frac );
		data->streamcap.timeperframe.numerator = 1000;
		data->streamcap.timeperframe.denominator = mfps_num * 1000 + mfps_frac;
		i5ccdhb_unlock(data);
	}
	return strnlen(buf, PAGE_SIZE);
}

static ssize_t i5ccdhb_show_clk(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	ssize_t result = 0;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	u32 clk_mhz = data->current_pixel_clock / (1000 * 1000);
	u32 clk_khz = data->current_pixel_clock % (1000 * 1000);
	clk_khz = (clk_khz + 500) / 1000;
	result = snprintf(buf, PAGE_SIZE, "%u.%.3u\n", clk_mhz, clk_khz);
	return result;
}

static ssize_t i5ccdhb_store_clk(struct device *dev, struct device_attribute *da,
			 const char *buf, size_t count)
{
	u32 clk_mhz;
	u32 clk_hz;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	sscanf(buf, "%u", &clk_mhz);
	if ((afe_frquency_mhz_min <= clk_mhz) && (afe_frquency_mhz_max >= clk_mhz)) {
		clk_hz = clk_mhz * 1000 * 1000;
		if (0 == i5ccdhb_lock(data)) {
			data->requested_pixel_clock = clk_hz;
			i5ccdhb_unlock(data);
		}
	}
	return strnlen(buf, PAGE_SIZE);
}

static ssize_t i5ccdhb_show_gain(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	ssize_t result = 0;
	unsigned gain_num, gain_frac;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);

	afe_get_gain( &data->regcfg, &gain_num, &gain_frac);
	result = snprintf(buf, PAGE_SIZE, "%u.%.3u\n", gain_num, gain_frac);
	return result;
}

static ssize_t i5ccdhb_store_gain(struct device *dev, struct device_attribute *da,
			 const char *buf, size_t count)
{
	unsigned gain_num, gain_frac;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	sscanf(buf, "%u%u", &gain_num, &gain_frac);
	if (0 == i5ccdhb_lock(data)) {
		afe_set_gain( data->ad9923a, &data->regcfg, gain_num, gain_frac);
		i5ccdhb_unlock(data);
	}
	return strnlen(buf, PAGE_SIZE);
}

static ssize_t i5ccdhb_show_cdsgain(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	ssize_t result = 0;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);

	result = snprintf(buf, PAGE_SIZE, "%u\n", afe_get_cdsgain(&data->regcfg));
	
	return result;
}

static ssize_t i5ccdhb_store_cdsgain(struct device *dev, struct device_attribute *da,
			 const char *buf, size_t count)
{
	u32 cdsgain;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	sscanf(buf, "%u", &cdsgain);
	if (0 == i5ccdhb_lock(data)) {
		afe_set_cdsgain( data->ad9923a, &data->regcfg, cdsgain);
		i5ccdhb_unlock(data);
	}
	return strnlen(buf, PAGE_SIZE);
}

static ssize_t i5ccdhb_show_clamp(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	ssize_t result = 0;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);

	result = snprintf(buf, PAGE_SIZE, "%u\n", afe_get_clamp(&data->regcfg));
	
	return result;
}

static ssize_t i5ccdhb_store_clamp(struct device *dev, struct device_attribute *da,
			 const char *buf, size_t count)
{
	u32 clamp;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	sscanf(buf, "%u", &clamp);
	if (0 == i5ccdhb_lock(data)) {
		afe_set_clamp( data->ad9923a, &data->regcfg, clamp);
		i5ccdhb_unlock(data);
	}
	return strnlen(buf, PAGE_SIZE);
}

static ssize_t i5ccdhb_show_aferegs(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	ssize_t result = 0;
	char* worker = buf;
	size_t i;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	
	for (i = 0; i < data->regcnt; ++i) {
		result += snprintf(worker, PAGE_SIZE - result, "%#03x\t%#06x\n", 
			data->afe_regs[i].address, data->afe_regs[i].value);
		worker += strlen(worker);
	}
	
	return result;
}

static ssize_t i5ccdhb_show_afeconf(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	ssize_t result = 0;
	char* worker = buf;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	struct afe_reg_conf *conf = &data->regcfg;
	
	result += snprintf(worker, PAGE_SIZE - result, "vpat_cnt %u\n", conf->afe_vpat_cnt);
	worker += strlen(worker);
	result += snprintf(worker, PAGE_SIZE - result, "vseq_cnt %u\n", conf->afe_vseq_cnt);
	worker += strlen(worker);
	result += snprintf(worker, PAGE_SIZE - result, "field_cnt %u\n", conf->afe_field_cnt);
	worker += strlen(worker);
	result += snprintf(worker, PAGE_SIZE - result, "cdsgain_cnt %u\n", conf->afe_cdsgain_cnt);
	worker += strlen(worker);
	result += snprintf(worker, PAGE_SIZE - result, "lines_cnt %u\n", conf->afe_lines_cnt);
	worker += strlen(worker);
	result += snprintf(worker, PAGE_SIZE - result, "subck_cnt %u\n", conf->afe_subck_cnt);
	worker += strlen(worker);
	result += snprintf(worker, PAGE_SIZE - result, "subck_max %u\n", conf->afe_subck_max);
	worker += strlen(worker);
	result += snprintf(worker, PAGE_SIZE - result, "gain_cnt %u / %x\n", conf->afe_gain_cnt, conf->afe_gain_cnt);
	worker += strlen(worker);
	result += snprintf(worker, PAGE_SIZE - result, "clamp_cnt %u\n", conf->afe_clamp_cnt);
	worker += strlen(worker);
	result += snprintf(worker, PAGE_SIZE - result, "pixel_hdrise %u\n", conf->afe_pixel_hdrise);
	worker += strlen(worker);
	result += snprintf(worker, PAGE_SIZE - result, "lines_vdrise %u\n", conf->afe_lines_vdrise);
	
	return result;
}

static ssize_t i5ccdhb_show_hbdata(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	ssize_t result = 0;
	char* worker = buf;
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	struct i5ccdhb_eeprom_data* hbdata = 
		hbdata = (struct i5ccdhb_eeprom_data*)data->at24_data;

	result += snprintf(worker, PAGE_SIZE - result, "at24 tag %s\n", hbdata->tag);
	worker += strlen(worker);
	result += snprintf(worker, PAGE_SIZE - result, "at24 sensor %s \n", 
			hbdata->sensor);
	worker += strlen(worker);
	result += snprintf(worker, PAGE_SIZE - result, "at24 afe %s \n", 
			hbdata->afe);
	worker += strlen(worker);
	result += snprintf(worker, PAGE_SIZE - result, "at24 version %s\n", 
			hbdata->version);
	
	return result;
}


static ssize_t i5ccdhb_store_afecmd(struct device *dev, struct device_attribute *da,
			 const char *buf, size_t count)
{
	struct i5ccdhb_data* data = device_to_i5ccdhb(dev);
	
	dev_info(dev, "%s - %s\n", __func__, buf);
	if (0 == i5ccdhb_lock(data)) {
		if (0 == strncmp(buf, "afeinit", 7)) {
			i5ccdhb_headboard_init(data);
		}
		else if (0 == strncmp(buf, "afereset", 8)) {
			i5ccdhb_headboard_reset(data);
		}
		else if (0 == strncmp(buf, "afesetup", 8)) {
			i5ccdhb_setup_afe(data);
		}
		else if (0 == strncmp(buf, "afeparse", 8)) {
			i5ccdhb_parse_setup(data);
		}
		else if (0 == strncmp(buf, "afeadaptclk", 11)) {
			afe_adapt_to_pixelclk(data->ad9923a, data->current_pixel_clock);
		}
#if 0
		else if (0 == strncmp(buf, "fpgatest", 8)) {
			u32 start = 0x0;
			u32 stop = 0x400;
			u32 shift = 0x3;
			u16 reg = I5FPGA_CONTROL_REG;
			u32 i;
			u32 e = 0;


			for (i = start; i < stop; ++i) {
				u32 regval = (i % 0x1000000) << shift;
				u32 readval = 0xffffffff;
				i5fpga_write_headboard_reg(data->fpga, reg, regval);
				i5fpga_read_headboard_reg(data->fpga, reg, &readval);
				if (readval != regval) {
					dev_info(dev, "%s - error @%u %x <=> %x\n", 
						__func__, i, regval, readval);
					++e;
				}
			}
			dev_info(dev, "%s - fpga test %u errorr\n", 
				__func__, e);

		}
#endif
		else {
			dev_info(dev, "%s - unknown\n", __func__);
		}
	
		i5ccdhb_unlock(data);
	}
	return strnlen(buf, PAGE_SIZE);
}

static DEVICE_ATTR(aferegs, S_IRUGO, i5ccdhb_show_aferegs, i5ccdhb_store_empty);
static DEVICE_ATTR(afeconf, S_IRUGO, i5ccdhb_show_afeconf, i5ccdhb_store_empty);
static DEVICE_ATTR(afecmd, S_IWUGO, i5ccdhb_show_empty, i5ccdhb_store_afecmd);
static DEVICE_ATTR(hbdata, S_IRUGO, i5ccdhb_show_hbdata, i5ccdhb_store_empty);

static DEVICE_ATTR(exposure, S_IRUGO | S_IWUGO, i5ccdhb_show_exposure, i5ccdhb_store_exposure);
static DEVICE_ATTR(fps, S_IRUGO | S_IWUGO, i5ccdhb_show_fps, i5ccdhb_store_fps);
static DEVICE_ATTR(clk, S_IRUGO | S_IWUGO, i5ccdhb_show_clk, i5ccdhb_store_clk);
static DEVICE_ATTR(gain, S_IRUGO | S_IWUGO, i5ccdhb_show_gain, i5ccdhb_store_gain);
static DEVICE_ATTR(cdsgain, S_IRUGO | S_IWUGO, i5ccdhb_show_cdsgain, i5ccdhb_store_cdsgain);
static DEVICE_ATTR(clamp, S_IRUGO | S_IWUGO, i5ccdhb_show_clamp, i5ccdhb_store_clamp);

static ssize_t i5ccdhb_afecfg_bin_read(struct kobject *kobj, struct bin_attribute *attr,
		char *buf, loff_t off, size_t count)
{
	struct i5ccdhb_data *i5ccdhb = device_to_i5ccdhb(container_of(kobj, 
							struct device, kobj));
/* TODO: */
	if (afe_raw_data_size >= off + count) {
		memcpy(buf, &i5ccdhb->bincfg[off], count);
		return count;
	} else {
		return -1;
	}
}

static ssize_t i5ccdhb_afecfg_bin_write(struct kobject *kobj, struct bin_attribute *attr,
		char *buf, loff_t off, size_t count)
{
	struct i5ccdhb_data *data = device_to_i5ccdhb(container_of(kobj, 
							struct device, kobj));
	ssize_t result = -1;

/* TODO: */
	if (0 == i5ccdhb_lock(data)) {
		if (afe_raw_data_size >= off + count) {
			memcpy(&data->bincfg[off], buf, count);
			result = count;
		}
		
		i5ccdhb_unlock(data);
	}
	return strnlen(buf, PAGE_SIZE);
}

static struct bin_attribute i5ccdhb_bin_config = {
	.attr = {
		.name = "afecfg",
		.mode = S_IRUGO | S_IWUGO, 
		},
	.size = afe_raw_data_size,
	.read = i5ccdhb_afecfg_bin_read,
	.write = i5ccdhb_afecfg_bin_write,
};


static int i5ccdhb_register_attributes(struct i5ccdhb_data *data)
{
	struct device* dev = i5ccdhb_to_device(data);
	int result = 0;
	
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_e2tag);
	}
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_e2version);
	}
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_e2serial);
	}
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_e2sensor);
	}
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_e2afe);
	}
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_aferegs);
	}
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_afeconf);
	}
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_afecmd);
	}
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_hbdata);
	}
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_exposure);
	}
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_fps);
	}
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_clk);
	}
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_gain);
	}
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_cdsgain);
	}
	if (0 == result) {
		result = device_create_file(dev, &dev_attr_clamp);
	}
	if (0 == result) {
		result = sysfs_create_bin_file(&dev->kobj, &i5ccdhb_bin_config);
	}
	dev_info(dev, "%s - debug files created: %i\n", __func__, result);

	return result;
}

static void i5ccdhb_remove_attributes(struct i5ccdhb_data *data)
{
	struct device* dev = i5ccdhb_to_device(data);

	sysfs_remove_bin_file(&dev->kobj, &i5ccdhb_bin_config);

	device_remove_file(dev, &dev_attr_e2tag);
	device_remove_file(dev, &dev_attr_e2version);
	device_remove_file(dev, &dev_attr_e2serial);
	device_remove_file(dev, &dev_attr_e2sensor);
	device_remove_file(dev, &dev_attr_e2afe);

	device_remove_file(dev, &dev_attr_aferegs);
	device_remove_file(dev, &dev_attr_afeconf);
	device_remove_file(dev, &dev_attr_afecmd);
	device_remove_file(dev, &dev_attr_hbdata);
/* TODO: */
	device_remove_file(dev, &dev_attr_exposure);
	device_remove_file(dev, &dev_attr_fps);
	device_remove_file(dev, &dev_attr_clk);
	device_remove_file(dev, &dev_attr_gain);
	device_remove_file(dev, &dev_attr_cdsgain);
	device_remove_file(dev, &dev_attr_clamp);
}


/* -------------------v4l2 device management ---------------------------------*/

// static int soc_i5ccdhb_s_config(struct v4l2_subdev *sd, int irq, void *platform_data);
// /static int soc_i5ccdhb_init(struct v4l2_subdev *sd, u32 val);
/* TODO: exclude basic register setup to firmware */
// static int soc_i5ccdhb_load_fw(struct v4l2_subdev *sd);

/* not used, static link to table of ctrls in soc_camera_ops
static int soc_i5ccdhb_queryctrl(struct v4l2_subdev *sd, struct v4l2_queryctrl *qc);
*/
static int soc_i5ccdhb_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl);
static int soc_i5ccdhb_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl);
/* TODO: */
#if 0
static int soc_i5ccdhb_s_power(struct v4l2_subdev *sd, int on);
static int i5ccdhb_set_power(struct v4l2_subdev *subdev, int on);
#endif

static struct v4l2_subdev_core_ops soc_i5ccdhb_subdev_core_ops = {

  .ioctl = i5ccdhb_dummy_op ,
	/* used for g/s_register -> use sysfs debus IF instead */
	/* int (*g_chip_ident)(struct v4l2_subdev *sd, struct v4l2_dbg_chip_ident *chip); */
	/* used to output status / fmt to syslog */
	/* int (*log_status)(struct v4l2_subdev *sd); */
	/*
	 * TODO: if set, then it is always called by the v4l2_i2c_new_subdev*
	* functions after the v4l2_subdev was registered. It is used to pass
	* platform data to the subdev which can be used during initialization
	*/
	// int (*s_config)(struct v4l2_subdev *sd, int irq, void *platform_data);
	// .s_config = soc_i5ccdhb_s_config,
	/* TODO: sensor default init */
	// .init = soc_i5ccdhb_init,
	// .load_fw = soc_i5ccdhb_load_fw,
	/* generic reset command. The argument selects which subsystems to
	 * reset. Passing 0 will always reset the whole chip. Do not use for new
	 * drivers without discussing this first on the linux-media mailinglist.
	 * There should be no reason normally to reset a device.
	 */
	.reset = i5ccdhb_dummy_op, // soc_i5ccdhb_reset,
//	int (*s_gpio)(struct v4l2_subdev *sd, u32 val);
/* query ctrl supeseeded by control array in ops structure */
/*	.queryctrl = soc_i5ccdhb_queryctrl,*/
	.g_ctrl = soc_i5ccdhb_g_ctrl,
	.s_ctrl = soc_i5ccdhb_s_ctrl,
//	int (*g_ext_ctrls)(struct v4l2_subdev *sd, struct v4l2_ext_controls *ctrls);
//	int (*s_ext_ctrls)(struct v4l2_subdev *sd, struct v4l2_ext_controls *ctrls);
//	int (*try_ext_ctrls)(struct v4l2_subdev *sd, struct v4l2_ext_controls *ctrls);
//	int (*querymenu)(struct v4l2_subdev *sd, struct v4l2_querymenu *qm);
// TODO:
	.s_std = i5ccdhb_dummy_op ,// soc_i5ccdhb_s_std,
//	long (*ioctl)(struct v4l2_subdev *sd, unsigned int cmd, void *arg);
#ifdef CONFIG_VIDEO_ADV_DEBUG
	/* get / set register of a chip -> use sysfs debus IF instead */
	/* int (*g_register)(struct v4l2_subdev *sd, struct v4l2_dbg_register *reg); */
	/* int (*s_register)(struct v4l2_subdev *sd, struct v4l2_dbg_register *reg); */
#endif
	//.s_power = i5ccdhb_set_power , //soc_i5ccdhb_s_power,
};


static int soc_i5ccdhb_s_stream(struct v4l2_subdev *sd, int enable);



static int soc_i5ccdhb_g_parm(struct v4l2_subdev *sd, 
				struct v4l2_streamparm *param);
static int soc_i5ccdhb_s_parm(struct v4l2_subdev *sd, 
				struct v4l2_streamparm *param);


static struct v4l2_subdev_video_ops soc_i5ccdhb_subdev_video_ops = {

	// int (*g_input_status)(struct v4l2_subdev *sd, u32 *status);
	// TODO: implement power saving
	.s_stream	= soc_i5ccdhb_s_stream,
	/* do not use, only for old drivers */
	// int (*enum_fmt)(struct v4l2_subdev *sd, struct v4l2_fmtdesc *fmtdesc);
	// int (*g_fmt)(struct v4l2_subdev *sd, struct v4l2_format *fmt);
	// int (*try_fmt)(struct v4l2_subdev *sd, struct v4l2_format *fmt);
	// int (*s_fmt)(struct v4l2_subdev *sd, struct v4l2_format *fmt);
  .g_parm = soc_i5ccdhb_g_parm,
	.s_parm = soc_i5ccdhb_s_parm,
};

static struct v4l2_subdev_pad_ops i5ccdhb_subdev_pad_ops = 
{
  
  .enum_mbus_code = i5ccdhb_enum_mbus_code , 
	.enum_frame_size = i5ccdhb_enum_frame_size, 
  .get_fmt = i5ccdhb_get_format,
	.set_fmt = i5ccdhb_set_format,
};

static struct v4l2_subdev_ops soc_i5ccdhb_subdev_ops = {
	.core	= &soc_i5ccdhb_subdev_core_ops,
	.video	= &soc_i5ccdhb_subdev_video_ops,
  .pad = &i5ccdhb_subdev_pad_ops ,
};

static int soc_i5ccdhb_g_parm(struct v4l2_subdev *sd, 
				struct v4l2_streamparm *sp)
{
	struct i5ccdhb_data *data = v4l2_get_subdevdata(sd);
	struct v4l2_captureparm *cparm = &sp->parm.capture;
	int result = 0;

	hb_trace(i5ccdhb_to_device(data), "%s: +++\n", __func__);

	switch (sp->type) {
	/* This is the only case currently handled. */
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		dev_info(i5ccdhb_to_device(data), "%s - V4L2_BUF_TYPE_VIDEO_CAPTURE\n", __func__);
		memset(sp, 0, sizeof(*sp));
		sp->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		cparm->capability = data->streamcap.capability;
		cparm->timeperframe = data->streamcap.timeperframe;

		hb_trace(i5ccdhb_to_device(data), "%s: tpf (%u / %u)\n", __func__, 
			cparm->timeperframe.numerator, cparm->timeperframe.denominator);

		cparm->capturemode = data->streamcap.capturemode;
		result = 0;
		break;

	/* These are all the possible cases. */
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		dev_err(i5ccdhb_to_device(data),
			"   type is not V4L2_BUF_TYPE_VIDEO_CAPTURE " \
			"but %d\n", sp->type);
		result = -EINVAL;
		break;

	default:
		dev_err(i5ccdhb_to_device(data),
			"   type is unknown - %d\n", sp->type);
		result = -EINVAL;
		break;
	}

	return result;
}

static int soc_i5ccdhb_s_parm(struct v4l2_subdev *sd, 
				struct v4l2_streamparm *sp)
{
	struct i5ccdhb_data *data = v4l2_get_subdevdata(sd);
	struct v4l2_fract *timeperframe = &sp->parm.capture.timeperframe;
	u32 fps_num, fps_frac, mfps;	/* target frames per secound */
	u32 fps_num_max, fps_frac_max, mfps_max;
	u32 fps_num_min, fps_frac_min, mfps_min;
	int result = 0;

	hb_trace(i5ccdhb_to_device(data), "%s: +++\n", __func__);

	switch (sp->type) {
	/* This is the only case currently handled. */
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		dev_info(i5ccdhb_to_device(data), 
			"%s: V4L2_BUF_TYPE_VIDEO_CAPTURE\n", __func__);
		/* Check that the new frame rate is allowed. */
		if ((timeperframe->numerator == 0)
		    || (timeperframe->denominator == 0)) {
			hb_trace(i5ccdhb_to_device(data), 
				"%s: V4L2_BUF_TYPE_VIDEO_CAPTURE - correct zero\n",
				 __func__);
			timeperframe->denominator = 1;
			timeperframe->numerator = 1;
		}
		fps_num = (timeperframe->denominator * 1000) / timeperframe->numerator;
		fps_frac = fps_num % 1000;
		fps_num /= 1000;

		hb_trace(i5ccdhb_to_device(data), 
			"%s: tpf_num / tpf_denom %u/%u -> fps %u.%.3u\n", __func__,
				timeperframe->numerator, timeperframe->denominator,
				fps_num, fps_frac);
		afe_get_max_fps ( data->current_pixel_clock, &fps_num_max, 
			&fps_frac_max );
		afe_get_min_fps ( data->current_pixel_clock, &fps_num_min, 
			&fps_frac_min );
			
		mfps = fps_num * 1000 + fps_frac;
		mfps_min = fps_num_min * 1000 + fps_frac_min;
		mfps_max = fps_num_max * 1000 + fps_frac_max;

		if (mfps > mfps_max) {
			hb_trace(i5ccdhb_to_device(data), 
				"%s: V4L2_BUF_TYPE_VIDEO_CAPTURE - limit max\n",
				 __func__);
			timeperframe->denominator = mfps_max;
			timeperframe->numerator = 1000;
			fps_num = fps_num_max;
			fps_frac = fps_frac_max;
		} else if (mfps < mfps_min) {
			hb_trace(i5ccdhb_to_device(data), 
				"%s: V4L2_BUF_TYPE_VIDEO_CAPTURE - limit min\n",
				 __func__);
			timeperframe->denominator = mfps_min;
			timeperframe->numerator = 1000;
			fps_num = fps_num_min;
			fps_frac = fps_frac_min;
		}
		data->streamcap.timeperframe = *timeperframe;
		if (0 == result) {
			u32 nsec;
			result = afe_set_fps(data->ad9923a, &data->regcfg, 
				data->current_pixel_clock, fps_num, fps_frac);
			nsec = afe_get_exposure_min_nsec( &data->regcfg, data->current_pixel_clock );
			data->controls[qc_exposure].minimum = (s32)((nsec + 500) / 1000);
			nsec = afe_get_exposure_max_nsec( &data->regcfg, data->current_pixel_clock );
			data->controls[qc_exposure].maximum = (s32)((nsec + 500) / 1000);
			afe_get_fps(&data->regcfg, data->current_pixel_clock, 
					&fps_num, &fps_frac);
			data->streamcap.timeperframe.numerator = 1000;
			data->streamcap.timeperframe.denominator = fps_num * 1000 + fps_frac % 1000;
			hb_trace(i5ccdhb_to_device(data), "%s: fps (%u.%.3u)\n", __func__, fps_num, fps_frac);
			hb_trace(i5ccdhb_to_device(data), "%s: tpf num %u  tpf denom %u)\n", 
					__func__, data->streamcap.timeperframe.numerator, 
					data->streamcap.timeperframe.denominator);
			*timeperframe = data->streamcap.timeperframe;
		}
		break;

	/* These are all the possible cases. */
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		dev_err(i5ccdhb_to_device(data),
			"   type is not V4L2_BUF_TYPE_VIDEO_CAPTURE " \
			"but %d\n", sp->type);
		result = -EINVAL;
		break;
	default:
		dev_err(i5ccdhb_to_device(data),
			"   type is unknown - %d\n", sp->type);
		result = -EINVAL;
		break;
	}

	return result;
}


/*!
 * soc_i5ccdhb_g_ctrl - V4L2 sensor interface handler for VIDIOC_G_CTRL ioctl
 * @sd: pointer to V4L2 subdevice structure
 * @ctrl: standard V4L2 VIDIOC_S_CTRL ioctl structure
 *
 * If the requested control is supported, returns the control's current
 * value from the video_control[] array.  Otherwise, returns -EINVAL
 * if the control is not supported.
 */
static int soc_i5ccdhb_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	int result = -EBUSY;
	u32 tmp1, tmp2;
	struct i5ccdhb_data *data = v4l2_get_subdevdata(sd);
	hb_trace(i5ccdhb_to_device(data), "%s: +++\n", __func__);
	
	result = i5ccdhb_lock(data);
	if (0 == result) {
		switch (ctrl->id) {
		case V4L2_CID_EXPOSURE:
			tmp1 = afe_get_exposure_nsec(&data->regcfg, data->current_pixel_clock);
			ctrl->value = (s32)((tmp1 + 500) / 1000);
			break;
		case V4L2_CID_BRIGHTNESS:
			tmp1 = afe_get_clamp(&data->regcfg);
			ctrl->value = (s32)tmp1;
			break;
		case V4L2_CID_CONTRAST:
			tmp1 = afe_get_cdsgain(&data->regcfg);
			ctrl->value = (s32)tmp1;
			break;
		case V4L2_CID_GAIN:
			afe_get_gain(&data->regcfg, &tmp1, &tmp2);
			ctrl->value = (s32)(tmp1 * 1000 + tmp2);
			break;
		case V4L2_CID_HUE:
		case V4L2_CID_SATURATION:
		case V4L2_CID_RED_BALANCE:
		case V4L2_CID_BLUE_BALANCE:
			result = -EINVAL;
			break;
		default:
			dev_info(i5ccdhb_to_device(data), "   Default case\n");
			result = -EPERM;
		}
		i5ccdhb_unlock(data);
	}
	hb_trace(i5ccdhb_to_device(data), "%s: ---\n", __func__);
	return result;
}

/*!
 * s_ctrl - V4L2 sensor interface handler for VIDIOC_S_CTRL ioctl
 * @sd: pointer to V4L2 subdevice structure
 * @ctrl: standard V4L2 VIDIOC_S_CTRL ioctl structure
 *
 * If the requested control is supported, sets the control's current
 * value in HW (and updates the video_control[] array).  Otherwise,
 * returns -EINVAL if the control is not supported.
 */
 static int soc_i5ccdhb_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i5ccdhb_data *data = v4l2_get_subdevdata(sd);
	int result = -EBUSY;
	u32 tmp1, tmp2;

	hb_trace(i5ccdhb_to_device(data), "%s: +++\n", __func__);
	if (ctrl->value < 0) {
		result = -EINVAL;
		return result;
	}

	result = i5ccdhb_lock(data);
	if (0 == result) {
		switch (ctrl->id) {
		case V4L2_CID_BRIGHTNESS:
			result = afe_set_clamp(data->ad9923a, &data->regcfg, (u32)ctrl->value);
			if (0 == result) {
				tmp1 = afe_get_clamp(&data->regcfg);
				ctrl->value = (s32)tmp1;
			}
			hb_trace(i5ccdhb_to_device(data), "   V4L2_CID_BRIGHTNESS \n");
			break;
		case V4L2_CID_CONTRAST:
			result = afe_set_cdsgain(data->ad9923a, &data->regcfg, (u32)ctrl->value);
			hb_trace(i5ccdhb_to_device(data), "   V4L2_CID_CONTRAST\n");
			if (0 == result) {
				tmp1 = afe_get_cdsgain(&data->regcfg);
				ctrl->value = (s32)tmp1;
			}
			break;
		case V4L2_CID_EXPOSURE:
			tmp1 = (u32)ctrl->value;
			tmp1 *= 1000;
			result = afe_set_exposure_nsec(data->ad9923a, &data->regcfg, 
					data->current_pixel_clock, tmp1);
			if (0 == result) {
				tmp1 = afe_get_exposure_nsec(&data->regcfg, 
					data->current_pixel_clock);
				tmp1 += 500;
				tmp1 /= 1000;
				ctrl->value = (s32)tmp1;
			}
			hb_trace(i5ccdhb_to_device(data), "   V4L2_CID_EXPOSURE\n");
			break;
		case V4L2_CID_GAIN:
			hb_trace(i5ccdhb_to_device(data), "   V4L2_CID_GAIN\n");
			tmp1 = (u32)ctrl->value;
			tmp1 /= 1000;
			tmp2 = (u32)ctrl->value;
			tmp2 %= 1000;
			result = afe_set_gain(data->ad9923a, &data->regcfg, 
					tmp1, tmp2);
			if (0 == result) {
				afe_get_gain(&data->regcfg, &tmp1, &tmp2);
				tmp1 *= 1000;
				tmp1 += tmp2;
				ctrl->value = (s32)tmp1;
			}
			break;
		case V4L2_CID_SATURATION:
		case V4L2_CID_HUE:
		case V4L2_CID_AUTO_WHITE_BALANCE:
		case V4L2_CID_DO_WHITE_BALANCE:
		case V4L2_CID_RED_BALANCE:
		case V4L2_CID_BLUE_BALANCE:
		case V4L2_CID_GAMMA:
		case V4L2_CID_AUTOGAIN:
		case V4L2_CID_HFLIP:
		case V4L2_CID_VFLIP:
			hb_trace(i5ccdhb_to_device(data), "   ctrl not supported\n");
			result = -EINVAL;
			break;
		default:
			hb_trace(i5ccdhb_to_device(data), "   Default case\n");
			result = -EPERM;
			break;
		}
		i5ccdhb_unlock(data);
	}
	hb_trace(i5ccdhb_to_device(data), "%s: ---\n", __func__);
	return result;
}

static int i5ccdhb_streamoff(struct i5ccdhb_data *data)
{
	int result;
  
  //printk("________________%s___________\r\n",__func__);

/* # disable VDR */
	i5ccdhb_set_vdren( data, 0);

/* # afe soft reset */
	result = i5ad9923a_write_reg( data->ad9923a, afe_swreset_reg, 0x01 );
	
	data->streaming = 0;
	
	return result;
}

static int i5ccdhb_streamon(struct i5ccdhb_data *data)
{
	int result = i5ccdhb_setup_afe(data);
  //printk("________________%s___________\r\n",__func__);
	dev_info(i5ccdhb_to_device(data), "%s: i5ccdhb_setup_afe (%d)\n", __func__, result);
	if (0 == result) {
		result = afe_adapt_to_pixelclk( data->ad9923a, data->current_pixel_clock ); 
		dev_info(i5ccdhb_to_device(data), "%s: afe_adapt_to_pixelclk (%d)\n", __func__, result);
	}
	if (0 == result) {
		u32 num, frac, nsec;
		afe_get_fps( &data->regcfg, data->current_pixel_clock,
			&num, &frac );
		data->streamcap.timeperframe.numerator = 1000;
		data->streamcap.timeperframe.denominator = num * 1000 + frac % 1000;
		dev_info(i5ccdhb_to_device(data), "%s: fps (%u.%.3u)\n", __func__, num, frac);
		dev_info(i5ccdhb_to_device(data), "%s: tpf num %u  tpf denom %u)\n", 
				__func__, data->streamcap.timeperframe.numerator, 
				data->streamcap.timeperframe.denominator);
		
		afe_get_gain_min(&data->regcfg, &num, &frac);
		data->controls[qc_gain].minimum = (s32)(num * 1000 + frac % 1000);
		afe_get_gain_max(&data->regcfg, &num, &frac);
		data->controls[qc_gain].maximum = (s32)(num * 1000 + frac % 1000);

		nsec = afe_get_exposure_min_nsec( &data->regcfg, data->current_pixel_clock );
		data->controls[qc_exposure].minimum = (s32)((nsec + 500) / 1000);
		nsec = afe_get_exposure_max_nsec( &data->regcfg, data->current_pixel_clock );
		data->controls[qc_exposure].maximum = (s32)((nsec + 500) / 1000);
	}
	if (0 == result) {
		data->streaming = 1;
	} else {
		i5ccdhb_streamoff(data);
	}
	
	return result;
}

static int soc_i5ccdhb_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct i5ccdhb_data *d = v4l2_get_subdevdata(sd);
	dev_info(i5ccdhb_to_device(d), "%s - enable %i\n", __func__, enable);
    
  if (d->streaming == enable) {
    printk("_______________ ERROR %s___________\r\n",__func__ );
		return -1;
	} else if (!enable) {
		return i5ccdhb_streamoff(d);
	} else {
		return i5ccdhb_streamon(d);
	}
  
	return 0;
}


static int i5ccdhb_open(struct v4l2_subdev *subdev, struct v4l2_subdev_fh *fh)
{
  //printk("_________%s________\r\n",__func__);
//  return 0;

	struct v4l2_mbus_framefmt *format;
  
  struct i5ccdhb_data *i5ccdhb = v4l2_get_subdevdata(subdev);
  
	format = v4l2_subdev_get_try_format(fh, 0);
	format->code = V4L2_MBUS_FMT_Y12_1X12;
	format->width = I5CCDHB_WIDTH; //I5CCDHB_WINDOW_WIDTH_DEF;
	format->height = I5CCDHB_HEIGHT; //I5CCDHB_WINDOW_HEIGHT_DEF;
	format->field = V4L2_FIELD_NONE;
	format->colorspace = V4L2_COLORSPACE_REC709; //V4L2_COLORSPACE_SRGB;
  //i5ccdhb->format = format;
  
  return 0;
}

static int i5ccdhb_close(struct v4l2_subdev *subdev, struct v4l2_subdev_fh *fh)
{
  return 0;
}

static const struct v4l2_subdev_internal_ops i5ccdhb_subdev_internal_ops = {
  .open = i5ccdhb_open,
  .close = i5ccdhb_close,
};

#if 0
/*
 * isp_pipeline_link_notify - Link management notification callback
 * @source: Pad at the start of the link
 * @sink: Pad at the end of the link
 * @flags: New link flags that will be applied
 *
 * React to link management on powered pipelines by updating the use count of
 * all entities in the source and sink sides of the link. Entities are powered
 * on or off accordingly.
 *
 * Return 0 on success or a negative error code on failure. Powering entities
 * off is assumed to never fail. This function will not fail for disconnection
 * events.
 */
static int i5ccdhb_pipeline_link_notify(struct media_pad *source,
				    struct media_pad *sink, u32 flags)
{
  dev_info(i5ccdhb_to_device(data),"_________%s________",__func__);
  return 0;

	int source_use = isp_pipeline_pm_use_count(source->entity);
	int sink_use = isp_pipeline_pm_use_count(sink->entity);
	int ret;

	if (!(flags & MEDIA_LNK_FL_ENABLED)) {
		/* Powering off entities is assumed to never fail. */
		isp_pipeline_pm_power(source->entity, -sink_use);
		isp_pipeline_pm_power(sink->entity, -source_use);
		return 0;
	}

	ret = isp_pipeline_pm_power(source->entity, sink_use);
	if (ret < 0)
		return ret;

	ret = isp_pipeline_pm_power(sink->entity, source_use);
	if (ret < 0)
		isp_pipeline_pm_power(source->entity, -sink_use);

	return ret;

}
#endif
static int i5ccdhb_get_v4l2_master_entity_callback(struct device *dev, void *p)
{
	struct v4l2_device *v4l2_dev = dev_get_drvdata(dev);

  void **pp = p;
	
  /* test if this device was inited */
  if (v4l2_dev == NULL)
  {
    return 0;
  }
  dev_info( dev ,"Found master V4L2 device: %s (%p)\r\n",
           v4l2_dev->name, v4l2_dev);
  *pp = v4l2_dev;
  return 0;
}

int i5ccdhb_get_v4l2_master_entity(void *p)
{
  struct device_driver *drv;
  int err = 0;
	
  /* Find driver 'omap3isp' on the platform device bus.
     platform_bus_type is a global. */
  drv = driver_find("omap3isp", &platform_bus_type);
  
  if( NULL == drv )
  {
    printk("omap3isp not found...\r\n");
  }
  else
  {
    /* iterate over all omap3isp device instances */
    err = driver_for_each_device(drv, NULL, p, i5ccdhb_get_v4l2_master_entity_callback );
    put_driver(drv);
  }
  
  return err;
}

const struct v4l2_file_operations i5ccdhb_v4l2_fops = {
  .owner          = THIS_MODULE,
  .open           = i5ccdhb_open ,
};


static int i5ccdhb_get_ccdc_entity( struct v4l2_device *v4l2_dev ,
                                    struct media_entity **entity )
{
  struct v4l2_subdev *sd;
  
  list_for_each_entry(sd, &v4l2_dev->subdevs, list) {
    if( 0 == strncmp( sd->name , "OMAP3 ISP CCDC" , V4L2_SUBDEV_NAME_SIZE ) )
    {
      *entity = &sd->entity;
      return 0;
    }
  }
  return -1;
  
}

static struct isp_v4l2_subdevs_group isp_subdev_data;

static int i5ccdhb_register_v4l2_device( struct platform_device* pdevice )
{
  /* Platform data: static struct in "board-omap3beagle.c" */
  
  struct soc_i5ccdhb_platform_data *plat_data = pdevice->dev.platform_data;
	struct i5ccdhb_data* data = get_priv(pdevice);
  
  struct v4l2_device *v4l2_dev = NULL; 
  struct v4l2_subdev *sd = NULL;
  struct video_device *vdev;
  struct media_entity *ccdc_entity = NULL;

  int result = 0;
	int ret;
    
  struct isp_parallel_platform_data *isp_parallel_pdata = &isp_subdev_data.bus.parallel;

  /* Configure Parallel Platform Data */
  isp_parallel_pdata->data_lane_shift = 0;
  isp_parallel_pdata->clk_pol = 0x1; /* 0x1: Clock inverted. The data are sampled on the falling edge of the clock. */
  isp_parallel_pdata->hs_pol = 0;
  isp_parallel_pdata->vs_pol = 0;
  isp_parallel_pdata->bridge = 0;
  

  /* Get data of master v4l device ISP */
  i5ccdhb_get_v4l2_master_entity( &v4l2_dev );
  
  if( NULL == v4l2_dev )
  {
    printk("Could not get ISP instance!\r\n");
    return -EINVAL;
  }
  
  data->v4l2_dev = v4l2_dev;
  
  /* Initialize V4L2 subdev */
  v4l2_subdev_init(&data->subdev, &soc_i5ccdhb_subdev_ops);
  data->subdev.internal_ops = &i5ccdhb_subdev_internal_ops;
  data->subdev.host_priv = &isp_subdev_data;
  
  snprintf(data->subdev.name, V4L2_SUBDEV_NAME_SIZE, "I5CCDHB Sensor" );
  data->subdev.grp_id = 1 << 16; /* group ID for isp subdevs */

	v4l2_set_subdevdata(&data->subdev, data );
  	
  data->subdev.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
  data->subdev.entity.type = MEDIA_ENT_T_V4L2_SUBDEV_SENSOR;
  data->subdev.entity.ops = &i5ccdhb_media_ops;
  
  data->media_pad.flags = MEDIA_PAD_FL_SOURCE;
  
	ret = media_entity_init(&data->subdev.entity, 1, &data->media_pad, 0);
  
  if( 0 > ret )
  {
    printk("Could not init media entity\r\n");
    return -EINVAL;
  }
  
  /* Register V4L2 subdev with ISP */
	ret = v4l2_device_register_subdev(data->v4l2_dev, &data->subdev);
  if( 0 > ret )
  {
    printk("Could not register subdev with master v4l2 device\r\n");
    return -EINVAL;
  }
  
  /* Register subdev device node 
   * see:  v4l2_device_register_subdev_nodes()
   */
  sd = &data->subdev;
  vdev = &sd->devnode;
  strlcpy(vdev->name, sd->name, sizeof(vdev->name));
  vdev->v4l2_dev = v4l2_dev;
  vdev->fops = &v4l2_subdev_fops;
  vdev->release = video_device_release_empty;
  vdev->ctrl_handler = sd->ctrl_handler;
  ret = __video_register_device(vdev, VFL_TYPE_SUBDEV, -1, 1,
                                sd->owner);
  if (ret < 0)
    return ret;
#if defined(CONFIG_MEDIA_CONTROLLER)
  sd->entity.v4l.major = VIDEO_MAJOR;
  sd->entity.v4l.minor = vdev->minor;
#endif

  /* Get entity of ISP CCDC */
  if( 0!= i5ccdhb_get_ccdc_entity( v4l2_dev ,
                                   &ccdc_entity ) )
  {
    printk("Failed to find ccdc media entity\r\n");
    return -1;

  }
  
  /* Create link between I5CCDHB source pad and ISP CCDC sink pad */
  ret = media_entity_create_link( &data->subdev.entity, 0, ccdc_entity , 0 ,
                                  0);
  if( 0 > ret )
  {
    printk("Failed to setup link...\r\n");
    return -1;
  }
  
  return 0;
   

  return result;
}

/*!
 * i5ccdhb probe function
 * Function set in platform_driver struct.
 * Called by insmod i5ccdhb_camera.ko.
 *
 * @param pdevice            struct platform_device*
 * @return  Error code indicating success or failure
 */
static int i5ccdhb_probe(struct platform_device* pdevice)
{
	int result = 0;
	struct soc_i5ccdhb_platform_data *plat_data = 
		pdevice->dev.platform_data;
	struct i5ccdhb_data* data = 0;


	dev_info(&pdevice->dev, "%s >>>\n", __func__);

	if (!plat_data) {
		dev_err(&pdevice->dev,
			"Platform has not set platform data!\n");
		return -EINVAL;
	}

	/* Set initial values for the sensor struct. */
	data = kzalloc(sizeof(struct i5ccdhb_data), GFP_KERNEL);
	if (data == NULL) {
		dev_err(&pdevice->dev,
			"allocation error\n");
		result = -ENOMEM;
		return result;
	}
	hb_trace(&pdevice->dev, "%s: allocation\n", __func__);

  /* Request GPIOs */
	gpio_request( plat_data->vdr_en_gpio , "VDR_EN" );
	gpio_direction_output( plat_data->vdr_en_gpio , 0 );
	
  gpio_request( plat_data->sh_rdy_gpio , "SH_RDY" );
  gpio_direction_input( plat_data->sh_rdy_gpio );

  gpio_request( plat_data->sh_en_gpio , "SH_EN" );
	gpio_direction_output( plat_data->sh_en_gpio , 0 );
  
	mutex_init(&data->lock);
  	
	// TODO:
	// data->pdevice = container_of(get_device(&pdevice->dev), struct platform_device, dev);
	data->pdevice = pdevice;
  
  /* v4l2-subdev: control's drvdata points to private data */
	platform_set_drvdata(pdevice, data );
    
	// platform_set_drvdata(pdevice, data);
#if 0
	result = i5ccdhb_probe_fpga(data);
	hb_trace(&pdevice->dev, "%s: probed fpga (%d)\n", __func__, result);
	if (0 != result) {
		goto err_fpga;
	}
	
/*
	-> query FPGA ID
*/
	if (0 == result) {
		result = i5ccdhb_verify_fpga_id(data->fpga);
		hb_trace(&pdevice->dev, "%s: verified fpga (%d)\n", __func__, result);
	}
#endif
/*
	-> power up headboard
*/
	if (0 == result) {
		result = i5ccdhb_power_up( data );
		hb_trace(&pdevice->dev, "%s: powered hb (%d)\n", __func__, result);
	}
/*
	-> alloc i2c drivers
*/
	if (0 == result) {
		result = i5ccdhb_probe_i2c_devs(data);
		hb_trace(&pdevice->dev, "%s: probed hb i2c (%d)\n", __func__, result);
	}

  
/*
	-> read eeprom data
*/
	if (0 == result) {
		result = i5ccdhb_verify_headboard(data);
		hb_trace(&pdevice->dev, "%s: verifies hb data (%d)\n", __func__, result);
	}

  
/*
	-> read frequency
*/
	if (0 == result) {
		data->current_pixel_clock = i5ds1086l_get_frequency(data->ds1086l);
		data->requested_pixel_clock = data->current_pixel_clock;
		dev_info(&pdevice->dev, "%s: read freq %d\n", __func__, data->current_pixel_clock);
		if (data->current_pixel_clock <= 0) {
			result = -1;
		}
	}
alloc_spi:
/*
	-> alloc spi driver
*/
	if (0 == result) {
		result = i5ccdhb_probe_spi_devs(data);
		dev_info(&pdevice->dev, "%s: probed hb spi (%d)\n", __func__, result);
	}

	if (0 != result) {
		goto err_subdev;
	}
  
  /* # afe soft reset */
  result = i5ad9923a_write_reg( data->ad9923a, afe_swreset_reg, 0x01 );
  
	data->streamcap.capability = V4L2_CAP_TIMEPERFRAME;
	data->streamcap.capturemode = 0;
	data->streamcap.timeperframe.denominator = 1;
	data->streamcap.timeperframe.numerator = 1;
  
	data->format.code = V4L2_MBUS_FMT_Y12_1X12;
	data->format.width = I5CCDHB_WIDTH; //I5CCDHB_WINDOW_WIDTH_DEF;
	data->format.height = I5CCDHB_HEIGHT; //I5CCDHB_WINDOW_HEIGHT_DEF;
	data->format.field = V4L2_FIELD_NONE;
	data->format.colorspace = V4L2_COLORSPACE_REC709; //V4L2_COLORSPACE_SRGB;

/*
	-> parse afe setup
*/
	if (0 == result) {
		result = i5ccdhb_headboard_reset(data);
	}
	if (0 == result) {
		result = i5ccdhb_parse_setup(data);
	}

  
  if (0 == result) {
    result = i5ccdhb_headboard_init(data);
  }
		
	if (0 != result) {
		goto err_afesetup;
	}
  
	result = i5ccdhb_register_attributes(data);
  
  /* Connect driver to media controller framework */
	if (0 == result) {

    result = i5ccdhb_register_v4l2_device( pdevice );
    
		if (result)
      goto err_v4l2reg;

	}

	dev_info(&pdevice->dev, "%s (%i) <<<\n", __func__, result);
	return result;

err_afesetup:

err_v4l2reg:
	platform_set_drvdata(pdevice, NULL);
err_subdev:
	if (NULL != data) {
/*
	-> power down headboard
*/
			i5ccdhb_power_down(data);

/*
	-> remove spi driver
*/
		i5ccdhb_remove_spi_devs(data);
/*
	-> remove i2c driver
*/
		i5ccdhb_remove_i2c_devs(data);
	}
err_fpga:
	if (NULL != data) {
		if (NULL != data->pdevice) {
			put_device(&data->pdevice->dev);
		}
		kfree(data);
	}
	dev_info(&pdevice->dev, "%s (fail) <<<\n", __func__);
	return result;
}

/*!
 * i5ccdhb detach function
 * Called on rmmod i5ccdhb_camera.ko
 *
 * @param client            struct i2c_client*
 * @return  Error code indicating success or failure
 */
static int i5ccdhb_remove(struct platform_device* pdevice)
{
	struct soc_i5ccdhb_platform_data *plat_data = pdevice->dev.platform_data;
	struct i5ccdhb_data* data = get_priv(pdevice);

	v4l2_device_unregister_subdev(&data->subdev);

	i5ccdhb_remove_attributes(data);
	put_device(&data->pdevice->dev);
/*
	-> power down headboard
*/
	i5ccdhb_power_down(data);
/*
	-> remove spi driver
*/
	i5ccdhb_remove_spi_devs(data);
/*
	-> remove i2c driver
*/
	i5ccdhb_remove_i2c_devs(data);
	
	platform_set_drvdata(pdevice, NULL);
	kfree(data);
	data= NULL;

	dev_info(&pdevice->dev, "%s <<<\n", __func__);
	return 0;
}

static int i5ccdhb_suspend (struct platform_device *pdev, pm_message_t state)
{
	int result = 0; 
	dev_info(&pdev->dev, "%s [%#04x] - todo >>>\n", __func__, state.event);
	dev_info(&pdev->dev, "%s <<<\n", __func__);
	return result;
}

static int i5ccdhb_resume(struct platform_device *pdev)
{
	int result = 0;
	dev_info(&pdev->dev, "%s  - todo >>>\n", __func__);
	dev_info(&pdev->dev, "%s <<<\n", __func__);
	return result;
}

struct platform_driver i5ccdhb_driver = {
	.driver = {
		   .owner = THIS_MODULE,
		   .name = "i5ccdhb",
		  },

	.probe = i5ccdhb_probe,
	.remove = i5ccdhb_remove,
/* suspend / resume should be done within s_power, but not called from soc code */
	.suspend = i5ccdhb_suspend,
	.resume = i5ccdhb_resume,

/*
	void (*shutdown)(struct platform_device *);
	int (*suspend_late)(struct platform_device *, pm_message_t state);
	int (*resume_early)(struct platform_device *);
*/
};


/*!
 * i5ccdhb init function
 * Called by insmod i5ccdhb_camera.ko.
 *
 * @return  Error code indicating success or failure
 */
static __init int i5ccdhb_init(void)
{
	u8 err;

	pr_info("%s >>>\n", __func__);

	err = platform_driver_register(&i5ccdhb_driver);
	if (err != 0)
		printk(KERN_ERR "%s: driver registration failed, error=%d \n",
			__func__, err);

	pr_info("%s <<<\n", __func__);

	return err;
}

/*!
 * IROQ5 CCD HB cleanup function
 * Called on rmmod i5ccdhb_camera.ko
 *
 * @return  Error code indicating success or failure
 */
static void __exit i5ccdhb_clean(void)
{
	//printk(KERN_INFO "%s >>>\n", __func__);
	platform_driver_unregister(&i5ccdhb_driver);
	//printk(KERN_INFO "%s <<<\n", __func__);
}

/* force init after all devices are done */
//module_init(i5ccdhb_init);
late_initcall(i5ccdhb_init);
/* need it during init of soc_camera !!! */
//device_initcall(i5ccdhb_init); /*-- prev!!*/
module_exit(i5ccdhb_clean);

MODULE_AUTHOR("TQ Systems GmbH");
MODULE_DESCRIPTION("IROQ5 CCD Headboard Camera Driver");
MODULE_LICENSE("GPL");
