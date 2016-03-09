//
// Zealtek D2 project
// ZT3120 with dual video decoder solutions
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// Aug 13, 2012.

#include "system.h"
#include "sdram.h"
#include "d2.h"

#ifdef SUPPORT_D2_OPERATION

	// In this solution:
	//
	// Buffer size for video decoders is 720x576 = 829,440.
	// 3 x 829,440 + 3 x 829,440 = 4,976,640, which is smaller than 8,388,608. (8MB)

	// SOU_FIXED_COLOR is used to send fixed color pattern to video destination.
	// This is very useful to verify and see if SOU timing is correct or not.
	// This option should be OFF on normal operation.
	//#define SOU_FIXED_COLOR

	// LDC_PATCH is used to fix the hardware LDC issue.
	//#define LDC_PATCH

	#define SOU_VGACLK    24
	#if (CLK_PLL / SOU_VGACLK >= 16)
		#error Maximum Clock Divider for SOU_VGACLK is 16.
	#endif // (CLK_PLL / SOU_VGACLK >= 16)

	#define SOU_DualVGACLK 40 //MHz
	//#define SOU_DualVGACLK 48 //MHz

	// DECODER_WIDTH_720 is used to send full 720-pixel data to video destination.
	// If this option is not enabled, cropped 640-pixel data is sent to video destination.
	//#define DECODER_WIDTH_720

	// DECODER_HEIGHT_480 is used to send only 480-line data to video destination.
	// If this option is enabled, cropped 480-line PAL data is sent to video destination.
	// If this option is not enabled, full 576-line PAL data is sent to video destination.
	// This option does not take effects on NTSC data.
	//#define DECODER_HEIGHT_480

	// DECODER_SINGLE_FIELD is used to capture single field to video destination.
	// If this option is not enabled, data from single field is sent to video destination.
	// DECODER_DE_INTERLACE is used to de-interlace odd and even fields to video destination.
	// If this option is not enabled, odd and even fields are sent to video destination in top and bottom.
	//#define DECODER_SINGLE_FIELD
	//#define DECODER_DE_INTERLACE

	// DECODER_SCALING is used to scale up the images come from video decoder.
	// The single field images are scaled up to fit the proper image size for video destination.
	//#define DECODER_SCALING

	#ifdef DECODER_SCALING
		#ifdef DECODER_DE_INTERLACE
			#undef DECODER_DE_INTERLACE
		#endif // DECODER_DE_INTERLACE
		#ifndef DECODER_SINGLE_FIELD
			//#define DECODER_SINGLE_FIELD
		#endif // DECODER_SINGLE_FIELD
		#ifdef DECODER_WIDTH_720
			#include "scale_720x288to720x576.c"
		#else  // DECODER_WIDTH_720
			#include "scale_640x288to640x576.c"
		#endif // DECODER_WIDTH_720
	#endif // DECODER_SCALING

tD2Register code D2TableBuffers[] =
{
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
#ifdef DECODER_WIDTH_720
	{0x210, 0xA0},
	{0x211, 0x05},
	{0x212, 0x00},
	{0x217, 0xA0},
	{0x218, 0x05},
	{0x219, 0x00},
#else  // DECODER_WIDTH_720
	{0x210, 0x00},
	{0x211, 0x05},
	{0x212, 0x00},
	{0x217, 0x00},
	{0x218, 0x05},
	{0x219, 0x00},
#endif // DECODER_WIDTH_720
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
#ifdef DECODER_WIDTH_720
	{0x37C, 0xA0},
	{0x37D, 0x05},
	{0x37E, 0x00},
	{0x37F, 0xA0},
	{0x380, 0x05},
	{0x381, 0x00},
#else  // DECODER_WIDTH_720
	{0x37C, 0x00},
	{0x37D, 0x05},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x05},
	{0x381, 0x00},
#endif // DECODER_WIDTH_720
#ifdef DECODER_SCALING
	{0x382, 0x00},
	{0x383, 0x10},
	{0x384, 0x00},
	{0x386, 0x00},
	{0x387, 0x40},
	{0x388, 0x00},
#endif // DECODER_SCALING
};

tD2Register code D2TableSIU_NTSC[] =
{
	// SIU Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x232, 0x00},
	{0x233, 0x00},
#else  // DECODER_WIDTH_720
	{0x232, 0x40},
	{0x233, 0x00},
#endif // DECODER_WIDTH_720
	{0x234, 0x00},
	{0x235, 0x00},
#ifdef DECODER_WIDTH_720
	{0x236, 0xA0},
	{0x237, 0x05},
#else  // DECODER_WIDTH_720
	{0x236, 0x00},
	{0x237, 0x05},
#endif // DECODER_WIDTH_720
#ifdef DECODER_DE_INTERLACE
	{0x238, 0xF0},
	{0x239, 0x00},
#else  // DECODER_DE_INTERLACE
	{0x238, 0xE0},
	{0x239, 0x01},
#endif // DECODER_DE_INTERLACE
#ifdef DECODER_WIDTH_720
	{0x23B, 0x00},
	{0x23C, 0x00},
#else  // DECODER_WIDTH_720
	{0x23B, 0x40},
	{0x23C, 0x00},
#endif // DECODER_WIDTH_720
	{0x23D, 0x00},
	{0x23E, 0x00},
#ifdef DECODER_WIDTH_720
	{0x23F, 0xA0},
	{0x240, 0x05},
#else  // DECODER_WIDTH_720
	{0x23F, 0x00},
	{0x240, 0x05},
#endif // DECODER_WIDTH_720
#ifdef DECODER_DE_INTERLACE
	{0x241, 0xF0},
	{0x242, 0x00},
#else  // DECODER_DE_INTERLACE
	{0x241, 0xE0},
	{0x242, 0x01},
#endif // DECODER_DE_INTERLACE
	// SIU Control and Configuration
#ifdef DECODER_DE_INTERLACE
	{0x201, 0x03},
	{0x244, 0x03},
#else  // DECODER_DE_INTERLACE
    #ifdef DECODER_SINGLE_FIELD
	{0x201, 0x00},
	{0x244, 0x00},
    #else  // DECODER_SINGLE_FIELD
	{0x201, 0x00},
	{0x244, 0x03},
    #endif // DECODER_SINGLE_FIELD
#endif // DECODER_DE_INTERLACE
	{0x23A, 0x40/*0x00*/},
	{0x243, 0x00},
	{0x203, 0x0C},
	{0x202, 0x03},
	{0x202, 0x00},
	{0x200, 0x03},
	{0x203, 0x03},
	{0x203, 0x00},
	//{0x200, 0x33},//Move inside firmware codes
};

tD2Register code D2TableSIU_PAL[] =
{
	// SIU Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x232, 0x00},
	{0x233, 0x00},
#else  // DECODER_WIDTH_720
	{0x232, 0x40},
	{0x233, 0x00},
#endif // DECODER_WIDTH_720
#ifdef DECODER_HEIGHT_480
	{0x234, 0x00},
	{0x235, 0x00},
#else  // DECODER_HEIGHT_480
	{0x234, 0x00},
	{0x235, 0x00},
#endif // DECODER_HEIGHT_480
#ifdef DECODER_WIDTH_720
	{0x236, 0xA0},
	{0x237, 0x05},
#else  // DECODER_WIDTH_720
	{0x236, 0x00},
	{0x237, 0x05},
#endif // DECODER_WIDTH_720
#ifdef DECODER_DE_INTERLACE
    #ifdef DECODER_HEIGHT_480
	{0x238, 0xF0},
	{0x239, 0x00},
    #else  // DECODER_HEIGHT_480
	{0x238, 0x20},
	{0x239, 0x01},
    #endif // DECODER_HEIGHT_480
#else  // DECODER_DE_INTERLACE
    #ifdef DECODER_HEIGHT_480
	{0x238, 0xE0},
	{0x239, 0x01},
    #else  // DECODER_HEIGHT_480
	{0x238, 0x40},
	{0x239, 0x02},
    #endif // DECODER_HEIGHT_480
#endif // DECODER_DE_INTERLACE
#ifdef DECODER_WIDTH_720
	{0x23B, 0x00},
	{0x23C, 0x00},
#else  // DECODER_WIDTH_720
	{0x23B, 0x40},
	{0x23C, 0x00},
#endif // DECODER_WIDTH_720
#ifdef DECODER_HEIGHT_480
	{0x23D, 0x00},
	{0x23E, 0x00},
#else  // DECODER_HEIGHT_480
	{0x23D, 0x00},
	{0x23E, 0x00},
#endif // DECODER_HEIGHT_480
#ifdef DECODER_WIDTH_720
	{0x23F, 0xA0},
	{0x240, 0x05},
#else  // DECODER_WIDTH_720
	{0x23F, 0x00},
	{0x240, 0x05},
#endif // DECODER_WIDTH_720
#ifdef DECODER_DE_INTERLACE
    #ifdef DECODER_HEIGHT_480
	{0x241, 0xF0},
	{0x242, 0x00},
    #else  // DECODER_HEIGHT_480
	{0x241, 0x20},
	{0x242, 0x01},
    #endif // DECODER_HEIGHT_480
#else  // DECODER_DE_INTERLACE
    #ifdef DECODER_HEIGHT_480
	{0x241, 0xE0},
	{0x242, 0x01},
    #else  // DECODER_HEIGHT_480
	{0x241, 0x40},
	{0x242, 0x02},
    #endif // DECODER_HEIGHT_480
#endif // DECODER_DE_INTERLACE
	// SIU Control and Configuration
#ifdef DECODER_DE_INTERLACE
	{0x201, 0x03},
	{0x244, 0x03},
#else  // DECODER_DE_INTERLACE
    #ifdef DECODER_SINGLE_FIELD
	{0x201, 0x00},
	{0x244, 0x00},
    #else  // DECODER_SINGLE_FIELD
	{0x201, 0x00},
	{0x244, 0x03},
    #endif // DECODER_SINGLE_FIELD
#endif // DECODER_DE_INTERLACE
	{0x23A, 0x40/*0x00*/},
	{0x243, 0x00},
	{0x203, 0x0C},
	{0x202, 0x03},
	{0x202, 0x00},
	{0x200, 0x03},
	{0x203, 0x03},
	{0x203, 0x00},
	//{0x200, 0x33},//Move inside firmware codes
};

tD2Register code D2TableSOU_NTSC[] =
{	// 720x480, 30fps at 24MHz
	// 640x480, 30fps at 24MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x412, 0xD0},
	{0x413, 0x02},
#else  // DECODER_WIDTH_720
	{0x412, 0x80},
	{0x413, 0x02},
#endif // DECODER_WIDTH_720
#if defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x414, 0xF0},
	{0x415, 0x01},
#else  // defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x414, 0xE0},
	{0x415, 0x01},
#endif // defined(DECODER_SCALING) && defined(LDC_PATCH)
#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0E},
#endif // SOU_FIXED_COLOR
	{0x41B, 0xF2},
	{0x41C, 0x05},
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
#ifdef DECODER_WIDTH_720
	{0x423, 0xE3},
	{0x424, 0x05},
#else  // DECODER_WIDTH_720
	{0x423, 0x43},
	{0x424, 0x05},
#endif // DECODER_WIDTH_720
	{0x425, 0x0D},
	{0x426, 0x02},
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
#if defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x431, 0xF1},
	{0x432, 0x01},
#else  // defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x431, 0xE1},
	{0x432, 0x01},
#endif // defined(DECODER_SCALING) && defined(LDC_PATCH)
	// SOU Control
	{0x411, 0x05},
};

#ifndef DECODER_HEIGHT_480
tD2Register code D2TableSOU_PAL[] =
{	// 720x576, 25fps at 24MHz
	// 640x576, 25fps at 24MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x412, 0xD0},
	{0x413, 0x02},
#else  // DECODER_WIDTH_720
	{0x412, 0x80},
	{0x413, 0x02},
#endif // DECODER_WIDTH_720
	{0x414, 0x40},
	{0x415, 0x02},
#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0D},
#endif // SOU_FIXED_COLOR
	{0x41B, 0x00},
	{0x41C, 0x06},
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
#ifdef DECODER_WIDTH_720
	{0x423, 0xE3},
	{0x424, 0x05},
#else  // DECODER_WIDTH_720
	{0x423, 0x43},
	{0x424, 0x05},
#endif // DECODER_WIDTH_720
	{0x425, 0x71},
	{0x426, 0x02},
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	{0x431, 0x41},
	{0x432, 0x02},
	// SOU Control
	{0x411, 0x05},
};
#endif // DECODER_HEIGHT_480

#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
tD2Register code D2TableSOU_DualNTSC[] =
{	// 1440x480, 26.5fps at 40MHz, LDC_PATCH on
	// 1280x480, 29.2fps at 40MHz, LDC_PATCH on
	// 1440x480, 27fps at 40MHz, LDC_PATCH off
	// 1280x480, 30fps at 40MHz, LDC_PATCH off
	// 1440x480, 30fps at 48MHz
	// 1280x480, 30fps at 48MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x412, 0xA0},
	{0x413, 0x05},
#else  // DECODER_WIDTH_720
	{0x412, 0x00},
	{0x413, 0x05},
#endif // DECODER_WIDTH_720
#if defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x414, 0xF0},
	{0x415, 0x01},
#else  // defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x414, 0xE0},
	{0x415, 0x01},
#endif // defined(DECODER_SCALING) && defined(LDC_PATCH)
#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0C},
#endif // SOU_FIXED_COLOR
#if SOU_DualVGACLK == 40
    #ifdef DECODER_WIDTH_720
	{0x41B, 0xCC},
	{0x41C, 0x0B},
    #else  // DECODER_WIDTH_720
	{0x41B, 0xB6},
	{0x41C, 0x0A},
    #endif // DECODER_WIDTH_720
#else  // SOU_DualVGACLK == 48
    #ifdef DECODER_WIDTH_720
	{0x41B, 0x34},
	{0x41C, 0x0C},
    #else  // DECODER_WIDTH_720
	{0x41B, 0xE4},
	{0x41C, 0x0B},
    #endif // DECODER_WIDTH_720
#endif // SOU_DualVGACLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
#ifdef DECODER_WIDTH_720
	{0x423, 0x83},
	{0x424, 0x0B},
#else  // DECODER_WIDTH_720
	{0x423, 0x43},
	{0x424, 0x0A},
#endif // DECODER_WIDTH_720
#if SOU_DualVGACLK == 40
    #if defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x425, 0xF4},
	{0x426, 0x01},
    #else  // defined(DECODER_SCALING) && defined(LDC_PATCH)
	#ifdef DECODER_WIDTH_720
	{0x425, 0xEA},
	{0x426, 0x01},
	#else  // DECODER_WIDTH_720
	{0x425, 0xE6},
	{0x426, 0x01},
	#endif // DECODER_WIDTH_720
    #endif // defined(DECODER_SCALING) && defined(LDC_PATCH)
#else  // SOU_DualVGACLK == 48
		#ifdef DECODER_WIDTH_720
	{0x425, 0x00},
	{0x426, 0x02},
		#else  // DECODER_WIDTH_720
	{0x425, 0x0D},
	{0x426, 0x02},
		#endif // DECODER_WIDTH_720
#endif // SOU_DualVGACLK
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
#if defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x431, 0xF1},
	{0x432, 0x01},
#else  // defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x431, 0xE1},
	{0x432, 0x01},
#endif // defined(DECODER_SCALING) && defined(LDC_PATCH)
	// SOU Control
	{0x411, 0x05},
};

#ifndef DECODER_HEIGHT_480
tD2Register code D2TableSOU_DualPAL[] =
{	// 1440x576, 23fps at 40MHz
	// 1280x576, 25fps at 40MHz
	// 1440x576, 25fps at 48MHz
	// 1280x576, 25fps at 48MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x412, 0xA0},
	{0x413, 0x05},
#else  // DECODER_WIDTH_720
	{0x412, 0x00},
	{0x413, 0x05},
#endif // DECODER_WIDTH_720
	{0x414, 0x40},
	{0x415, 0x02},
#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0B},
#endif // SOU_FIXED_COLOR
#if SOU_DualVGACLK == 40
    #ifdef DECODER_WIDTH_720
	{0x41B, 0xA4},
	{0x41C, 0x0B},
    #else  // DECODER_WIDTH_720
	{0x41B, 0x78},
	{0x41C, 0x0A},
    #endif // DECODER_WIDTH_720
#else  // SOU_DualVGACLK == 48
	{0x41B, 0x00},
	{0x41C, 0x0C},
#endif // SOU_DualVGACLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
#ifdef DECODER_WIDTH_720
	{0x423, 0x83},
	{0x424, 0x0B},
#else  // DECODER_WIDTH_720
	{0x423, 0x43},
	{0x424, 0x0A},
#endif // DECODER_WIDTH_720
#if SOU_DualVGACLK == 40
    #ifdef DECODER_WIDTH_720
	{0x425, 0x47},
	{0x426, 0x02},
    #else  // DECODER_WIDTH_720
	{0x425, 0x54},
	{0x426, 0x02},
    #endif // DECODER_WIDTH_720
#else  // SOU_DualVGACLK == 48
	{0x425, 0x71},
	{0x426, 0x02},
#endif // SOU_DualVGACLK
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	{0x431, 0x41},
	{0x432, 0x02},
	// SOU Control
	{0x411, 0x05},
};
#endif // DECODER_HEIGHT_480
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE

tD2Register code D2TableSingleSensor0_NTSC[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31E, 0xA0},
	{0x31F, 0x05},
	{0x324, 0xA0},
	{0x325, 0x05},
#else  // DECODER_WIDTH_720
	{0x31E, 0x00},
	{0x31F, 0x05},
	{0x324, 0x00},
	{0x325, 0x05},
#endif // DECODER_WIDTH_720
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31C, 0xA0},
	{0x31D, 0x05},
	{0x322, 0xA0},
	{0x323, 0x05},
#else  // DECODER_WIDTH_720
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x322, 0x00},
	{0x323, 0x05},
#endif // DECODER_WIDTH_720
#if defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x320, 0xF0},
	{0x321, 0x01},
	{0x326, 0xF0},
	{0x327, 0x01},
#else  // defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x320, 0xE0},
	{0x321, 0x01},
	{0x326, 0xE0},
	{0x327, 0x01},
#endif // defined(DECODER_SCALING) && defined(LDC_PATCH)
	// IPU Control and Configuration
	{0x32C, 0x00},
	{0x302, 0x35},
	{0x302, 0x30},
#ifdef DECODER_SCALING
	{0x32F, 0x20},
	{0x32F, 0x00},
	{0x300, 0x52},
#else  // DECODER_SCALING
	{0x300, 0x03},
#endif // DECODER_SCALING
};

#ifndef DECODER_HEIGHT_480
tD2Register code D2TableSingleSensor0_PAL[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31E, 0xA0},
	{0x31F, 0x05},
	{0x324, 0xA0},
	{0x325, 0x05},
#else  // DECODER_WIDTH_720
	{0x31E, 0x00},
	{0x31F, 0x05},
	{0x324, 0x00},
	{0x325, 0x05},
#endif // DECODER_WIDTH_720
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31C, 0xA0},
	{0x31D, 0x05},
	{0x322, 0xA0},
	{0x323, 0x05},
#else  // DECODER_WIDTH_720
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x322, 0x00},
	{0x323, 0x05},
#endif // DECODER_WIDTH_720
	{0x320, 0x40},
	{0x321, 0x02},
	{0x326, 0x40},
	{0x327, 0x02},
	// IPU Control and Configuration
	{0x32C, 0x00},
	{0x302, 0x35},
	{0x302, 0x30},
#ifdef DECODER_SCALING
	{0x32F, 0x20},
	{0x32F, 0x00},
	{0x300, 0x52},
#else  // DECODER_SCALING
	{0x300, 0x03},
#endif // DECODER_SCALING
};
#endif // DECODER_HEIGHT_480

tD2Register code D2TableSingleSensor1_NTSC[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31E, 0xA0},
	{0x31F, 0x05},
	{0x324, 0xA0},
	{0x325, 0x05},
#else  // DECODER_WIDTH_720
	{0x31E, 0x00},
	{0x31F, 0x05},
	{0x324, 0x00},
	{0x325, 0x05},
#endif // DECODER_WIDTH_720
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31C, 0xA0},
	{0x31D, 0x05},
	{0x322, 0xA0},
	{0x323, 0x05},
#else  // DECODER_WIDTH_720
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x322, 0x00},
	{0x323, 0x05},
#endif // DECODER_WIDTH_720
#if defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x320, 0xF0},
	{0x321, 0x01},
	{0x326, 0xF0},
	{0x327, 0x01},
#else  // defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x320, 0xE0},
	{0x321, 0x01},
	{0x326, 0xE0},
	{0x327, 0x01},
#endif // defined(DECODER_SCALING) && defined(LDC_PATCH)
	// IPU Control and Configuration
	{0x32C, 0x20},
	{0x302, 0x35},
	{0x302, 0x30},
#ifdef DECODER_SCALING
	{0x32F, 0x20},
	{0x32F, 0x00},
	{0x300, 0xA1},
#else  // DECODER_SCALING
	{0x300, 0x03},
#endif // DECODER_SCALING
};

#ifndef DECODER_HEIGHT_480
tD2Register code D2TableSingleSensor1_PAL[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31E, 0xA0},
	{0x31F, 0x05},
	{0x324, 0xA0},
	{0x325, 0x05},
#else  // DECODER_WIDTH_720
	{0x31E, 0x00},
	{0x31F, 0x05},
	{0x324, 0x00},
	{0x325, 0x05},
#endif // DECODER_WIDTH_720
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31C, 0xA0},
	{0x31D, 0x05},
	{0x322, 0xA0},
	{0x323, 0x05},
#else  // DECODER_WIDTH_720
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x322, 0x00},
	{0x323, 0x05},
#endif // DECODER_WIDTH_720
	{0x320, 0x40},
	{0x321, 0x02},
	{0x326, 0x40},
	{0x327, 0x02},
	// IPU Control and Configuration
	{0x32C, 0x20},
	{0x302, 0x35},
	{0x302, 0x30},
#ifdef DECODER_SCALING
	{0x32F, 0x20},
	{0x32F, 0x00},
	{0x300, 0xA1},
#else  // DECODER_SCALING
	{0x300, 0x03},
#endif // DECODER_SCALING
};
#endif // DECODER_HEIGHT_480

#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
tD2Register code D2TableFullSideBySide_NTSC[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31E, 0xA0},
	{0x31F, 0x05},
	{0x324, 0xA0},
	{0x325, 0x05},
#else  // DECODER_WIDTH_720
	{0x31E, 0x00},
	{0x31F, 0x05},
	{0x324, 0x00},
	{0x325, 0x05},
#endif // DECODER_WIDTH_720
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31C, 0xA0},
	{0x31D, 0x05},
	{0x322, 0xA0},
	{0x323, 0x05},
#else  // DECODER_WIDTH_720
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x322, 0x00},
	{0x323, 0x05},
#endif // DECODER_WIDTH_720
#if defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x320, 0xF0},
	{0x321, 0x01},
	{0x326, 0xF0},
	{0x327, 0x01},
#else  // defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x320, 0xE0},
	{0x321, 0x01},
	{0x326, 0xE0},
	{0x327, 0x01},
#endif // defined(DECODER_SCALING) && defined(LDC_PATCH)
	// IPU Control and Configuration
	{0x32C, 0x02},
	{0x302, 0x35},
	{0x302, 0x30},
#ifdef DECODER_SCALING
	{0x32F, 0x20},
	{0x32F, 0x00},
	{0x300, 0xF0},
#else  // DECODER_SCALING
	{0x300, 0x03},
#endif // DECODER_SCALING
};

#ifndef DECODER_HEIGHT_480
tD2Register code D2TableFullSideBySide_PAL[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31E, 0xA0},
	{0x31F, 0x05},
	{0x324, 0xA0},
	{0x325, 0x05},
#else  // DECODER_WIDTH_720
	{0x31E, 0x00},
	{0x31F, 0x05},
	{0x324, 0x00},
	{0x325, 0x05},
#endif // DECODER_WIDTH_720
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31C, 0xA0},
	{0x31D, 0x05},
	{0x322, 0xA0},
	{0x323, 0x05},
#else  // DECODER_WIDTH_720
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x322, 0x00},
	{0x323, 0x05},
#endif // DECODER_WIDTH_720
	{0x320, 0x40},
	{0x321, 0x02},
	{0x326, 0x40},
	{0x327, 0x02},
	// IPU Control and Configuration
	{0x32C, 0x02},
	{0x302, 0x35},
	{0x302, 0x30},
#ifdef DECODER_SCALING
	{0x32F, 0x20},
	{0x32F, 0x00},
	{0x300, 0xF0},
#else  // DECODER_SCALING
	{0x300, 0x03},
#endif // DECODER_SCALING
};
#endif // DECODER_HEIGHT_480

tD2Register code D2TableHalfSideBySide_NTSC[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31E, 0xA0},
	{0x31F, 0x05},
	{0x324, 0xA0},
	{0x325, 0x05},
#else  // DECODER_WIDTH_720
	{0x31E, 0x00},
	{0x31F, 0x05},
	{0x324, 0x00},
	{0x325, 0x05},
#endif // DECODER_WIDTH_720
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31C, 0xA0},
	{0x31D, 0x05},
	{0x322, 0xA0},
	{0x323, 0x05},
#else  // DECODER_WIDTH_720
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x322, 0x00},
	{0x323, 0x05},
#endif // DECODER_WIDTH_720
#if defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x320, 0xF0},
	{0x321, 0x01},
	{0x326, 0xF0},
	{0x327, 0x01},
#else  // defined(DECODER_SCALING) && defined(LDC_PATCH)
	{0x320, 0xE0},
	{0x321, 0x01},
	{0x326, 0xE0},
	{0x327, 0x01},
#endif // defined(DECODER_SCALING) && defined(LDC_PATCH)
	// IPU Control and Configuration
	{0x32C, 0x42},
	{0x302, 0x35},
	{0x302, 0x30},
#ifdef DECODER_SCALING
	{0x32F, 0x20},
	{0x32F, 0x00},
	{0x300, 0xF0},
#else  // DECODER_SCALING
	{0x300, 0x03},
#endif // DECODER_SCALING
};

	#ifndef DECODER_HEIGHT_480
tD2Register code D2TableHalfSideBySide_PAL[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31E, 0xA0},
	{0x31F, 0x05},
	{0x324, 0xA0},
	{0x325, 0x05},
#else  // DECODER_WIDTH_720
	{0x31E, 0x00},
	{0x31F, 0x05},
	{0x324, 0x00},
	{0x325, 0x05},
#endif // DECODER_WIDTH_720
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
#ifdef DECODER_WIDTH_720
	{0x31C, 0xA0},
	{0x31D, 0x05},
	{0x322, 0xA0},
	{0x323, 0x05},
#else  // DECODER_WIDTH_720
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x322, 0x00},
	{0x323, 0x05},
#endif // DECODER_WIDTH_720
	{0x320, 0x40},
	{0x321, 0x02},
	{0x326, 0x40},
	{0x327, 0x02},
	// IPU Control and Configuration
	{0x32C, 0x42},
	{0x302, 0x35},
	{0x302, 0x30},
#ifdef DECODER_SCALING
	{0x32F, 0x20},
	{0x32F, 0x00},
	{0x300, 0xF0},
#else  // DECODER_SCALING
	{0x300, 0x03},
#endif // DECODER_SCALING
};
#endif // DECODER_HEIGHT_480
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE

#ifdef DECODER_SCALING
// Note 1:
// For better performance, procedure fnD2MoveCodeToSingleBankDRAM is perferred.
// Programmers must be sure that the full buffer to be copied are in a single SDRAM bank.
// If the buffer cannot be in a single SDRAM bank, call procedure fnD2MoveCodeToDRAM instead.
// Note 2:
// DRAM buffer address for D2Scaler640x360to640x720 shall be the same as register
// 0x384:0x382 and register 0x388:0x386.

void fnD2TableLoad()
{
	if (nD2TableVerified == 0)
	{
    #ifdef DECODER_WIDTH_720
		fnD2MoveCodeToSingleBankDRAM(D2Scale720x288to720x576, 0x01000L, (720/16+1)*(576/16+1)*8);
		fnD2MoveCodeToSingleBankDRAM(D2Scale720x288to720x576, 0x04000L, (720/16+1)*(576/16+1)*8);
    #else  // DECODER_WIDTH_720
		fnD2MoveCodeToSingleBankDRAM(D2Scale640x288to640x576, 0x01000L, (640/16+1)*(576/16+1)*8);
		fnD2MoveCodeToSingleBankDRAM(D2Scale640x288to640x576, 0x04000L, (640/16+1)*(576/16+1)*8);
    #endif // DECODER_WIDTH_720
	}
}
#endif // DECODER_SCALING

void fnD2TableVerify()
{
#ifdef DECODER_SCALING
	if (nD2TableVerified == 0)
	{
    #ifdef DECODER_WIDTH_720
		fnD2VerifyCodeToSingleBankDRAM(D2Scale720x288to720x576, 0x01000L, (720/16+1)*(576/16+1)*8);
		fnD2VerifyCodeToSingleBankDRAM(D2Scale720x288to720x576, 0x04000L, (720/16+1)*(576/16+1)*8);
    #else  // DECODER_WIDTH_720
		fnD2VerifyCodeToSingleBankDRAM(D2Scale640x288to640x576, 0x01000L, (640/16+1)*(576/16+1)*8);
		fnD2VerifyCodeToSingleBankDRAM(D2Scale640x288to640x576, 0x04000L, (640/16+1)*(576/16+1)*8);
    #endif // DECODER_WIDTH_720
		nD2TableVerified = 1;
	}
#endif // DECODER_SCALING
}


unsigned char fnD2RunByPassSensor0(unsigned char ucResolution)
{
	fnD2WriteTable(D2TableBuffers,  sizeof(D2TableBuffers) / sizeof(tD2Register));
	D2WriteOutputClock(SOU_VGACLK);			// Set Host Clock in MHz
    #ifdef DECODER_SCALING
	fnD2TableLoad();
    #endif // DECODER_SCALING
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
		fnD2WriteTable(D2TableSOU_NTSC, sizeof(D2TableSOU_NTSC) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor0_NTSC, sizeof(D2TableSingleSensor0_NTSC) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_NTSC, sizeof(D2TableSIU_NTSC) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x31;
		return SUCCESS;
	case D2_RESOLUTION_VGA_50Hz:
    #ifdef DECODER_HEIGHT_480
		fnD2WriteTable(D2TableSOU_NTSC, sizeof(D2TableSOU_NTSC) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor0_NTSC, sizeof(D2TableSingleSensor0_NTSC) / sizeof(tD2Register));
    #else  // DECODER_HEIGHT_480
		fnD2WriteTable(D2TableSOU_PAL, sizeof(D2TableSOU_PAL) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor0_PAL, sizeof(D2TableSingleSensor0_PAL) / sizeof(tD2Register));
    #endif // DECODER_HEIGHT_480
		fnD2WriteTable(D2TableSIU_PAL, sizeof(D2TableSIU_PAL) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x31;
		return SUCCESS;
	default:
		return FAILURE;
	}
}

unsigned char fnD2RunByPassSensor1(unsigned char ucResolution)
{
	fnD2WriteTable(D2TableBuffers,  sizeof(D2TableBuffers) / sizeof(tD2Register));
	D2WriteOutputClock(SOU_VGACLK);			// Set Host Clock in MHz
    #ifdef DECODER_SCALING
	fnD2TableLoad();
    #endif // DECODER_SCALING
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
		fnD2WriteTable(D2TableSOU_NTSC, sizeof(D2TableSOU_NTSC) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor1_NTSC, sizeof(D2TableSingleSensor1_NTSC) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_NTSC, sizeof(D2TableSIU_NTSC) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x32;
		return SUCCESS;
	case D2_RESOLUTION_VGA_50Hz:
    #ifdef DECODER_HEIGHT_480
		fnD2WriteTable(D2TableSOU_NTSC, sizeof(D2TableSOU_NTSC) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor1_NTSC, sizeof(D2TableSingleSensor1_NTSC) / sizeof(tD2Register));
    #else  // DECODER_HEIGHT_480
		fnD2WriteTable(D2TableSOU_PAL, sizeof(D2TableSOU_PAL) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor1_PAL, sizeof(D2TableSingleSensor1_PAL) / sizeof(tD2Register));
    #endif // DECODER_HEIGHT_480
		fnD2WriteTable(D2TableSIU_PAL, sizeof(D2TableSIU_PAL) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x32;
		return SUCCESS;
	default:
		return FAILURE;
	}
}

#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
unsigned char fnD2RunFullSideBySide(unsigned char ucResolution)
{
	fnD2WriteTable(D2TableBuffers,  sizeof(D2TableBuffers) / sizeof(tD2Register));
	D2WriteOutputClock(SOU_DualVGACLK);		// Set Host Clock in MHz
    #ifdef DECODER_SCALING
	fnD2TableLoad();
    #endif // DECODER_SCALING
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_HD_60Hz:
		fnD2WriteTable(D2TableSOU_DualNTSC, sizeof(D2TableSOU_DualNTSC) / sizeof(tD2Register));
		fnD2WriteTable(D2TableFullSideBySide_NTSC, sizeof(D2TableFullSideBySide_NTSC) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_NTSC, sizeof(D2TableSIU_NTSC) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x33;
		return SUCCESS;
	case D2_RESOLUTION_VGA_50Hz:
	case D2_RESOLUTION_HD_50Hz:
    #ifdef DECODER_HEIGHT_480
		fnD2WriteTable(D2TableSOU_DualNTSC, sizeof(D2TableSOU_DualNTSC) / sizeof(tD2Register));
		fnD2WriteTable(D2TableFullSideBySide_NTSC, sizeof(D2TableFullSideBySide_NTSC) / sizeof(tD2Register));
    #else  // DECODER_HEIGHT_480
		fnD2WriteTable(D2TableSOU_DualPAL, sizeof(D2TableSOU_DualPAL) / sizeof(tD2Register));
		fnD2WriteTable(D2TableFullSideBySide_PAL, sizeof(D2TableFullSideBySide_PAL) / sizeof(tD2Register));
    #endif // DECODER_HEIGHT_480
		fnD2WriteTable(D2TableSIU_PAL, sizeof(D2TableSIU_PAL) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x33;
		return SUCCESS;
	default:
		return FAILURE;
	}
}

unsigned char fnD2RunHalfSideBySide(unsigned char ucResolution)
{
	fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
	D2WriteOutputClock(SOU_VGACLK);			// Set Host Clock in MHz
    #ifdef DECODER_SCALING
	fnD2TableLoad();
    #endif // DECODER_SCALING
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
		fnD2WriteTable(D2TableSOU_NTSC, sizeof(D2TableSOU_NTSC) / sizeof(tD2Register));
		fnD2WriteTable(D2TableHalfSideBySide_NTSC, sizeof(D2TableHalfSideBySide_NTSC) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_NTSC, sizeof(D2TableSIU_NTSC) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x33;
		return SUCCESS;
	case D2_RESOLUTION_VGA_50Hz:
    #ifdef DECODER_HEIGHT_480
		fnD2WriteTable(D2TableSOU_NTSC, sizeof(D2TableSOU_NTSC) / sizeof(tD2Register));
		fnD2WriteTable(D2TableHalfSideBySide_NTSC, sizeof(D2TableHalfSideBySide_NTSC) / sizeof(tD2Register));
    #else  // DECODER_HEIGHT_480
		fnD2WriteTable(D2TableSOU_PAL, sizeof(D2TableSOU_PAL) / sizeof(tD2Register));
		fnD2WriteTable(D2TableHalfSideBySide_PAL, sizeof(D2TableHalfSideBySide_PAL) / sizeof(tD2Register));
    #endif // DECODER_HEIGHT_480
		fnD2WriteTable(D2TableSIU_PAL, sizeof(D2TableSIU_PAL) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x33;
		return SUCCESS;
	default:
		return FAILURE;
	}
}
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE

// for AWB calibration, by XYZ, 2014.01.22 - begin
unsigned char opmode_calibration(unsigned char act)
{
	act = act;
	return SUCCESS;
}
// for AWB calibration, by XYZ, 2014.01.22 - end

unsigned char fnSetOperatingMode(unsigned char ucMode, unsigned char ucResolution)
{
#ifdef DBG
	fnUartTxChar('M');
	fnUartTx8bit(ucMode);
	fnUartTxChar(' ');
	fnUartTxChar('R');
	fnUartTx8bit(ucResolution);
	fnUartTxChar('\n');
	fnUartTxChar('\r');
#endif // DBG
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

	// for AWB calibration, by XYZ, 2014.01.22 - begin
	case D2_OPERATION_3A_CALIBRATION:
		return opmode_calibration(ucResolution);
	// for AWB calibration, by XYZ, 2014.01.22 - end
	}
	return FAILURE;
}

#endif // SUPPORT_D2_OPERATION
