#include <linux/types.h>
#include <media/v4l2-subdev.h>


#include "i5ccdhb_v4l.h"
#include "i5ccdhb_internal.h"


/* -----------------------------------------------------------------------------
 * V4L2 subdev video operations
 */

static struct v4l2_mbus_framefmt *
__i5ccdhb_get_pad_format(struct soc_i5ccdhb_data *i5ccdhb, struct v4l2_subdev_fh *fh,
			 unsigned int pad, enum v4l2_subdev_format_whence which)
{
	switch (which) {
	case V4L2_SUBDEV_FORMAT_TRY:
  {
    return v4l2_subdev_get_try_format(fh, pad);
  }
	case V4L2_SUBDEV_FORMAT_ACTIVE:
  {
    return &i5ccdhb->format;
  }
	default:
  {
    return NULL;
  }
	}
}

static struct v4l2_rect *
__i5ccdhb_get_pad_crop(struct soc_i5ccdhb_data *i5ccdhb, struct v4l2_subdev_fh *fh,
		       unsigned int pad, enum v4l2_subdev_format_whence which)
{
	switch (which) {
	case V4L2_SUBDEV_FORMAT_TRY:
		return v4l2_subdev_get_try_crop(fh, pad);
	case V4L2_SUBDEV_FORMAT_ACTIVE:
		return &i5ccdhb->crop;
	default:
		return NULL;
	}
}


int i5ccdhb_enum_mbus_code(struct v4l2_subdev *subdev,
                           struct v4l2_subdev_fh *fh,
                           struct v4l2_subdev_mbus_code_enum *code)
{
  if (code->index > 0)
		return -EINVAL;

	code->code = V4L2_MBUS_FMT_Y12_1X12;
	return 0;
}

int i5ccdhb_enum_frame_size(struct v4l2_subdev *subdev,
                            struct v4l2_subdev_fh *fh,
                            struct v4l2_subdev_frame_size_enum *fse)
{
  if (fse->index >= 8 || fse->code != V4L2_MBUS_FMT_SGRBG10_1X10)
		return -EINVAL;
  
	fse->min_width = I5CCDHB_WINDOW_WIDTH_DEF / fse->index;
	fse->max_width = fse->min_width;
	fse->min_height = I5CCDHB_WINDOW_HEIGHT_DEF / fse->index;
	fse->max_height = fse->min_height;

	return 0;
}

int i5ccdhb_get_format(struct v4l2_subdev *subdev,
                       struct v4l2_subdev_fh *fh,
                       struct v4l2_subdev_format *format)
{
	struct soc_i5ccdhb_data *i5ccdhb = v4l2_get_subdevdata(subdev);

  format->format = *__i5ccdhb_get_pad_format(i5ccdhb, fh, format->pad,
                                             format->which);
  
	return 0;
}


int i5ccdhb_set_format(struct v4l2_subdev *subdev,
                       struct v4l2_subdev_fh *fh,
                       struct v4l2_subdev_format *format)
{
	struct soc_i5ccdhb_data *i5ccdhb = v4l2_get_subdevdata(subdev);
	struct v4l2_mbus_framefmt *__format;
	struct v4l2_rect *__crop;
	unsigned int width;
	unsigned int height;
	unsigned int hratio;
	unsigned int vratio;


	__crop = __i5ccdhb_get_pad_crop(i5ccdhb, fh, format->pad,
					format->which);

	/* Clamp the width and height to avoid dividing by zero. */
	width = clamp_t(unsigned int, ALIGN(format->format.width, 2),
			max(__crop->width / 8, I5CCDHB_WINDOW_WIDTH_MIN),
			__crop->width);
	height = clamp_t(unsigned int, ALIGN(format->format.height, 2),
			 max(__crop->height / 8, I5CCDHB_WINDOW_HEIGHT_MIN),
			 __crop->height);

	hratio = DIV_ROUND_CLOSEST(__crop->width, width);
	vratio = DIV_ROUND_CLOSEST(__crop->height, height);

	__format = __i5ccdhb_get_pad_format(i5ccdhb, fh, format->pad,
					    format->which);
	__format->width = I5CCDHB_WIDTH; //1024; //__crop->width / hratio;
  __format->height = I5CCDHB_HEIGHT; // 768; //__crop->height / vratio;
  __format->code = V4L2_MBUS_FMT_Y12_1X12; 

	format->format = *__format;
  i5ccdhb->format = *__format;

	return 0;
}

static int i5ccdhb_get_crop(struct v4l2_subdev *subdev,
			    struct v4l2_subdev_fh *fh,
			    struct v4l2_subdev_crop *crop)
{
	struct soc_i5ccdhb_data *i5ccdhb = v4l2_get_subdevdata(subdev);

	crop->rect = *__i5ccdhb_get_pad_crop(i5ccdhb, fh, crop->pad,
					     crop->which);
	return 0;
}

