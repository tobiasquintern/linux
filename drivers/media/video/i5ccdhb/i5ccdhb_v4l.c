#include <linux/types.h>
#include <media/v4l2-subdev.h>


#include "i5ccdhb_v4l.h"
#include "i5ccdhb_internal.h"


/* -----------------------------------------------------------------------------
 * V4L2 subdev video operations
 */

static struct v4l2_mbus_framefmt *
__i5ccdhb_get_pad_format(struct i5ccdhb_data *i5ccdhb, struct v4l2_subdev_fh *fh,
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
  if( (fse->index > 0) || (fse->code != V4L2_MBUS_FMT_Y12_1X12 ) )
  {
		return -EINVAL;
  }
  
  fse->min_width = I5CCDHB_WIDTH;
	fse->max_width = fse->min_width;
	fse->min_height = I5CCDHB_HEIGHT;
	fse->max_height = fse->min_height;

	return 0;
}

int i5ccdhb_get_format(struct v4l2_subdev *subdev,
                       struct v4l2_subdev_fh *fh,
                       struct v4l2_subdev_format *format)
{
	struct i5ccdhb_data *i5ccdhb = v4l2_get_subdevdata(subdev);

  format->format = *__i5ccdhb_get_pad_format(i5ccdhb, fh, format->pad,
                                             format->which);
  
	return 0;
}


int i5ccdhb_set_format(struct v4l2_subdev *subdev,
                       struct v4l2_subdev_fh *fh,
                       struct v4l2_subdev_format *format)
{
	struct i5ccdhb_data *i5ccdhb = v4l2_get_subdevdata(subdev);
	struct v4l2_mbus_framefmt *__format;

	__format = __i5ccdhb_get_pad_format(i5ccdhb, fh, format->pad,
                                      format->which);
	__format->width = I5CCDHB_WIDTH;
  __format->height = I5CCDHB_HEIGHT;
  __format->code = V4L2_MBUS_FMT_Y12_1X12; 

	format->format = *__format;
  i5ccdhb->format = *__format;

	return 0;
}
