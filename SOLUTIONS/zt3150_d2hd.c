//
// Zealtek D2 project
// ZT3125 Twin D2 1920x540 solutions
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// August 15, 2013.

#include "system.h"
#include "sdram.h"
#include "d2.h"
#include "gendef.h"
#include "sensor0.h"
#include "sensor1.h"
#include "i2c.h"
#ifdef SUPPORT_D2_LAYER2
#include "d2layer2.h"
#endif // SUPPORT_D2_LAYER2

#ifndef D2_I2C0_ENABLE
	#error D2_I2C0_ENABLE and D2_I2C1_ENABLE shall be both enabled.
#endif // D2_I2C0_ENABLE
#ifndef D2_I2C1_ENABLE
	#error D2_I2C0_ENABLE and D2_I2C1_ENABLE shall be both enabled.
#endif // D2_I2C1_ENABLE

#ifdef SUPPORT_D2_OPERATION


	
	// In this solution:
	//
	// Buffer size for ZT3150 VGA video sensor is 640x480x2x2 = 1,228,800.
	// 3 x 1,228,800 + 3 x 1,228,800 = 7,372,800, which is smaller than 8,388,608. (8MB)

	// SOU_FIXED_COLOR is used to send fixed color pattern to video destination.
	// This is very useful to verify and see if SOU timing is correct or not.
	// This option should be OFF on normal operation.
	//#define SOU_FIXED_COLOR

	// SENSOR_BYPASS_PATH allows the images from sensor source, bypasses all D2 sub-modules,
	// and then sends to the destination. Disabling this option (preferred) allows the images
	// storing into SDRAM buffers, and then sends to the destination.
	//#define SENSOR_BYPASS_PATH

	#define SOU_VGACLK    24
	#if (CLK_PLL / SOU_VGACLK >= 16)
		#error Maximum Clock Divider for SOU_VGACLK is 16.
	#endif // (CLK_PLL / SOU_VGACLK >= 16)

	#if CLK_SDRAM == 120
		#define SOU_DualVGACLK 40 //MHz
		//#define SOU_DualVGACLK 48 //MHz
		#define SOU_QuadVGACLK 60 //MHz
		//#define SOU_QuadVGACLK 80 //MHz // not good
	#else  // CLK_SDRAM == 160
		#define SOU_DualVGACLK 40 //MHz
		//#define SOU_QuadVGACLK 53 //MHz
		//#define SOU_QuadVGACLK 64 //MHz
		#define SOU_QuadVGACLK 80 //MHz // not good
	#endif // CLK_SDRAM

	#ifdef SUPPORT_FUZZY_STITCHING
		#include "zt3150_Accurate_Para.h"

		// Parameters for accurate stitching overlap size and table size.
		#define ACST_OUT_WIDTH		1920
		#define ACST_OUT_HEIGHT			1080

		#ifdef SUPPORT_OUTPUT_SCALING
			#define ACST_OVERLAP_WIDTH	128
		#else
			#define ACST_OVERLAP_WIDTH	64
		#endif // SUPPORT_SENSOR_SCALING

		#define ACST_STITCH_SRC_WIDTH		((ACST_OUT_WIDTH/2)+(ACST_OVERLAP_WIDTH/2))
		#define ACST_STITCH_SRC_HEIGHT		ACST_OUT_HEIGHT
	#endif // SUPPORT_FUZZY_STITCHING

	// ZT3125 Operating Modes
	#define D2_SENSOR_IDLE			0x00	// Idle
	#define D2_SENSOR_SINGLE_A		0x01	// Bypass video from channel A
	#define D2_SENSOR_SINGLE_B		0x02	// Bypass video from channel B
	#define D2_SENSOR_SINGLE_C		0x03	// Bypass video from channel C
	#define D2_SENSOR_SINGLE_D		0x04	// Bypass video from channel D
	#define D2_SENSOR_BOTH_AB		0x11	// Horizontal side-by-side from channels A and B
	#define D2_SENSOR_BOTH_AC		0x12	// Horizontal side-by-side from channels A and C
	#define D2_SENSOR_BOTH_AD		0x13	// Horizontal side-by-side from channels A and D
	#define D2_SENSOR_BOTH_BC		0x14	// Horizontal side-by-side from channels B and C
	#define D2_SENSOR_BOTH_BD		0x15	// Horizontal side-by-side from channels B and D
	#define D2_SENSOR_BOTH_CD		0x16	// Horizontal side-by-side from channels C and D
	#define D2_SENSOR_STITCH_AB		0x18	// Horizontal stitcing from channels A and B
	#define D2_SENSOR_STITCH_CD		0x19	// Horizontal stitcing from channels C and D
	#define D2_SENSOR_VERTICAL_AB		0x21	// Vertical side-by-side from channels A and B
	#define D2_SENSOR_VERTICAL_AC		0x22	// Vertical side-by-side from channels A and C
	#define D2_SENSOR_VERTICAL_AD		0x23	// Vertical side-by-side from channels A and D
	#define D2_SENSOR_VERTICAL_BC		0x24	// Vertical side-by-side from channels B and C
	#define D2_SENSOR_VERTICAL_BD		0x25	// Vertical side-by-side from channels B and D
	#define D2_SENSOR_VERTICAL_CD		0x26	// Vertical side-by-side from channels C and D
	#define D2_SENSOR_QUAD_PANORAMA		0x31	// Quad channels in panorama sequence
	#define D2_SENSOR_QUAD_ABCD		0x32	// Quad channels in A-B-C-D sequence
	#define D2_SENSOR_QUAD_ACBD		0x33	// Quad channels in A-C-B-D sequence
	#define D2_SENSOR_STITCH_PANORAMA	0x38	// Stitched channels in panorama sequence
	#define D2_SENSOR_STITCH_ABCD		0x39	// Stitched channels in A-B-C-D sequence

/*
 ******************************************************************************
 *
 *
 *
 ******************************************************************************
 */
tD2Register code D2TableBuffers[] = {
#ifdef SENSOR_BYPASS_PATH
	{0x022, 0x02},
#endif // SENSOR_BYPASS_PATH
	SDRAM_CONFIGURATION
	SDRAM_TIMING
	// SDA Memory Buffers
	{0x21A,	0x00},	{0x21B,	0x00},	{0x21C,	0x01},
  {0x21E,	0x00},	{0x21F,	0xA4},	{0x220,	0x20},
  {0x222,	0x00},	{0x223,	0x48},	{0x224,	0x40},
  {0x226,	0x00},	{0x227,	0xEC},	{0x228,	0x5F},
  {0x22A,	0x00},	{0x22B,	0x90},	{0x22C,	0x7F},
  {0x22E,	0x00},	{0x22F,	0x34},	{0x230,	0x9F},

	// IPU Memory Buffers
	
  {0x304,	0x00},	{0x305,	0x00},	{0x306,	0x01},
  {0x308,	0x00},	{0x309,	0xA4},	{0x30A,	0x20},
  {0x30C,	0x00},	{0x30D,	0x48},	{0x30E,	0x40},
  {0x310,	0x00},	{0x311,	0xEC},	{0x312,	0x5F},
  {0x314,	0x00},	{0x315,	0x90},	{0x316,	0x7F},
  {0x318,	0x00},	{0x319,	0x34},	{0x31A,	0x9F},
  
};


/*
 ******************************************************************************
 *
 *
 *
 ******************************************************************************
 */
tD2Register code D2TableSIU_ZT3150HD[] = {
	// SDA Memory Strides
	{0x210, 0x00}, {0x211, 0x0F}, {0x212, 0x00},
	{0x217, 0x00}, {0x218, 0x0F}, {0x219, 0x00},
	// SIU0 Windowing Parameters
	{0x232,	0x00},	{0x233,	0x00},
  {0x234,	0x00},	{0x235,	0x00},
  {0x236,	0x80},	{0x237,	0x07},
  {0x239,	0x1C},	{0x239,	0x02},
  // SIU0 Windowing Parameters
  {0x23B,	0x00},	{0x23C,	0x00},
  {0x23D,	0x00},	{0x23E,	0x00},
  {0x23F,	0x80},	{0x240,	0x07},
  {0x241,	0x1C},	{0x242,	0x02},

	// SIU Control and Configuration
	{0x201, 0x00},
	{0x23A, 0x21}, {0x243, 0x21},
	{0x203, 0x0C},
	{0x202, 0x03}, {0x202, 0x00},
	{0x200, 0x03},
	{0x203, 0x03}, {0x203, 0x00},
	//{0x200, 0x0F},//Move inside firmware codes
};

tD2Register code D2TableSIU_VGA[] = {
	// SDA Memory Strides
	{0x210, 0x00}, {0x211, 0x05}, {0x212, 0x00},
	{0x217, 0x00}, {0x218, 0x05}, {0x219, 0x00},
	// SIU Windowing Parameters
	{0x232, 0x00}, {0x233, 0x00},
	{0x234, 0x00}, {0x235, 0x00},
	{0x236, 0x00}, {0x237, 0x05},
	{0x238, 0xE0}, {0x239, 0x01},

	{0x23B, 0x00}, {0x23C, 0x00},
	{0x23D, 0x00}, {0x23E, 0x00},
	{0x23F, 0x00}, {0x240, 0x05},
	{0x241, 0xE0}, {0x242, 0x01},
	// SIU Control and Configuration
	{0x201, 0x00},
	{0x23A, 0x21}, {0x243, 0x21},
	{0x203, 0x0C},
	{0x202, 0x03}, {0x202, 0x00},
	{0x200, 0x03},
	{0x203, 0x03}, {0x203, 0x00},
	//{0x200, 0x0F},//Move inside firmware codes
};

tD2Register code D2TableSIU_DualVGA[] = {
	// SDA Memory Strides
	{0x210, 0x00}, {0x211, 0x0A}, {0x212, 0x00},
	{0x217, 0x00}, {0x218, 0x0A}, {0x219, 0x00},
	// SIU Windowing Parameters
	{0x232, 0x00}, {0x233, 0x00},
	{0x234, 0x00}, {0x235, 0x00},
	{0x236, 0x00}, {0x237, 0x0A},
	{0x238, 0xE0}, {0x239, 0x01},

	{0x23B, 0x00}, {0x23C, 0x00},
	{0x23D, 0x00}, {0x23E, 0x00},
	{0x23F, 0x00}, {0x240, 0x0A},
	{0x241, 0xE0}, {0x242, 0x01},
	// SIU Control and Configuration
	{0x201, 0x00},
	{0x23A, 0x21}, {0x243, 0x21},
	{0x203, 0x0C},
	{0x202, 0x03}, {0x202, 0x00}, 
	{0x200, 0x03},
	{0x203, 0x03}, {0x203, 0x00},
	//{0x200, 0x0F},//Move inside firmware codes
};

tD2Register code D2TableSIU_2xVGA[] = {
	// SDA Memory Strides
	{0x210, 0x00}, {0x211, 0x05}, {0x212, 0x00},
	{0x217, 0x00}, {0x218, 0x05}, {0x219, 0x00},
	// SIU Windowing Parameters
	{0x232, 0x00}, {0x233, 0x00},
	{0x234, 0x00}, {0x235, 0x00},
	{0x236, 0x00}, {0x237, 0x05},
	{0x238, 0xC0}, {0x239, 0x03},

	{0x23B, 0x00}, {0x23C, 0x00},
	{0x23D, 0x00}, {0x23E, 0x00},
	{0x23F, 0x00}, {0x240, 0x05},
	{0x241, 0xC0}, {0x242, 0x03},
	// SIU Control and Configuration
	{0x201, 0x00},
	{0x23A, 0x21}, {0x243, 0x21},
	{0x203, 0x0C},
	{0x202, 0x03}, {0x202, 0x00},
	{0x200, 0x03},
	{0x203, 0x03}, {0x203, 0x00},
	//{0x200, 0x0F},//Move inside firmware codes
};


/*
 ******************************************************************************
 *
 *
 *
 ******************************************************************************
 */
tD2Register code D2TableSOU_VGA[] = {
	// 640 x 480, 30fps at 24MHz (CLK_SDRAM = 120MHz)
	// 640 x 480, 30fps at 24.5MHz (CLK_SDRAM = 160MHz)
	// 640 x 480, 30fps at 26.67MHz (CLK_SDRAM = 160MHz)
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80}, {0x413, 0x02},
	{0x414, 0xE0}, {0x415, 0x01},
#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0E},
#endif // SOU_FIXED_COLOR

#if CLK_SDRAM == 120
	{0x41B, 0xF2}, {0x41C, 0x05},
#else  // CLK_SDRAM == 120
    #if 1
	{0x41B, 0x12}, {0x41C, 0x06}, // at 24.5MHz
    #else
	{0x41B, 0x9C}, {0x41C, 0x06}, // at 26.67MHz
    #endif
#endif // CLK_SDRAM == 120
	{0x41D, 0x04}, {0x41E, 0x00},
	{0x41F, 0x44}, {0x420, 0x00},
	{0x421, 0x44}, {0x422, 0x00},
	{0x423, 0x43}, {0x424, 0x05},

	{0x425, 0x0D}, {0x426, 0x02},
	{0x427, 0x01}, {0x428, 0x00},
	{0x42B, 0x02}, {0x42C, 0x00},
	{0x42F, 0x02}, {0x430, 0x00},
	{0x431, 0xE1}, {0x432, 0x01},
	// SOU Control
	{0x411, 0x05},
};

tD2Register code D2TableSOU_DualVGA[] = {
	// 1280x480, 30fps at 40MHz, 48MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x00}, {0x413, 0x05},
	{0x414, 0xE0}, {0x415, 0x01},
#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0D},
#endif // SOU_FIXED_COLOR

#if SOU_DualVGACLK == 40
	{0x41B, 0xB6}, {0x41C, 0x0A},
#else  // SOU_DualVGACLK == 48
	{0x41B, 0xDC}, {0x41C, 0x0A},
#endif // SOU_DualVGACLK
	{0x41D, 0x04}, {0x41E, 0x00},
	{0x41F, 0x44}, {0x420, 0x00},
	{0x421, 0x44}, {0x422, 0x00},
	{0x423, 0x43}, {0x424, 0x0A},

#if SOU_DualVGACLK == 40
	{0x425, 0xE6}, {0x426, 0x01},
#else  // SOU_DualVGACLK == 48
	{0x425, 0x3F}, {0x426, 0x02},
#endif // SOU_DualVGACLK
	{0x427, 0x01}, {0x428, 0x00},
	{0x42B, 0x02}, {0x42C, 0x00},
	{0x42F, 0x02}, {0x430, 0x00},
	{0x431, 0xE1}, {0x432, 0x01},
	// SOU Control
	{0x411, 0x05},
};

tD2Register code D2TableSOU_2xVGA[] = {
	// 640x960, 30fps at 40MHz, 48MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80}, {0x413, 0x02},
	{0x414, 0xC0}, {0x415, 0x03},
#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0C},
#endif // SOU_FIXED_COLOR

#if SOU_DualVGACLK == 40
	{0x41B, 0x60}, {0x41C, 0x05},
#else  // SOU_DualVGACLK == 48
	{0x41B, 0xF2}, {0x41C, 0x05},
#endif // SOU_DualVGACLK
	{0x41D, 0x04}, {0x41E, 0x00},
	{0x41F, 0x44}, {0x420, 0x00},
	{0x421, 0x44}, {0x422, 0x00},
	{0x423, 0x43}, {0x424, 0x05},

#if SOU_DualVGACLK == 40
	{0x425, 0xC8}, {0x426, 0x03},
#else  // SOU_DualVGACLK == 48
	{0x425, 0x1A}, {0x426, 0x04},
#endif // SOU_DualVGACLK
	{0x427, 0x01}, {0x428, 0x00},
	{0x42B, 0x02}, {0x42C, 0x00},
	{0x42F, 0x02}, {0x430, 0x00},
	{0x431, 0xC1}, {0x432, 0x03},
	// SOU Control
	{0x411, 0x05},
};

tD2Register code D2TableSOU_QuadVGA[] = {
	// 2560x480, 21fps at 53MHz
	// 2560x480, 24fps at 64MHz
	// 2560x480, 30fps at 80MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x00}, {0x413, 0x0A},
	{0x414, 0xE0}, {0x415, 0x01},
#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0B},
#endif // SOU_FIXED_COLOR

#if SOU_QuadVGACLK == 53
	{0x41B, 0x8A}, {0x41C, 0x14},
#else  // SOU_QuadVGACLK == 64, 80
	{0x41B, 0x36}, {0x41C, 0x15},
#endif // SOU_QuadVGACLK
	{0x41D, 0x04}, {0x41E, 0x00},
	{0x41F, 0x44}, {0x420, 0x00},
	{0x421, 0x44}, {0x422, 0x00},
	{0x423, 0x43}, {0x424, 0x14},

#if SOU_QuadVGACLK == 53
	{0x425, 0xE3}, {0x426, 0x01},
#else  // SOU_QuadVGACLK == 64, 80
	{0x425, 0xEB}, {0x426, 0x01},
#endif // SOU_QuadVGACLK
	{0x427, 0x01}, {0x428, 0x00},
	{0x42B, 0x02}, {0x42C, 0x00},
	{0x42F, 0x02}, {0x430, 0x00},
	{0x431, 0xE1}, {0x432, 0x01},
	// SOU Control
	{0x411, 0x05},
};

tD2Register code D2TableSOU_CubeVGA[] = {
	// 1280x960, 21fps at 53MHz
	// 1280x960, 24fps at 64MHz
	// 1280x960, 30fps at 80MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x00}, {0x413, 0x05},
	{0x414, 0xC0}, {0x415, 0x03},
#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0A},
#endif // SOU_FIXED_COLOR

#if SOU_QuadVGACLK == 53
	{0x41B, 0x50}, {0x41C, 0x0A},
#else  // SOU_QuadVGACLK == 64, 80
	{0x41B, 0x9B}, {0x41C, 0x0A},
#endif // SOU_QuadVGACLK
	{0x41D, 0x04}, {0x41E, 0x00},
	{0x41F, 0x44}, {0x420, 0x00},
	{0x421, 0x44}, {0x422, 0x00},
	{0x423, 0x43}, {0x424, 0x0A},

#if SOU_QuadVGACLK == 53
	{0x425, 0xC2}, {0x426, 0x03},
#else  // SOU_QuadVGACLK == 64, 80
	{0x425, 0xD6}, {0x426, 0x03},
#endif // SOU_QuadVGACLK
	{0x427, 0x01}, {0x428, 0x00},
	{0x42B, 0x02}, {0x42C, 0x00},
	{0x42F, 0x02}, {0x430, 0x00},
	{0x431, 0xC1}, {0x432, 0x03},
	// SOU Control
	{0x411, 0x05},
};

tD2Register code D2TableSOU_FullHD[] = {
	// 1920x1080, 20fps at 80MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80}, {0x413, 0x07},
	{0x414, 0x38}, {0x415, 0x04},
#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0A},
#endif // SOU_FIXED_COLOR

//SOU TG parameters					
  {0x41B,	0x4B},	{0x41C,	0x0F},		
  {0x41D,	0x04},	{0x41E,	0x00},		
  {0x41F,	0x08},	{0x420,	0x00},		
  {0x421,	0x0C},	{0x422,	0x00},		
  {0x423,	0x0B},	{0x424,	0x0F},		
  {0x425,	0x87},	{0x426,	0x08},		
  {0x427,	0x02},	{0x428,	0x00},		
  {0x42B,	0x06},	{0x42C,	0x00},		
  {0x42F,	0x08},	{0x430,	0x00},		
  {0x431,	0x77},	{0x432,	0x08},		

	// SOU Control
	{0x411, 0x05},
};

/*
 ******************************************************************************
 *
 *
 *
 ******************************************************************************
 */
#ifdef SENSOR_BYPASS_PATH
tD2Register code D2TableBypassSensor0[] = {
	{0x022, 0x00},
};

tD2Register code D2TableBypassSensor1[] = {
	{0x022, 0x01},
};
#else  // SENSOR_BYPASS_PATH
tD2Register code D2TableSingleSensor0[] = {
	// IPU Control and Configuration
	{0x32C, 0x00},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};

tD2Register code D2TableSingleSensor1[] = {
	// IPU Control and Configuration
	{0x32C, 0x20},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};
#endif // SENSOR_BYPASS_PATH

tD2Register code D2TableHalfSideBySide[] = {
	// IPU Control and Configuration
	{0x32C, 0x42},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};

tD2Register code D2TableHorizontalSideBySide[] = {
	// IPU Control and Configuration
	{0x32C, 0x02},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};

tD2Register code D2TableVerticalSideBySide[] = {
	// IPU Control and Configuration
	{0x32C, 0x04},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};

tD2Register code D2TableAccurateStitch[] = {
	// STCH Windowing Parameters
	{0x31C, (unsigned char)  (ACST_STITCH_SRC_WIDTH*2)},	 {0x31D, (unsigned char) ((ACST_STITCH_SRC_WIDTH*2)>>8)},
	{0x320, (unsigned char)  (ACST_STITCH_SRC_HEIGHT)},	 {0x321, (unsigned char) ((ACST_STITCH_SRC_HEIGHT)>>8)},
	{0x322, (unsigned char)  (ACST_STITCH_SRC_WIDTH*2)},	 {0x323, (unsigned char) ((ACST_STITCH_SRC_WIDTH*2)>>8)},
	{0x326, (unsigned char)  (ACST_STITCH_SRC_HEIGHT)},	 {0x327, (unsigned char) ((ACST_STITCH_SRC_HEIGHT)>>8)},

	{0x32E, (unsigned char)  (2048/(ACST_OVERLAP_WIDTH*2))}, {0x32F, (unsigned char) ((2048/(ACST_OVERLAP_WIDTH*2))>>8)},
	{0x330, (unsigned char)  (ACST_OVERLAP_WIDTH*2)},	 {0x331, (unsigned char) ((ACST_OVERLAP_WIDTH*2)>>8)},

	// LDC table
	{0x382, 0x00}, {0x383, 0x10}, {0x384, 0x00},
	{0x386, 0x00}, {0x387, 0x90}, {0x388, 0x00},
	// IPU Control and Configuration
	{0x32C, 0x06},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0xF0},
};


/*
 ******************************************************************************
 *
 *
 *
 ******************************************************************************
 */
tD2Register code D2TableIPU_VGA[] = {
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x00}, {0x37D, 0x05}, {0x37E, 0x00},
	{0x37F, 0x00}, {0x380, 0x05}, {0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00}, {0x31F, 0x05},
	{0x324, 0x00}, {0x325, 0x05},

	{0x328, 0x1E}, {0x329, 0x00},
	{0x32A, 0x1E}, {0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x00}, {0x31D, 0x05},
	{0x320, 0xE0}, {0x321, 0x01},
	{0x322, 0x00}, {0x323, 0x05},
	{0x326, 0xE0}, {0x327, 0x01},
};

tD2Register code D2TableIPU_DualVGA[] = {
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x00}, {0x37D, 0x0A}, {0x37E, 0x00},
	{0x37F, 0x00}, {0x380, 0x0A}, {0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00}, {0x31F, 0x0A},
	{0x324, 0x00}, {0x325, 0x0A},
	{0x328, 0x1E}, {0x329, 0x00},
	{0x32A, 0x1E}, {0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x00}, {0x31D, 0x0A},
	{0x320, 0xE0}, {0x321, 0x01},
	{0x322, 0x00}, {0x323, 0x0A},
	{0x326, 0xE0}, {0x327, 0x01},
};

tD2Register code D2TableIPU_2xVGA[] = {
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x00}, {0x37D, 0x05}, {0x37E, 0x00},
	{0x37F, 0x00}, {0x380, 0x05}, {0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00}, {0x31F, 0x05},
	{0x324, 0x00}, {0x325, 0x05},
	{0x328, 0x1E}, {0x329, 0x00},
	{0x32A, 0x1E}, {0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x00}, {0x31D, 0x05},
	{0x320, 0xC0}, {0x321, 0x03},
	{0x322, 0x00}, {0x323, 0x05},
	{0x326, 0xC0}, {0x327, 0x03},
};

tD2Register code D2TableIPU_ZT3150HD[] = {
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x00}, {0x37D, 0x0F}, {0x37E, 0x00},
	{0x37F, 0x00}, {0x380, 0x0F}, {0x381, 0x00},
  // IPU Windowing Parameters					
  {0x31E,	0x00},	{0x31F,	0x0F},		
  {0x324,	0x00},	{0x325,	0x0F},		
  {0x328,	0x1E},	{0x329,	0x00},		
  {0x32A,	0x1E},	{0x32B,	0x00},		
  					
  // STCH Windowing Parameters					
  {0x31C,	0x00},	{0x31D,	0x0F},		
  {0x320,	0x70},	{0x321,	0x08},		
  {0x322,	0x00},	{0x323,	0x0F},		
  {0x326,	0x70},	{0x327,	0x08},		

};

/*
 ******************************************************************************
 *
 *
 *
 ******************************************************************************
 */
void fnD2TableLoad(void)
{
	if (nD2TableVerified == 0) {
		fnD2MoveCodeToSingleBankDRAM(D2CalibrationTable1, 0x01000L, ((ACST_STITCH_SRC_WIDTH)/16+1)*(ACST_STITCH_SRC_HEIGHT/16+1)*8);
		fnD2MoveCodeToSingleBankDRAM(D2CalibrationTable2, 0x09000L, ((ACST_STITCH_SRC_WIDTH)/16+1)*(ACST_STITCH_SRC_HEIGHT/16+1)*8);	
	}
}

void fnD2TableVerify(void)
{
	if (nD2TableVerified == 0) {
		fnD2VerifyCodeToSingleBankDRAM(D2CalibrationTable1, 0x01000L, ((ACST_STITCH_SRC_WIDTH)/16+1)*(ACST_STITCH_SRC_HEIGHT/16+1)*8);
		fnD2VerifyCodeToSingleBankDRAM(D2CalibrationTable2, 0x09000L, ((ACST_STITCH_SRC_WIDTH)/16+1)*(ACST_STITCH_SRC_HEIGHT/16+1)*8);
		nD2TableVerified = 1;
	}
}


/*
 ******************************************************************************
 *
 *
 *
 ******************************************************************************
 */
unsigned char xdata ucSOUfixedColor   _at_ 0x041A;

unsigned char fnD2RunSingleSensor(unsigned ucMode)
{
	D2WriteOutputClock(SOU_VGACLK);		// Set Host Clock in MHz
	switch (ucMode) {
	case D2_SENSOR_SINGLE_A:
	case D2_SENSOR_SINGLE_B:
#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor0, sizeof(D2TableBypassSensor0) / sizeof(tD2Register));
#else
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableIPU_VGA, sizeof(D2TableIPU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor0, sizeof(D2TableSingleSensor0) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0D;
#endif // SENSOR_BYPASS_PATH
		return SUCCESS;
	case D2_SENSOR_SINGLE_C:
	case D2_SENSOR_SINGLE_D:
#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor1, sizeof(D2TableBypassSensor1) / sizeof(tD2Register));
#else
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableIPU_VGA, sizeof(D2TableIPU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor1, sizeof(D2TableSingleSensor1) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0E;
#endif // SENSOR_BYPASS_PATH
		return SUCCESS;
	}
	return FAILURE;
}

unsigned char fnD2RunDualSensors(unsigned ucMode)
{
	D2WriteOutputClock(SOU_DualVGACLK);		// Set Host Clock in MHz
	switch (ucMode) {
	case D2_SENSOR_BOTH_AB:
	case D2_SENSOR_STITCH_AB:
#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor0, sizeof(D2TableBypassSensor0) / sizeof(tD2Register));
#else
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_DualVGA, sizeof(D2TableSOU_DualVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableIPU_DualVGA, sizeof(D2TableIPU_DualVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor0, sizeof(D2TableSingleSensor0) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_DualVGA, sizeof(D2TableSIU_DualVGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0D;
#endif // SENSOR_BYPASS_PATH
		return SUCCESS;
	case D2_SENSOR_BOTH_CD:
	case D2_SENSOR_STITCH_CD:
#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor1, sizeof(D2TableBypassSensor1) / sizeof(tD2Register));
#else
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_DualVGA, sizeof(D2TableSOU_DualVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableIPU_DualVGA, sizeof(D2TableIPU_DualVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor1, sizeof(D2TableSingleSensor1) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_DualVGA, sizeof(D2TableSIU_DualVGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0E;
#endif // SENSOR_BYPASS_PATH
		return SUCCESS;
	case D2_SENSOR_BOTH_AC:
	case D2_SENSOR_BOTH_AD:
	case D2_SENSOR_BOTH_BC:
	case D2_SENSOR_BOTH_BD:
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_DualVGA, sizeof(D2TableSOU_DualVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableHorizontalSideBySide, sizeof(D2TableHorizontalSideBySide) / sizeof(tD2Register));
		fnD2WriteTable(D2TableIPU_VGA, sizeof(D2TableIPU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;

	case D2_SENSOR_VERTICAL_AB:
#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor0, sizeof(D2TableBypassSensor0) / sizeof(tD2Register));
#else
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_2xVGA, sizeof(D2TableSOU_2xVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableIPU_2xVGA, sizeof(D2TableIPU_2xVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor0, sizeof(D2TableSingleSensor0) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_2xVGA, sizeof(D2TableSIU_2xVGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0D;
#endif // SENSOR_BYPASS_PATH
		return SUCCESS;
	case D2_SENSOR_VERTICAL_CD:
#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor1, sizeof(D2TableBypassSensor1) / sizeof(tD2Register));
#else
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_2xVGA, sizeof(D2TableSOU_2xVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableIPU_2xVGA, sizeof(D2TableIPU_2xVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor1, sizeof(D2TableSingleSensor1) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_2xVGA, sizeof(D2TableSIU_2xVGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0E;
#endif // SENSOR_BYPASS_PATH
		return SUCCESS;
	case D2_SENSOR_VERTICAL_AC:
	case D2_SENSOR_VERTICAL_AD:
	case D2_SENSOR_VERTICAL_BC:
	case D2_SENSOR_VERTICAL_BD:
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_2xVGA, sizeof(D2TableSOU_2xVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableIPU_VGA, sizeof(D2TableIPU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableVerticalSideBySide, sizeof(D2TableVerticalSideBySide) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;
	}
	return FAILURE;
}

unsigned char fnD2RunQuadSensors(unsigned ucMode)
{
	D2WriteOutputClock(SOU_QuadVGACLK);		// Set Host Clock in MHz
	switch (ucMode) {
	case D2_SENSOR_QUAD_PANORAMA:
	case D2_SENSOR_STITCH_PANORAMA:
	case D2_SENSOR_QUAD_ABCD:
	case D2_SENSOR_QUAD_ACBD:
	case D2_SENSOR_STITCH_ABCD:
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_FullHD, sizeof(D2TableSOU_FullHD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableIPU_ZT3150HD, sizeof(D2TableIPU_ZT3150HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableVerticalSideBySide, sizeof(D2TableVerticalSideBySide) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_ZT3150HD, sizeof(D2TableSIU_ZT3150HD) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;

	}
	return FAILURE;
}


/*
 ******************************************************************************
 *
 *
 *
 ******************************************************************************
 */
unsigned char xdata ucSN0_CLK   _at_ 0x0032;
unsigned char xdata ucSN1_CLK   _at_ 0x0034;

unsigned char fnD2ResetLayer2(void)
{
	unsigned char  ucCmdAck;

	ucSN0_CLK = ucSN1_CLK = 0x08;
	D2WriteSensorClock(24);  // Set Sensor Clock in MHz
	fnD2SetSN0Reset();
	fnD2SetSN1Reset();
	fnD2IdleDelay(200); // count 100 around 12ms
	fnD2ClrSN0Reset();
	fnD2ClrSN1Reset();
	fnD2IdleDelay(200); // count 100 around 12ms

	ucCmdAck = 0x00;
	while (ucCmdAck != 0x80) {
		I2CREAD_SN0_REGISTER(0x0080, ucCmdAck);
	}
	ucCmdAck = 0x00;
	while (ucCmdAck != 0x80) {
		I2CREAD_SN1_REGISTER(0x0080, ucCmdAck);
	}
	return SUCCESS;
}

unsigned char fnD2InitLayer2(unsigned char ucMode, unsigned char ucResolution)
{
	unsigned char ucD2_0Mode;
	unsigned char ucD2_1Mode;
	unsigned char ucD2_0Status;
	unsigned char ucD2_1Status;

	fnD2ResetLayer2();
	ucResetControl = 0x74;   // 40:SIU Reset, 20:IPU Reset, 10:SOU Reset, 04: DRAM Reset

	switch (ucMode) {
	case D2_SENSOR_SINGLE_A:
	case D2_SENSOR_SINGLE_C:
	case D2_SENSOR_BOTH_AC:
	case D2_SENSOR_VERTICAL_AC:
		ucD2_0Mode = D2_OPERATION_BYPASS_SENSOR0;
		ucD2_1Mode = D2_OPERATION_BYPASS_SENSOR0;
		break;
	case D2_SENSOR_SINGLE_D:
	case D2_SENSOR_BOTH_AD:
	case D2_SENSOR_VERTICAL_AD:
		ucD2_0Mode = D2_OPERATION_BYPASS_SENSOR0;
		ucD2_1Mode = D2_OPERATION_BYPASS_SENSOR1;
		break;
	case D2_SENSOR_SINGLE_B:
	case D2_SENSOR_BOTH_BC:
	case D2_SENSOR_VERTICAL_BC:
		ucD2_0Mode = D2_OPERATION_BYPASS_SENSOR1;
		ucD2_1Mode = D2_OPERATION_BYPASS_SENSOR0;
		break;
	case D2_SENSOR_BOTH_BD:
	case D2_SENSOR_VERTICAL_BD:
		ucD2_0Mode = D2_OPERATION_BYPASS_SENSOR1;
		ucD2_1Mode = D2_OPERATION_BYPASS_SENSOR1;
		break;
	case D2_SENSOR_STITCH_AB:
	case D2_SENSOR_STITCH_CD:
	case D2_SENSOR_STITCH_PANORAMA:
	case D2_SENSOR_STITCH_ABCD:
		ucD2_0Mode = D2_OPERATION_ACCURATE_STITCHING;
		ucD2_1Mode = D2_OPERATION_ACCURATE_STITCHING;
		break;
	case D2_SENSOR_BOTH_AB:
	case D2_SENSOR_BOTH_CD:
	case D2_SENSOR_QUAD_PANORAMA:
	case D2_SENSOR_QUAD_ABCD:
		ucD2_0Mode = D2_OPERATION_HORIZONTAL_SIDE_BY_SIDE;
		ucD2_1Mode = D2_OPERATION_HORIZONTAL_SIDE_BY_SIDE;
		break;
	case D2_SENSOR_VERTICAL_AB:
	case D2_SENSOR_VERTICAL_CD:
	case D2_SENSOR_QUAD_ACBD:
		ucD2_0Mode = D2_OPERATION_VERTICAL_SIDE_BY_SIDE;
		ucD2_1Mode = D2_OPERATION_VERTICAL_SIDE_BY_SIDE;
		break;
	default:
		ucResetControl = 0x00;
		return FAILURE;
	}
	ucD2_0Status = fnD2SetSN0Command(ucD2_0Mode, ucResolution);
	ucD2_1Status = fnD2SetSN1Command(ucD2_1Mode, ucResolution);

	if ((ucD2_0Status == STATUS_D2_CMD_NOT_SENT) || (ucD2_1Status == STATUS_D2_CMD_NOT_SENT)) {
		ucResetControl = 0x00;
		return FAILURE;
	}
	while (ucD2_0Status == STATUS_D2_CMD_TIMEOUT)
		ucD2_0Status = fnD2GetSN0Status();
	while (ucD2_1Status == STATUS_D2_CMD_TIMEOUT)
		ucD2_1Status = fnD2GetSN1Status();
	if ((ucD2_0Status == STATUS_D2_CMD_OK) && (ucD2_1Status == STATUS_D2_CMD_OK)) {
		ucResetControl = 0x00;
		return SUCCESS;
	} else {
		ucResetControl = 0x00;
		return FAILURE;
	}

}


/*
 ******************************************************************************
 *
 *
 *
 ******************************************************************************
 */
unsigned char fnSetOperatingMode(unsigned char ucMode, unsigned char ucResolution)
{
	switch (ucMode) {
	case D2_OPERATION_IDLE:
		D2DisableSensorClock();
		D2WriteOutputClock(SOU_VGACLK);		// Set Host Clock in MHz
		return SUCCESS;

	case D2_SENSOR_SINGLE_A:
	case D2_SENSOR_SINGLE_B:
	case D2_SENSOR_SINGLE_C:
	case D2_SENSOR_SINGLE_D:
		if (fnD2InitLayer2(ucMode, ucResolution) == FAILURE)
			return FAILURE;
		return fnD2RunSingleSensor(ucMode);

	case D2_SENSOR_BOTH_AB:
	case D2_SENSOR_BOTH_AC:
	case D2_SENSOR_BOTH_AD:
	case D2_SENSOR_BOTH_BC:
	case D2_SENSOR_BOTH_BD:
	case D2_SENSOR_BOTH_CD:
	case D2_SENSOR_VERTICAL_AB:
	case D2_SENSOR_VERTICAL_AC:
	case D2_SENSOR_VERTICAL_AD:
	case D2_SENSOR_VERTICAL_BC:
	case D2_SENSOR_VERTICAL_BD:
	case D2_SENSOR_VERTICAL_CD:
	case D2_SENSOR_STITCH_AB:
	case D2_SENSOR_STITCH_CD:
		if (fnD2InitLayer2(ucMode, ucResolution) == FAILURE)
			return FAILURE;
		return fnD2RunDualSensors(ucMode);

	case D2_SENSOR_QUAD_PANORAMA:
	case D2_SENSOR_QUAD_ABCD:
	case D2_SENSOR_QUAD_ACBD:
	case D2_SENSOR_STITCH_PANORAMA:
	case D2_SENSOR_STITCH_ABCD:
		if (fnD2InitLayer2(ucMode, ucResolution) == FAILURE)
			return FAILURE;
		return fnD2RunQuadSensors(ucMode);
	}
	return FAILURE;
}


/*
 ******************************************************************************
 *
 *
 *
 ******************************************************************************
 */
void fnDoHouseKeeping(void)
{
}


/*
 ******************************************************************************
 *
 *
 *
 ******************************************************************************
 */
void fnDoInitialization(void)
{
#ifdef D2_I2C0_ENABLE
	I2CWorkItem0.I2CConfiguration = SENSOR0_I2C_CONFIG;
	I2CWorkItem0.I2CSlaveAddress  = SENSOR0_I2C_SLAVE;
#endif // D2_I2C0_ENABLE
#ifdef D2_I2C1_ENABLE
	I2CWorkItem1.I2CConfiguration = SENSOR1_I2C_CONFIG;
	I2CWorkItem1.I2CSlaveAddress  = SENSOR1_I2C_SLAVE;
#endif // D2_I2C1_ENABLE

	fnD2ResetLayer2();

#if 0
	//#define D2_TESTING_MODE D2_SENSOR_SINGLE_A
	//#define D2_TESTING_MODE D2_SENSOR_SINGLE_B
	//#define D2_TESTING_MODE D2_SENSOR_SINGLE_C
	//#define D2_TESTING_MODE D2_SENSOR_SINGLE_D

	//#define D2_TESTING_MODE D2_SENSOR_BOTH_AB
	//#define D2_TESTING_MODE D2_SENSOR_BOTH_AC
	//#define D2_TESTING_MODE D2_SENSOR_BOTH_AD
	//#define D2_TESTING_MODE D2_SENSOR_BOTH_BC
	//#define D2_TESTING_MODE D2_SENSOR_BOTH_BD
	#define D2_TESTING_MODE D2_SENSOR_BOTH_CD
	//#define D2_TESTING_MODE D2_SENSOR_STITCH_AB
	//#define D2_TESTING_MODE D2_SENSOR_STITCH_CD

	//#define D2_TESTING_MODE D2_SENSOR_VERTICAL_AB
	//#define D2_TESTING_MODE D2_SENSOR_VERTICAL_AC
	//#define D2_TESTING_MODE D2_SENSOR_VERTICAL_AD
	//#define D2_TESTING_MODE D2_SENSOR_VERTICAL_BC
	//#define D2_TESTING_MODE D2_SENSOR_VERTICAL_BD
	//#define D2_TESTING_MODE D2_SENSOR_VERTICAL_CD

	//#define D2_TESTING_MODE D2_SENSOR_QUAD_PANORAMA
	//#define D2_TESTING_MODE D2_SENSOR_QUAD_ABCD
	//#define D2_TESTING_MODE D2_SENSOR_QUAD_ACBD

	fnSetOperatingMode(D2_TESTING_MODE, D2_RESOLUTION_VGA_60Hz);
#endif
}

#endif // SUPPORT_D2_OPERATION
