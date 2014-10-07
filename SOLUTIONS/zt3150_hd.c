//
// Zealtek D2 project
// ZT3150 Twin HD sensor solutions
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// July 23, 2012.

#include "system.h"
#include "sdram.h"
#include "d2.h"
#include "gendef.h"

#ifdef SUPPORT_D2_OPERATION

	// In this solution:
	//
	// Buffer size for HD video sensor is 1280x720x2 = 1,843,200.
	// 3 x 1,843,200 + 3 x 1,843,200 = 11,059,200, which is smaller than 16,777,216. (16MB)

	// SOU_FIXED_COLOR is used to send fixed color pattern to video destination.
	// This is very useful to verify and see if SOU timing is correct or not.
	// This option should be OFF on normal operation.
	//#define SOU_FIXED_COLOR

	#define SOU_VGACLK    24
	#if (CLK_PLL / SOU_VGACLK >= 16)
		#error Maximum Clock Divider for SOU_VGACLK is 16.
	#endif // (CLK_PLL / SOU_VGACLK >= 16)

	#if CLK_SDRAM == 120
		#define SOU_DualVGACLK 40 //MHz
		//#define SOU_DualVGACLK 48 //MHz
		#define SOU_HDCLK     48 //MHz
		//#define SOU_HDCLK     60 //MHz // not good
		#define SOU_DualHDCLK 60 //MHz
		//#define SOU_DualHDCLK 80 //MHz // not good
	#else  // CLK_SDRAM == 160
		#define SOU_DualVGACLK 40 //MHz
		#define SOU_HDCLK     53 //MHz
		//#define SOU_HDCLK     64 //MHz
		#define SOU_DualHDCLK 53 //MHz
		//#define SOU_DualHDCLK 64 //MHz
		//#define SOU_DualHDCLK 80 //MHz // not good
	#endif // CLK_SDRAM

	// SENSOR_BYPASS_PATH allows the images from sensor source, bypasses all D2 sub-modules,
	// and then sends to the destination. Disabling this option (preferred) allows the images
	// storing into SDRAM buffers, and then sends to the destination.
	//#define SENSOR_BYPASS_PATH

	// SUPPORT_VGA_STITCHING_PATTERN is used to send fixed color pattern to video destination.
	// Because of the code size limitation, one set of stitching table can be supported.
	// On system testing, turn on this option to make sure the SS9203 firmware runs smoothly.
	// This option should be OFF on normal operation.
	#define SUPPORT_VGA_STITCHING_PATTERN

	#ifdef SUPPORT_FUZZY_STITCHING
		#include "zt3150_Accurate_Para.h"

		// Parameters for accurate stitching overlap size and table size.
		#define ACST_OUT_WIDTH	1920
		#if IMAGE_STITCHING_SIZE == STITCHING_SIZE_1920x560
		#define ACST_OUT_HEIGHT	560
		#else  // IMAGE_STITCHING_SIZE == STITCHING_SIZE_1920x544
		#define ACST_OUT_HEIGHT	544
		#endif // IMAGE_STITCHING_SIZE
		#ifdef SUPPORT_SENSOR_SCALING
		#define ACST_OVERLAP_WIDTH	64
		#else  // SUPPORT_SENSOR_SCALING
		#define ACST_OVERLAP_WIDTH	128
		#endif // SUPPORT_SENSOR_SCALING

		#define ACST_STITCH_SRC_WIDTH	((ACST_OUT_WIDTH/2)+(ACST_OVERLAP_WIDTH/2))
		#define ACST_STITCH_SRC_HEIGHT	ACST_OUT_HEIGHT
	#endif // SUPPORT_FUZZY_STITCHING

tD2Register code D2TableBuffers_VGA[] =
{
#ifdef SENSOR_BYPASS_PATH
	{0x022, 0x02},
#endif // SENSOR_BYPASS_PATH
	SDRAM_CONFIGURATION
	SDRAM_TIMING
	// SDA Memory Buffers
	{0x21A, 0x00},
	{0x21B, 0x00},
	{0x21C, 0x01},
	{0x21E, 0x00},
	{0x21F, 0x20},
	{0x220, 0x1D},
	{0x222, 0x00},
	{0x223, 0x40},
	{0x224, 0x39},
	{0x226, 0x00},
	{0x227, 0x60},
	{0x228, 0x55},
	{0x22A, 0x00},
	{0x22B, 0x80},
	{0x22C, 0x71},
	{0x22E, 0x00},
	{0x22F, 0xA0},
	{0x230, 0x8D},
	// IPU Memory Buffers
	{0x304, 0x00},
	{0x305, 0x00},
	{0x306, 0x01},
	{0x308, 0x00},
	{0x309, 0x20},
	{0x30A, 0x1D},
	{0x30C, 0x00},
	{0x30D, 0x40},
	{0x30E, 0x39},
	{0x310, 0x00},
	{0x311, 0x60},
	{0x312, 0x55},
	{0x314, 0x00},
	{0x315, 0x80},
	{0x316, 0x71},
	{0x318, 0x00},
	{0x319, 0xA0},
	{0x31A, 0x8D},
};

tD2Register code D2TableBuffers_HD[] =
{
#ifdef SENSOR_BYPASS_PATH
	{0x022, 0x02},
#endif // SENSOR_BYPASS_PATH
	SDRAM_CONFIGURATION
	SDRAM_TIMING
	// SDA Memory Buffers
	{0x21A, 0x00},
	{0x21B, 0x00},
	{0x21C, 0x01},
	{0x21E, 0x00},
	{0x21F, 0x20},
	{0x220, 0x1D},
	{0x222, 0x00},
	{0x223, 0x40},
	{0x224, 0x39},
	{0x226, 0x00},
	{0x227, 0x60},
	{0x228, 0x55},
	{0x22A, 0x00},
	{0x22B, 0x80},
	{0x22C, 0x71},
	{0x22E, 0x00},
	{0x22F, 0xA0},
	{0x230, 0x8D},
	// IPU Memory Buffers
	{0x304, 0x00},
	{0x305, 0x00},
	{0x306, 0x01},
	{0x308, 0x00},
	{0x309, 0x20},
	{0x30A, 0x1D},
	{0x30C, 0x00},
	{0x30D, 0x40},
	{0x30E, 0x39},
	{0x310, 0x00},
	{0x311, 0x60},
	{0x312, 0x55},
	{0x314, 0x00},
	{0x315, 0x80},
	{0x316, 0x71},
	{0x318, 0x00},
	{0x319, 0xA0},
	{0x31A, 0x8D},
};

tD2Register code D2TableSIU_VGA[] =
{
	// SDA Memory Strides
	{0x210, 0x00},
	{0x211, 0x05},
	{0x212, 0x00},
	{0x217, 0x00},
	{0x218, 0x05},
	{0x219, 0x00},
	// SIU Windowing Parameters
	{0x232, 0x00},
	{0x233, 0x00},
	{0x234, 0x00},
	{0x235, 0x00},
	{0x236, 0x00},
	{0x237, 0x05},
	{0x238, 0xE0},
	{0x239, 0x01},
	{0x23B, 0x00},
	{0x23C, 0x00},
	{0x23D, 0x00},
	{0x23E, 0x00},
	{0x23F, 0x00},
	{0x240, 0x05},
	{0x241, 0xE0},
	{0x242, 0x01},
	// SIU Control and Configuration
	{0x201, 0x00},
	{0x23A, 0x21},
	{0x243, 0x21},
	{0x203, 0x0C},
	{0x202, 0x03},
	{0x202, 0x00},
	{0x200, 0x03},
	{0x203, 0x03},
	{0x203, 0x00},
	//{0x200, 0x0F},//Move inside firmware codes
};

tD2Register code D2TableSIU_HD[] =
{
	// SDA Memory Strides
	{0x210, 0x00},
	{0x211, 0x0A},
	{0x212, 0x00},
	{0x217, 0x00},
	{0x218, 0x0A},
	{0x219, 0x00},
	// SIU Windowing Parameters
	{0x232, 0x00},
	{0x233, 0x00},
	{0x234, 0x00},
	{0x235, 0x00},
	{0x236, 0x00},
	{0x237, 0x0A},
	{0x238, 0xD0},
	{0x239, 0x02},
	{0x23B, 0x00},
	{0x23C, 0x00},
	{0x23D, 0x00},
	{0x23E, 0x00},
	{0x23F, 0x00},
	{0x240, 0x0A},
	{0x241, 0xD0},
	{0x242, 0x02},
	// SIU Control and Configuration
	{0x201, 0x00},
	{0x23A, 0x21},
	{0x243, 0x21},
	{0x203, 0x0C},
	{0x202, 0x03},
	{0x202, 0x00},
	{0x200, 0x03},
	{0x203, 0x03},
	{0x203, 0x00},
	//{0x200, 0x0F},//Move inside firmware codes
};

#ifdef SUPPORT_SENSOR_SCALING
tD2Register code D2TableSIU_sHD[] =
{
	// SDA Memory Strides
	{0x210, 0x80},
	{0x211, 0x07},
	{0x212, 0x00},
	{0x217, 0x80},
	{0x218, 0x07},
	{0x219, 0x00},
	// SIU Windowing Parameters
	{0x232, 0x00},
	{0x233, 0x00},
	{0x234, 0x00},
	{0x235, 0x00},
	{0x236, 0x80},
	{0x237, 0x07},
	{0x238, 0x20},
	{0x239, 0x02},
	{0x23B, 0x00},
	{0x23C, 0x00},
	{0x23D, 0x00},
	{0x23E, 0x00},
	{0x23F, 0x80},
	{0x240, 0x07},
	{0x241, 0x20},
	{0x242, 0x02},
	// SIU Control and Configuration
	{0x201, 0x00},
	{0x23A, 0x21},
	{0x243, 0x21},
	{0x203, 0x0C},
	{0x202, 0x03},
	{0x202, 0x00},
	{0x200, 0x03},
	{0x203, 0x03},
	{0x203, 0x00},
	//{0x200, 0x0F},//Move inside firmware codes
};
#endif // SUPPORT_SENSOR_SCALING

tD2Register code D2TableSOU_VGA[] =
{	// 640 x 480, 30fps at 24MHz (CLK_SDRAM = 120MHz)
	// 640 x 480, 30fps at 24.5MHz (CLK_SDRAM = 160MHz)
	// 640 x 480, 30fps at 26.67MHz (CLK_SDRAM = 160MHz)
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x02},
	{0x414, 0xE0},
	{0x415, 0x01},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0D},
	#endif // SOU_FIXED_COLOR
	#if CLK_SDRAM == 120
	{0x41B, 0xF2},
	{0x41C, 0x05},
	#else  // CLK_SDRAM == 120
		#if 1
	{0x41B, 0x12}, // at 24.5MHz
	{0x41C, 0x06}, // at 24.5MHz
		#else
	{0x41B, 0x9C}, // at 26.67MHz
	{0x41C, 0x06}, // at 26.67MHz
		#endif
	#endif // CLK_SDRAM == 120
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x05},
	{0x425, 0x0D},
	{0x426, 0x02},
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	{0x431, 0xE1},
	{0x432, 0x01},
	// SOU Control
	{0x411, 0x05},
};

tD2Register code D2TableSOU_DualVGA[] =
{	// 1280x480, 30fps at 40MHz, 48MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x00},
	{0x413, 0x05},
	{0x414, 0xE0},
	{0x415, 0x01},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0B},
	#endif // SOU_FIXED_COLOR
	#if SOU_DualVGACLK == 40
	{0x41B, 0xB6},
	{0x41C, 0x0A},
	#else  // SOU_DualVGACLK == 48
	{0x41B, 0xDC},
	{0x41C, 0x0A},
	#endif // SOU_DualVGACLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x0A},
	#if SOU_DualVGACLK == 40
	{0x425, 0xE6},
	{0x426, 0x01},
	#else  // SOU_DualVGACLK == 48
	{0x425, 0x3F},
	{0x426, 0x02},
	#endif // SOU_DualVGACLK
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	{0x431, 0xE1},
	{0x432, 0x01},
	// SOU Control
	{0x411, 0x05},
};

tD2Register code D2TableSOU_HD[] =
{	// 1280 x 720, 24fps at 48MHz, 53MHz, 60MHz, 64MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x00},
	{0x413, 0x05},
	{0x414, 0xD0},
	{0x415, 0x02},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0E},
	#endif // SOU_FIXED_COLOR
	#if SOU_HDCLK == 53
	{0x41B, 0x78},
	{0x41C, 0x0B},
	#elif SOU_HDCLK == 60
	{0x41B, 0xFC},
	{0x41C, 0x0C},
	#elif SOU_HDCLK == 64
	{0x41B, 0xDA},
	{0x41C, 0x0D},
	#else  // SOU_HDCLK == 48
	{0x41B, 0x78},
	{0x41C, 0x0A},
	#endif // SOU_HDCLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x0A},
	#if SOU_HDCLK == 48
	{0x425, 0xEA},
	{0x426, 0x02},
	#else  // SOU_HDCLK
	{0x425, 0xF0},
	{0x426, 0x02},
	#endif // SOU_HDCLK
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	{0x431, 0xD1},
	{0x432, 0x02},
	// SOU Control
	{0x411, 0x05},
};

	#ifdef SUPPORT_SENSOR_SCALING
tD2Register code D2TableSOU_DualHD[] =
{	// 1920 x 544, 24fps at 53MHz, 60MHz, 64MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x07},
	{0x414, 0x20},
	{0x415, 0x02},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0C},
	#endif // SOU_FIXED_COLOR
	#if SOU_DualHDCLK == 53
	{0x41B, 0x78},
	{0x41C, 0x0F},
	#elif SOU_DualHDCLK == 60
	{0x41B, 0x68},
	{0x41C, 0x11},
	#else  // SOU_DualHDCLK == 64
	{0x41B, 0x90},
	{0x41C, 0x12},
	#endif // SOU_DualHDCLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x0F},
	{0x425, 0x31},
	{0x426, 0x02},
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	{0x431, 0x21},
	{0x432, 0x02},
	// SOU Control
	{0x411, 0x05},
};
	#else  // SUPPORT_SENSOR_SCALING
tD2Register code D2TableSOU_DualHD[] =
{	// 2560 x 720, 14fps at 53MHz
	// 2560 x 720, 15fps at 60MHz, 64MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x00},
	{0x413, 0x0A},
	{0x414, 0xD0},
	{0x415, 0x02},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0C},
	#endif // SOU_FIXED_COLOR
	#if SOU_DualHDCLK == 53
	{0x41B, 0x50},
	{0x41C, 0x14},
	#elif SOU_DualHDCLK == 60
	{0x41B, 0xC1},
	{0x41C, 0x15},
	#else  // SOU_DualHDCLK == 64
	{0x41B, 0x84},
	{0x41C, 0x16},
	#endif // SOU_DualHDCLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x14},
	#if SOU_DualHDCLK == 53
	{0x425, 0xDC},
	{0x426, 0x02},
	#elif SOU_DualHDCLK == 60
	{0x425, 0xE4},
	{0x426, 0x02},
	#else  // SOU_DualHDCLK == 64
	{0x425, 0xE4},
	{0x426, 0x02},
	#endif // SOU_DualHDCLK
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	{0x431, 0xD1},
	{0x432, 0x02},
	// SOU Control
	{0x411, 0x05},
};
	#endif // SUPPORT_SENSOR_SCALING

#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
tD2Register code D2TableSOU_2xVGA[] =
{	// 640x960, 30fps at 40MHz, 48MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x02},
	{0x414, 0xC0},
	{0x415, 0x03},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x09},
	#endif // SOU_FIXED_COLOR
	#if SOU_DualVGACLK == 40
	{0x41B, 0x60},
	{0x41C, 0x05},
	#else  // SOU_DualVGACLK == 48
	{0x41B, 0xF2},
	{0x41C, 0x05},
	#endif // SOU_DualVGACLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x05},
	#if SOU_DualVGACLK == 40
	{0x425, 0xC8},
	{0x426, 0x03},
	#else  // SOU_DualVGACLK == 48
	{0x425, 0x1A},
	{0x426, 0x04},
	#endif // SOU_DualVGACLK
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	{0x431, 0xC1},
	{0x432, 0x03},
	// SOU Control
	{0x411, 0x05},
};

	#ifdef SUPPORT_SENSOR_SCALING
tD2Register code D2TableSOU_2xHD[] =
{	//  960 x 1088, 24fps at 53MHz, 60MHz, 64MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0xC0},
	{0x413, 0x03},
	{0x414, 0x40},
	{0x415, 0x04},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0A},
	#endif // SOU_FIXED_COLOR
	#if SOU_DualHDCLK == 53
	{0x41B, 0xDC},
	{0x41C, 0x07},
	#elif SOU_DualHDCLK == 60
	{0x41B, 0xD8},
	{0x41C, 0x08},
	#else  // SOU_DualHDCLK == 64
	{0x41B, 0x6E},
	{0x41C, 0x09},
	#endif // SOU_DualHDCLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0xC3},
	{0x424, 0x07},
	{0x425, 0x50},
	{0x426, 0x04},
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	{0x431, 0x41},
	{0x432, 0x04},
	// SOU Control
	{0x411, 0x05},
};
	#else  // SUPPORT_SENSOR_SCALING
tD2Register code D2TableSOU_2xHD[] =
{	// 1280 x 1440, 14fps at 53MHz
	// 1280 x 1440, 15fps at 60MHz, 64MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x00},
	{0x413, 0x05},
	{0x414, 0xA0},
	{0x415, 0x05},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0A},
	#endif // SOU_FIXED_COLOR
	#if SOU_DualHDCLK == 53
	{0x41B, 0x48},
	{0x41C, 0x0A},
	#elif SOU_DualHDCLK == 60
	{0x41B, 0x8E},
	{0x41C, 0x0A},
	#else  // SOU_DualHDCLK == 64
	{0x41B, 0x42},
	{0x41C, 0x0B},
	#endif // SOU_DualHDCLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x0A},
	#if SOU_DualHDCLK == 53
	{0x425, 0xA6},
	{0x426, 0x05},
	#else  // SOU_DualHDCLK == 60 or 64
	{0x425, 0xC8},
	{0x426, 0x05},
	#endif // SOU_DualHDCLK
	{0x426, 0x05},
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	{0x431, 0xA1},
	{0x432, 0x05},
	// SOU Control
	{0x411, 0x05},
};
	#endif // SUPPORT_SENSOR_SCALING
#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE

#ifdef SUPPORT_FUZZY_STITCHING
tD2Register code D2TableSOU_Stitching[] =
{
	#if IMAGE_STITCHING_SIZE == STITCHING_SIZE_1920x560
	// 1920 x 560, 24fps at 53MHz, the measured frame rate is TBD
	// 1920 x 560, 24fps at 64MHz
	#else  // IMAGE_STITCHING_SIZE == STITCHING_SIZE_1920x544
	// 1920 x 544, 24fps at 53MHz, the measured frame rate is 19fps
	// 1920 x 544, 24fps at 64MHz
	#endif // IMAGE_STITCHING_SIZE
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x07},
	#if IMAGE_STITCHING_SIZE == STITCHING_SIZE_1920x560
	{0x414, 0x30},
	{0x415, 0x02},
	#else  // IMAGE_STITCHING_SIZE == STITCHING_SIZE_1920x544
	{0x414, 0x20},
	{0x415, 0x02},
	#endif // IMAGE_STITCHING_SIZE
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x08},
	#endif // SOU_FIXED_COLOR
	#if 1
	{0x41B, 0x68}, // try to reach measured 24fps
	{0x41C, 0x0F}, // try to reach measured 24fps
	#else
		#if SOU_DualHDCLK == 53
	{0x41B, 0x78},
	{0x41C, 0x0F},
		#else  // SOU_DualHDCLK == 64
	{0x41B, 0x90},
	{0x41C, 0x12},
		#endif // SOU_DualHDCLK
	#endif
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x0F},
	#if IMAGE_STITCHING_SIZE == STITCHING_SIZE_1920x560
	{0x425, 0x33}, // try to reach measured 24fps
	{0x426, 0x02}, // try to reach measured 24fps
	#else  // IMAGE_STITCHING_SIZE == STITCHING_SIZE_1920x544
		#if 1
	{0x425, 0x29}, // try to reach measured 24fps
	{0x426, 0x02}, // try to reach measured 24fps
		#else
	{0x425, 0x31},
	{0x426, 0x02},
		#endif
	#endif // IMAGE_STITCHING_SIZE
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	#if IMAGE_STITCHING_SIZE == STITCHING_SIZE_1920x560
	{0x431, 0x31},
	{0x432, 0x02},
	#else  // IMAGE_STITCHING_SIZE == STITCHING_SIZE_1920x544
	{0x431, 0x21},
	{0x432, 0x02},
	#endif // IMAGE_STITCHING_SIZE
	// SOU Control
	{0x411, 0x05},
};
#endif // SUPPORT_FUZZY_STITCHING

#ifdef SENSOR_BYPASS_PATH
tD2Register code D2TableBypassSensor0[] =
{
	{0x022, 0x00},
};

tD2Register code D2TableBypassSensor1[] =
{
	{0x022, 0x01},
};
#else  // SENSOR_BYPASS_PATH
tD2Register code D2TableSingleSensor0_VGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x00},
	{0x37D, 0x05},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x05},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00},
	{0x31F, 0x05},
	{0x324, 0x00},
	{0x325, 0x05},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x320, 0xE0},
	{0x321, 0x01},
	{0x322, 0x00},
	{0x323, 0x05},
	{0x326, 0xE0},
	{0x327, 0x01},
	// IPU Control and Configuration
	{0x32C, 0x00},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};

tD2Register code D2TableSingleSensor1_VGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x00},
	{0x37D, 0x05},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x05},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00},
	{0x31F, 0x05},
	{0x324, 0x00},
	{0x325, 0x05},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x320, 0xE0},
	{0x321, 0x01},
	{0x322, 0x00},
	{0x323, 0x05},
	{0x326, 0xE0},
	{0x327, 0x01},
	// IPU Control and Configuration
	{0x32C, 0x20},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};

tD2Register code D2TableSingleSensor0_HD[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x00},
	{0x37D, 0x0A},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x0A},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00},
	{0x31F, 0x0A},
	{0x324, 0x00},
	{0x325, 0x0A},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x00},
	{0x31D, 0x0A},
	{0x320, 0xD0},
	{0x321, 0x02},
	{0x322, 0x00},
	{0x323, 0x0A},
	{0x326, 0xD0},
	{0x327, 0x02},
	// IPU Control and Configuration
	{0x32C, 0x00},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};

tD2Register code D2TableSingleSensor1_HD[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x00},
	{0x37D, 0x0A},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x0A},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00},
	{0x31F, 0x0A},
	{0x324, 0x00},
	{0x325, 0x0A},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x00},
	{0x31D, 0x0A},
	{0x320, 0xD0},
	{0x321, 0x02},
	{0x322, 0x00},
	{0x323, 0x0A},
	{0x326, 0xD0},
	{0x327, 0x02},
	// IPU Control and Configuration
	{0x32C, 0x20},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};
#endif // SENSOR_BYPASS_PATH

#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
tD2Register code D2TableHalfSideBySide_VGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x00},
	{0x37D, 0x05},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x05},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00},
	{0x31F, 0x05},
	{0x324, 0x00},
	{0x325, 0x05},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x320, 0xE0},
	{0x321, 0x01},
	{0x322, 0x00},
	{0x323, 0x05},
	{0x326, 0xE0},
	{0x327, 0x01},
	// IPU Control and Configuration
	{0x32C, 0x42},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};

tD2Register code D2TableFullSideBySide_VGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x00},
	{0x37D, 0x05},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x05},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00},
	{0x31F, 0x05},
	{0x324, 0x00},
	{0x325, 0x05},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x320, 0xE0},
	{0x321, 0x01},
	{0x322, 0x00},
	{0x323, 0x05},
	{0x326, 0xE0},
	{0x327, 0x01},
	// IPU Control and Configuration
	{0x32C, 0x02},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};

tD2Register code D2TableHalfSideBySide_HD[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x00},
	{0x37D, 0x0A},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x0A},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00},
	{0x31F, 0x0A},
	{0x324, 0x00},
	{0x325, 0x0A},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x00},
	{0x31D, 0x0A},
	{0x320, 0xD0},
	{0x321, 0x02},
	{0x322, 0x00},
	{0x323, 0x0A},
	{0x326, 0xD0},
	{0x327, 0x02},
	// IPU Control and Configuration
	{0x32C, 0x42},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};

tD2Register code D2TableFullSideBySide_HD[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	#ifdef SUPPORT_SENSOR_SCALING
	// IPU Memory Strides
	{0x37C, 0x80},
	{0x37D, 0x07},
	{0x37E, 0x00},
	{0x37F, 0x80},
	{0x380, 0x07},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x80},
	{0x31F, 0x07},
	{0x324, 0x80},
	{0x325, 0x07},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x80},
	{0x31D, 0x07},
	{0x320, 0x20},
	{0x321, 0x02},
	{0x322, 0x80},
	{0x323, 0x07},
	{0x326, 0x20},
	{0x327, 0x02},
	#else  // SUPPORT_SENSOR_SCALING
	// IPU Memory Strides
	{0x37C, 0x00},
	{0x37D, 0x0A},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x0A},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00},
	{0x31F, 0x0A},
	{0x324, 0x00},
	{0x325, 0x0A},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x00},
	{0x31D, 0x0A},
	{0x320, 0xD0},
	{0x321, 0x02},
	{0x322, 0x00},
	{0x323, 0x0A},
	{0x326, 0xD0},
	{0x327, 0x02},
	#endif // SUPPORT_SENSOR_SCALING
	// IPU Control and Configuration
	{0x32C, 0x02},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE

#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
tD2Register code D2TableVerticalSideBySide_VGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x00},
	{0x37D, 0x05},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x05},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00},
	{0x31F, 0x05},
	{0x324, 0x00},
	{0x325, 0x05},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x320, 0xE0},
	{0x321, 0x01},
	{0x322, 0x00},
	{0x323, 0x05},
	{0x326, 0xE0},
	{0x327, 0x01},
	// IPU Control and Configuration
	{0x32C, 0x04},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};

tD2Register code D2TableVerticalSideBySide_HD[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	#ifdef SUPPORT_SENSOR_SCALING
	// IPU Memory Strides
	{0x37C, 0x80},
	{0x37D, 0x07},
	{0x37E, 0x00},
	{0x37F, 0x80},
	{0x380, 0x07},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x80},
	{0x31F, 0x07},
	{0x324, 0x80},
	{0x325, 0x07},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x80},
	{0x31D, 0x07},
	{0x320, 0x20},
	{0x321, 0x02},
	{0x322, 0x80},
	{0x323, 0x07},
	{0x326, 0x20},
	{0x327, 0x02},
	#else  // SUPPORT_SENSOR_SCALING
	// IPU Memory Strides
	{0x37C, 0x00},
	{0x37D, 0x0A},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x0A},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00},
	{0x31F, 0x0A},
	{0x324, 0x00},
	{0x325, 0x0A},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x00},
	{0x31D, 0x0A},
	{0x320, 0xD0},
	{0x321, 0x02},
	{0x322, 0x00},
	{0x323, 0x0A},
	{0x326, 0xD0},
	{0x327, 0x02},
	#endif // SUPPORT_SENSOR_SCALING
	// IPU Control and Configuration
	{0x32C, 0x04},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};
#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE

#ifdef SUPPORT_FUZZY_STITCHING
tD2Register code D2TableAccurateStitch_HD[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	#ifdef SUPPORT_SENSOR_SCALING
	// IPU Memory Strides
	{0x37C, 0x80},
	{0x37D, 0x07},
	{0x37E, 0x00},
	{0x37F, 0x80},
	{0x380, 0x07},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x80},
	{0x31F, 0x07},
	{0x324, 0x80},
	{0x325, 0x07},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	#else  // SUPPORT_SENSOR_SCALING
	// IPU Memory Strides
	{0x37C, 0x00},
	{0x37D, 0x0A},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x0A},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00},
	{0x31F, 0x0A},
	{0x324, 0x00},
	{0x325, 0x0A},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	#endif // SUPPORT_SENSOR_SCALING
	// STCH Windowing Parameters
	{0x31C, (unsigned char)(ACST_STITCH_SRC_WIDTH*2)},
	{0x31D, (unsigned char)((ACST_STITCH_SRC_WIDTH*2)>>8)},
	{0x320, (unsigned char)(ACST_STITCH_SRC_HEIGHT)},
	{0x321, (unsigned char)((ACST_STITCH_SRC_HEIGHT)>>8)},
	{0x322, (unsigned char)(ACST_STITCH_SRC_WIDTH*2)},
	{0x323, (unsigned char)((ACST_STITCH_SRC_WIDTH*2)>>8)},
	{0x326, (unsigned char)(ACST_STITCH_SRC_HEIGHT)},
	{0x327, (unsigned char)((ACST_STITCH_SRC_HEIGHT)>>8)},
	{0x32E, (unsigned char)(2048/(ACST_OVERLAP_WIDTH*2))},
	{0x32F, (unsigned char)((2048/(ACST_OVERLAP_WIDTH*2))>>8)},
	{0x330, (unsigned char)(ACST_OVERLAP_WIDTH*2)},
	{0x331, (unsigned char)((ACST_OVERLAP_WIDTH*2)>>8)},
	{0x382, 0x00},
	{0x383, 0x10},
	{0x384, 0x00},
	{0x386, 0x00},
	{0x387, 0x90},
	{0x388, 0x00},
	// IPU Control and Configuration
	{0x32C, 0x06},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0xF0},
};
#endif // SUPPORT_FUZZY_STITCHING

unsigned char fnD2RunByPassSensor0(
	unsigned char ucResolution)
{
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK);		// Set Host Clock in MHz
	#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor0, sizeof(D2TableBypassSensor0) / sizeof(tD2Register));
	#else  // SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBuffers_VGA, sizeof(D2TableBuffers_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor0_VGA, sizeof(D2TableSingleSensor0_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0D;
	#endif // SENSOR_BYPASS_PATH
		return SUCCESS;
	case D2_RESOLUTION_HD_60Hz:
	case D2_RESOLUTION_HD_50Hz:
		D2WriteOutputClock(SOU_HDCLK);		// Set Host Clock in MHz
	#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor0, sizeof(D2TableBypassSensor0) / sizeof(tD2Register));
	#else  // SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBuffers_HD, sizeof(D2TableBuffers_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_HD, sizeof(D2TableSOU_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor0_HD, sizeof(D2TableSingleSensor0_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_HD, sizeof(D2TableSIU_HD) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0D;
	#endif // SENSOR_BYPASS_PATH
		return SUCCESS;
	default:
		return FAILURE;
	}
}

unsigned char fnD2RunByPassSensor1(
	unsigned char ucResolution)
{
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK);		// Set Host Clock in MHz
	#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor0, sizeof(D2TableBypassSensor0) / sizeof(tD2Register));
	#else  // SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBuffers_VGA, sizeof(D2TableBuffers_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor1_VGA, sizeof(D2TableSingleSensor1_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0E;
	#endif // SENSOR_BYPASS_PATH
		return SUCCESS;
	case D2_RESOLUTION_HD_60Hz:
	case D2_RESOLUTION_HD_50Hz:
		D2WriteOutputClock(SOU_HDCLK);		// Set Host Clock in MHz
	#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor1, sizeof(D2TableBypassSensor1) / sizeof(tD2Register));
	#else  // SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBuffers_HD, sizeof(D2TableBuffers_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_HD, sizeof(D2TableSOU_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor1_HD, sizeof(D2TableSingleSensor1_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_HD, sizeof(D2TableSIU_HD) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0E;
	#endif // SENSOR_BYPASS_PATH
		return SUCCESS;
	default:
		return FAILURE;
	}
}

#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
unsigned char fnD2RunFullSideBySide(
	unsigned char ucResolution)
{
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_DualVGACLK);	// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers_VGA, sizeof(D2TableBuffers_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_DualVGA, sizeof(D2TableSOU_DualVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableFullSideBySide_VGA, sizeof(D2TableFullSideBySide_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;
	case D2_RESOLUTION_HD_60Hz:
	case D2_RESOLUTION_HD_50Hz:
		D2WriteOutputClock(SOU_DualHDCLK);	// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers_HD, sizeof(D2TableBuffers_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_DualHD, sizeof(D2TableSOU_DualHD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableFullSideBySide_HD, sizeof(D2TableFullSideBySide_HD) / sizeof(tD2Register));
	#ifdef SUPPORT_SENSOR_SCALING
		fnD2WriteTable(D2TableSIU_sHD, sizeof(D2TableSIU_sHD) / sizeof(tD2Register));
	#else  // SUPPORT_SENSOR_SCALING
		fnD2WriteTable(D2TableSIU_HD,  sizeof(D2TableSIU_HD) / sizeof(tD2Register));
	#endif // SUPPORT_SENSOR_SCALING
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;
	default:
		return FAILURE;
	}
}

unsigned char fnD2RunHalfSideBySide(
	unsigned char ucResolution)
{
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK);		// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers_VGA, sizeof(D2TableBuffers_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableHalfSideBySide_VGA, sizeof(D2TableHalfSideBySide_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;
	case D2_RESOLUTION_HD_60Hz:
	case D2_RESOLUTION_HD_50Hz:
		D2WriteOutputClock(SOU_HDCLK);		// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers_HD, sizeof(D2TableBuffers_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_HD, sizeof(D2TableSOU_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableHalfSideBySide_HD, sizeof(D2TableHalfSideBySide_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_HD, sizeof(D2TableSIU_HD) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;
	default:
		return FAILURE;
	}
}
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE

#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
unsigned char fnD2RunVerticalSideBySide(
	unsigned char ucResolution)
{
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_DualVGACLK);	// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers_VGA, sizeof(D2TableBuffers_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_2xVGA, sizeof(D2TableSOU_2xVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableVerticalSideBySide_VGA, sizeof(D2TableVerticalSideBySide_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;
	case D2_RESOLUTION_HD_60Hz:
	case D2_RESOLUTION_HD_50Hz:
		D2WriteOutputClock(SOU_DualHDCLK);	// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers_HD, sizeof(D2TableBuffers_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_2xHD, sizeof(D2TableSOU_2xHD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableVerticalSideBySide_HD, sizeof(D2TableVerticalSideBySide_HD) / sizeof(tD2Register));
	#ifdef SUPPORT_SENSOR_SCALING
		fnD2WriteTable(D2TableSIU_sHD, sizeof(D2TableSIU_sHD) / sizeof(tD2Register));
	#else  // SUPPORT_SENSOR_SCALING
		fnD2WriteTable(D2TableSIU_HD,  sizeof(D2TableSIU_HD) / sizeof(tD2Register));
	#endif // SUPPORT_SENSOR_SCALING
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;
	default:
		return FAILURE;
	}
}
#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE

#ifdef SUPPORT_FUZZY_STITCHING
	#ifdef SUPPORT_VGA_STITCHING_PATTERN
unsigned char xdata SouPattern _at_ 0x041A; // SOU PATTERN
	#endif // SUPPORT_VGA_STITCHING_PATTERN

// Note 1:
// For better performance, procedure fnD2MoveCodeToSingleBankDRAM is perferred.
// Programmers must be sure that the full buffer to be copied are in a single SDRAM bank.
// If the buffer cannot be in a single SDRAM bank, call procedure fnD2MoveCodeToDRAM instead.
// Note 2:
// DRAM buffer address for D2CalibrationTable1 shall be the same as register 0x384:0x382.
// DRAM buffer address for D2CalibrationTable2 shall be the same as register 0x388:0x386.

void fnD2TableLoad()
{
	if (nD2TableVerified == 0)
	{
	#ifdef SUPPORT_SENSOR_SCALING
		fnD2MoveCodeToSingleBankDRAM(D2CalibrationTable1, 0x01000L, ((ACST_STITCH_SRC_WIDTH)/16+1)*(ACST_STITCH_SRC_HEIGHT/16+1)*8);
		fnD2MoveCodeToSingleBankDRAM(D2CalibrationTable2, 0x09000L, ((ACST_STITCH_SRC_WIDTH)/16+1)*(ACST_STITCH_SRC_HEIGHT/16+1)*8);	
	#else  // SUPPORT_SENSOR_SCALING
		fnD2MoveCodeToSingleBankDRAM(D2CalibrationTable1, 0x01000L, (1024/16+1)*(544/16+1)*8);
		fnD2MoveCodeToSingleBankDRAM(D2CalibrationTable2, 0x09000L, (1024/16+1)*(544/16+1)*8);
	#endif // SUPPORT_SENSOR_SCALING
	}
}

void fnD2TableVerify()
{
	if (nD2TableVerified == 0)
	{
	#ifdef SUPPORT_SENSOR_SCALING
		fnD2VerifyCodeToSingleBankDRAM(D2CalibrationTable1, 0x01000L, ((ACST_STITCH_SRC_WIDTH)/16+1)*(ACST_STITCH_SRC_HEIGHT/16+1)*8);
		fnD2VerifyCodeToSingleBankDRAM(D2CalibrationTable2, 0x09000L, ((ACST_STITCH_SRC_WIDTH)/16+1)*(ACST_STITCH_SRC_HEIGHT/16+1)*8);
	#else  // SUPPORT_SENSOR_SCALING
		fnD2VerifyCodeToSingleBankDRAM(D2CalibrationTable1, 0x01000L, (1024/16+1)*(544/16+1)*8);
		fnD2VerifyCodeToSingleBankDRAM(D2CalibrationTable2, 0x09000L, (1024/16+1)*(544/16+1)*8);
	#endif // SUPPORT_SENSOR_SCALING
		nD2TableVerified = 1;
	}
}

unsigned char fnD2RunAccurateStitching(
	unsigned char ucResolution)
{
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_VGA_50Hz:
	#ifdef SUPPORT_VGA_STITCHING_PATTERN
		D2WriteOutputClock(SOU_DualVGACLK); // Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_DualVGA, sizeof(D2TableSOU_DualVGA) / sizeof(tD2Register));
		SouPattern = 0x0E; // Enable SOU Pattern
		return SUCCESS;
	#else  // SUPPORT_VGA_STITCHING_PATTERN
		return FAILURE;
	#endif // SUPPORT_VGA_STITCHING_PATTERN
	case D2_RESOLUTION_HD_60Hz:
	case D2_RESOLUTION_HD_50Hz:
		fnD2WriteTable(D2TableBuffers_HD, sizeof(D2TableBuffers_HD) / sizeof(tD2Register));
		fnD2TableLoad();
		D2WriteOutputClock(SOU_DualHDCLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_Stitching, sizeof(D2TableSOU_Stitching) / sizeof(tD2Register));
		fnD2WriteTable(D2TableAccurateStitch_HD, sizeof(D2TableAccurateStitch_HD) / sizeof(tD2Register));
	#ifdef SUPPORT_SENSOR_SCALING
		fnD2WriteTable(D2TableSIU_sHD, sizeof(D2TableSIU_sHD) / sizeof(tD2Register));
	#else  // SUPPORT_SENSOR_SCALING
		fnD2WriteTable(D2TableSIU_HD,  sizeof(D2TableSIU_HD) / sizeof(tD2Register));
	#endif // SUPPORT_SENSOR_SCALING
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;
	default:
		return FAILURE;
	}
}
#endif // SUPPORT_FUZZY_STITCHING

// for AWB calibration, by XYZ, 2014.01.22 - begin
unsigned char opmode_calibration(unsigned char act)
{
#if (ZT3150_SENSOR == ZT3150_SENSOR_NT99141)
	extern void	nt99141_3a_calibration(unsigned char mode);

	nt99141_3a_calibration(act);
#endif
	return SUCCESS;
}
// for AWB calibration, by XYZ, 2014.01.22 - end

unsigned char fnSetOperatingMode(
	unsigned char ucMode,
	unsigned char ucResolution)
{
	switch (ucMode)
	{
	case D2_OPERATION_IDLE:
		D2DisableSensorClock();
		D2WriteOutputClock(SOU_VGACLK);		// Set Host Clock in MHz
		return SUCCESS;

	case D2_OPERATION_BYPASS_SENSOR0:
		if (fnDoInitSensor(ucResolution) == FAILURE)
			return FAILURE;
		return fnD2RunByPassSensor0(ucResolution);

	case D2_OPERATION_BYPASS_SENSOR1:
		if (fnDoInitSensor(ucResolution) == FAILURE)
			return FAILURE;
		return fnD2RunByPassSensor1(ucResolution);

#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
	case D2_OPERATION_HORIZONTAL_SIDE_BY_SIDE:
	#ifdef SUPPORT_SENSOR_SCALING
		if (fnDoInitSensor(ucResolution+D2_RESOLUTION_SCALED_HD_OFFSET) == FAILURE)
	#else  // SUPPORT_SENSOR_SCALING
		if (fnDoInitSensor(ucResolution) == FAILURE)
	#endif // SUPPORT_SENSOR_SCALING
			return FAILURE;
		return fnD2RunFullSideBySide(ucResolution);

	case D2_OPERATION_HALF_SIDE_BY_SIDE:
		if (fnDoInitSensor(ucResolution) == FAILURE)
			return FAILURE;
		return fnD2RunHalfSideBySide(ucResolution);
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE

#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
	case D2_OPERATION_VERTICAL_SIDE_BY_SIDE:
	#ifdef SUPPORT_SENSOR_SCALING
		if (fnDoInitSensor(ucResolution+D2_RESOLUTION_SCALED_HD_OFFSET) == FAILURE)
	#else  // SUPPORT_SENSOR_SCALING
		if (fnDoInitSensor(ucResolution) == FAILURE)
	#endif // SUPPORT_SENSOR_SCALING
			return FAILURE;
		return fnD2RunVerticalSideBySide(ucResolution);
#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE

#ifdef SUPPORT_FUZZY_STITCHING
	case D2_OPERATION_FUZZY_STITCHING:
		return FAILURE;

	case D2_OPERATION_ACCURATE_STITCHING:
	#ifdef SUPPORT_SENSOR_SCALING
		if (fnDoInitSensor(ucResolution+D2_RESOLUTION_SCALED_HD_OFFSET) == FAILURE)
	#else  // SUPPORT_SENSOR_SCALING
		if (fnDoInitSensor(ucResolution) == FAILURE)
	#endif // SUPPORT_SENSOR_SCALING
			return FAILURE;
		return fnD2RunAccurateStitching(ucResolution);
#endif // SUPPORT_FUZZY_STITCHING

	// for AWB calibration, by XYZ, 2014.01.22 - begin
	case D2_OPERATION_3A_CALIBRATION:
		return opmode_calibration(ucResolution);
	// for AWB calibration, by XYZ, 2014.01.22 - end
	}
	return FAILURE;
}

#endif // SUPPORT_D2_OPERATION
