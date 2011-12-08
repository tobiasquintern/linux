#ifndef __I5CCDHB_INTERNAL_H__
#define __I5CCDHB_INTERNAL_H__

#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/videodev2.h>
#include <media/v4l2-subdev.h>
#include <media/media-device.h>
#include <linux/spi/spi.h>


#include "i5ad9923a.h"


enum {
	qc_gain = 0,
	qc_contrast,
	qc_brightness,
	qc_exposure,
	qc_count,
};

/*!
 * Maintains the information on the current state of the headboard.
 */
struct soc_i5ccdhb_data {
	struct i2c_client *lm73;
	struct i2c_client *at24;
	struct i2c_client *ds1086l;

	struct spi_device *ad9923a;
	//struct spi_device *fpga;

	struct platform_device *pdevice;
	struct v4l2_subdev subdev;
  struct v4l2_device *v4l2_dev; /* V4L2 Master, will be probed */
  //struct media_device media_dev;
  struct media_pad media_pad;
	
	//struct v4l2_mbus_framefmt mbus_fmt; /* OBSOLTE?*/
	struct v4l2_captureparm streamcap; /* OBSOLETE? */
	struct v4l2_queryctrl controls[qc_count];
	//struct v4l2_rect croprect;


  struct v4l2_mbus_framefmt format;
	struct v4l2_rect crop;

	struct memory_accessor *at24_macc;
	u8 at24_data[256];
	
	u32 current_pixel_clock;
	u32 requested_pixel_clock;
	u8 bincfg[afe_raw_data_size];
	size_t regcnt;
	struct afe_reg afe_regs[afe_max_reg_count];
	struct afe_reg_conf regcfg;

	struct mutex lock;
  struct mutex power_lock;

  s32 power_count;
  
	bool streaming;
};

#endif /* __I5CCDHB_INTERNAL_H__ */
