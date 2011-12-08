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
 * @file i5ad9923a.h
 *
 * @brief iroq5 AD9923A analog frontend subdriver for IROQ5 CCD headboard
 *
 * @ingroup Camera
 */


#ifndef __I5AD9923A_H__
#define __I5AD9923A_H__

#include <linux/types.h>

struct spi_device;

extern int i5ad9923a_write_reg(struct spi_device *sdev, u16 address, u32 data);

struct afe_reg_conf {
	u32 afe_vpat_cnt;
	u32 afe_vseq_cnt;
	u32 afe_field_cnt;
	u32 afe_cdsgain_cnt;
	u32 afe_lines_cnt;
	u32 afe_pixels_cnt;
	u32 afe_subck_cnt;
	u32 afe_subck_max;
	u32 afe_gain_cnt;
	u32 afe_clamp_cnt;
	u32 afe_pixel_hdrise;
	u32 afe_lines_vdrise;
};

struct afe_reg {
	u16 address;
	u32 value;
};

enum {
	afe_cdsgain_reg     = 0x04,
	afe_vgagain_reg     = 0x05,
	afe_clamplevel_reg  = 0x06,
	afe_swreset_reg     = 0x10,
	afe_sync_reg      = 0x013,
	afe_hdvdrise_reg  = 0x022,

	afe_hdr1time_reg   = 0x31,
	afe_rgtime_reg     = 0x34,

	afe_vpat_reg      = 0x090,
	afe_vseq_reg      = 0x091,
	afe_gain_reg      = 0x005,
	afe_clamp_reg     = 0x006,

	afe_trigger_reg      = 0x61,
	afe_longshutter_reg  = 0x63,
	afe_shutter_reg   = 0x064,

	vdr_trigger   = 0x0ea,
};

enum {
	afe_vdhdrise_m_vdrise = 0x01ffe000,
	afe_vdhdrise_s_vdrise = 13,
	afe_vdhdrise_m_hdrise = 0x01fff,
	afe_vdhdrise_s_hdrise = 0,
/*
  afe_gaindb_base      = 5.5,
  afe_gaindb_scale     = 0.0358,
*/
	afe_pixel_delay       = 16,
	afe_pixel_delay_extra = 2,

	afe_lines_vdrise_def  = 6,
	afe_pixel_hdrise_def  = 78,

	afe_vpat_base		 = 0x400,
	afe_vpat_regcnt   =   40,
	afe_vseq_regcnt   =   20,
	afe_field_regcnt  =   0x00C,

	afe_fld_scp8vdlen_reg     =     0x007,
	afe_fld_scp8vdlen_s_vdlen =        13,
	afe_fld_scp8vdlen_m_vdlen = 0x1ffe000,
	afe_fld_scp8vdlen_s_scp8  =         0,
	afe_fld_scp8vdlen_m_scp8  = 0x0000fff,

	afe_frequency_min  =  5000000,
	afe_frequency_max  = 25000000,
	afe_frquency_mhz_min = afe_frequency_min / (1000 * 1000),
	afe_frquency_mhz_max = afe_frequency_max / (1000 * 1000),
	afe_frequency_lookup_offset = 5,
};

enum {
/* # total line (dummy + dark + active) per frame */
	ccd_lines_per_frame  = 525,
	ccd_lines_vsg        =  1,
	ccd_lines_dummy_front = 5,
	ccd_lines_black_front = 8,
	ccd_lines_active      = 494,
	ccd_lines_black_back  = 2,
	ccd_lines_total        = ccd_lines_black_front +
				ccd_lines_black_back +
				ccd_lines_active,

/* # total pixel (dummy + dark + active + sync) per line */
	ccd_pix_per_line      = 780,

	ccd_pixel_dummy_pos   = 107,
	ccd_pixel_dummy_front =  16,
	ccd_pixel_black_front =   2,
	ccd_pixel_active      = 659,
	ccd_pixel_black_back  =  31,
	ccd_pixel_total       = ccd_pixel_black_front +
				ccd_pixel_black_back +
				ccd_pixel_active,


/*
# default number of vertical pattern
*/
	ccd_vpat_cntdef        = 2,
//	ccd_vpat_cnt           = ccd_vpat_cntdef,
	afe_vpat_max           = 31,

/* # default number of sequences */
	ccd_vseq_cntdef        = 3,
// ccd_vseq_cnt           = ccd_vseq_cntdef,
	afe_vseq_max           = 31,

/* # default number of fields */
	ccd_field_cnt_def      = 1,
	afe_field_min          = 1,
	afe_field_max          = 6,

	afe_lines_min          = ccd_lines_per_frame,
	afe_lines_max          = 4095,

	afe_subck_min          = 1,

	afe_cdsgain_min        = 0,
	afe_cdsgain_max        = 3,
	afe_cdsgain_def        = afe_cdsgain_max,

	afe_gain_min           = 0,
	afe_gain_max           = 1023,
	afe_gain_def           = 15,

	afe_clamp_min          = 0,
	afe_clamp_max          = 1023,
	afe_clamp_def          = 64,
// afe_clamp_cnt          = afe_clamp_min,
};

enum
{
	afe_raw_data_size = 4096,
	afe_max_reg_count = 256,

	afe_raw_line_length = 13,
};

extern const u8 afe_default_regs[];

extern const size_t afe_default_regs_bytes;

static inline 
u32 afe_get_pixelclk_mhz( u32 frequency )
{
	return (frequency + (1000000 / 2)) / 1000000;
}

static inline u32 afe_get_pixelclk_min_mhz(void)
{
	return afe_get_pixelclk_mhz(afe_frequency_min);
}

static inline u32 afe_get_pixelclk_max_mhz(void)
{
	return afe_get_pixelclk_mhz(afe_frequency_max);
}

extern void afe_reset_config(struct afe_reg_conf* regcfg);

extern int afe_adapt_to_pixelclk( struct spi_device* sdev, u32 frequency );

extern int afe_parse_regs(const u8* rawdata, size_t rawsize, 
			struct afe_reg* regs, size_t max_regs);

extern int afe_get_setup(const struct afe_reg* regs, size_t reg_cnt,
			struct afe_reg_conf* reg_config);

static inline u32 afe_get_line_time_nsec(u32 frequency)
{
	u64 ulen = (u64)ccd_pix_per_line * (u64)1000 * (u64)1000 * (u64)1000;
	u64 result = div64_u64((ulen + (u64)(frequency / 2)), (u64)frequency);
	return result & 0xffffffff;
}

static inline u32 afe_get_time_nsec( u32 frequency, u32 lines )
{
	return afe_get_line_time_nsec(frequency) * lines;
}

static inline u32 afe_get_frame_time_nsec( struct afe_reg_conf *regcfg, 
				u32 frequency )
{
	return afe_get_time_nsec( frequency, regcfg->afe_lines_cnt );
}

static inline u32 afe_get_exposure_min_nsec( struct afe_reg_conf *regcfg, 
				u32 frequency )
{
	return afe_get_time_nsec( frequency, 1 );
}

static inline u32 afe_get_exposure_max_nsec( struct afe_reg_conf *regcfg, 
				u32 frequency )
{
	return afe_get_time_nsec( frequency, regcfg->afe_lines_cnt - 
				afe_subck_min );
}

static inline u32 afe_get_exposure_nsec( struct afe_reg_conf *regcfg, 
					u32 frequency )
{
	return afe_get_time_nsec( frequency, regcfg->afe_lines_cnt - 
				regcfg->afe_subck_cnt );
}

/*
static inline u32 afe_get_exposure_msec( struct afe_reg_conf *regcfg, u32 frequency )
{
	u32 nsec = afe_get_exposure_nsec(regcfg, frequency);
	return (nsec + (500 * 1000)) / (1000 * 1000);
}
*/

extern int afe_set_exposure( struct spi_device *spi, struct afe_reg_conf *regcfg, 
			u32 value );
extern int afe_set_exposure_nsec( struct spi_device *spi, struct afe_reg_conf *regcfg, 
			u32 frequency, u32 nsec );
/*
extern int afe_set_exposure_msec( struct spi_device *spi, struct afe_reg_conf *regcfg, 
			u32 frequency, u32 msec );
*/

extern int afe_set_fps( struct spi_device *sdev, struct afe_reg_conf *regcfg, 
		u32 frequency, u32 fps, u32 frac);
extern void afe_get_fps( struct afe_reg_conf *regcfg, u32 frequency, u32* num, 
		u32* frac );
extern void afe_get_min_fps ( u32 frequency, u32* num, u32* frac );
extern void afe_get_max_fps ( u32 frequency, u32* num, u32* frac );

extern int afe_set_gain( struct spi_device *sdev, struct afe_reg_conf *regcfg, 
		u32 gain_num, u32 gain_frac);

extern void afe_get_gain( struct afe_reg_conf *regcfg, u32 *gain_num, u32 *gain_frac);
extern void afe_get_gain_max( struct afe_reg_conf *regcfg, u32 *gain_num, u32 *gain_frac);
extern void afe_get_gain_min( struct afe_reg_conf *regcfg, u32 *gain_num, u32 *gain_frac);

extern int afe_set_cdsgain( struct spi_device *spi, struct afe_reg_conf *regcfg,
                         u32 value );
static inline u32 afe_get_cdsgain(struct afe_reg_conf *regcfg)
{
	return regcfg->afe_cdsgain_cnt;
}

extern int afe_set_clamp( struct spi_device *spi, struct afe_reg_conf *regcfg,
                            u32 value );
static inline u32 afe_get_clamp(struct afe_reg_conf *regcfg)
{
	return regcfg->afe_clamp_cnt;
}

#endif

