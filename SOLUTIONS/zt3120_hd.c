//
// Zealtek D2 project
// ZT3120 with HD image sensor solutions
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// July 27, 2012.

#include "system.h"
#include "sdram.h"
#include "d2.h"

#ifdef SUPPORT_D2_OPERATION

	// In this solution:
	//
	// Buffer size for video decoders   is  640x480 =   614,400.
	// Buffer size for HD video sensors is 1280x720 = 1,843,200.
	// 3 x 614,400 + 3 x 1,843,200 = 8359936, which is smaller than 8,388,608. (8MB)

	// SOU_FIXED_COLOR is used to send fixed color pattern to video destination.
	// This is very useful to verify and see if SOU timing is correct or not.
	// This option should be OFF on normal operation.
	//#define SOU_FIXED_COLOR

	// LDC_PATCH_VGA is used to fix the hardware LDC issue on VGA modes.
	// LDC_PATCH_HD is used to fix the hardware LDC issue on HD modes.
	#define LDC_PATCH_VGA
	//#define LDC_PATCH_HD

	#define SOU_VGACLK 24
	#if (CLK_PLL / SOU_VGACLK >= 16)
		#error Maximum Clock Divider for SOU_VGACLK is 16.
	#endif // (CLK_PLL / SOU_VGACLK >= 16)

	#if CLK_SDRAM == 120
		#define SOU_DualVGACLK 40 //MHz
		//#define SOU_DualVGACLK 48 //MHz // not good
		#define SOU_HDCLK     48 //MHz
		//#define SOU_HDCLK     60 //MHz // not good
		#define SOU_HalfHDCLK 40 //MHz
		//#define SOU_HalfHDCLK 60 //MHz
		#define SOU_DualHDCLK 60 //MHz
		//#define SOU_DualHDCLK 80 //MHz
	#else  // CLK_SDRAM == 160
		#define SOU_DualVGACLK 40 //MHz
		#define SOU_HDCLK     53 //MHz
		//#define SOU_HDCLK     64 //MHz
		#define SOU_HalfHDCLK 40 //MHz
		//#define SOU_HalfHDCLK 53 //MHz
		//#define SOU_HalfHDCLK 64 //MHz // not good
		//#define SOU_DualHDCLK 53 //MHz
		#define SOU_DualHDCLK 64 //MHz
		//#define SOU_DualHDCLK 80 //MHz // not good
	#endif // CLK_SDRAM

	// SUPPORT_HD_CROPPED is used to crop the images come from video sensor (HD).
	// Therefore, the side-by-side images look better on the generated images.
	// If these options are not enabled, images are sent as original resolution.
	#define SUPPORT_HD_CROPPED

	// DECODER_SINGLE_FIELD is used to capture single field to video destination.
	// If this option is not enabled, data from single field is sent to video destination.
	// DECODER_DE_INTERLACE is used to de-interlace odd and even fields to video destination.
	// If this option is not enabled, odd and even fields are sent to video destination in top and bottom.
	//#define DECODER_SINGLE_FIELD
	//#define DECODER_DE_INTERLACE

	// The following options are conditional options controlled by the options list above.
	// DECODER_FILL_FIXEDCOLOR
	// DECODER_SCALING_VGA
	// DECODER_SCALING_HD
	// DECODER_SCALING
	//
	// DECODER_FILL_FIXEDCOLOR is used to pre-fill fixed color pattern into SDRAM frame buffers.
	// The fixed color pattern can be sent out if the second field data is not captured from SIU.
	// DECODER_SCALING_VGA is used to scale up the video data from video decoder in resolution VGA.
	// DECODER_SCALING_HD is used to scale up the video data from video decoder in resolution HD.

	#ifdef DECODER_SINGLE_FIELD
		#ifdef DECODER_DE_INTERLACE
			#undef DECODER_DE_INTERLACE
		#endif // DECODER_DE_INTERLACE
		#ifndef SUPPORT_HD_CROPPED
			#define DECODER_FILL_FIXEDCOLOR // Could be marked out for testing.
		#endif  // SUPPORT_HD_CROPPED
		#define DECODER_SCALING_VGA // Could be marked out for testing.
	#else  // DECODER_SINGLE_FIELD
		#define DECODER_SCALING_VGA // Could be marked out for testing.
	#endif // DECODER_SINGLE_FIELD
	#ifdef SUPPORT_HD_CROPPED
		#define DECODER_SCALING_HD
	#endif // SUPPORT_HD_CROPPED
	#if (defined(DECODER_SCALING_VGA) || defined(DECODER_SCALING_HD))
		#define DECODER_SCALING
	#endif // (defined(DECODER_SCALING_VGA) || defined(DECODER_SCALING_HD))

	// SENSOR_HD_FRAMERATE_30 is used for solutions that sensor generates 30fps on HD resolution.
	// Otherwise, sensor generates 24fps on HD resolution.
	//#define SENSOR_HD_FRAMERATE_30

	#ifdef DECODER_SCALING
		#ifdef DECODER_SCALING_HD
			#ifdef DECODER_SCALING_VGA
				#include "scale_640x240to960x720.c"
			#else  // DECODER_SCALING_VGA
				#include "scale_640x480to960x720.c"
			#endif // DECODER_SCALING_VGA
		#endif // DECODER_SCALING_HD
			#include "scale_640x360to640x720.c"
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
	{0x21F, 0x10},
	{0x220, 0x0F},
	{0x222, 0x00},
	{0x223, 0x20},
	{0x224, 0x1D},
	{0x226, 0x00},
	{0x227, 0x30},
	{0x228, 0x2B},
	{0x22A, 0x00},
	{0x22B, 0x50},
	{0x22C, 0x47},
	{0x22E, 0x00},
	{0x22F, 0x70},
	{0x230, 0x63},
	// IPU Memory Buffers
	{0x304, 0x00},
	{0x305, 0x00},
	{0x306, 0x01},
	{0x308, 0x00},
	{0x309, 0x10},
	{0x30A, 0x0F},
	{0x30C, 0x00},
	{0x30D, 0x20},
	{0x30E, 0x1D},
	{0x310, 0x00},
	{0x311, 0x30},
	{0x312, 0x2B},
	{0x314, 0x00},
	{0x315, 0x50},
	{0x316, 0x47},
	{0x318, 0x00},
	{0x319, 0x70},
	{0x31A, 0x63},
	#ifdef DECODER_SCALING
	{0x382, 0x00},
	{0x383, 0x10},
	{0x384, 0x00},
	#endif // DECODER_SCALING
};

tD2Register code D2TableSIU_NTSC_VGA[] =
{
	{0x210, 0x00},
	{0x211, 0x05},
	{0x212, 0x00},
	{0x217, 0x00},
	{0x218, 0x05},
	{0x219, 0x00},
	// SIU Windowing Parameters
	{0x232, 0x40},
	{0x233, 0x00},
	{0x234, 0x00},
	{0x235, 0x00},
	{0x236, 0x00},
	{0x237, 0x05},
	#ifdef DECODER_DE_INTERLACE
	{0x238, 0xF0},
	{0x239, 0x00},
	#else  // DECODER_DE_INTERLACE
	{0x238, 0xE0},
	{0x239, 0x01},
	#endif // DECODER_DE_INTERLACE
	{0x23B, 0x00},
	{0x23C, 0x00},
	{0x23D, 0x00},
	{0x23E, 0x00},
	{0x23F, 0x00},
	{0x240, 0x05},
	{0x241, 0xE0},
	{0x242, 0x01},
	// SIU Control and Configuration
	#ifdef DECODER_DE_INTERLACE
	{0x201, 0x01},
	{0x244, 0x01},
	#else  // DECODER_DE_INTERLACE
		#ifdef DECODER_SINGLE_FIELD
	{0x201, 0x00},
	{0x244, 0x00},
		#else  // DECODER_SINGLE_FIELD
	{0x201, 0x00},
	{0x244, 0x01},
		#endif // DECODER_SINGLE_FIELD
	#endif // DECODER_DE_INTERLACE
	{0x23A, 0x00},
	{0x243, 0x21},
	{0x203, 0x0C},
	{0x202, 0x03},
	{0x202, 0x00},
	{0x200, 0x03},
	{0x203, 0x03},
	{0x203, 0x00},
	//{0x200, 0x1B},//Move inside firmware codes
};

tD2Register code D2TableSIU_PAL_VGA[] =
{
	{0x210, 0x00},
	{0x211, 0x05},
	{0x212, 0x00},
	{0x217, 0x00},
	{0x218, 0x05},
	{0x219, 0x00},
	// SIU Windowing Parameters
	{0x232, 0x40},
	{0x233, 0x00},
	#if 1
	{0x234, 0x02},//Cropped PAL height because of the limitation on buffer size.
	{0x235, 0x00},//Cropped PAL height because of the limitation on buffer size.
	#else
	{0x234, 0x10},//Cropped PAL height because of the limitation on buffer size.
	{0x235, 0x00},//Cropped PAL height because of the limitation on buffer size.
	#endif
	{0x236, 0x00},
	{0x237, 0x05},
	#ifdef DECODER_DE_INTERLACE
	{0x238, 0xF0},//Cropped PAL height because of the limitation on buffer size.
	{0x239, 0x00},//Cropped PAL height because of the limitation on buffer size.
	#else  // DECODER_DE_INTERLACE
	{0x238, 0xE0},//Cropped PAL height because of the limitation on buffer size.
	{0x239, 0x01},//Cropped PAL height because of the limitation on buffer size.
	#endif // DECODER_DE_INTERLACE
	{0x23B, 0x00},
	{0x23C, 0x00},
	{0x23D, 0x00},
	{0x23E, 0x00},
	{0x23F, 0x00},
	{0x240, 0x05},
	{0x241, 0xE0},
	{0x242, 0x01},
	// SIU Control and Configuration
	#ifdef DECODER_DE_INTERLACE
	{0x201, 0x01},
	{0x244, 0x01},
	#else  // DECODER_DE_INTERLACE
		#ifdef DECODER_SINGLE_FIELD
	{0x201, 0x00},
	{0x244, 0x00},
		#else  // DECODER_SINGLE_FIELD
	{0x201, 0x00},
	{0x244, 0x01},
		#endif // DECODER_SINGLE_FIELD
	#endif // DECODER_DE_INTERLACE
	{0x23A, 0x00},
	{0x243, 0x21},
	{0x203, 0x0C},
	{0x202, 0x03},
	{0x202, 0x00},
	{0x200, 0x03},
	{0x203, 0x03},
	{0x203, 0x00},
	//{0x200, 0x1B},//Move inside firmware codes
};

	#ifdef SUPPORT_HD_CROPPED
tD2Register code D2TableSIU_HD[] =
{
	{0x210, 0x00},
	{0x211, 0x05},
	{0x212, 0x00},
	{0x217, 0x00},
	{0x218, 0x0A},
	{0x219, 0x00},
	// SIU Windowing Parameters
	{0x232, 0x40},
	{0x233, 0x00},
	{0x234, 0x00},
	{0x235, 0x00},
	{0x236, 0x00},
	{0x237, 0x05},
	#ifdef DECODER_DE_INTERLACE
	{0x238, 0xF0},
	{0x239, 0x00},
	#else  // DECODER_DE_INTERLACE
	{0x238, 0xE0},
	{0x239, 0x01},
	#endif // DECODER_DE_INTERLACE
	{0x23B, 0x00},
	{0x23C, 0x00},
	{0x23D, 0x00},
	{0x23E, 0x00},
	{0x23F, 0x00},
	{0x240, 0x0A},
	{0x241, 0xD0},
	{0x242, 0x02},
	// SIU Control and Configuration
	#ifdef DECODER_DE_INTERLACE
	{0x201, 0x01},
	{0x244, 0x01},
	#else  // DECODER_DE_INTERLACE
		#ifdef DECODER_SINGLE_FIELD
	{0x201, 0x00},
	{0x244, 0x00},
		#else  // DECODER_SINGLE_FIELD
	{0x201, 0x00},
	{0x244, 0x01},
		#endif // DECODER_SINGLE_FIELD
	#endif // DECODER_DE_INTERLACE
	{0x23A, 0x00},
	{0x243, 0x21},
	{0x203, 0x0C},
	{0x202, 0x03},
	{0x202, 0x00},
	{0x200, 0x03},
	{0x203, 0x03},
	{0x203, 0x00},
	//{0x200, 0x1A},//Move inside firmware codes
};
	#endif // SUPPORT_HD_CROPPED

tD2Register code D2TableSIU_NTSC_HD[] =
{
	{0x210, 0x00},
	{0x211, 0x05},
	{0x212, 0x00},
	#ifdef SUPPORT_HD_CROPPED
	{0x217, 0x80},
	{0x218, 0x07},
	{0x219, 0x00},
	#else  // SUPPORT_HD_CROPPED
	{0x217, 0x00},
	{0x218, 0x0A},
	{0x219, 0x00},
	#endif // SUPPORT_HD_CROPPED
	// SIU Windowing Parameters
	{0x232, 0x40},
	{0x233, 0x00},
	{0x234, 0x00},
	{0x235, 0x00},
	{0x236, 0x00},
	{0x237, 0x05},
	#ifdef DECODER_DE_INTERLACE
	{0x238, 0xF0},
	{0x239, 0x00},
	#else  // DECODER_DE_INTERLACE
	{0x238, 0xE0},
	{0x239, 0x01},
	#endif // DECODER_DE_INTERLACE
	#ifdef SUPPORT_HD_CROPPED
	{0x23B, 0x40},
	{0x23C, 0x01},
	{0x23F, 0x80},
	{0x240, 0x07},
	#else  // SUPPORT_HD_CROPPED
	{0x23B, 0x00},
	{0x23C, 0x00},
	{0x23F, 0x00},
	{0x240, 0x0A},
	#endif // SUPPORT_HD_CROPPED
	{0x23D, 0x00},
	{0x23E, 0x00},
	{0x241, 0xD0},
	{0x242, 0x02},
	// SIU Control and Configuration
	#ifdef DECODER_DE_INTERLACE
	{0x201, 0x01},
	{0x244, 0x01},
	#else  // DECODER_DE_INTERLACE
		#ifdef DECODER_SINGLE_FIELD
	{0x201, 0x00},
	{0x244, 0x00},
		#else  // DECODER_SINGLE_FIELD
	{0x201, 0x00},
	{0x244, 0x01},
		#endif // DECODER_SINGLE_FIELD
	#endif // DECODER_DE_INTERLACE
	{0x23A, 0x00},
	{0x243, 0x21},
	{0x203, 0x0C},
	{0x202, 0x03},
	{0x202, 0x00},
	{0x200, 0x03},
	{0x203, 0x03},
	{0x203, 0x00},
	//{0x200, 0x1B},//Move inside firmware codes
};

tD2Register code D2TableSIU_PAL_HD[] =
{
	{0x210, 0x00},
	{0x211, 0x05},
	{0x212, 0x00},
	#ifdef SUPPORT_HD_CROPPED
	{0x217, 0x80},
	{0x218, 0x07},
	{0x219, 0x00},
	#else  // SUPPORT_HD_CROPPED
	{0x217, 0x00},
	{0x218, 0x0A},
	{0x219, 0x00},
	#endif // SUPPORT_HD_CROPPED
	// SIU Windowing Parameters
	{0x232, 0x40},
	{0x233, 0x00},
	#if 1
	{0x234, 0x02},//Cropped PAL height because of the limitation on buffer size.
	{0x235, 0x00},//Cropped PAL height because of the limitation on buffer size.
	#else
	{0x234, 0x10},//Cropped PAL height because of the limitation on buffer size.
	{0x235, 0x00},//Cropped PAL height because of the limitation on buffer size.
	#endif
	{0x236, 0x00},
	{0x237, 0x05},
	#ifdef DECODER_DE_INTERLACE
	{0x238, 0xF0},//Cropped PAL height because of the limitation on buffer size.
	{0x239, 0x00},//Cropped PAL height because of the limitation on buffer size.
	#else  // DECODER_DE_INTERLACE
	{0x238, 0xE0},//Cropped PAL height because of the limitation on buffer size.
	{0x239, 0x01},//Cropped PAL height because of the limitation on buffer size.
	#endif // DECODER_DE_INTERLACE
	#ifdef SUPPORT_HD_CROPPED
	{0x23B, 0x40},
	{0x23C, 0x01},
	{0x23F, 0x80},
	{0x240, 0x07},
	#else  // SUPPORT_HD_CROPPED
	{0x23B, 0x00},
	{0x23C, 0x00},
	{0x23F, 0x00},
	{0x240, 0x0A},
	#endif // SUPPORT_HD_CROPPED
	{0x23D, 0x00},
	{0x23E, 0x00},
	{0x241, 0xD0},
	{0x242, 0x02},
	// SIU Control and Configuration
	#ifdef DECODER_DE_INTERLACE
	{0x201, 0x01},
	{0x244, 0x01},
	#else  // DECODER_DE_INTERLACE
		#ifdef DECODER_SINGLE_FIELD
	{0x201, 0x00},
	{0x244, 0x00},
		#else  // DECODER_SINGLE_FIELD
	{0x201, 0x00},
	{0x244, 0x01},
		#endif // DECODER_SINGLE_FIELD
	#endif // DECODER_DE_INTERLACE
	{0x23A, 0x00},
	{0x243, 0x21},
	{0x203, 0x0C},
	{0x202, 0x03},
	{0x202, 0x00},
	{0x200, 0x03},
	{0x203, 0x03},
	{0x203, 0x00},
	//{0x200, 0x1B},//Move inside firmware codes
};

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
	#if defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x414, 0xF0},
	{0x415, 0x01},
	#else  // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x414, 0xE0},
	{0x415, 0x01},
	#endif // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0E},
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
	#if defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x431, 0xF1},
	{0x432, 0x01},
	#else  // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x431, 0xE1},
	{0x432, 0x01},
	#endif // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	// SOU Control
	{0x411, 0x05},
};

tD2Register code D2TableSOU_HD[] =
{	// 1280 x 720, 24fps at 48MHz, 53MHz, 60MHz, 64MHz
	// 1280 x 720, 30fps at 60MHz, 64MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x00},
	{0x413, 0x05},
	{0x414, 0xD0},
	{0x415, 0x02},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0D},
	#endif // SOU_FIXED_COLOR
	#if  SOU_HDCLK == 53
	{0x41B, 0x78},
	{0x41C, 0x0B},
	#elif SOU_HDCLK == 60
		#ifdef SENSOR_HD_FRAMERATE_30
	{0x41B, 0x62},
	{0x41C, 0x0A},
		#else  // SENSOR_HD_FRAMERATE_30
	{0x41B, 0xFC},
	{0x41C, 0x0C},
		#endif // SENSOR_HD_FRAMERATE_30
	#elif SOU_HDCLK == 64
		#ifdef SENSOR_HD_FRAMERATE_30
	{0x41B, 0x14},
	{0x41C, 0x0B},
		#else  // SENSOR_HD_FRAMERATE_30
	{0x41B, 0xDA},
	{0x41C, 0x0D},
		#endif // SENSOR_HD_FRAMERATE_30
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
	#endif // SOU_HDCLK == 48
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

#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
tD2Register code D2TableSOU_DualVGA[] =
{	// 1280 x 480, 30fps at 40MHz, 48MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x00},
	{0x413, 0x05},
	#if defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x414, 0xF0},
	{0x415, 0x01},
	#else  // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x414, 0xE0},
	{0x415, 0x01},
	#endif // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0C},
	#endif // SOU_FIXED_COLOR
	#if SOU_DualVGACLK == 40
	{0x41B, 0xB6},
	{0x41C, 0x0A},
	#else  // SOU_DualVGACLK == 48
	{0x41B, 0xE4},
	{0x41C, 0x0B},
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
		#ifdef LDC_PATCH_VGA
	{0x425, 0xF4},
	{0x426, 0x01},
		#else  // LDC_PATCH_VGA
	{0x425, 0xE6},
	{0x426, 0x01},
		#endif // LDC_PATCH_VGA
	#else  // SOU_DualVGACLK == 48
	{0x425, 0x0D},
	{0x426, 0x02},
	#endif // SOU_DualVGACLK
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	#if defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x431, 0xF1},
	{0x432, 0x01},
	#else  // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x431, 0xE1},
	{0x432, 0x01},
	#endif // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	// SOU Control
	{0x411, 0x05},
};

tD2Register code D2TableSOU_DualHD[] =
{	// 1920 x 720, 18fps at 53MHz, LDC_PATCH_HD off
	// 1920 x 720, 21fps at 60MHz, LDC_PATCH_HD off
	// 1920 x 720, 22.4fps at 64MHz, LDC_PATCH_HD off
	// 1920 x 720, 24fps at 80MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x07},
	#if defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	{0x414, 0xE0},
	{0x415, 0x02},
	#else  // defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	{0x414, 0xD0},
	{0x415, 0x02},
	#endif // defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0A},
	#endif // SOU_FIXED_COLOR
	#if SOU_DualHDCLK == 53
	{0x41B, 0xF2},
	{0x41C, 0x0F},
	#elif SOU_DualHDCLK == 60
	{0x41B, 0x64},
	{0x41C, 0x0F},
	#elif SOU_DualHDCLK == 64
	{0x41B, 0x64},
	{0x41C, 0x0F},
	#else  // SOU_DualHDCLK == 80
	{0x41B, 0x2C},
	{0x41C, 0x11},
	#endif // SOU_DualHDCLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x0F},
	#if SOU_DualHDCLK == 80
	{0x425, 0xF6},
	{0x426, 0x02},
	#else  // SOU_DualHDCLK
		#if defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	{0x425, 0xE2},
	{0x426, 0x02},
		#else  // defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	{0x425, 0xD5},
	{0x426, 0x02},
		#endif // defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	#endif // SOU_DualHDCLK
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	#if defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	{0x431, 0xE1},
	{0x432, 0x02},
	#else  // defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	{0x431, 0xD1},
	{0x432, 0x02},
	#endif // defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	// SOU Control
	{0x411, 0x05},
};

tD2Register code D2TableSOU_HalfHD[] =
{	//  960 x 720, 24fps at 40MHz, 48MHz, 53MHz, 60MHz, 64MHz
	//  960 x 544, 30fps at 48MHz, 53MHz, 60MHz, 64MHz
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0xC0},
	{0x413, 0x03},
	#if defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	{0x414, 0xE0},
	{0x415, 0x02},
	#else  // defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	{0x414, 0xD0},
	{0x415, 0x02},
	#endif // defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x09},
	#endif // SOU_FIXED_COLOR
	#if SOU_HalfHDCLK == 40
	{0x41B, 0xBA},
	{0x41C, 0x08},
	#elif SOU_HalfHDCLK == 48
		#ifdef SENSOR_HD_FRAMERATE_30
	{0x41B, 0x60},
	{0x41C, 0x08},
		#else  // SENSOR_HD_FRAMERATE_30
	{0x41B, 0x78},
	{0x41C, 0x0A},
		#endif // SENSOR_HD_FRAMERATE_30
	#elif SOU_HalfHDCLK == 53
		#ifdef SENSOR_HD_FRAMERATE_30
	{0x41B, 0x4C},
	{0x41C, 0x09},
		#else  // SENSOR_HD_FRAMERATE_30
	{0x41B, 0xA2},
	{0x41C, 0x0B},
		#endif // SENSOR_HD_FRAMERATE_30
	#elif SOU_HalfHDCLK == 60
		#ifdef SENSOR_HD_FRAMERATE_30
	{0x41B, 0x78},
	{0x41C, 0x0A},
		#else  // SENSOR_HD_FRAMERATE_30
	{0x41B, 0x16},
	{0x41C, 0x0D},
		#endif // SENSOR_HD_FRAMERATE_30
	#else  // SOU_HalfHDCLK == 64
		#ifdef SENSOR_HD_FRAMERATE_30
	{0x41B, 0x2A},
	{0x41C, 0x0B},
		#else  // SENSOR_HD_FRAMERATE_30
	{0x41B, 0xF6},
	{0x41C, 0x0D},
		#endif // SENSOR_HD_FRAMERATE_30
	#endif // SOU_HalfHDCLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0xC3},
	{0x424, 0x07},
	{0x425, 0xEA},
	{0x426, 0x02},
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x02},
	{0x430, 0x00},
	#if defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	{0x431, 0xE1},
	{0x432, 0x02},
	#else  // defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	{0x431, 0xD1},
	{0x432, 0x02},
	#endif // defined(DECODER_SCALING_HD) && defined(LDC_PATCH_HD)
	// SOU Control
	{0x411, 0x05},
};
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE

tD2Register code D2TableSingleSensor0_VGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Buffers
	{0x37C, 0x00},
	{0x37D, 0x05},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x0A},
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
	{0x322, 0x00},
	{0x323, 0x05},
	#if defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x320, 0xF0},
	{0x321, 0x01},
	{0x326, 0xF0},
	{0x327, 0x01},
	#else  // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x320, 0xE0},
	{0x321, 0x01},
	{0x326, 0xE0},
	{0x327, 0x01},
	#endif // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	// IPU Control and Configuration
	{0x32C, 0x00},
	{0x302, 0x35},
	{0x302, 0x30},
	#ifdef DECODER_SCALING_VGA
	{0x32F, 0x20},
	{0x32F, 0x00},
	{0x300, 0x52},
	#else  // DECODER_SCALING_VGA
	{0x300, 0x03},
	#endif // DECODER_SCALING_VGA
};

tD2Register code D2TableSingleSensor1_VGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Buffers
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
	{0x322, 0x00},
	{0x323, 0x05},
	#if defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	// Set IPU registers to meet the setting requirement of SOU registers.
	{0x320, 0xF0},
	{0x321, 0x01},
	{0x326, 0xF0},
	{0x327, 0x01},
	#else  // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x320, 0xE0},
	{0x321, 0x01},
	{0x326, 0xE0},
	{0x327, 0x01},
	#endif // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	// IPU Control and Configuration
	{0x32C, 0x20},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};

tD2Register code D2TableSingleSensor1_HD[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Buffers
	{0x37C, 0x00},
	{0x37D, 0x05},
	{0x37E, 0x00},
	{0x37F, 0x00},
	{0x380, 0x0A},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x00},
	{0x31F, 0x05},
	{0x324, 0x00},
	{0x325, 0x0A},
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
	{0x323, 0x0A},
	{0x326, 0xD0},
	{0x327, 0x02},
	// IPU Control and Configuration
	{0x32C, 0x20},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};

#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
tD2Register code D2TableFullSideBySide_VGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Buffers
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
	{0x322, 0x00},
	{0x323, 0x05},
	#if defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x320, 0xF0},
	{0x321, 0x01},
	{0x326, 0xF0},
	{0x327, 0x01},
	#else  // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x320, 0xE0},
	{0x321, 0x01},
	{0x326, 0xE0},
	{0x327, 0x01},
	#endif // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	// IPU Control and Configuration
	{0x32C, 0x02},
	{0x302, 0x35},
	{0x302, 0x30},
	#ifdef DECODER_SCALING_VGA
	{0x32F, 0x20},
	{0x32F, 0x00},
	{0x300, 0x52},
	#else  // DECODER_SCALING_VGA
	{0x300, 0x03},
	#endif // DECODER_SCALING_VGA
};

tD2Register code D2TableHalfSideBySide_VGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Buffers
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
	{0x322, 0x00},
	{0x323, 0x05},
	#if defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x320, 0xF0},
	{0x321, 0x01},
	{0x326, 0xF0},
	{0x327, 0x01},
	#else  // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	{0x320, 0xE0},
	{0x321, 0x01},
	{0x326, 0xE0},
	{0x327, 0x01},
	#endif // defined(DECODER_SCALING_VGA) && defined(LDC_PATCH_VGA)
	// IPU Control and Configuration
	{0x32C, 0x42},
	{0x302, 0x35},
	{0x302, 0x30},
	#ifdef DECODER_SCALING_VGA
	{0x32F, 0x20},
	{0x32F, 0x00},
	{0x300, 0x52},
	#else  // DECODER_SCALING_VGA
	{0x300, 0x03},
	#endif // DECODER_SCALING_VGA
};

tD2Register code D2TableFullSideBySide_HD[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Buffers
	{0x37C, 0x00},
	{0x37D, 0x05},
	{0x37E, 0x00},
	#ifdef SUPPORT_HD_CROPPED
	{0x37F, 0x80},
	{0x380, 0x07},
	{0x381, 0x00},
	#else  // SUPPORT_HD_CROPPED
	{0x37F, 0x00},
	{0x380, 0x0A},
	{0x381, 0x00},
	#endif // SUPPORT_HD_CROPPED
	// IPU Windowing Parameters
	{0x31E, 0x00},
	{0x31F, 0x05},
	#ifdef SUPPORT_HD_CROPPED
	{0x324, 0x80},
	{0x325, 0x07},
	#else  // SUPPORT_HD_CROPPED
	{0x324, 0x00},
	{0x325, 0x0A},
	#endif // SUPPORT_HD_CROPPED
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	#ifdef DECODER_SCALING
		#ifdef SUPPORT_HD_CROPPED
	{0x31C, 0x80},
	{0x31D, 0x07},
	{0x322, 0x80},
	{0x323, 0x07},
		#else  // SUPPORT_HD_CROPPED
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x322, 0x00},
	{0x323, 0x0A},
		#endif // SUPPORT_HD_CROPPED
		#ifdef LDC_PATCH_HD
	{0x320, 0xE0},
	{0x321, 0x02},
	{0x326, 0xE0},
	{0x327, 0x02},
		#else  // LDC_PATCH_HD
	{0x320, 0xD0},
	{0x321, 0x02},
	{0x326, 0xD0},
	{0x327, 0x02},
		#endif // LDC_PATCH_HD
	#else  // DECODER_SCALING
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x322, 0x00},
	{0x323, 0x0A},
	{0x320, 0xD0},//{0x320, 0xE0},
	{0x321, 0x02},//{0x321, 0x01},
	{0x326, 0xD0},
	{0x327, 0x02},
	#endif // DECODER_SCALING
	// IPU Control and Configuration
	{0x32C, 0x02},
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

tD2Register code D2TableHalfSideBySide_HD[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Buffers
	{0x37C, 0x00},
	{0x37D, 0x05},
	{0x37E, 0x00},
	#ifdef SUPPORT_HD_CROPPED
	{0x37F, 0x80},
	{0x380, 0x07},
	{0x381, 0x00},
	#else  // SUPPORT_HD_CROPPED
	{0x37F, 0x00},
	{0x380, 0x0A},
	{0x381, 0x00},
	#endif // SUPPORT_HD_CROPPED
	// IPU Windowing Parameters
	{0x31E, 0x00},
	{0x31F, 0x05},
	#ifdef SUPPORT_HD_CROPPED
	{0x324, 0x80},
	{0x325, 0x07},
	#else  // SUPPORT_HD_CROPPED
	{0x324, 0x00},
	{0x325, 0x0A},
	#endif // SUPPORT_HD_CROPPED
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	#ifdef DECODER_SCALING
		#ifdef SUPPORT_HD_CROPPED
	{0x31C, 0x80},
	{0x31D, 0x07},
	{0x322, 0x80},
	{0x323, 0x07},
		#else  // SUPPORT_HD_CROPPED
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x322, 0x00},
	{0x323, 0x0A},
		#endif // SUPPORT_HD_CROPPED
		#ifdef LDC_PATCH_HD
	{0x320, 0xE0},
	{0x321, 0x02},
	{0x326, 0xE0},
	{0x327, 0x02},
		#else  // LDC_PATCH_HD
	{0x320, 0xD0},
	{0x321, 0x02},
	{0x326, 0xD0},
	{0x327, 0x02},
		#endif // LDC_PATCH_HD
	#else  // DECODER_SCALING
	{0x31C, 0x00},
	{0x31D, 0x05},
	{0x320, 0xD0},//{0x320, 0xE0},
	{0x321, 0x02},//{0x321, 0x01},
	{0x322, 0x00},
	{0x323, 0x0A},
	{0x326, 0xD0},
	{0x327, 0x02},
	#endif // DECODER_SCALING
	// IPU Control and Configuration
	{0x32C, 0x42},
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
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE

#ifdef DECODER_FILL_FIXEDCOLOR

tD2Register code D2TableFixedColor[] =
{
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x02},
	{0x414, 0xE0},
	{0x415, 0x01},
	//{0x41A, 0x08}, // White Color
	//{0x41A, 0x0B}, // Green Color
	//{0x41A, 0x0D}, // Red Color
	{0x41A, 0x0F}, // Black Color
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

	// SIU Buffer Parameters
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
	{0x244, 0x00},
	{0x23A, 0x21},
	{0x243, 0x21},
	{0x203, 0x0C},
	{0x202, 0x03},
	{0x202, 0x00},
	{0x200, 0x03},
	{0x203, 0x03},
	{0x203, 0x00},
	{0x200, 0x0F},
	// SOU Control
	{0x411, 0x05},
};

unsigned char xdata ucLOOP_CONTROL _at_ 0x0027; // SOU loopback control

void fnD2FillFixedColorToDRAM()
{
	ucResetControl = 0x74;      // 40:SIU Reset, 20:IPU Reset, 10:SOU Reset, 04: DRAM Reset
	ucLOOP_CONTROL = 0xC0;
	ucResetControl = 0x00;
	D2WriteOutputClock(60); 		// Set Host Clock in MHz
	fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
	fnD2WriteTable(D2TableFixedColor, sizeof(D2TableFixedColor) / sizeof(tD2Register));
	fnD2IdleDelay(5000); // count 5000 around 600ms, at least 3 frames
	ucResetControl = 0x74;      // 40:SIU Reset, 20:IPU Reset, 10:SOU Reset, 04: DRAM Reset
	ucLOOP_CONTROL = 0x00;
	ucResetControl = 0x00;
}

#endif // DECODER_FILL_FIXEDCOLOR


#ifdef DECODER_SCALING
// Note 1:
// For better performance, procedure fnD2MoveCodeToSingleBankDRAM is perferred.
// Programmers must be sure that the full buffer to be copied are in a single SDRAM bank.
// If the buffer cannot be in a single SDRAM bank, call procedure fnD2MoveCodeToDRAM instead.
// Note 2:
// DRAM buffer address for D2Scaler640x240to960x720, D2Scaler640x480to960x720, D2Scale640x360to640x720
// shall be the same as register 0x384:0x382.

unsigned char ucTableId;

void fnD2TableLoad(
	unsigned char ucResolution)
{
	switch (ucResolution)
	{
	case D2_RESOLUTION_HD_60Hz:
	case D2_RESOLUTION_HD_50Hz:
	#ifdef DECODER_SCALING_HD
		if (nD2TableVerified != D2_RESOLUTION_HD_60Hz)
		#ifdef DECODER_SCALING_VGA
			#ifdef LDC_PATCH_HD
			fnD2MoveCodeToSingleBankDRAM(D2Scale640x240to960x720, 0x01000L, (960/16+1)*(736/16+1)*8);
			#else  // LDC_PATCH_HD
			fnD2MoveCodeToSingleBankDRAM(D2Scale640x240to960x720, 0x01000L, (960/16+1)*(720/16+1)*8);
			#endif // LDC_PATCH_HD
		#else  // DECODER_SCALING_VGA
			#ifdef LDC_PATCH_HD
			fnD2MoveCodeToSingleBankDRAM(D2Scale640x480to960x720, 0x01000L, (960/16+1)*(736/16+1)*8);
			#else  // LDC_PATCH_HD
			fnD2MoveCodeToSingleBankDRAM(D2Scale640x480to960x720, 0x01000L, (960/16+1)*(720/16+1)*8);
			#endif // LDC_PATCH_HD
		#endif // DECODER_SCALING_VGA
		ucTableId = D2_RESOLUTION_HD_60Hz;
		break;
	#endif // DECODER_SCALING_HD
	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_VGA_50Hz:
		if (nD2TableVerified != D2_RESOLUTION_VGA_60Hz)
		#if defined(LDC_PATCH_VGA) || defined(LDC_PATCH_HD)
			fnD2MoveCodeToSingleBankDRAM(D2Scale640x360to640x720, 0x01000L, (640/16+1)*(736/16+1)*8);
		#else  // defined(LDC_PATCH_VGA) || defined(LDC_PATCH_HD)
			fnD2MoveCodeToSingleBankDRAM(D2Scale640x360to640x720, 0x01000L, (640/16+1)*(720/16+1)*8);
		#endif // defined(LDC_PATCH_VGA) || defined(LDC_PATCH_HD)
		ucTableId = D2_RESOLUTION_VGA_60Hz;
		break;
	}
}
#endif // DECODER_SCALING

void fnD2TableVerify()
{
	#ifdef DECODER_SCALING
	if (nD2TableVerified != ucTableId)
	{
		switch (ucTableId)
		{
		#ifdef DECODER_SCALING_HD
		case D2_RESOLUTION_HD_60Hz:
			#ifdef DECODER_SCALING_VGA
				#ifdef LDC_PATCH_HD
			fnD2VerifyCodeToSingleBankDRAM(D2Scale640x240to960x720, 0x01000L, (960/16+1)*(736/16+1)*8);
				#else  // LDC_PATCH_HD
			fnD2VerifyCodeToSingleBankDRAM(D2Scale640x240to960x720, 0x01000L, (960/16+1)*(720/16+1)*8);
				#endif // LDC_PATCH_HD
			#else  // DECODER_SCALING_VGA
				#ifdef LDC_PATCH_HD
			fnD2VerifyCodeToSingleBankDRAM(D2Scale640x480to960x720, 0x01000L, (960/16+1)*(736/16+1)*8);
				#else  // LDC_PATCH_HD
			fnD2VerifyCodeToSingleBankDRAM(D2Scale640x480to960x720, 0x01000L, (960/16+1)*(720/16+1)*8);
				#endif // LDC_PATCH_HD
			#endif // DECODER_SCALING_VGA
			nD2TableVerified = ucTableId;
			break;
		#endif // DECODER_SCALING_HD
		case D2_RESOLUTION_VGA_60Hz:
		#if defined(LDC_PATCH_VGA) || defined(LDC_PATCH_HD)
			fnD2VerifyCodeToSingleBankDRAM(D2Scale640x360to640x720, 0x01000L, (640/16+1)*(736/16+1)*8);
		#else  // defined(LDC_PATCH_VGA) || defined(LDC_PATCH_HD)
			fnD2VerifyCodeToSingleBankDRAM(D2Scale640x360to640x720, 0x01000L, (640/16+1)*(720/16+1)*8);
		#endif // defined(LDC_PATCH_VGA) || defined(LDC_PATCH_HD)
			nD2TableVerified = ucTableId;
			break;
		}
	}
	#endif // DECODER_SCALING
}

unsigned char fnD2RunByPassSensor0(
	unsigned char ucResolution)
{
	fnD2WriteTable(D2TableBuffers,  sizeof(D2TableBuffers) / sizeof(tD2Register));
	#ifdef DECODER_SCALING_VGA
	fnD2TableLoad(D2_RESOLUTION_VGA_60Hz);
	#endif // DECODER_SCALING_VGA
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_HD_60Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor0_VGA, sizeof(D2TableSingleSensor0_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_NTSC_VGA, sizeof(D2TableSIU_NTSC_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x19;
		return SUCCESS;
	case D2_RESOLUTION_VGA_50Hz:
	case D2_RESOLUTION_HD_50Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor0_VGA, sizeof(D2TableSingleSensor0_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_PAL_VGA, sizeof(D2TableSIU_PAL_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x19;
		return SUCCESS;
	default:
		return FAILURE;
	}
}

unsigned char fnD2RunByPassSensor1(
	unsigned char ucResolution)
{
	fnD2WriteTable(D2TableBuffers,  sizeof(D2TableBuffers) / sizeof(tD2Register));
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor1_VGA, sizeof(D2TableSingleSensor1_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_NTSC_VGA, sizeof(D2TableSIU_NTSC_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x1A;
		return SUCCESS;
	case D2_RESOLUTION_HD_60Hz:
	case D2_RESOLUTION_HD_50Hz:
		D2WriteOutputClock(SOU_HDCLK); 		// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_HD, sizeof(D2TableSOU_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor1_HD, sizeof(D2TableSingleSensor1_HD) / sizeof(tD2Register));
	#ifdef SUPPORT_HD_CROPPED
		fnD2WriteTable(D2TableSIU_HD, sizeof(D2TableSIU_HD) / sizeof(tD2Register));
	#else  // SUPPORT_HD_CROPPED
		fnD2WriteTable(D2TableSIU_NTSC_HD, sizeof(D2TableSIU_NTSC_HD) / sizeof(tD2Register));
	#endif // SUPPORT_HD_CROPPED
		ucSIU_CONTROL = 0x1A;
		return SUCCESS;
	default:
		return FAILURE;
	}
}

#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
unsigned char fnD2RunFullSideBySide(
	unsigned char ucResolution)
{
	fnD2WriteTable(D2TableBuffers,  sizeof(D2TableBuffers) / sizeof(tD2Register));
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	#ifdef DECODER_SCALING_VGA
		fnD2TableLoad(D2_RESOLUTION_VGA_60Hz);
	#endif // DECODER_SCALING_VGA
		D2WriteOutputClock(SOU_DualVGACLK); // Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_DualVGA, sizeof(D2TableSOU_DualVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableFullSideBySide_VGA, sizeof(D2TableFullSideBySide_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_NTSC_VGA, sizeof(D2TableSIU_NTSC_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x1B;
		return SUCCESS;
	case D2_RESOLUTION_VGA_50Hz:
	#ifdef DECODER_SCALING_VGA
		fnD2TableLoad(D2_RESOLUTION_VGA_50Hz);
	#endif // DECODER_SCALING_VGA
		D2WriteOutputClock(SOU_DualVGACLK); // Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_DualVGA, sizeof(D2TableSOU_DualVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableFullSideBySide_VGA, sizeof(D2TableFullSideBySide_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_PAL_VGA, sizeof(D2TableSIU_PAL_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x1B;
		return SUCCESS;
	case D2_RESOLUTION_HD_60Hz:
	#ifdef DECODER_SCALING_HD
		fnD2TableLoad(D2_RESOLUTION_HD_60Hz);
	#endif // DECODER_SCALING_HD
		D2WriteOutputClock(SOU_DualHDCLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_DualHD, sizeof(D2TableSOU_DualHD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableFullSideBySide_HD, sizeof(D2TableFullSideBySide_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_NTSC_HD, sizeof(D2TableSIU_NTSC_HD) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x1B;
		return SUCCESS;
	case D2_RESOLUTION_HD_50Hz:
	#ifdef DECODER_SCALING_HD
		fnD2TableLoad(D2_RESOLUTION_HD_50Hz);
	#endif // DECODER_SCALING_HD
		D2WriteOutputClock(SOU_DualHDCLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_DualHD, sizeof(D2TableSOU_DualHD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableFullSideBySide_HD, sizeof(D2TableFullSideBySide_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_PAL_HD, sizeof(D2TableSIU_PAL_HD) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x1B;
		return SUCCESS;
	default:
		return FAILURE;
	}
}

unsigned char fnD2RunHalfSideBySide(
	unsigned char ucResolution)
{
	fnD2WriteTable(D2TableBuffers,  sizeof(D2TableBuffers) / sizeof(tD2Register));
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	#ifdef DECODER_SCALING_VGA
		fnD2TableLoad(D2_RESOLUTION_VGA_60Hz);
	#endif // DECODER_SCALING_VGA
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableHalfSideBySide_VGA, sizeof(D2TableHalfSideBySide_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_NTSC_VGA, sizeof(D2TableSIU_NTSC_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x1B;
		return SUCCESS;
	case D2_RESOLUTION_VGA_50Hz:
	#ifdef DECODER_SCALING_VGA
		fnD2TableLoad(D2_RESOLUTION_VGA_50Hz);
	#endif // DECODER_SCALING_VGA
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableHalfSideBySide_VGA, sizeof(D2TableHalfSideBySide_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_PAL_VGA, sizeof(D2TableSIU_PAL_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x1B;
		return SUCCESS;
	case D2_RESOLUTION_HD_60Hz:
	#ifdef DECODER_SCALING_HD
		fnD2TableLoad(D2_RESOLUTION_HD_60Hz);
	#endif // DECODER_SCALING_HD
		D2WriteOutputClock(SOU_HalfHDCLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_HalfHD, sizeof(D2TableSOU_HalfHD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableHalfSideBySide_HD, sizeof(D2TableHalfSideBySide_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_NTSC_HD, sizeof(D2TableSIU_NTSC_HD) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x1B;
		return SUCCESS;
	case D2_RESOLUTION_HD_50Hz:
	#ifdef DECODER_SCALING_HD
		fnD2TableLoad(D2_RESOLUTION_HD_50Hz);
	#endif // DECODER_SCALING_HD
		D2WriteOutputClock(SOU_HalfHDCLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableSOU_HalfHD, sizeof(D2TableSOU_HalfHD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableHalfSideBySide_HD, sizeof(D2TableHalfSideBySide_HD) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_PAL_HD, sizeof(D2TableSIU_PAL_HD) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x1B;
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

unsigned char fnSetOperatingMode(
	unsigned char ucMode,
	unsigned char ucResolution)
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
	#ifdef DECODER_FILL_FIXEDCOLOR
		fnD2FillFixedColorToDRAM();
	#endif // DECODER_FILL_FIXEDCOLOR
		if (fnDoInitSensor(ucResolution) == FAILURE)
			return FAILURE;
		return fnD2RunByPassSensor0(ucResolution);

	case D2_OPERATION_BYPASS_SENSOR1:
		if (fnDoInitSensor(ucResolution) == FAILURE)
			return FAILURE;
		return fnD2RunByPassSensor1(ucResolution);

#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
	case D2_OPERATION_HORIZONTAL_SIDE_BY_SIDE:
	#ifdef DECODER_FILL_FIXEDCOLOR
		fnD2FillFixedColorToDRAM();
	#endif // DECODER_FILL_FIXEDCOLOR
		if (fnDoInitSensor(ucResolution) == FAILURE)
			return FAILURE;
		return fnD2RunFullSideBySide(ucResolution);

	case D2_OPERATION_HALF_SIDE_BY_SIDE:
	#ifdef DECODER_FILL_FIXEDCOLOR
		fnD2FillFixedColorToDRAM();
	#endif // DECODER_FILL_FIXEDCOLOR
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
