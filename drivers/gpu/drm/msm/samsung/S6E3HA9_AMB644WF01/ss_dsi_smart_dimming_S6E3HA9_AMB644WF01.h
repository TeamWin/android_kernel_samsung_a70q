/*
 * =================================================================
 *
 *       Filename:  ss_dsi_smart_dimming_S6E3HA9_AMB644WF01.h
 *
 *    Description:  Smart dimming algorithm implementation
 *
 *        Company:  Samsung Electronics
 *
 * ================================================================
 */
/*
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) 2018, Samsung Electronics. All rights reserved.

 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _SS_DSI_SMART_DIMMING_S6E3HA9_AMB644WF01_H_
#define _SS_DSI_SMART_DIMMING_S6E3HA9_AMB644WF01_H_

#include "ss_dsi_panel_common.h"
#include "ss_dsi_smart_dimming_common.h"

static int id1, id2, id3;

#define LUMINANCE_MAX 74
#define HMT_LUMINANCE_MAX 37
#define GAMMA_SET_MAX 34
#define GRAY_SCALE_MAX 256
#define RGB_COMPENSATION 27

/* BIT_SHIFT is used for right bit shfit */
#define BIT_SHIFT 22
#define BIT_SHFIT_MUL 4194304 // pow(2,BIT_SHIFT)

#define VREG1_REF_6P8 28521267	/* 6.8*4194304 */
#define VREG1_REF_6P5 27262976	/* 6.5*4194304 */
#define VREG1_REF_6P4 26843546	/* 6.4*4194304 */
#define VREG1_REF_6P3 26424115	/* 6.3*4194304 */
#define VREG1_REF_6P2 26004685	/* 6.2*4194304 */

#define VREF_1P0 4194304 /* 1.0*4194304 */

#define HBM_INTERPOLATION_STEP 80

enum {
	GAMMA_CURVE_1P9 = 0,
	GAMMA_CURVE_2P15,
	GAMMA_CURVE_2P2,
	GAMMA_CURVE_2P2_400CD,
};

/* TP */
enum {
	VT = 0,
	V0,
	V1,
	V7,
	V11,
	V23,
	V35,
	V51,
	V87,
	V151,
	V203,
	V255,
	V_MAX,
};

/* RGB */
enum {
	R = 0,
	G = 1,
	B = 2,
	RGB_MAX,
};

static char V_LIST[V_MAX][5] = {
	"VT",
	"V0",
	"V1",
	"V7",
	"V11",
	"V23",
	"V35",
	"V51",
	"V87",
	"V151",
	"V203",
	"V255",
};

struct illuminance_table {
	int lux;
	char gamma_setting[GAMMA_SET_MAX];
} __packed;

struct SMART_DIM {
	/* read C8h 1st ~ 35th */
	char MTP_ORIGN[GAMMA_SET_MAX];

	/* copy MTP_ORIGN -> MTP */
	int MTP[V_MAX][RGB_MAX];

	/* TP's gamma voltage */
	int RGB_OUTPUT[V_MAX][RGB_MAX];

	/* GRAY (0~255) */
	int GRAY[GRAY_SCALE_MAX][RGB_MAX];

	/* Because of AID funtion, below members are added*/
	int lux_table_max;
	int *plux_table;
	struct illuminance_table gen_table[LUMINANCE_MAX];

	int brightness_level;
	int ldi_revision;
	int vregout_voltage;
	int vref;
	char panel_revision;

	/* HBM interpolation */
	struct illuminance_table hbm_interpolation_table[HBM_INTERPOLATION_STEP + 1];
	char *hbm_payload;
	int hbm_brightness_level;

	struct illuminance_table hmt_gen_table[HMT_LUMINANCE_MAX];
} __packed;

/* V0,V1,V7,V11,V23,V35,V51,V87,V151,V203,V255 */
static int INFLECTION_VOLTAGE_ARRAY[V_MAX] = {0, 0, 1, 7, 11, 23, 35, 51, 87, 151, 203, 255};

static int vt_coefficient[] = {
	0, 24, 48, 72,
	96, 120, 144, 168,
	192, 216, 276, 296,
	316, 336, 356, 372
};

static int v0_coefficient[] = {
	0, 12, 24, 36,
	48, 60, 72, 84,
	96, 108, 120, 132,
	144, 156, 168, 180
};

/* center (max) gamma value (Hex) */
static int center_gamma[V_MAX][RGB_MAX] = {
	{0x0, 0x0, 0x0},	/* VT */
	{0x3, 0x3, 0x3},	/* V0 */
	{0x80, 0x80, 0x80},	/* V1 */
	{0x80, 0x80, 0x80},	/* V7 */
	{0x80, 0x80, 0x80},	/* V11 */
	{0x80, 0x80, 0x80},	/* V23 */
	{0x80, 0x80, 0x80},	/* V35 */
	{0x80, 0x80, 0x80},	/* V51 */
	{0x80, 0x80, 0x80},	/* V87 */
	{0x80, 0x80, 0x80},	/* V151 */
	{0x80, 0x80, 0x80},	/* V203 */
	{0x200, 0x200, 0x200},	/* V255 */
};

/* fraction for gamma code */
static int fraction[V_MAX][2] = {
/* {numerator, denominator} */
	{0,  1200},		/* VT */
	{0,  1200},		/* V0 */
	{0,  256},		/* V1 */
	{64, 320},		/* V7 */
	{64, 320},		/* V11 */
	{64, 320},		/* V23 */
	{64, 320},		/* V35 */
	{64, 320},		/* V51 */
	{64, 320},		/* V87 */
	{64, 320},		/* V151 */
	{64, 320},		/* V203 */
	{177, 1200},		/* V255 */
};

static int hbm_interpolation_candela_table_revA[HBM_INTERPOLATION_STEP] = {
	425, 430, 433, 438, 444, 448, 453, 458, 463, 468,
	472, 477, 481, 486, 491, 495, 500, 505, 510, 515,
	520, 525, 528, 533, 539, 543, 548, 553, 558, 563,
	567, 572, 576, 581, 586, 591, 596, 599, 604, 610,
	614, 619, 624, 629, 634, 639, 644, 647, 652, 658,
	662, 667, 671, 676, 681, 686, 691, 694, 699, 705,
	710, 715, 718, 723, 729, 733, 738, 743, 748, 753,
	757, 762, 766, 771, 776, 780, 785, 790, 795, 800,
};

/* B2 rev.A ~ F */
static unsigned int base_luminance_star_revA[LUMINANCE_MAX][2] = {
	{2, 102},
	{3, 102},
	{4, 102},
	{5, 102},
	{6, 102},
	{7, 102},
	{8, 102},
	{9, 102},
	{10, 102},
	{11, 102},
	{12, 102},
	{13, 102},
	{14, 102},
	{15, 102},
	{16, 102},
	{17, 102},
	{19, 102},
	{20, 102},
	{21, 102},
	{22, 102},
	{24, 102},
	{25, 102},
	{27, 102},
	{29, 102},
	{30, 102},
	{32, 102},
	{34, 102},
	{37, 102},
	{39, 102},
	{41, 102},
	{44, 102},
	{47, 102},
	{50, 102},
	{53, 102},
	{56, 102},
	{60, 103},
	{64, 108},
	{68, 115},
	{72, 123},
	{77, 132},
	{82, 139},
	{87, 145},
	{93, 155},
	{98, 164},
	{105, 175},
	{111, 184},
	{119, 196},
	{126, 208},
	{134, 221},
	{143, 234},
	{152, 246},
	{162, 260},
	{172, 275},
	{183, 290},
	{195, 290},
	{207, 290},
	{220, 290},
	{234, 290},
	{249, 295},
	{265, 311},
	{282, 326},
	{300, 345},
	{316, 359},
	{333, 373},
	{350, 389},
	{357, 396},
	{365, 403},
	{372, 403},
	{380, 403},
	{387, 403},
	{395, 403},
	{403, 409},
	{412, 417},
	{420, 420},
};

static int gradation_offset_star_revA[LUMINANCE_MAX][9] = {
/*	V255 V203 V151 V87 V51 V35 V23 V11 V7 */
	{0, 4, 6, 14, 20, 21, 24, 28, 27},
	{0, 3, 5, 11, 15, 17, 20, 23, 24},
	{0, 3, 4, 10, 13, 15, 17, 21, 22},
	{0, 3, 4, 8, 12, 13, 15, 19, 20},
	{0, 3, 4, 8, 11, 13, 14, 18, 19},
	{0, 3, 3, 7, 9, 11, 13, 17, 18},
	{0, 2, 3, 6, 9, 11, 13, 17, 18},
	{0, 2, 3, 6, 9, 11, 13, 17, 18},
	{0, 2, 3, 6, 9, 11, 13, 17, 18},
	{0, 2, 3, 6, 9, 11, 13, 17, 18},
	{0, 2, 3, 6, 9, 11, 13, 18, 19},
	{0, 3, 4, 6, 9, 11, 13, 17, 18},
	{0, 3, 4, 6, 9, 11, 13, 17, 18},
	{0, 3, 4, 6, 9, 11, 13, 18, 19},
	{0, 2, 3, 5, 8, 10, 12, 17, 18},
	{0, 2, 3, 5, 7, 9, 11, 16, 17},
	{0, 2, 3, 5, 6, 8, 10, 15, 16},
	{0, 2, 3, 4, 6, 8, 9, 14, 15},
	{0, 2, 3, 4, 6, 7, 8, 13, 14},
	{0, 2, 3, 4, 5, 6, 8, 13, 14},
	{0, 2, 3, 4, 5, 6, 8, 12, 13},
	{0, 2, 3, 3, 5, 6, 7, 11, 12},
	{0, 2, 2, 3, 4, 4, 6, 10, 11},
	{0, 2, 2, 3, 4, 4, 6, 10, 11},
	{0, 2, 2, 3, 4, 4, 6, 10, 11},
	{0, 1, 2, 2, 3, 3, 5, 9, 10},
	{0, 1, 2, 2, 2, 3, 5, 9, 10},
	{0, 1, 2, 2, 2, 3, 5, 8, 9},
	{0, 1, 2, 2, 2, 3, 4, 7, 8},
	{0, 1, 1, 1, 2, 2, 3, 7, 8},
	{0, 1, 1, 1, 1, 1, 2, 5, 6},
	{0, 1, 1, 1, 1, 1, 2, 5, 6},
	{0, 1, 1, 1, 1, 0, 1, 4, 5},
	{0, 1, 1, 1, 1, 0, 1, 4, 5},
	{0, 1, 1, 1, 1, 0, 1, 4, 5},
	{0, 0, 0, 1, 1, 2, 2, 4, 5},
	{0, 0, 0, 1, 0, 1, 3, 4, 5},
	{0, 1, 0, 1, 1, 2, 3, 3, 4},
	{0, 1, 0, 1, 1, 1, 2, 4, 5},
	{0, 1, 1, 1, 1, 1, 3, 4, 5},
	{0, 1, 1, 1, 0, 1, 2, 4, 5},
	{0, 0, 1, 1, 0, 1, 2, 4, 5},
	{0, 0, 1, 1, 0, 1, 2, 4, 5},
	{0, 0, 1, 1, 0, 1, 1, 3, 5},
	{0, 0, 0, 1, 0, 1, 1, 3, 5},
	{0, -1, 1, 1, 0, 1, 1, 3, 5},
	{0, -1, 1, 1, 0, 1, 1, 3, 5},
	{0, -1, 1, 1, 0, 1, 1, 2, 5},
	{0, -1, 2, 1, 0, 1, 1, 2, 4},
	{0, -1, 2, 1, 0, 1, 1, 2, 4},
	{0, -1, 2, 1, 0, 1, 1, 2, 4},
	{0, -1, 1, 1, 0, 1, 1, 2, 4},
	{0, 0, 1, 1, 0, 1, 1, 2, 4},
	{0, 0, 1, 1, -1, -1, 0, 1, 3},
	{0, 0, 1, 1, -1, -1, 0, 1, 3},
	{0, 0, 1, 0, -1, -1, 0, 1, 3},
	{0, -1, 0, -1, -1, -1, -1, 0, 2},
	{0, -1, 0, -1, -1, -1, -1, 0, 2},
	{0, -1, 0, -1, -1, -1, -1, 0, 1},
	{0, -2, 0, -1, -1, -1, -1, 0, 0},
	{0, -1, 0, -1, -1, -2, -2, -1, 0},
	{0, -1, -1, -1, -2, -1, -1, -1, 0},
	{0, -1, -1, -1, -2, -1, -1, -1, 0},
	{0, -1, -1, -2, -2, -1, -1, -1, 0},
	{0, -1, -1, -2, -2, -1, -1, -1, 0},
	{0, -1, -1, -2, -2, -2, -2, -2, 0},
	{0, -1, -1, -2, -2, -2, -2, -2, 0},
	{0, -2, -2, -2, -2, -2, -2, -2, 0},
	{0, -2, -2, -2, -2, -2, -2, -2, 0},
	{0, -2, -2, -2, -2, -2, -2, -2, 0},
	{0, -2, -2, -2, -2, -2, -2, -1, 0},
	{0, -2, -2, -2, -2, -1, -1, 0, 0},
	{0, -1, -2, -2, -3, -2, -1, -1, -1},
	{0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static int rgb_offset_star_revA[LUMINANCE_MAX][RGB_COMPENSATION] = {
/*	R255 G255 B255 R203 G203 B203 R151 G151 B151
	R87 G87 B87 R51 G51 B51 R35 G35 B35
	R23 G23 B23 R11 G11 B11
*/
	{-9, 0, -6, -3, 0, -2, -5, 2, -2, -15, 1, -8, -16, 3, -9, -24, 1, -18, -24, -3, -18, -7, 1, -11, 7, -2, -8},
	{-6, 0, -4, -3, 0, -1, -3, 1, -1, -13, 0, -7, -18, 2, -12, -27, 0, -18, -24, -2, -18, -8, 1, -11, 7, -2, -8},
	{-6, 0, -4, -2, 0, -1, -3, 1, -1, -11, 0, -6, -15, 1, -11, -25, 0, -17, -21, -1, -20, -8, 1, -11, 7, -2, -8},
	{-3, 0, -2, -2, 0, -1, -3, 0, -1, -12, 0, -7, -14, 0, -9, -24, 1, -18, -19, -1, -19, -8, 1, -11, 7, -2, -8},
	{-3, 0, -2, -2, 0, -1, -3, 0, -1, -10, 0, -6, -13, 0, -9, -22, 0, -17, -19, 0, -23, -8, 1, -11, 7, -2, -10},
	{-2, 0, -2, -2, 0, -1, -3, 0, -1, -8, 0, -4, -12, 0, -10, -21, 1, -17, -21, -3, -23, -8, 1, -13, 7, -2, -10},
	{-2, 0, -2, -2, 0, -2, -2, 0, -1, -8, 0, -4, -11, 0, -9, -19, 0, -16, -21, -3, -23, -8, 0, -16, 7, -2, -13},
	{-1, 0, -2, -1, 0, -1, -2, 0, -1, -8, 0, -4, -11, -1, -9, -17, 0, -16, -20, -2, -22, -8, -1, -16, 7, -2, -13},
	{-1, 0, -2, -1, 0, -1, -2, 0, -1, -7, 0, -4, -11, -2, -9, -17, 0, -16, -19, -2, -22, -9, -1, -16, 7, -2, -13},
	{-1, 0, -2, -1, 0, -1, -2, 0, -1, -7, 0, -4, -10, -1, -9, -14, 0, -15, -18, -2, -22, -9, -1, -16, 7, -2, -13},
	{-1, 0, -2, -1, 0, -1, -2, 0, -1, -6, 0, -4, -9, 0, -8, -13, 0, -15, -16, -2, -22, -10, -4, -18, 7, -2, -13},
	{1, 0, 0, -2, 0, -1, -2, 0, -1, -5, 0, -4, -8, 1, -7, -11, 0, -14, -16, -2, -22, -9, -1, -19, 7, 0, -13},
	{1, 0, 0, -2, 0, -1, -2, 0, -1, -5, 0, -4, -7, 1, -7, -11, 0, -14, -14, -2, -22, -9, 0, -18, 7, 0, -10},
	{1, 0, 0, -2, 0, -1, -1, 0, 0, -5, 0, -4, -7, 1, -8, -11, 0, -15, -12, -2, -21, -11, -2, -20, 7, 0, -10},
	{1, 0, 0, -1, 0, -1, -1, 0, -1, -5, 0, -4, -6, 1, -7, -11, 0, -14, -13, -2, -22, -11, -3, -20, 7, 0, -10},
	{1, 0, 0, -1, 0, -1, -1, 0, -1, -5, 0, -4, -7, 1, -7, -9, 1, -13, -15, -1, -23, -9, -2, -18, 7, 0, -10},
	{1, 0, 0, -1, 0, -1, -1, 0, -1, -4, 0, -3, -5, 1, -6, -8, 1, -12, -15, -3, -23, -11, -2, -22, 6, 0, -10},
	{1, 0, 0, -1, 0, -1, -1, 0, -1, -4, 0, -3, -5, 1, -5, -7, 1, -11, -13, 0, -21, -12, -2, -22, 6, 2, -10},
	{1, 0, 0, -1, 0, -1, -1, 0, -1, -4, 0, -3, -5, 1, -5, -7, 2, -11, -12, 1, -19, -10, -2, -21, 6, 2, -10},
	{1, 0, 0, -1, 0, -1, -1, 0, -1, -3, 0, -2, -5, 1, -5, -7, 2, -11, -13, -1, -18, -10, -3, -22, 6, 2, -10},
	{2, 0, 1, -1, 0, -1, -1, 0, -1, -3, 0, -2, -6, 0, -6, -7, 1, -11, -13, -1, -17, -9, 0, -21, 6, 2, -10},
	{2, 0, 1, -1, 0, -1, -1, 0, -1, -3, 0, -2, -4, 0, -5, -7, 1, -11, -11, 2, -14, -9, 0, -21, 6, 2, -10},
	{2, 0, 1, -1, 0, -1, -1, 0, 0, -2, 0, -1, -3, 2, -4, -7, 2, -10, -11, 1, -13, -9, 0, -23, 6, 2, -10},
	{2, 0, 1, -1, 0, -1, -1, 0, 0, -2, 0, -1, -3, 1, -4, -7, 1, -10, -13, 0, -13, -9, 1, -22, 6, 2, -10},
	{2, 0, 1, -1, 0, -1, -1, 0, 0, -2, 0, -1, -3, 0, -4, -7, 1, -10, -13, 0, -13, -10, 0, -23, 6, 2, -10},
	{2, 0, 1, -1, 0, -1, -1, 0, 0, -1, 0, 0, -1, 2, -3, -7, 1, -10, -12, 0, -13, -10, 0, -23, 6, 2, -10},
	{2, 0, 1, -1, 0, -1, -1, 0, 0, -1, 0, 0, 0, 2, -3, -7, 0, -9, -13, -2, -13, -12, -2, -27, -1, 2, -10},
	{2, 0, 1, -1, 0, -1, -1, 0, 0, -1, 0, 0, 0, 1, -3, -6, 0, -8, -11, -2, -9, -11, 0, -26, -1, 2, -10},
	{2, 0, 1, -1, 0, -1, -1, 0, 0, -1, 0, 0, -1, 0, -4, -5, 0, -7, -9, 0, -7, -11, 2, -22, -1, 2, -10},
	{2, 0, 1, -1, 0, -1, -1, 0, 0, -2, 0, 0, 0, 0, -3, -4, 1, -7, -10, 3, -7, -11, -2, -20, -1, 2, -10},
	{2, 0, 1, -1, 0, -1, 1, 0, 1, -2, 0, 0, 1, 2, -2, -3, 1, -6, -8, 3, -5, -6, 2, -17, -1, 2, -10},
	{2, 0, 1, -1, 0, -1, 1, 0, 1, -2, 0, 0, 0, 0, -3, -2, 1, -5, -8, 3, -5, -7, 0, -17, -1, 2, -10},
	{2, 0, 1, -1, 0, -1, 1, 0, 1, -2, 0, 0, 0, 0, -3, -1, 3, -4, -8, 3, -5, -6, 3, -14, -1, 2, -10},
	{2, 0, 2, -1, 0, -1, 0, 0, 0, -2, 0, 0, 1, 0, -2, -1, 2, -4, -9, 2, -5, -10, 2, -15, -1, 2, -10},
	{3, 0, 2, -1, 0, -1, 0, 0, 0, -1, 0, 1, 1, 0, -2, -2, 1, -5, -10, 0, -6, -10, 0, -21, -1, 2, -10},
	{3, 0, 2, -2, 0, -1, 1, 0, 1, -2, 0, 0, 1, 0, -2, -1, 0, -4, -8, 0, -3, -9, 2, -16, -1, 0, -23},
	{3, 0, 2, 0, 0, 0, 1, 0, 1, -2, 0, 0, 2, 0, -1, -2, 0, -5, -8, 0, -3, -12, 2, -18, -3, 0, -22},
	{3, 0, 2, -1, 0, -1, 1, 0, 1, -2, 0, 0, 2, 0, -1, -2, -1, -4, -8, 0, -3, -11, 2, -19, -3, 0, -18},
	{3, 0, 2, -1, 0, -1, 0, 0, 0, -1, 0, 1, 2, 0, -1, -1, 0, -4, -7, 0, -2, -15, -1, -20, -3, 0, -16},
	{4, 0, 2, -1, 0, 0, 0, 0, 0, -1, 0, 0, 2, 0, 0, 0, 0, -4, -7, 0, -1, -14, -1, -19, -4, 0, -16},
	{4, 0, 2, -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, -4, -6, 0, 0, -14, -1, -20, -5, 0, -16},
	{4, 0, 2, -1, 0, -1, 0, 0, 0, -1, 0, 0, 0, 0, -1, 0, 0, -4, -6, 0, -1, -14, -2, -19, -6, 0, -16},
	{4, 0, 2, -1, 0, -1, 0, 0, 0, 0, 0, 0, 1, 0, 0, -1, 0, -4, -6, 0, -2, -13, -2, -18, -9, 2, -3},
	{2, 0, 2, 0, 0, 0, -1, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, -3, -5, 0, -2, -14, 0, -16, -6, -1, -15},
	{4, 0, 2, -1, 0, -1, -1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, -3, -5, 0, -2, -15, 0, -16, -4, 0, -16},
	{3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, -3, -5, 0, -2, -15, 0, -16, -4, 0, -16},
	{3, 0, 2, 1, 0, 0, -1, 0, -1, -1, 0, 1, 0, 0, 0, 1, 0, -1, -5, 0, -2, -15, 0, -14, -1, 0, -14},
	{3, 0, 3, 1, 0, 0, -1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, -2, -5, 0, -2, -15, 0, -15, -3, 0, -15},
	{3, 0, 3, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, -2, -4, 0, -2, -14, 0, -14, -3, 0, -15},
	{2, 0, 3, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, -1, -4, 0, -2, -13, 0, -14, -2, 0, -15},
	{2, 0, 2, 0, 0, -1, 0, 0, 0, -1, 0, 0, 1, 0, 1, 1, 0, -2, -3, 0, -1, -13, 0, -12, -5, 0, -15},
	{2, 0, 3, 1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 1, 0, -2, -3, 0, -1, -10, 0, -10, -6, 0, -14},
	{2, 0, 3, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, -2, -2, 0, -1, -11, 0, -9, -5, 0, -14},
	{2, 0, 2, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, -2, -2, 0, -1, -10, 1, -10, -5, 0, -12},
	{2, 0, 2, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 1, 1, 0, -2, -2, 0, -1, -9, 0, -7, -7, 0, -12},
	{2, 0, 2, 0, 0, 0, 0, 0, -1, -1, 0, 0, 1, 0, 1, 1, 0, -1, -2, 0, -1, -8, 0, -6, -7, 0, -12},
	{2, 0, 2, 0, 0, -1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, -1, -1, 0, 0, -6, 0, -4, -7, 0, -10},
	{2, 0, 2, 0, 0, -1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, -1, 0, 0, 1, -5, 0, -2, -10, 0, -10},
	{1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, -1, 0, 0, 1, -3, 0, 0, -8, 0, -12},
	{1, 0, 2, 0, 0, 0, 1, 0, 0, -1, 0, 1, 0, 0, 2, 3, 0, 0, -1, 0, 1, -3, -2, -1, -7, 0, -9},
	{1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 2, 2, 0, -1, -1, 0, 1, 0, 0, 3, -9, 0, -8},
	{1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 2, 0, 0, 0, 0, 1, 0, 0, 3, -8, 0, -7},
	{2, 0, 1, 1, 0, 0, 1, 0, 0, -1, 0, 0, 1, 0, 2, 2, 0, 0, 0, 0, 0, -1, 0, 3, -8, 0, -6},
	{1, 0, 1, 0, 0, 1, 0, 0, -1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, -1, 0, 2, -8, 0, -6},
	{1, 0, 1, 1, 0, 1, 0, 0, -1, -1, 0, 0, -1, 0, 1, 2, 0, 0, 0, 0, 0, -2, 0, 2, -7, 0, -5},
	{1, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 1, 0, 0, 4, -7, 0, -1},
	{1, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, 0, 1, 0, 1, -1, 0, 0, 1, 0, 1, 2, 0, 3, -7, 0, -3},
	{1, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, 0, 1, 0, 1, -1, 0, 0, 1, 0, 1, 2, 0, 4, -7, 0, -3},
	{1, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, 0, 1, 0, 1, -1, 0, 0, 1, 0, 1, 2, 0, 4, -7, 0, -1},
	{1, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1, -1, 0, 0, 1, 0, 1, 2, 0, 4, -4, 0, 0},
	{1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, -1, 0, -1, 1, 0, 0, 0, 0, 2, -3, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, -1, 0, -1, 1, 0, 0, 0, 0, 2, -3, 0, -1},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, -1, 0, -1, 1, 0, 0, 0, 0, 2, -3, 0, 1},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

/************
 *	  HMT   *
 ************/
static unsigned int base_luminance_reverse_hmt_single[][2] = {
	{10, 46},
	{11, 50},
	{12, 55},
	{13, 59},
	{14, 62},
	{15, 68},
	{16, 72},
	{17, 77},
	{19, 85},
	{20, 89},
	{21, 93},
	{22, 97},
	{23, 102},
	{25, 109},
	{27, 117},
	{29, 125},
	{31, 133},
	{33, 141},
	{35, 149},
	{37, 156},
	{39, 165},
	{41, 172},
	{44, 183},
	{47, 194},
	{50, 205},
	{53, 214},
	{56, 225},
	{60, 240},
	{64, 255},
	{68, 267},
	{72, 279},
	{77, 215},
	{82, 227},
	{87, 242},
	{93, 256},
	{99, 269},
	{105, 284},
};

static int gradation_offset_reverse_hmt_single[][9] = {
/*	V255 V203 V151 V87 V51 V35 V23 V11 V7 */
	{0, -1, -1, 2, 6, 7, 8, 11, 10},
	{0, -2, -1, 1, 5, 7, 8, 11, 10},
	{0, -1, -1, 2, 5, 6, 7, 10, 10},
	{0, -2, -1, 1, 5, 7, 8, 10, 10},
	{0, 0, 1, 3, 6, 7, 8, 10, 10},
	{0, -1, -1, 2, 5, 6, 7, 11, 11},
	{0, -1, -1, 2, 5, 7, 7, 10, 10},
	{0, 0, -1, 2, 5, 6, 7, 11, 10},
	{0, 0, -1, 2, 4, 5, 7, 9, 9},
	{0, 1, 1, 1, 5, 6, 7, 10, 10},
	{0, 1, 0, 1, 5, 7, 7, 10, 10},
	{0, 2, 0, 1, 5, 6, 7, 10, 10},
	{0, 2, 1, 1, 5, 6, 7, 10, 10},
	{0, 1, 1, 2, 4, 6, 7, 10, 10},
	{0, 2, 2, 1, 4, 6, 6, 9, 10},
	{0, 1, 2, 3, 5, 6, 6, 9, 10},
	{0, 1, 2, 2, 4, 6, 6, 10, 10},
	{0, 1, 2, 2, 4, 6, 6, 9, 9},
	{0, 2, 4, 2, 4, 6, 6, 10, 9},
	{0, 3, 5, 3, 5, 7, 7, 9, 10},
	{0, 2, 4, 3, 3, 5, 6, 9, 10},
	{0, 2, 4, 3, 5, 6, 6, 9, 10},
	{0, 2, 5, 4, 5, 6, 5, 9, 10},
	{0, 1, 4, 3, 4, 5, 5, 9, 10},
	{0, 2, 6, 4, 5, 6, 5, 9, 10},
	{0, 2, 6, 3, 4, 5, 5, 8, 9},
	{0, 2, 6, 4, 4, 5, 6, 9, 9},
	{0, 3, 6, 4, 5, 6, 5, 9, 9},
	{0, 3, 6, 4, 5, 5, 6, 9, 10},
	{0, 4, 8, 4, 5, 5, 6, 9, 10},
	{0, 5, 8, 5, 5, 6, 6, 9, 10},
	{0, 1, 4, 2, 2, 3, 3, 5, 5},
	{0, 3, 5, 3, 3, 3, 4, 5, 6},
	{0, 1, 5, 4, 3, 4, 3, 5, 6},
	{0, 3, 6, 5, 3, 3, 4, 6, 6},
	{0, 3, 6, 5, 3, 3, 3, 5, 7},
	{0, 4, 6, 5, 3, 3, 3, 5, 6},
};

static int rgb_offset_reverse_hmt_single[][RGB_COMPENSATION] = {
/*	R255 G255 B255 R203 G203 B203 R151 G151 B151
	R87 G87 B87 R51 G51 B51 R35 G35 B35
	R23 G23 B23 R11 G11 B11
*/
	{-1, 0, 0, 6, 0, 4, -1, 0, -2, -8, 2, -7, -8, 3, -9, -8, 1, -8, -12, 2, -8, -12, 3, -10, -2, 1, -4},
	{0, 0, 1, 5, 0, 3, 0, 0, 0, -7, 2, -7, -8, 2, -10, -8, 1, -8, -12, 3, -9, -12, 5, -8, -3, 1, -4},
	{0, 0, 1, 3, 0, 2, 1, 0, 0, -6, 2, -6, -9, 2, -10, -7, 0, -8, -10, 2, -9, -12, 6, -8, -4, 1, -4},
	{1, 0, 1, 0, 0, 0, 3, 0, 1, -5, 2, -4, -9, 2, -9, -6, 1, -8, -8, 1, -8, -12, 7, -8, -4, 1, -4},
	{2, 0, 3, -1, 0, -1, 4, 0, 3, -4, 2, -5, -9, 2, -9, -6, 1, -8, -8, 1, -8, -12, 7, -8, -4, 1, -4},
	{2, 0, 3, -1, 0, -1, 4, 0, 2, -4, 2, -5, -11, 2, -10, -5, 1, -8, -9, 1, -9, -12, 4, -8, -2, 1, -4},
	{4, 0, 4, -1, 0, -1, 4, 0, 2, -5, 1, -4, -9, 4, -9, -4, 3, -7, -10, 2, -9, -10, 4, -10, -2, 1, -4},
	{4, 0, 4, -2, 0, -2, 4, 0, 3, -3, 2, -3, -10, 2, -9, -5, 2, -7, -10, 2, -8, -6, 3, -9, 0, 8, -6},
	{4, 0, 4, -2, 0, -1, 3, 0, 2, -2, 1, -3, -9, 4, -8, -6, 3, -8, -7, 2, -6, -10, 6, -9, 0, 6, -6},
	{5, -1, 4, -2, 0, -1, 3, 0, 2, -3, 1, -4, -9, 3, -8, -5, 2, -8, -8, 2, -8, -8, 2, -8, -1, 6, -5},
	{6, -1, 5, -2, 0, -1, 4, 0, 3, -4, 1, -4, -9, 3, -8, -4, 3, -6, -8, 3, -9, -8, 2, -10, 1, 6, -3},
	{5, -1, 4, -1, 0, 0, 3, 0, 2, -3, 1, -4, -8, 4, -8, -4, 3, -5, -7, 3, -6, -8, 2, -10, -1, 6, -4},
	{6, -1, 5, -1, 0, -1, 0, 0, 0, -1, 0, -2, -10, 4, -9, -4, 2, -6, -6, 3, -6, -8, 5, -10, -1, 6, -4},
	{7, -1, 6, -1, 0, 0, -3, 0, -3, 1, 0, 0, -11, 4, -10, -2, 2, -4, -6, 3, -7, -9, 5, -10, 0, 6, -4},
	{7, -1, 6, -1, 0, -1, -2, 0, -2, 1, 0, 0, -6, 3, -6, -6, 3, -8, -7, 2, -7, -10, 4, -10, 0, 6, -5},
	{9, -1, 7, -2, 0, -1, -3, 0, -2, 1, 0, 0, -7, 3, -7, -5, 3, -6, -7, 3, -8, -8, 4, -10, -1, 6, -5},
	{10, -1, 8, -1, 0, 0, -4, 0, -3, 2, 0, 0, -7, 3, -6, -4, 3, -6, -7, 2, -9, -9, 2, -10, -3, 6, -7},
	{10, -1, 8, 0, 0, 0, -3, 0, -2, 1, 0, 0, -7, 3, -6, -5, 3, -6, -5, 3, -6, -9, 4, -10, 0, 6, -6},
	{10, -1, 8, -1, 0, -1, -3, 0, -2, 1, 0, 0, -6, 2, -6, -5, 3, -6, -6, 3, -8, -9, 4, -11, -1, 6, -5},
	{11, -2, 8, -1, 0, 0, -4, 0, -3, 1, 0, 0, -4, 2, -4, -5, 3, -5, -7, 2, -7, -11, 6, -10, -1, 4, -5},
	{10, -1, 8, -1, 0, 0, -2, 0, -2, 1, 0, 0, -6, 2, -5, -5, 2, -6, -4, 3, -4, -11, 5, -10, -1, 4, -5},
	{11, -1, 8, -1, 0, -1, -3, 0, -2, 0, 0, 0, -4, 2, -4, -5, 3, -6, -7, 3, -8, -10, 5, -10, -1, 4, -4},
	{11, -1, 8, -1, 0, 0, -4, 0, -2, 1, 0, 1, -4, 2, -5, -6, 2, -6, -6, 4, -8, -11, 6, -12, 0, 4, -4},
	{12, -1, 8, -1, 0, 0, -3, 0, -2, 2, 0, 1, -4, 2, -5, -5, 2, -6, -8, 4, -8, -11, 6, -10, -2, 4, -4},
	{13, -2, 9, -1, 0, 0, -3, 0, -2, 0, 0, 0, -4, 1, -4, -6, 3, -7, -7, 3, -8, -11, 3, -12, -1, 5, -6},
	{13, -2, 9, 0, 0, 0, -3, 0, -3, 0, 0, 1, -3, 2, -4, -6, 3, -7, -6, 2, -6, -12, 5, -10, -1, 5, -4},
	{14, -2, 10, 0, 0, 0, -3, 0, -1, 0, 0, 0, -4, 1, -4, -8, 3, -7, -4, 2, -6, -10, 6, -10, -1, 5, -6},
	{13, -2, 9, 0, 0, 0, -2, 0, -1, 0, 0, 0, -3, 2, -4, -5, 2, -5, -7, 3, -8, -10, 6, -10, -2, 6, -4},
	{14, -2, 10, 0, 0, 0, -3, 0, -1, 0, 0, 0, -2, 2, -4, -7, 2, -6, -6, 2, -8, -12, 4, -13, -3, 4, -5},
	{15, -2, 10, -1, 0, 0, -1, 0, -1, 0, 0, 0, -3, 1, -4, -7, 2, -6, -5, 3, -7, -12, 3, -11, -3, 5, -6},
	{15, -2, 12, 0, 0, 0, -2, 0, -2, 0, 0, 0, -3, 1, -4, -5, 2, -5, -7, 3, -6, -12, 4, -10, -2, 5, -7},
	{9, -2, 8, 0, 0, 0, -2, 0, -1, 0, 0, 0, -2, 1, -2, -2, 2, -4, -3, 2, -4, -10, 5, -10, -8, 3, -10},
	{9, -2, 8, 0, 0, -1, -2, 0, -1, 0, 0, 0, -2, 1, -3, -2, 1, -4, -4, 2, -5, -12, 5, -10, -2, 3, -5},
	{10, -2, 8, 0, 0, 0, -1, 0, -1, -3, 0, -2, 0, 0, -1, -2, 1, -3, -4, 2, -5, -9, 6, -7, -6, 2, -9},
	{12, -2, 10, -1, 0, -1, -1, 0, -1, -2, 1, -2, 0, 0, 0, -2, 1, -4, -4, 2, -5, -9, 5, -9, -6, 4, -8},
	{10, -2, 9, 0, 0, -1, -2, 0, -1, -3, 1, -2, 0, 0, 0, -1, 1, -3, -5, 2, -6, -7, 4, -9, -3, 3, -5},
	{12, -2, 10, -1, 0, 0, -1, 0, -1, -2, 1, -2, 0, 0, 0, -2, 1, -3, -4, 3, -6, -6, 5, -7, -4, 5, -6},
};
#endif
