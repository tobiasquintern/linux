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
 * @file i5ccdhb_afehlpr.c
 *
 * @brief iroq5 ccd headboard camera driver afe helper
 *
 * @ingroup Camera
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/ctype.h>
#include <linux/device.h>
#include <linux/spi/spi.h>

#include "i5ad9923a.h"

/*#undef  pr_info(fmt, ...)
  #define pr_info(fmt, ...)do { } while(0)*/

IMPORT_SYMBOL(i5ad9923a_write_reg);

enum
{
	afe_setup_none      = 0x0000,
	afe_setup_gain      = 0x0001,
	afe_setup_clamp     = 0x0002,
	afe_setup_subck     = 0x0004,
	afe_setup_vseq      = 0x0008,
	afe_setup_vpat      = 0x0010,
	afe_setup_hdvdrise  = 0x0020,
	afe_setup_cdsgain   = 0x0040,
	afe_setup_lines     = 0x0080,

	afe_setup_all       = 0x00ff,
};


static const u32 afe_rg_lookup[] = {
	0x10C3A,
	0x10B39,
	0x10B39,
	0x10A38,
	0x10A38,
	0x10937,
	0x10937,
	0x10836,
	0x10836,
	0x10735,
	0x10735,
	0x10634,
	0x10634,
	0x10533,
	0x10533,
	0x10432,
	0x10432,
	0x10331,
	0x10331,
	0x10230,
	0x10230,
	0x10230,
	0x13D2B,
	0x13D2B,
	0x13C2A,
	0x13C2A,
};

static const u32 afe_hdr_lookup[] = {
	0x11A3A,
	0x11939,
	0x11939,
	0x11838,
	0x11838,
	0x11737,
	0x11737,
	0x11636,
	0x11636,
	0x11535,
	0x11535,
	0x11434,
	0x11434,
	0x11333,
	0x11333,
	0x11232,
	0x11232,
	0x11131,
	0x11131,
	0x11030,
	0x11030,
	0x11030,
	0x10B2B,
	0x10B2B,
	0x10A2A,
	0x10A2A,
};

static u32 afe_get_hdr_lookup( u32 frequency )
{
	u32 result = 0;
	if ( (afe_frequency_max >= frequency) && (afe_frequency_min <= frequency) ) {
		u32 freq_mhz = afe_get_pixelclk_mhz(frequency);
		result = afe_hdr_lookup[freq_mhz - afe_frequency_lookup_offset];
		pr_info("%s: %u Hz -> %u MHz => hdr = %x\n", __func__, frequency, freq_mhz, result);
	}
	return result;
}

static u32 afe_get_rg_lookup( u32 frequency )
{
	u32 result = 0;
	if ( (afe_frequency_max >= frequency) && (afe_frequency_min <= frequency) ) {
		u32 freq_mhz = afe_get_pixelclk_mhz(frequency);
		result = afe_rg_lookup[freq_mhz - afe_frequency_lookup_offset];
		pr_info("%s: %u Hz -> %u MHz => rg = %x\n", __func__, frequency, freq_mhz, result);
	}
	return result;
}

enum {
	afe_gain_lookup_scale = 1000,
	afe_gain_small_lookup_step  = 100,
	afe_gain_big_lookup_step = 250,
	afe_gain_small_lookup_start = 1800,
	afe_gain_small_lookup_end = 10000,
	afe_gain_big_lookup_start = afe_gain_small_lookup_end,
	afe_gain_big_lookup_end = 127000,
};

static const u32 afe_gainreg_lookup_small[] = {
	0,
	2,
	15,
	26,
	38,
	48,
	59,
	69,
	78,
	87,
	96,
	105,
	113,
	121,
	129,
	136,
	143,
	150,
	157,
	164,
	170,
	177,
	183,
	189,
	195,
	200,
	206,
	211,
	217,
	222,
	227,
	232,
	237,
	242,
	246,
	251,
	256,
	260,
	264,
	269,
	273,
	277,
	281,
	285,
	289,
	293,
	297,
	301,
	304,
	308,
	311,
	315,
	318,
	322,
	325,
	329,
	332,
	335,
	338,
	342,
	345,
	348,
	351,
	354,
	357,
	360,
	363,
	366,
	368,
	371,
	374,
	377,
	379,
	382,
	385,
	387,
	390,
	393,
	395,
	398,
	400,
	403,
	405,
};

static const u32 afe_gainreg_lookup_big[] = {
	405,
	411,
	417,
	423,
	428,
	434,
	439,
	444,
	449,
	454,
	459,
	464,
	469,
	473,
	478,
	482,
	487,
	491,
	495,
	499,
	503,
	507,
	511,
	515,
	519,
	523,
	527,
	530,
	534,
	537,
	541,
	544,
	548,
	551,
	554,
	558,
	561,
	564,
	567,
	570,
	573,
	576,
	579,
	582,
	585,
	588,
	591,
	594,
	596,
	599,
	602,
	604,
	607,
	610,
	612,
	615,
	617,
	620,
	622,
	625,
	627,
	630,
	632,
	635,
	637,
	639,
	641,
	644,
	646,
	648,
	650,
	653,
	655,
	657,
	659,
	661,
	663,
	665,
	667,
	670,
	672,
	674,
	676,
	678,
	680,
	681,
	683,
	685,
	687,
	689,
	691,
	693,
	695,
	697,
	698,
	700,
	702,
	704,
	705,
	707,
	709,
	711,
	712,
	714,
	716,
	717,
	719,
	721,
	722,
	724,
	726,
	727,
	729,
	731,
	732,
	734,
	735,
	737,
	738,
	740,
	741,
	743,
	744,
	746,
	747,
	749,
	750,
	752,
	753,
	755,
	756,
	758,
	759,
	760,
	762,
	763,
	764,
	766,
	767,
	769,
	770,
	771,
	773,
	774,
	775,
	777,
	778,
	779,
	781,
	782,
	783,
	784,
	786,
	787,
	788,
	789,
	791,
	792,
	793,
	794,
	796,
	797,
	798,
	799,
	800,
	802,
	803,
	804,
	805,
	806,
	807,
	809,
	810,
	811,
	812,
	813,
	814,
	815,
	816,
	818,
	819,
	820,
	821,
	822,
	823,
	824,
	825,
	826,
	827,
	828,
	829,
	830,
	832,
	833,
	834,
	835,
	836,
	837,
	838,
	839,
	840,
	841,
	842,
	843,
	844,
	845,
	846,
	847,
	848,
	849,
	850,
	851,
	852,
	853,
	854,
	854,
	855,
	856,
	857,
	858,
	859,
	860,
	861,
	862,
	863,
	864,
	865,
	866,
	867,
	867,
	868,
	869,
	870,
	871,
	872,
	873,
	874,
	875,
	875,
	876,
	877,
	878,
	879,
	880,
	881,
	881,
	882,
	883,
	884,
	885,
	886,
	886,
	887,
	888,
	889,
	890,
	891,
	891,
	892,
	893,
	894,
	895,
	896,
	896,
	897,
	898,
	899,
	899,
	900,
	901,
	902,
	903,
	903,
	904,
	905,
	906,
	906,
	907,
	908,
	909,
	910,
	910,
	911,
	912,
	913,
	913,
	914,
	915,
	916,
	916,
	917,
	918,
	918,
	919,
	920,
	921,
	921,
	922,
	923,
	924,
	924,
	925,
	926,
	926,
	927,
	928,
	929,
	929,
	930,
	931,
	931,
	932,
	933,
	933,
	934,
	935,
	935,
	936,
	937,
	937,
	938,
	939,
	939,
	940,
	941,
	941,
	942,
	943,
	943,
	944,
	945,
	945,
	946,
	947,
	947,
	948,
	949,
	949,
	950,
	951,
	951,
	952,
	953,
	953,
	954,
	954,
	955,
	956,
	956,
	957,
	958,
	958,
	959,
	959,
	960,
	961,
	961,
	962,
	962,
	963,
	964,
	964,
	965,
	966,
	966,
	967,
	967,
	968,
	968,
	969,
	970,
	970,
	971,
	971,
	972,
	973,
	973,
	974,
	974,
	975,
	976,
	976,
	977,
	977,
	978,
	978,
	979,
	980,
	980,
	981,
	981,
	982,
	982,
	983,
	983,
	984,
	985,
	985,
	986,
	986,
	987,
	987,
	988,
	988,
	989,
	990,
	990,
	991,
	991,
	992,
	992,
	993,
	993,
	994,
	994,
	995,
	995,
	996,
	997,
	997,
	998,
	998,
	999,
	999,
	1000,
	1000,
	1001,
	1001,
	1002,
	1002,
	1003,
	1003,
	1004,
	1004,
	1005,
	1005,
	1006,
	1006,
	1007,
	1007,
	1008,
	1008,
	1009,
	1009,
	1010,
	1010,
	1011,
	1011,
	1012,
	1012,
	1013,
	1013,
	1014,
	1014,
	1015,
	1015,
	1016,
	1016,
	1017,
	1017,
	1018,
	1018,
	1019,
	1019,
	1020,
	1020,
	1021,
	1021,
	1022,
	1022,
	1023,
};

static u32 afe_lookup_gainreg(u32 gain_num, u32 gain_frac)
{
	u32 result = afe_gain_def;
	u32 scaled = gain_num * afe_gain_lookup_scale + 
		gain_frac % afe_gain_lookup_scale;
	
	if (scaled < afe_gain_small_lookup_start) {
		scaled = afe_gain_small_lookup_start;
	}
	if (scaled > afe_gain_big_lookup_end) {
		scaled = afe_gain_big_lookup_end;
	}
	if (scaled < afe_gain_small_lookup_end) {
		u32 index = scaled - afe_gain_small_lookup_start;
		index += (afe_gain_small_lookup_step / 2);
		index /= afe_gain_small_lookup_step;
		result = afe_gainreg_lookup_small[index];
	} else {
		u32 index = scaled - afe_gain_big_lookup_start;
		index += (afe_gain_big_lookup_step / 2);
		index /= afe_gain_big_lookup_step;
		result = afe_gainreg_lookup_big[index];
	}
	return result;
}

static const u32 afe_gain_lookup[] = {
	1884,
	1891,
	1899,
	1907,
	1915,
	1923,
	1931,
	1939,
	1947,
	1955,
	1963,
	1971,
	1979,
	1987,
	1996,
	2004,
	2012,
	2020,
	2029,
	2037,
	2046,
	2054,
	2062,
	2071,
	2080,
	2088,
	2097,
	2105,
	2114,
	2123,
	2132,
	2140,
	2149,
	2158,
	2167,
	2176,
	2185,
	2194,
	2203,
	2212,
	2221,
	2230,
	2240,
	2249,
	2258,
	2268,
	2277,
	2286,
	2296,
	2305,
	2315,
	2324,
	2334,
	2344,
	2353,
	2363,
	2373,
	2382,
	2392,
	2402,
	2412,
	2422,
	2432,
	2442,
	2452,
	2462,
	2473,
	2483,
	2493,
	2503,
	2514,
	2524,
	2534,
	2545,
	2555,
	2566,
	2577,
	2587,
	2598,
	2609,
	2619,
	2630,
	2641,
	2652,
	2663,
	2674,
	2685,
	2696,
	2707,
	2718,
	2730,
	2741,
	2752,
	2764,
	2775,
	2786,
	2798,
	2810,
	2821,
	2833,
	2844,
	2856,
	2868,
	2880,
	2892,
	2904,
	2916,
	2928,
	2940,
	2952,
	2964,
	2976,
	2989,
	3001,
	3013,
	3026,
	3038,
	3051,
	3064,
	3076,
	3089,
	3102,
	3114,
	3127,
	3140,
	3153,
	3166,
	3179,
	3192,
	3206,
	3219,
	3232,
	3245,
	3259,
	3272,
	3286,
	3299,
	3313,
	3327,
	3340,
	3354,
	3368,
	3382,
	3396,
	3410,
	3424,
	3438,
	3452,
	3467,
	3481,
	3495,
	3510,
	3524,
	3539,
	3554,
	3568,
	3583,
	3598,
	3613,
	3628,
	3643,
	3658,
	3673,
	3688,
	3703,
	3718,
	3734,
	3749,
	3765,
	3780,
	3796,
	3811,
	3827,
	3843,
	3859,
	3875,
	3891,
	3907,
	3923,
	3939,
	3955,
	3972,
	3988,
	4005,
	4021,
	4038,
	4055,
	4071,
	4088,
	4105,
	4122,
	4139,
	4156,
	4173,
	4190,
	4208,
	4225,
	4243,
	4260,
	4278,
	4295,
	4313,
	4331,
	4349,
	4367,
	4385,
	4403,
	4421,
	4439,
	4458,
	4476,
	4495,
	4513,
	4532,
	4551,
	4569,
	4588,
	4607,
	4626,
	4645,
	4664,
	4684,
	4703,
	4722,
	4742,
	4762,
	4781,
	4801,
	4821,
	4841,
	4861,
	4881,
	4901,
	4921,
	4942,
	4962,
	4982,
	5003,
	5024,
	5044,
	5065,
	5086,
	5107,
	5128,
	5149,
	5171,
	5192,
	5214,
	5235,
	5257,
	5278,
	5300,
	5322,
	5344,
	5366,
	5388,
	5411,
	5433,
	5455,
	5478,
	5500,
	5523,
	5546,
	5569,
	5592,
	5615,
	5638,
	5661,
	5685,
	5708,
	5732,
	5756,
	5779,
	5803,
	5827,
	5851,
	5875,
	5900,
	5924,
	5949,
	5973,
	5998,
	6023,
	6047,
	6072,
	6097,
	6123,
	6148,
	6173,
	6199,
	6224,
	6250,
	6276,
	6302,
	6328,
	6354,
	6380,
	6407,
	6433,
	6460,
	6486,
	6513,
	6540,
	6567,
	6594,
	6621,
	6649,
	6676,
	6704,
	6731,
	6759,
	6787,
	6815,
	6843,
	6872,
	6900,
	6929,
	6957,
	6986,
	7015,
	7044,
	7073,
	7102,
	7131,
	7161,
	7190,
	7220,
	7250,
	7280,
	7310,
	7340,
	7370,
	7401,
	7431,
	7462,
	7493,
	7524,
	7555,
	7586,
	7617,
	7649,
	7681,
	7712,
	7744,
	7776,
	7808,
	7840,
	7873,
	7905,
	7938,
	7971,
	8004,
	8037,
	8070,
	8103,
	8137,
	8170,
	8204,
	8238,
	8272,
	8306,
	8340,
	8375,
	8409,
	8444,
	8479,
	8514,
	8549,
	8585,
	8620,
	8656,
	8691,
	8727,
	8763,
	8800,
	8836,
	8872,
	8909,
	8946,
	8983,
	9020,
	9057,
	9095,
	9132,
	9170,
	9208,
	9246,
	9284,
	9322,
	9361,
	9399,
	9438,
	9477,
	9516,
	9556,
	9595,
	9635,
	9675,
	9714,
	9755,
	9795,
	9835,
	9876,
	9917,
	9958,
	9999,
	10040,
	10082,
	10123,
	10165,
	10207,
	10249,
	10292,
	10334,
	10377,
	10420,
	10463,
	10506,
	10549,
	10593,
	10637,
	10680,
	10725,
	10769,
	10813,
	10858,
	10903,
	10948,
	10993,
	11038,
	11084,
	11130,
	11176,
	11222,
	11268,
	11315,
	11362,
	11409,
	11456,
	11503,
	11550,
	11598,
	11646,
	11694,
	11742,
	11791,
	11840,
	11889,
	11938,
	11987,
	12036,
	12086,
	12136,
	12186,
	12237,
	12287,
	12338,
	12389,
	12440,
	12491,
	12543,
	12595,
	12647,
	12699,
	12751,
	12804,
	12857,
	12910,
	12963,
	13017,
	13071,
	13125,
	13179,
	13233,
	13288,
	13343,
	13398,
	13453,
	13509,
	13565,
	13621,
	13677,
	13733,
	13790,
	13847,
	13904,
	13962,
	14019,
	14077,
	14135,
	14194,
	14252,
	14311,
	14370,
	14430,
	14489,
	14549,
	14609,
	14670,
	14730,
	14791,
	14852,
	14914,
	14975,
	15037,
	15099,
	15161,
	15224,
	15287,
	15350,
	15413,
	15477,
	15541,
	15605,
	15670,
	15734,
	15799,
	15865,
	15930,
	15996,
	16062,
	16128,
	16195,
	16262,
	16329,
	16396,
	16464,
	16532,
	16600,
	16669,
	16738,
	16807,
	16876,
	16946,
	17016,
	17086,
	17157,
	17228,
	17299,
	17370,
	17442,
	17514,
	17587,
	17659,
	17732,
	17805,
	17879,
	17953,
	18027,
	18101,
	18176,
	18251,
	18327,
	18402,
	18478,
	18555,
	18631,
	18708,
	18785,
	18863,
	18941,
	19019,
	19098,
	19177,
	19256,
	19335,
	19415,
	19495,
	19576,
	19657,
	19738,
	19819,
	19901,
	19983,
	20066,
	20149,
	20232,
	20316,
	20400,
	20484,
	20568,
	20653,
	20739,
	20824,
	20910,
	20997,
	21083,
	21170,
	21258,
	21346,
	21434,
	21522,
	21611,
	21701,
	21790,
	21880,
	21971,
	22061,
	22152,
	22244,
	22336,
	22428,
	22521,
	22614,
	22707,
	22801,
	22895,
	22990,
	23084,
	23180,
	23276,
	23372,
	23468,
	23565,
	23662,
	23760,
	23858,
	23957,
	24056,
	24155,
	24255,
	24355,
	24456,
	24557,
	24658,
	24760,
	24862,
	24965,
	25068,
	25172,
	25276,
	25380,
	25485,
	25590,
	25696,
	25802,
	25908,
	26015,
	26123,
	26231,
	26339,
	26448,
	26557,
	26667,
	26777,
	26887,
	26999,
	27110,
	27222,
	27334,
	27447,
	27561,
	27675,
	27789,
	27904,
	28019,
	28135,
	28251,
	28367,
	28485,
	28602,
	28720,
	28839,
	28958,
	29078,
	29198,
	29318,
	29439,
	29561,
	29683,
	29806,
	29929,
	30052,
	30177,
	30301,
	30426,
	30552,
	30678,
	30805,
	30932,
	31060,
	31188,
	31317,
	31446,
	31576,
	31707,
	31838,
	31969,
	32101,
	32234,
	32367,
	32500,
	32635,
	32770,
	32905,
	33041,
	33177,
	33314,
	33452,
	33590,
	33729,
	33868,
	34008,
	34148,
	34289,
	34431,
	34573,
	34716,
	34859,
	35003,
	35148,
	35293,
	35439,
	35585,
	35732,
	35880,
	36028,
	36177,
	36326,
	36476,
	36627,
	36778,
	36930,
	37083,
	37236,
	37390,
	37544,
	37699,
	37855,
	38011,
	38168,
	38326,
	38484,
	38643,
	38803,
	38963,
	39124,
	39285,
	39448,
	39610,
	39774,
	39938,
	40103,
	40269,
	40435,
	40602,
	40770,
	40938,
	41107,
	41277,
	41448,
	41619,
	41791,
	41963,
	42137,
	42311,
	42485,
	42661,
	42837,
	43014,
	43192,
	43370,
	43549,
	43729,
	43910,
	44091,
	44273,
	44456,
	44640,
	44824,
	45009,
	45195,
	45382,
	45569,
	45757,
	45946,
	46136,
	46327,
	46518,
	46710,
	46903,
	47097,
	47291,
	47486,
	47683,
	47880,
	48077,
	48276,
	48475,
	48675,
	48876,
	49078,
	49281,
	49485,
	49689,
	49894,
	50100,
	50307,
	50515,
	50724,
	50933,
	51143,
	51355,
	51567,
	51780,
	51994,
	52208,
	52424,
	52640,
	52858,
	53076,
	53295,
	53516,
	53737,
	53959,
	54181,
	54405,
	54630,
	54855,
	55082,
	55310,
	55538,
	55767,
	55998,
	56229,
	56461,
	56694,
	56929,
	57164,
	57400,
	57637,
	57875,
	58114,
	58354,
	58595,
	58837,
	59080,
	59324,
	59569,
	59815,
	60062,
	60310,
	60559,
	60809,
	61060,
	61313,
	61566,
	61820,
	62075,
	62332,
	62589,
	62848,
	63107,
	63368,
	63630,
	63893,
	64156,
	64421,
	64687,
	64955,
	65223,
	65492,
	65763,
	66034,
	66307,
	66581,
	66856,
	67132,
	67409,
	67688,
	67967,
	68248,
	68530,
	68813,
	69097,
	69383,
	69669,
	69957,
	70246,
	70536,
	70827,
	71120,
	71413,
	71708,
	72005,
	72302,
	72601,
	72900,
	73202,
	73504,
	73807,
	74112,
	74418,
	74726,
	75034,
	75344,
	75655,
	75968,
	76282,
	76597,
	76913,
	77231,
	77550,
	77870,
	78192,
	78515,
	78839,
	79164,
	79491,
	79820,
	80149,
	80480,
	80813,
	81147,
	81482,
	81818,
	82156,
	82495,
	82836,
	83178,
	83522,
	83867,
	84213,
	84561,
	84910,
	85261,
	85613,
	85967,
	86322,
	86678,
	87036,
	87396,
	87757,
	88119,
	88483,
	88848,
	89215,
	89584,
	89954,
	90325,
	90698,
	91073,
	91449,
	91827,
	92206,
	92587,
	92969,
	93353,
	93739,
	94126,
	94515,
	94905,
	95297,
	95691,
	96086,
	96483,
	96881,
	97281,
	97683,
	98087,
	98492,
	98899,
	99307,
	99717,
	100129,
	100543,
	100958,
	101375,
	101793,
	102214,
	102636,
	103060,
	103486,
	103913,
	104342,
	104773,
	105206,
	105640,
	106077,
	106515,
	106955,
	107396,
	107840,
	108285,
	108733,
	109182,
	109633,
	110085,
	110540,
	110997,
	111455,
	111915,
	112378,
	112842,
	113308,
	113776,
	114246,
	114718,
	115191,
	115667,
	116145,
	116625,
	117106,
	117590,
	118076,
	118563,
	119053,
	119545,
	120038,
	120534,
	121032,
	121532,
	122034,
	122538,
	123044,
	123552,
	124062,
	124575,
	125089,
	125606,
	126125,
	126646,
	127169,
	127694,
};

static void afe_lookup_gainval(u32 regval, u32 *gain_num, u32 *gain_frac)
{
	u32 index = afe_gain_def;
	u32 gain;
	
	if (regval < afe_gain_min) {
		index = afe_gain_min;
	} else if (regval > afe_gain_max) {
		index = afe_gain_max;
	} else {
		index = regval;
	}
	
	gain = afe_gain_lookup[index];
	pr_info("%s reg %u -> index %u val %u\n", __func__, regval, index, gain);
	*gain_num = gain / afe_gain_lookup_scale;
	*gain_frac = gain % afe_gain_lookup_scale;
}

static const u8* afe_decode_reg(const u8* rawdata, struct afe_reg* regval)
{
	char helper[16] = "0x";
	char* end;
	const u8* result = 0;
	if (('\t' == rawdata[3]) && ('\r' == rawdata[11]) && ('\n' == rawdata[12])) {
		helper[2] = rawdata[0];
		helper[3] = rawdata[1];
		helper[4] = rawdata[2];
		helper[5] = '\0';
		regval->address = simple_strtol(helper, &end, 16);
		helper[2] = rawdata[4];
		helper[3] = rawdata[5];
		helper[4] = rawdata[6];
		helper[5] = rawdata[7];
		helper[6] = rawdata[8];
		helper[7] = rawdata[9];
		helper[8] = rawdata[10];
		helper[9] = '\0';
		regval->value = simple_strtol(helper, &end, 16);
		result = rawdata + afe_raw_line_length;
	}
	return result;
}

static inline u32 afe_get_vpat_start( struct afe_reg_conf *regcfg )
{
	return afe_vpat_base;
}

static inline u32 afe_get_vpat_base( struct afe_reg_conf *regcfg, u32 vpat )
{
	u32 result = 0;
	if ((vpat < regcfg->afe_vpat_cnt) && (vpat >= 0)) {
		result = afe_get_vpat_start(regcfg) + (vpat * afe_vpat_regcnt);
	}
	return result;
}

static inline u32 afe_get_vpat_reg( struct afe_reg_conf *regcfg, u32 vpat, u32 regoff )
{
	u32 result = 0;
	u32 base = afe_get_vpat_base( regcfg, vpat );
	if ((base > 0) && (regoff < afe_vpat_regcnt)) {
		result = base + regoff;
	}
	return result;
}

static inline u32 afe_get_vseq_start( struct afe_reg_conf *regcfg)
{
	return afe_get_vpat_start(regcfg) + (regcfg->afe_vpat_cnt * afe_vpat_regcnt);
}

static inline u32 afe_get_vseq_base( struct afe_reg_conf *regcfg, u32 vseq )
{
	u32 base = afe_get_vseq_start(regcfg);
	u32 result = 0;
	if ((vseq < regcfg->afe_vseq_cnt) && (vseq >= 0)) {
		result = base + (afe_vseq_regcnt * vseq);
	}
	return result;
}

static inline u32 afe_get_vseq_reg( struct afe_reg_conf *regcfg, u32 vseq,  u32 regoff )
{
	u32 result = 0;
	u32 base = afe_get_vseq_base( regcfg, vseq );
	if ((base > 0) && (regoff < afe_vseq_regcnt)) {
		result = base + regoff;
	}
	return result;
}

static inline u32 afe_get_field_start(struct afe_reg_conf *regcfg)
{
	return afe_get_vseq_start(regcfg) + (regcfg->afe_vseq_cnt * afe_vseq_regcnt);
}

static inline u32 afe_get_field_base( struct afe_reg_conf *regcfg, u32 field )
{
	u32 result = 0;
	u32 base = afe_get_field_start(regcfg);
	if ((field >= 0) && (field <= afe_field_max)) {
		result = base + (afe_field_regcnt * field);
	}
	return result;
}

static inline u32 afe_get_field_reg( struct afe_reg_conf *regcfg, u32 field, u32 regoff)
{
  u32 result = 0;
  u32 base = afe_get_field_base( regcfg, field );
  if ((base > 0) && (regoff < afe_field_regcnt)) {
    result = base + regoff;
  }
  return result;
}

extern int afe_parse_regs(const u8* rawdata, size_t rawsize, 
			struct afe_reg* regs, size_t max_regs)
{
	int regcnt = 0;
	const u8* data = rawdata;
	const u8* stop = rawdata + rawsize;

	while (data && data < stop) {
		data = afe_decode_reg(data, &regs[regcnt]);
		if (data) {
			++regcnt;
		} else {
			regcnt = -1;
			break;
		}
	};
	return regcnt;
}

void afe_reset_config(struct afe_reg_conf* regcfg)
{
	regcfg->afe_field_cnt	= ccd_field_cnt_def;
	regcfg->afe_vseq_cnt	= ccd_vseq_cntdef;
	regcfg->afe_vpat_cnt	= ccd_vpat_cntdef;

	regcfg->afe_cdsgain_cnt	= afe_cdsgain_min;
	regcfg->afe_lines_cnt	= afe_lines_min;

	regcfg->afe_subck_cnt	= afe_subck_min;
	regcfg->afe_subck_max	= regcfg->afe_lines_cnt - 1;

	regcfg->afe_gain_cnt	= afe_gain_def;
	regcfg->afe_clamp_cnt	= afe_clamp_min;
	
	regcfg->afe_pixel_hdrise = afe_pixel_hdrise_def;
	regcfg->afe_lines_vdrise = afe_lines_vdrise_def;
}

int afe_get_setup(const struct afe_reg* regs, size_t reg_cnt,
		struct afe_reg_conf* reg_config)
{
	unsigned i;
	u32 setup_flags = afe_setup_none;
	const u32 setup_mask = afe_setup_vpat | afe_setup_vseq;

	for (i = 0; i < reg_cnt; ++i) {
		if (afe_cdsgain_reg == regs[i].address) {
			reg_config->afe_cdsgain_cnt = regs[i].value;
			setup_flags |= afe_setup_cdsgain;
		}
		if (afe_gain_reg == regs[i].address) {
			reg_config->afe_gain_cnt = regs[i].value;
			setup_flags |= afe_setup_gain;
		}
		if (afe_clamp_reg == regs[i].address) {
			reg_config->afe_clamp_cnt = regs[i].value;
			setup_flags |= afe_setup_clamp;
		}
		if (afe_shutter_reg == regs[i].address) {
			reg_config->afe_subck_cnt = (regs[i].value >> 12);
			setup_flags |= afe_setup_subck;
		}
		if (afe_vseq_reg == regs[i].address) {
			reg_config->afe_vseq_cnt = regs[i].value;
			setup_flags |= afe_setup_vseq;
		}
		if (afe_vpat_reg == regs[i].address) {
			reg_config->afe_vpat_cnt = regs[i].value;
			setup_flags |= afe_setup_vpat;
		}
		if (afe_hdvdrise_reg == regs[i].address) {
			u32 val = regs[i].value;
			reg_config->afe_pixel_hdrise = 
				(val & afe_vdhdrise_m_hdrise) >> afe_vdhdrise_s_hdrise;
			reg_config->afe_lines_vdrise = 
				(val & afe_vdhdrise_m_vdrise) >> afe_vdhdrise_s_vdrise;
			setup_flags |= afe_setup_hdvdrise;
		}
	}
	if (setup_mask == (setup_flags & setup_mask))
	{
		u32 vdlen_reg = afe_get_field_reg( reg_config, 0, afe_fld_scp8vdlen_reg );
		for (i = 0; i < reg_cnt; ++i) {
			if (vdlen_reg == regs[i].address) {
				u32 val = regs[i].value & afe_fld_scp8vdlen_m_vdlen;
				reg_config->afe_lines_cnt = (val >> afe_fld_scp8vdlen_s_vdlen);
				pr_info("%s: reg %x = %x (lines %x)\n", __func__, 
					regs[i].address, regs[i].value, 
					reg_config->afe_lines_cnt);
				setup_flags |= afe_setup_lines;
			}
		}
	}
	
	pr_info("%s: flags = %x (expext %x)\n", __func__, setup_flags, afe_setup_all);
	
	return (afe_setup_all == setup_flags) ? 0 : -1;
};

int afe_adapt_to_pixelclk( struct spi_device* sdev, u32 frequency )
{
	int result = -1;
	u32 hdr_reg_val = afe_get_hdr_lookup(frequency);
	u32 rg_reg_val = afe_get_rg_lookup(frequency);

	if ((hdr_reg_val > 0) && (rg_reg_val > 0)) {
		result = i5ad9923a_write_reg(sdev, afe_hdr1time_reg, hdr_reg_val );
		if (0 == result) {
			result = i5ad9923a_write_reg(sdev, afe_rgtime_reg, rg_reg_val);
		}
		pr_info("%s: result = %d\n", __func__, result);
	}
	return result;
}

/*
# set VDLEN register of field 0
# used to control the framerate
# do not call directly
*/
static int afe_set_vdlen( struct spi_device *sdev, struct afe_reg_conf *regcfg, u32 lines )
{
	int result = -1;

	if ( (lines >= afe_lines_min) && (lines <= afe_lines_max)) {
		u32 regval = (lines << afe_fld_scp8vdlen_s_vdlen);
		u32 regaddr = afe_get_field_reg( regcfg, 0, afe_fld_scp8vdlen_reg );
		result = i5ad9923a_write_reg(sdev, regaddr, regval );
		pr_info("%s: %x -> %x (%d)\n", __func__, (u32)regval, regaddr, result);
		if (0 == result ) {
/* # update current lines per frame and new max subck cnt */
			pr_info("%s: cur lines %u -> new lines %u\n", __func__, regcfg->afe_lines_cnt, lines);
			regcfg->afe_lines_cnt = lines;
			regcfg->afe_subck_max = lines - 1;
		}
	}
	return result;
}

static int afe_set_subck_cnt(struct afe_reg_conf *regcfg, u32 cnt)
{
	int result = -1;
	if ((cnt >= afe_subck_min) && (cnt <= regcfg->afe_subck_max)) {
		pr_info("%s: cur subck %u -> new subck %u\n", __func__, regcfg->afe_subck_cnt, cnt);
		regcfg->afe_subck_cnt = cnt;
		result = 0;
	}
	return result;
}

int afe_set_exposure( struct spi_device *spi, struct afe_reg_conf *regcfg, u32 value )
{
	int result = 0;
	
	if ((value > 0)  &&  (value < regcfg->afe_lines_cnt)) {
		u32 subck_cnt = regcfg->afe_lines_cnt - value;
		u32 regval = (subck_cnt << 12) | 0;
		result = i5ad9923a_write_reg(spi, afe_shutter_reg, regval );
		pr_info("%s:  %x -> %x (%d)\n", __func__, (u32)regval, 
				afe_shutter_reg, result);
	/*# disable long exposure*/
		i5ad9923a_write_reg(spi, afe_longshutter_reg, 0x00 );
		if (0 == result) {
			result = afe_set_subck_cnt(regcfg, subck_cnt);
		}
	}
	
	return result;
}

int afe_set_exposure_nsec( struct spi_device *spi, struct afe_reg_conf *regcfg, u32 frequency, u32 nsec )
{
	int result = 0;
	u32 line_len = afe_get_line_time_nsec(frequency);
	u32 lines = (nsec + line_len / 2) / line_len;
	result = afe_set_exposure(spi, regcfg, lines);
	return result;
}

/*
int afe_set_exposure_msec( struct spi_device *spi, struct afe_reg_conf *regcfg, u32 frequency, u32 msec )
{
	u32 nsec = msec * 1000 * 1000;
	int result = afe_set_exposure_nsec(spi, regcfg, frequency, nsec);
	return result;
}
*/

int afe_set_fps( struct spi_device *sdev, struct afe_reg_conf *regcfg, u32 frequency, u32 fps_num, u32 fps_frac)
{
	u32 fps_min_num;
	u32 fps_max_num;
	u32 fps_min_frac;
	u32 fps_max_frac;
	u32 mfps_min, mfps_max, mfps_new;
	u32 min_exp_nsec;
	u32 max_exp_nsec;
	u32 frame_time_nsec;
	u32 line_time_nsec;
	u32 exposure_nsec;
	u32 lines;
	int result = 0;

	fps_frac %= 1000;
	
	afe_get_min_fps( frequency, &fps_min_num, &fps_min_frac );
	afe_get_max_fps( frequency, &fps_max_num, &fps_max_frac );

	pr_info("%s: fps %u.%.3u\n", __func__, fps_num, fps_frac);
	pr_info("%s: min %u.%.3u\n", __func__, fps_min_num, fps_min_frac);
	pr_info("%s: max %u.%.3u\n", __func__, fps_max_num, fps_max_frac);

	mfps_min = fps_min_num * 1000 + fps_min_frac;
	mfps_max = fps_max_num * 1000 + fps_max_frac;
	mfps_new = fps_num * 1000 + fps_frac;

	exposure_nsec = afe_get_exposure_nsec( regcfg, frequency );

	if ( ((mfps_min) <= mfps_new) && (mfps_max >= mfps_new)) {
		u64 tmp = ((u64)1000 * (u64)1000 * (u64)1000 * (u64)1000);
		tmp += (u64)(mfps_new / 2);
		tmp = div64_u64(tmp, (u64)mfps_new);
		frame_time_nsec = (u32)tmp;

		pr_info("%s: exposure %u nsec frame nsec %u\n", __func__, 
			exposure_nsec, frame_time_nsec);

		line_time_nsec  = afe_get_line_time_nsec( frequency );
		lines = (frame_time_nsec + (line_time_nsec / 2)) / line_time_nsec;

		pr_info("%s: lines %u min %u, max %u\n", __func__, 
			lines, afe_lines_min, afe_lines_max);

		if ( lines < afe_lines_min ) {
			lines = afe_lines_min;
		}
		if ( lines > afe_lines_max ) {
			lines = afe_lines_max;
		}

		result = afe_set_vdlen(sdev, regcfg, lines);
	}

	if (0 == result) {
		min_exp_nsec = afe_get_exposure_min_nsec(regcfg, frequency);
		max_exp_nsec = afe_get_exposure_max_nsec(regcfg, frequency);

		pr_info("%s: exposure %u min %u, max %u\n", __func__, 
			exposure_nsec, min_exp_nsec, max_exp_nsec);

		if ( exposure_nsec > max_exp_nsec ) {
			result = afe_set_exposure( sdev, regcfg, regcfg->afe_lines_cnt - 1 );
		} else if ( exposure_nsec < min_exp_nsec ) {
			result = afe_set_exposure( sdev, regcfg, 1 );
		} else {
			result = afe_set_exposure_nsec(sdev, regcfg, frequency, exposure_nsec);
		}
	}

	return result;
}


static void get_fps ( u32 frame_time_nsec, u32* num, u32* frac )
{
	u32 frame_time_msec = (frame_time_nsec + ((1000 * 1000) / 2)) / (1000 * 1000);
	u32 usec = 1000 * 1000;
	u32 mfps = usec + frame_time_msec / 2;
	mfps /= frame_time_msec;
	*num = mfps / 1000;
	*frac = mfps % 1000;
/*
	u32 remain = 1000 % frame_time_msec;
	*num = (1000 + (frame_time_msec / 2)) / frame_time_msec;
	*frac = 0;
	if (remain) {
		*frac = (remain * 1000) / frame_time_msec;
	}
*/
}

/*
# query current framerate
# usable only if timing is controlled via the afe_ functions
*/
void afe_get_fps( struct afe_reg_conf *regcfg, u32 frequency, u32* num, u32* frac )
{
	u32 frame_time_nsec = afe_get_frame_time_nsec( regcfg, frequency );
	get_fps ( frame_time_nsec, num, frac );
}

void afe_get_min_fps ( u32 frequency, u32* num, u32* frac )
{
	u32 frame_time_nsec = afe_get_time_nsec( frequency, afe_lines_max );
	get_fps ( frame_time_nsec, num, frac );
}

void afe_get_max_fps ( u32 frequency, u32* num, u32* frac )
{
	u32 frame_time_nsec = afe_get_time_nsec( frequency, afe_lines_min );
	get_fps ( frame_time_nsec, num, frac );
}

int afe_set_gain( struct spi_device *sdev, struct afe_reg_conf *regcfg, 
		u32 gain_num, u32 gain_frac)
{
	int result;
	u32 regval = afe_lookup_gainreg(gain_num, gain_frac);
	result = i5ad9923a_write_reg(sdev, afe_gain_reg, regval);
	pr_info("%s gain %u.%.3u :: %u -> %x (%u)\n", __func__, gain_num, gain_frac, 
			regval, (u32)afe_gain_reg, result);
	if (0 == result) {
		regcfg->afe_gain_cnt = regval;
	}
	
	return result;
}

void afe_get_gain( struct afe_reg_conf *regcfg, u32 *gain_num, u32 *gain_frac)
{
	afe_lookup_gainval(regcfg->afe_gain_cnt, gain_num, gain_frac);
}

void afe_get_gain_max( struct afe_reg_conf *regcfg, u32 *gain_num, u32 *gain_frac)
{
	afe_lookup_gainval(afe_gain_max, gain_num, gain_frac);
}

void afe_get_gain_min( struct afe_reg_conf *regcfg, u32 *gain_num, u32 *gain_frac)
{
	afe_lookup_gainval(afe_gain_min, gain_num, gain_frac);
}


int afe_set_cdsgain( struct spi_device *spi, struct afe_reg_conf *regcfg,
                         u32 value )
{
	int result = -ERANGE;
	if ( (value >= afe_cdsgain_min) && (value <= afe_cdsgain_max) ) {
		result = i5ad9923a_write_reg(spi, afe_cdsgain_reg, value);
		if (0 == result)
		{
			regcfg->afe_cdsgain_cnt = value;
		}
	}
	return result;
}

/* # set clamp level */
int afe_set_clamp( struct spi_device *spi, struct afe_reg_conf *regcfg,
                            u32 value )
{
	int result = -ERANGE;
	if ( (value >= afe_clamp_min) && (value <= afe_clamp_max) ) {
		result = i5ad9923a_write_reg(spi, afe_clamplevel_reg, value );
		if (0 == result) {
			regcfg->afe_clamp_cnt = value;
		}
	}
	return result;
}

