//
// Zealtek D2 project
// ZT3150 Twin VGA sensor solutions
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 22, 2012.

#include "system.h"
#include "sdram.h"
#include "d2.h"
#include "gendef.h"

#ifdef SUPPORT_D2_OPERATION

	// In this solution:
	//
	// Buffer size for VGA video sensor is 720x576 = 829,440.
	// 3 x 829,440 + 3 x 829,440 = 4,976,640, which is smaller than 8,388,608. (8MB)

	// SOU_FIXED_COLOR is used to send fixed color pattern to video destination.
	// This is very useful to verify and see if SOU timing is correct or not.
	// This option should be OFF on normal operation.
	//#define SOU_FIXED_COLOR

	#define SOU_VGACLK 24
	#if (CLK_PLL / SOU_VGACLK >= 16)
		#error Maximum Clock Divider for SOU_VGACLK is 16.
	#endif // (CLK_PLL / SOU_VGACLK >= 16)

	#define SOU_DualVGACLK 40 //MHz
	//#define SOU_DualVGACLK 48 //MHz

	// SENSOR_BYPASS_PATH allows the images from sensor source, bypasses all D2 sub-modules,
	// and then sends to the destination. Disabling this option (preferred) allows the images
	// storing into SDRAM buffers, and then sends to the destination.
	//#define SENSOR_BYPASS_PATH

	#ifdef SUPPORT_FUZZY_STITCHING
		#include "zt3150_Accurate_Para.h"
		#if (ZT3150_SENSOR == ZT3150_SENSOR_OV7725)
			#if (ZT3150_CALITYPE == ZT3150_CALIBRATION_SCALED)
			#define ACST_OVERLAP_WIDTH	32
			#else  // (ZT3150_CALITYPE == ZT3150_CALIBRATION_SCALED)
			#define ACST_OVERLAP_WIDTH	64
			#endif // (ZT3150_CALITYPE == ZT3150_CALIBRATION_SCALED)
		#else  // (ZT3150_SENSOR == ZT3150_SENSOR_OV7725)
			// Modified Setting Value for Salix Project.
			#define ACST_OVERLAP_WIDTH	48
		#endif // (ZT3150_SENSOR == ZT3150_SENSOR_OV7725)
		#if (IMAGE_STITCHING_SIZE == STITCHING_SIZE_1280x496)
			#define ACST_IMAGE_WIDTH   672
			#define ACST_IMAGE_HEIGHT  496
		#elif  (IMAGE_STITCHING_SIZE == STITCHING_SIZE_1280x480)
			#define ACST_IMAGE_WIDTH   672
			#define ACST_IMAGE_HEIGHT  480
		#elif  (IMAGE_STITCHING_SIZE == STITCHING_SIZE_640x256)
			#define ACST_IMAGE_WIDTH   336
			#define ACST_IMAGE_HEIGHT  256
		#elif  (IMAGE_STITCHING_SIZE == STITCHING_SIZE_640x240)
			#define ACST_IMAGE_WIDTH   336
			#define ACST_IMAGE_HEIGHT  240
		#elif  (IMAGE_STITCHING_SIZE == STITCHING_SIZE_640x496)
			#define ACST_IMAGE_WIDTH   336
			#define ACST_IMAGE_HEIGHT  496
		#elif  (IMAGE_STITCHING_SIZE == STITCHING_SIZE_640x480)
			#define ACST_IMAGE_WIDTH   336
			#define ACST_IMAGE_HEIGHT  480
		#else  // IMAGE_STITCHING_SIZE
			#define ACST_IMAGE_WIDTH   640
			#define ACST_IMAGE_HEIGHT  480
			#error Improper Image Stitching Size is defined!
		#endif // IMAGE_STITCHING_SIZE
		#define IMAGE_STITCHING_TABLE_LENGTH ((ACST_IMAGE_WIDTH/16+1)*(ACST_IMAGE_HEIGHT/16+1)*8)
	#endif // SUPPORT_FUZZY_STITCHING

tD2Register code D2TableBuffers[] =
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
	{0x21F, 0xA8},
	{0x220, 0x0D},
	{0x222, 0x00},
	{0x223, 0x50},
	{0x224, 0x1A},
	{0x226, 0x00},
	{0x227, 0xF8},
	{0x228, 0x26},
	{0x22A, 0x00},
	{0x22B, 0xA0},
	{0x22C, 0x33},
	{0x22E, 0x00},
	{0x22F, 0x48},
	{0x230, 0x40},
	// SDA Memory Strides
	{0x210, 0x00},
	{0x211, 0x05},
	{0x212, 0x00},
	{0x217, 0x00},
	{0x218, 0x05},
	{0x219, 0x00},
	// IPU Memory Buffers
	{0x304, 0x00},
	{0x305, 0x00},
	{0x306, 0x01},
	{0x308, 0x00},
	{0x309, 0xA8},
	{0x30A, 0x0D},
	{0x30C, 0x00},
	{0x30D, 0x50},
	{0x30E, 0x1A},
	{0x310, 0x00},
	{0x311, 0xF8},
	{0x312, 0x26},
	{0x314, 0x00},
	{0x315, 0xA0},
	{0x316, 0x33},
	{0x318, 0x00},
	{0x319, 0x48},
	{0x31A, 0x40},
	// IPU Memory Strides
	{0x37C, 0x00},
	{0x37D, 0x05},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x05},
	{0x381, 0x00},
};

tD2Register code D2TableSIU_VGA[] =
{
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

tD2Register code D2TableSOU_VGA[] =
{	// 640x480, 30fps at 24MHz
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
	{0x41B, 0xF2},
	{0x41C, 0x05},
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

#ifdef SUPPORT_FUZZY_STITCHING
	#if (IMAGE_STITCHING_SIZE == STITCHING_SIZE_640x240) || (IMAGE_STITCHING_SIZE == STITCHING_SIZE_640x256)
tD2Register code D2TableSOU_ScaledVGA[] =
{	// 640x480, 30fps at 24MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x02},
	{0x414, 0xF0},
	{0x415, 0x00},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0A},
	#endif // SOU_FIXED_COLOR
	{0x41B, 0xF2},
	{0x41C, 0x05},
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
	{0x42F, 0x7A},
	{0x430, 0x00},
	{0x431, 0x69},
	{0x432, 0x01},
	// SOU Control
	{0x411, 0x05},
};
	#endif //  (IMAGE_STITCHING_SIZE == STITCHING_SIZE_640x240) || (IMAGE_STITCHING_SIZE == STITCHING_SIZE_640x256)
	#if (IMAGE_STITCHING_SIZE == STITCHING_SIZE_1280x496)
tD2Register code D2TableSOU_StitchVGA[] =
{	// 640x480, 30fps at 24MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x02},
	{0x414, 0xF0},
	{0x415, 0x01},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0D},
	#endif // SOU_FIXED_COLOR
	{0x41B, 0xF2},
	{0x41C, 0x05},
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
	{0x431, 0xF1},
	{0x432, 0x01},
	// SOU Control
	{0x411, 0x05},
};

tD2Register code D2TableSOU_StitchDualVGA[] =
{	// 1280x480, 30fps at 40MHz
	// 1280x480, 30fps at 48MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x00},
	{0x413, 0x05},
	#if 0 //#if SOU_DualVGACLK == 40
	{0x414, 0xE0},
	{0x415, 0x01},
	{0x418, 0x06}, // Y_OFFSET
	{0x419, 0x00}, // Y_OFFSET
	#else  // SOU_DualVGACLK == 48
	{0x414, 0xF0},
	{0x415, 0x01},
	#endif // SOU_DualVGACLK
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0B},
	#endif // SOU_FIXED_COLOR
	#if 0 //#if SOU_DualVGACLK == 40
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
	#if 0 //#if SOU_DualVGACLK == 40
	{0x425, 0xE6},
	{0x426, 0x01},
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	{0x431, 0xE1},
	{0x432, 0x01},
	#else  // SOU_DualVGACLK == 48
	{0x425, 0x3F},
	{0x426, 0x02},
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	{0x431, 0xF1},
	{0x432, 0x01},
	#endif // SOU_DualVGACLK
	// SOU Control
	{0x411, 0x05},
};
	#endif // (IMAGE_STITCHING_SIZE == STITCHING_SIZE_1280x496)
#endif // SUPPORT_FUZZY_STITCHING

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
#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE

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
#endif // SENSOR_BYPASS_PATH

#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
tD2Register code D2TableHalfSideBySide_VGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
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
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE

#ifdef SUPPORT_FUZZY_STITCHING
tD2Register code D2TableAccurateStitch_VGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
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
	{0x31C, (unsigned char)(ACST_IMAGE_WIDTH*2)},
	{0x31D, (unsigned char)((ACST_IMAGE_WIDTH*2)>>8)},
	{0x320, (unsigned char)(ACST_IMAGE_HEIGHT)},
	{0x321, (unsigned char)((ACST_IMAGE_HEIGHT)>>8)},
	{0x322, (unsigned char)(ACST_IMAGE_WIDTH*2)},
	{0x323, (unsigned char)((ACST_IMAGE_WIDTH*2)>>8)},
	{0x326, (unsigned char)(ACST_IMAGE_HEIGHT)},
	{0x327, (unsigned char)((ACST_IMAGE_HEIGHT)>>8)},
	{0x32E, (unsigned char)(2048/(ACST_OVERLAP_WIDTH*2))},
	{0x32F, (unsigned char)((2048/(ACST_OVERLAP_WIDTH*2))>>8)},
	{0x330, (unsigned char)(ACST_OVERLAP_WIDTH*2)},
	{0x331, (unsigned char)((ACST_OVERLAP_WIDTH*2)>>8)},
	{0x382, 0x00},
	{0x383, 0x10},
	{0x384, 0x00},
	{0x386, 0x00},
	{0x387, 0x40},
	{0x388, 0x00},
	// IPU Control and Configuration
	{0x32C, 0x06},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x32F, 0x20},
	{0x32F, 0x00},
	{0x300, 0xF0},
};
#endif // SUPPORT_FUZZY_STITCHING

#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
tD2Register code D2TableVerticalSideBySide_VGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
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
#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE

#ifdef SUPPORT_D2_PAN
unsigned char xdata ucSOU_X_OFFSET_LO  _at_ 0x0416;
unsigned char xdata ucSOU_X_OFFSET_HI  _at_ 0x0417;
unsigned char xdata ucSOU_Y_OFFSET_LO  _at_ 0x0418;
unsigned char xdata ucSOU_Y_OFFSET_HI  _at_ 0x0419;
unsigned short usSOUOffsetX, usSOUOffsetY;
#endif // SUPPORT_D2_PAN

unsigned char fnD2RunByPassSensor0(
	unsigned char ucResolution)
{
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor0, sizeof(D2TableBypassSensor0) / sizeof(tD2Register));
#else  // SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor0_VGA, sizeof(D2TableSingleSensor0_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
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
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor1, sizeof(D2TableBypassSensor1) / sizeof(tD2Register));
#else  // SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor1_VGA, sizeof(D2TableSingleSensor1_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
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
	fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
	switch (ucResolution)
	{
	#ifdef SUPPORT_D2_PAN
	case D2_RESOLUTION_PAN+D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_PAN+D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		ucSOU_X_OFFSET_LO = GETWORDLSB(usSOUOffsetX);
		ucSOU_X_OFFSET_HI = GETWORDMSB(usSOUOffsetX);
		fnD2WriteTable(D2TableFullSideBySide_VGA, sizeof(D2TableFullSideBySide_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;
	#endif // SUPPORT_D2_PAN

	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_DualVGACLK); // Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_DualVGA, sizeof(D2TableSOU_DualVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableFullSideBySide_VGA, sizeof(D2TableFullSideBySide_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;

	default:
		return FAILURE;
	}
}

unsigned char fnD2RunHalfSideBySide(
	unsigned char ucResolution)
{
	fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableHalfSideBySide_VGA, sizeof(D2TableHalfSideBySide_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
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
	fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
	switch (ucResolution)
	{
	#ifdef SUPPORT_D2_PAN
	case D2_RESOLUTION_PAN+D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_PAN+D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		ucSOU_Y_OFFSET_LO = GETWORDLSB(usSOUOffsetY);
		ucSOU_Y_OFFSET_HI = GETWORDMSB(usSOUOffsetY);
		fnD2WriteTable(D2TableVerticalSideBySide_VGA, sizeof(D2TableVerticalSideBySide_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;
	#endif // SUPPORT_D2_PAN

	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_DualVGACLK); // Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_2xVGA, sizeof(D2TableSOU_2xVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableVerticalSideBySide_VGA, sizeof(D2TableVerticalSideBySide_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;

	default:
		return FAILURE;
	}
}
#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE

#ifdef SUPPORT_FUZZY_STITCHING

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
		fnD2MoveCodeToSingleBankDRAM(D2CalibrationTable1, 0x01000L, IMAGE_STITCHING_TABLE_LENGTH);
		fnD2MoveCodeToSingleBankDRAM(D2CalibrationTable2, 0x04000L, IMAGE_STITCHING_TABLE_LENGTH);
	}
}

void fnD2TableVerify()
{
	if (nD2TableVerified == 0)
	{
		fnD2VerifyCodeToSingleBankDRAM(D2CalibrationTable1, 0x01000L, IMAGE_STITCHING_TABLE_LENGTH);
		fnD2VerifyCodeToSingleBankDRAM(D2CalibrationTable2, 0x04000L, IMAGE_STITCHING_TABLE_LENGTH);
		nD2TableVerified = 1;
	}
}

unsigned char fnD2RunAccurateStitching(
	unsigned char ucResolution)
{
	fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
	fnD2TableLoad();

	switch (ucResolution)
	{
	#ifdef SUPPORT_D2_PAN
	case D2_RESOLUTION_PAN+D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_PAN+D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
	#if (IMAGE_STITCHING_SIZE == STITCHING_SIZE_1280x496)
		fnD2WriteTable(D2TableSOU_StitchVGA, sizeof(D2TableSOU_StitchVGA) / sizeof(tD2Register));
	#elif  (IMAGE_STITCHING_SIZE == STITCHING_SIZE_1280x480)
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
	#else  // IMAGE_STITCHING_SIZE
		#error Improper Image Stitching Size is defined!
	#endif // IMAGE_STITCHING_SIZE
		ucSOU_X_OFFSET_LO = GETWORDLSB(usSOUOffsetX);
		ucSOU_X_OFFSET_HI = GETWORDMSB(usSOUOffsetX);
		fnD2WriteTable(D2TableAccurateStitch_VGA, sizeof(D2TableAccurateStitch_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;
	#endif // SUPPORT_D2_PAN

	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_VGA_50Hz:
	#if (IMAGE_STITCHING_SIZE == STITCHING_SIZE_1280x496)
		D2WriteOutputClock(SOU_DualVGACLK); // Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_StitchDualVGA, sizeof(D2TableSOU_StitchDualVGA) / sizeof(tD2Register));
	#elif  (IMAGE_STITCHING_SIZE == STITCHING_SIZE_1280x480)
		D2WriteOutputClock(SOU_DualVGACLK); // Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_DualVGA, sizeof(D2TableSOU_DualVGA) / sizeof(tD2Register));
	#elif  (IMAGE_STITCHING_SIZE == STITCHING_SIZE_640x256) || (IMAGE_STITCHING_SIZE == STITCHING_SIZE_640x240)
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_ScaledVGA, sizeof(D2TableSOU_ScaledVGA) / sizeof(tD2Register));
	#elif  (IMAGE_STITCHING_SIZE == STITCHING_SIZE_640x496) || (IMAGE_STITCHING_SIZE == STITCHING_SIZE_640x480)
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
	#else  // IMAGE_STITCHING_SIZE
		#error Improper Image Stitching Size is defined!
	#endif // IMAGE_STITCHING_SIZE
		fnD2WriteTable(D2TableAccurateStitch_VGA, sizeof(D2TableAccurateStitch_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
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
	act = act;
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
		if (fnDoInitSensor(ucResolution) == FAILURE)
			return FAILURE;
		return fnD2RunFullSideBySide(ucResolution);

	case D2_OPERATION_HALF_SIDE_BY_SIDE:
		if (fnDoInitSensor(ucResolution) == FAILURE)
			return FAILURE;
		return fnD2RunHalfSideBySide(ucResolution);
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE

#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
	case D2_OPERATION_VERTICAL_SIDE_BY_SIDE:
		if (fnDoInitSensor(ucResolution) == FAILURE)
			return FAILURE;
		return fnD2RunVerticalSideBySide(ucResolution);
#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE

#ifdef SUPPORT_FUZZY_STITCHING
	case D2_OPERATION_FUZZY_STITCHING:
		return FAILURE;

	case D2_OPERATION_ACCURATE_STITCHING:
		if (fnDoInitSensor(ucResolution) == FAILURE)
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

#ifdef SUPPORT_D2_PAN
unsigned char fnPanSetValue(
	unsigned char ucPanDirection,
	unsigned char ucPanXMotion,
	unsigned char ucPanYMotion)
{
	if ((nD2Resolution & 0x80) == 0)
		return FAILURE;

	while ((ucSIU_CONTROL & 0x10) == 0) //SN0_VSYNC
		;

	ucResetControl = 0x74;      // 40:SIU Reset, 20:IPU Reset, 10:SOU Reset, 04: DRAM Reset
	ucResetControl = 0x00;

	switch (nD2Mode)
	{
	#ifdef SUPPORT_FUZZY_STITCHING
	//case D2_OPERATION_FUZZY_STITCHING:
	case D2_OPERATION_ACCURATE_STITCHING:
		switch (ucPanDirection & 0x0F)
		{
		case D2_PAN_LEFT:
			if (usSOUOffsetX >= ucPanXMotion)
				usSOUOffsetX -= ucPanXMotion;
			break;
		case D2_PAN_RIGHT:
			if (usSOUOffsetX <= (640 - ucPanXMotion))
				usSOUOffsetX += ucPanXMotion;
			break;
		}
		return fnD2RunAccurateStitching(nD2Resolution);
	#endif // SUPPORT_FUZZY_STITCHING
	#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
	case D2_OPERATION_HORIZONTAL_SIDE_BY_SIDE:
		switch (ucPanDirection & 0x0F)
		{
		case D2_PAN_LEFT:
			if (usSOUOffsetX >= ucPanXMotion)
				usSOUOffsetX -= ucPanXMotion;
			break;
		case D2_PAN_RIGHT:
			if (usSOUOffsetX <= (640 - ucPanXMotion))
				usSOUOffsetX += ucPanXMotion;
			break;
		}
		return fnD2RunFullSideBySide(nD2Resolution);
	#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE
	#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
	case D2_OPERATION_VERTICAL_SIDE_BY_SIDE:
		switch (ucPanDirection & 0xF0)
		{
		case D2_PAN_UP:
			if (usSOUOffsetY >= ucPanYMotion)
				usSOUOffsetY -= ucPanYMotion;
			break;
		case D2_PAN_DOWN:
			if (usSOUOffsetY <= (480 - ucPanYMotion))
				usSOUOffsetY += ucPanYMotion;
			break;
		}
		return fnD2RunVerticalSideBySide(nD2Resolution);
	#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE
	default:
		return FAILURE;
	}
}

void fnPanInitial()
{
	usSOUOffsetX  = 640/2; // D2_OPERATION_ACCURATE_STITCHING
	usSOUOffsetY  = 480/2; // D2_OPERATION_VERTICAL_SIDE_BY_SIDE
}
#endif // SUPPORT_D2_PAN

#endif // SUPPORT_D2_OPERATION
