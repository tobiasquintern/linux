#ifndef __I5CCDHB_V4L_H__
#define __I5CCDHB_V4L_H__

#include <linux/types.h>



#define I5CCDHB_PIXEL_ARRAY_HEIGHT			492
#define I5CCDHB_PIXEL_ARRAY_WIDTH			782

#define I5CCDHB_CHIP_VERSION				0x00
#define		I5CCDHB_CHIP_ID_REV1			0x1311
#define		I5CCDHB_CHIP_ID_REV3			0x1313
#define I5CCDHB_COLUMN_START				0x01
#define		I5CCDHB_COLUMN_START_MIN		1
#define		I5CCDHB_COLUMN_START_DEF		1
#define		I5CCDHB_COLUMN_START_MAX		752
#define I5CCDHB_ROW_START				0x02
#define		I5CCDHB_ROW_START_MIN			4
#define		I5CCDHB_ROW_START_DEF			5
#define		I5CCDHB_ROW_START_MAX			482
#define I5CCDHB_WINDOW_HEIGHT				0x03
#define		I5CCDHB_WINDOW_HEIGHT_MIN		1
#define		I5CCDHB_WINDOW_HEIGHT_DEF		480
#define		I5CCDHB_WINDOW_HEIGHT_MAX		480
#define I5CCDHB_WINDOW_WIDTH				0x04
#define		I5CCDHB_WINDOW_WIDTH_MIN		1
#define		I5CCDHB_WINDOW_WIDTH_DEF		752
#define		I5CCDHB_WINDOW_WIDTH_MAX		752
#define I5CCDHB_HORIZONTAL_BLANKING			0x05
#define		I5CCDHB_HORIZONTAL_BLANKING_MIN		43
#define		I5CCDHB_HORIZONTAL_BLANKING_MAX		1023
#define I5CCDHB_VERTICAL_BLANKING			0x06
#define		I5CCDHB_VERTICAL_BLANKING_MIN		4
#define		I5CCDHB_VERTICAL_BLANKING_MAX		3000
#define I5CCDHB_CHIP_CONTROL				0x07
#define		I5CCDHB_CHIP_CONTROL_MASTER_MODE	(1 << 3)
#define		I5CCDHB_CHIP_CONTROL_DOUT_ENABLE	(1 << 7)
#define		I5CCDHB_CHIP_CONTROL_SEQUENTIAL		(1 << 8)
#define I5CCDHB_SHUTTER_WIDTH1				0x08
#define I5CCDHB_SHUTTER_WIDTH2				0x09
#define I5CCDHB_SHUTTER_WIDTH_CONTROL			0x0a
#define I5CCDHB_TOTAL_SHUTTER_WIDTH			0x0b
#define		I5CCDHB_TOTAL_SHUTTER_WIDTH_MIN		1
#define		I5CCDHB_TOTAL_SHUTTER_WIDTH_DEF		480
#define		I5CCDHB_TOTAL_SHUTTER_WIDTH_MAX		32767
#define I5CCDHB_RESET					0x0c
#define I5CCDHB_READ_MODE				0x0d
#define		I5CCDHB_READ_MODE_ROW_BIN_MASK		(3 << 0)
#define		I5CCDHB_READ_MODE_ROW_BIN_SHIFT		0
#define		I5CCDHB_READ_MODE_COLUMN_BIN_MASK	(3 << 2)
#define		I5CCDHB_READ_MODE_COLUMN_BIN_SHIFT	2
#define		I5CCDHB_READ_MODE_ROW_FLIP		(1 << 4)
#define		I5CCDHB_READ_MODE_COLUMN_FLIP		(1 << 5)
#define		I5CCDHB_READ_MODE_DARK_COLUMNS		(1 << 6)
#define		I5CCDHB_READ_MODE_DARK_ROWS		(1 << 7)
#define I5CCDHB_PIXEL_OPERATION_MODE			0x0f
#define		I5CCDHB_PIXEL_OPERATION_MODE_COLOR	(1 << 2)
#define		I5CCDHB_PIXEL_OPERATION_MODE_HDR	(1 << 6)
#define I5CCDHB_ANALOG_GAIN				0x35
#define		I5CCDHB_ANALOG_GAIN_MIN			16
#define		I5CCDHB_ANALOG_GAIN_DEF			16
#define		I5CCDHB_ANALOG_GAIN_MAX			64
#define I5CCDHB_MAX_ANALOG_GAIN				0x36
#define		I5CCDHB_MAX_ANALOG_GAIN_MAX		127
#define I5CCDHB_FRAME_DARK_AVERAGE			0x42
#define I5CCDHB_DARK_AVG_THRESH				0x46
#define		I5CCDHB_DARK_AVG_LOW_THRESH_MASK	(255 << 0)
#define		I5CCDHB_DARK_AVG_LOW_THRESH_SHIFT	0
#define		I5CCDHB_DARK_AVG_HIGH_THRESH_MASK	(255 << 8)
#define		I5CCDHB_DARK_AVG_HIGH_THRESH_SHIFT	8
#define I5CCDHB_ROW_NOISE_CORR_CONTROL			0x70
#define		I5CCDHB_ROW_NOISE_CORR_ENABLE		(1 << 5)
#define		I5CCDHB_ROW_NOISE_CORR_USE_BLK_AVG	(1 << 7)
#define I5CCDHB_PIXEL_CLOCK				0x74
#define		I5CCDHB_PIXEL_CLOCK_INV_LINE		(1 << 0)
#define		I5CCDHB_PIXEL_CLOCK_INV_FRAME		(1 << 1)
#define		I5CCDHB_PIXEL_CLOCK_XOR_LINE		(1 << 2)
#define		I5CCDHB_PIXEL_CLOCK_CONT_LINE		(1 << 3)
#define		I5CCDHB_PIXEL_CLOCK_INV_PXL_CLK		(1 << 4)
#define I5CCDHB_TEST_PATTERN				0x7f
#define		I5CCDHB_TEST_PATTERN_DATA_MASK		(1023 << 0)
#define		I5CCDHB_TEST_PATTERN_DATA_SHIFT		0
#define		I5CCDHB_TEST_PATTERN_USE_DATA		(1 << 10)
#define		I5CCDHB_TEST_PATTERN_GRAY_MASK		(3 << 11)
#define		I5CCDHB_TEST_PATTERN_GRAY_NONE		(0 << 11)
#define		I5CCDHB_TEST_PATTERN_GRAY_VERTICAL	(1 << 11)
#define		I5CCDHB_TEST_PATTERN_GRAY_HORIZONTAL	(2 << 11)
#define		I5CCDHB_TEST_PATTERN_GRAY_DIAGONAL	(3 << 11)
#define		I5CCDHB_TEST_PATTERN_ENABLE		(1 << 13)
#define		I5CCDHB_TEST_PATTERN_FLIP		(1 << 14)
#define I5CCDHB_AEC_AGC_ENABLE				0xaf
#define		I5CCDHB_AEC_ENABLE			(1 << 0)
#define		I5CCDHB_AGC_ENABLE			(1 << 1)
#define I5CCDHB_THERMAL_INFO				0xc1


#define I5CCDHB_WIDTH 784
#define I5CCDHB_HEIGHT 524

int i5ccdhb_v4l_test( void );


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
