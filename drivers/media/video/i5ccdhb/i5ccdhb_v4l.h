#ifndef __I5CCDHB_V4L_H__
#define __I5CCDHB_V4L_H__

#include <linux/types.h>

#define I5CCDHB_WIDTH               784
#define I5CCDHB_HEIGHT              524


int i5ccdhb_get_format(struct v4l2_subdev *subdev,
                       struct v4l2_subdev_fh *fh,
                       struct v4l2_subdev_format *format);

int i5ccdhb_set_format(struct v4l2_subdev *subdev,
                       struct v4l2_subdev_fh *fh,
                       struct v4l2_subdev_format *format);

int i5ccdhb_enum_mbus_code(struct v4l2_subdev *subdev,
                           struct v4l2_subdev_fh *fh,
                           struct v4l2_subdev_mbus_code_enum *code);

int i5ccdhb_enum_frame_size(struct v4l2_subdev *subdev,
                            struct v4l2_subdev_fh *fh,
                            struct v4l2_subdev_frame_size_enum *fse);


#endif /* __I5CCDHB_V4L_H__ */
