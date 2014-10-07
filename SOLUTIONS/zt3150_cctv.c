//
// Zealtek D2 project
// ZT3150 Twin VGA sensor solutions for CCTV
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// Aug 17, 2012.

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

	#ifdef SUPPORT_FUZZY_STITCHING
		#include "zt3150_Accurate_Para.h"
	#endif // SUPPORT_FUZZY_STITCHING

	#if (CLK_XIN == 27)
		#define SOU_VGACLK    54
	#else  // (CLK_XIN == 27)
		#define SOU_VGACLK    48
	#endif // (CLK_XIN == 27)
	#if (CLK_PLL / SOU_VGACLK >= 16)
		#error Maximum Clock Divider for SOU_VGACLK is 16.
	#endif // (CLK_PLL / SOU_VGACLK >= 16)

	// SENSOR_BYPASS_PATH allows the images from sensor source, bypasses all D2 sub-modules,
	// and then sends to the destination. Disabling this option (preferred) allows the images
	// storing into SDRAM buffers, and then sends to the destination.
	//#define SENSOR_BYPASS_PATH

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
};

tD2Register code D2TableSIU_QVGA[] =
{
	// SDA Memory Strides
	{0x210, 0x80},
	{0x211, 0x02},
	{0x212, 0x00},
	{0x217, 0x80},
	{0x218, 0x02},
	{0x219, 0x00},
	// SIU Windowing Parameters
	{0x232, 0x00},
	{0x233, 0x00},
	{0x234, 0x00},
	{0x235, 0x00},
	{0x236, 0x80},
	{0x237, 0x02},
	{0x238, 0xF0},
	{0x239, 0x00},
	{0x23B, 0x00},
	{0x23C, 0x00},
	{0x23D, 0x00},
	{0x23E, 0x00},
	{0x23F, 0x80},
	{0x240, 0x02},
	{0x241, 0xF0},
	{0x242, 0x00},
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

tD2Register code D2TableSOU_50HzVGA[] =
{	// 640x480, 50fps at 48MHz
	// 640x480, 50fps at 54MHz
	// SOU Configuration
	#ifdef CONTROLLER_SONIX_CCTV
	{0x400, 0x0A},
	#else  // CONTROLLER_SONIX_CCTV
	{0x400, 0x08},
	#endif // CONTROLLER_SONIX_CCTV
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x02},
	{0x414, 0xE0},
	{0x415, 0x01},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0D},
	#endif // SOU_FIXED_COLOR
	#if SOU_VGACLK == 48
	{0x41B, 0x24},//0x724=1828
	//{0x41B, 0x20},//0x720=1824
	{0x41C, 0x07},
	#else  // SOU_VGACLK == 54
	{0x41B, 0x08},//0x808=2056
	{0x41C, 0x08},
	#endif // SOU_VGACLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x05},
	{0x425, 0x0D},//0x20D=525
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


tD2Register code D2TableSOU_60HzVGA[] =
{	// 640x480, 59.94fps at 48MHz
	// 640x480, 59.94fps at 54MHz
	// SOU Configuration
	#ifdef CONTROLLER_SONIX_CCTV
	{0x400, 0x0A},
	#else  // CONTROLLER_SONIX_CCTV
	{0x400, 0x08},
	#endif // CONTROLLER_SONIX_CCTV
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x02},
	{0x414, 0xE0},
	{0x415, 0x01},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0D},
	#endif // SOU_FIXED_COLOR
	#if SOU_VGACLK == 48
	{0x41B, 0xF5},//0x5F5=1525
	{0x41C, 0x05},
	#else  // SOU_VGACLK == 54
	{0x41B, 0xB2},//0x706=1714
	{0x41C, 0x06},
	#endif // SOU_VGACLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x05},
	{0x425, 0x0D},//0x20D=525
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

tD2Register code D2TableSOU_50HzDualQVGA[] =
{	// 640x240, 50fps at 48MHz (compatible to 640x480)
	// 640x240, 50fps at 54MHz (compatible to 640x480)
	// SOU Configuration
	#ifdef CONTROLLER_SONIX_CCTV
	{0x400, 0x0A},
	#else  // CONTROLLER_SONIX_CCTV
	{0x400, 0x08},
	#endif // CONTROLLER_SONIX_CCTV
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x02},
	{0x414, 0xF0},
	{0x415, 0x00},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0A},
	#endif // SOU_FIXED_COLOR
	#if SOU_VGACLK == 48
	{0x41B, 0x24},//0x724=1828
	//{0x41B, 0x20},//0x720=1824
	{0x41C, 0x07},
	#else  // SOU_VGACLK == 54
	{0x41B, 0x08},//0x808=2056
	{0x41C, 0x08},
	#endif // SOU_VGACLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x05},
	{0x425, 0x0D},//0x20D=525
	{0x426, 0x02},
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x7A},
	{0x430, 0x00},
	{0x431, 0x69},
	{0x432, 0x01},
	// SOU Control
	{0x411, 0x05},
};

tD2Register code D2TableSOU_60HzDualQVGA[] =
{	// 640x240, 59.94fps at 48MHz (compatible to 640x480)
	// 640x240, 59.94fps at 54MHz (compatible to 640x480)
	// SOU Configuration
	#ifdef CONTROLLER_SONIX_CCTV
	{0x400, 0x0A},
	#else  // CONTROLLER_SONIX_CCTV
	{0x400, 0x08},
	#endif // CONTROLLER_SONIX_CCTV
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x02},
	{0x414, 0xF0},
	{0x415, 0x00},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x0A},
	#endif // SOU_FIXED_COLOR
	#if SOU_VGACLK == 48
	{0x41B, 0xF5},//0x5F5=1525
	{0x41C, 0x05},
	#else  // SOU_VGACLK == 54
	{0x41B, 0xB2},//0x706=1714
	{0x41C, 0x06},
	#endif // SOU_VGACLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x05},
	{0x425, 0x0D},//0x20D=525
	{0x426, 0x02},
	{0x427, 0x01},
	{0x428, 0x00},
	{0x42B, 0x02},
	{0x42C, 0x00},
	{0x42F, 0x7A},
	{0x430, 0x00},
	{0x431, 0x69},
	{0x432, 0x01},
	// SOU Control
	{0x411, 0x05},
};

#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
tD2Register code D2TableSOU_50Hz2xQVGA[] =
{	// 320x480, 50fps at 48MHz (compatible to 640x480)
	// 320x480, 50fps at 54MHz (compatible to 640x480)
	// SOU Configuration
	#ifdef CONTROLLER_SONIX_CCTV
	{0x400, 0x0A},
	#else  // CONTROLLER_SONIX_CCTV
	{0x400, 0x08},
	#endif // CONTROLLER_SONIX_CCTV
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x40},
	{0x413, 0x01},
	{0x414, 0xE0},
	{0x415, 0x01},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x09},
	#endif // SOU_FIXED_COLOR
	#if SOU_VGACLK == 48
	{0x41B, 0x24},//0x724=1828
	//{0x41B, 0x20},//0x720=1824
	{0x41C, 0x07},
	#else  // SOU_VGACLK == 54
	{0x41B, 0x08},//0x808=2056
	{0x41C, 0x08},
	#endif // SOU_VGACLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x84},
	{0x422, 0x01},
	{0x423, 0x03},
	{0x424, 0x04},
	{0x425, 0x0D},//0x20D=525
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

tD2Register code D2TableSOU_60Hz2xQVGA[] =
{	// 320x480, 59.94fps at 48MHz (compatible to 640x480)
	// 320x480, 59.94fps at 54MHz (compatible to 640x480)
	// SOU Configuration
	#ifdef CONTROLLER_SONIX_CCTV
	{0x400, 0x0A},
	#else  // CONTROLLER_SONIX_CCTV
	{0x400, 0x08},
	#endif // CONTROLLER_SONIX_CCTV
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x40},
	{0x413, 0x01},
	{0x414, 0xE0},
	{0x415, 0x01},
	#ifdef SOU_FIXED_COLOR
	{0x41A, 0x09},
	#endif // SOU_FIXED_COLOR
	#if SOU_VGACLK == 48
	{0x41B, 0xF5},//0x5F5=1525
	{0x41C, 0x05},
	#else  // SOU_VGACLK == 54
	{0x41B, 0xB2},//0x706=1714
	{0x41C, 0x06},
	#endif // SOU_VGACLK
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x84},
	{0x422, 0x01},
	{0x423, 0x03},
	{0x424, 0x04},
	{0x425, 0x0D},//0x20D=525
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

tD2Register code D2TableFullSideBySide_QVGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x80},
	{0x37D, 0x02},
	{0x37E, 0x00},
	{0x37F, 0x80},
	{0x380, 0x02},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x80},
	{0x31F, 0x02},
	{0x324, 0x80},
	{0x325, 0x02},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x80},
	{0x31D, 0x02},
	{0x320, 0xF0},
	{0x321, 0x00},
	{0x322, 0x80},
	{0x323, 0x02},
	{0x326, 0xF0},
	{0x327, 0x00},
	// IPU Control and Configuration
	{0x32C, 0x02},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE

#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
tD2Register code D2TableVerticalSideBySide_QVGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x80},
	{0x37D, 0x02},
	{0x37E, 0x00},
	{0x37F, 0x80},
	{0x380, 0x02},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x80},
	{0x31F, 0x02},
	{0x324, 0x80},
	{0x325, 0x02},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0x80},
	{0x31D, 0x02},
	{0x320, 0xF0},
	{0x321, 0x00},
	{0x322, 0x80},
	{0x323, 0x02},
	{0x326, 0xF0},
	{0x327, 0x00},
	// IPU Control and Configuration
	{0x32C, 0x04},
	{0x302, 0x35},
	{0x302, 0x30},
	{0x300, 0x03},
};
#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE

#ifdef SUPPORT_FUZZY_STITCHING
tD2Register code D2TableAccurateStitch_QVGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x80},
	{0x37D, 0x02},
	{0x37E, 0x00},
	{0x37F, 0x80},
	{0x380, 0x02},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x80},
	{0x31F, 0x02},
	{0x324, 0x80},
	{0x325, 0x02},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0xA0},
	{0x31D, 0x02},
	{0x320, 0xF0},
	{0x321, 0x00},
	{0x322, 0xA0},
	{0x323, 0x02},
	{0x326, 0xF0},
	{0x327, 0x00},
	{0x32E, 0x20},
	{0x32F, 0x00},
	{0x330, 0x40},
	{0x331, 0x00},
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

tD2Register code D2TableScaledStitch_QVGA[] =
{
	// IPU Control and Configuration
	{0x301, 0xA0},
	{0x302, 0x30},
	// IPU Memory Strides
	{0x37C, 0x80},
	{0x37D, 0x02},
	{0x37E, 0x00},
	{0x37F, 0x80},
	{0x380, 0x02},
	{0x381, 0x00},
	// IPU Windowing Parameters
	{0x31E, 0x80},
	{0x31F, 0x02},
	{0x324, 0x80},
	{0x325, 0x02},
	{0x328, 0x1E},
	{0x329, 0x00},
	{0x32A, 0x1E},
	{0x32B, 0x00},
	// STCH Windowing Parameters
	{0x31C, 0xA0},
	{0x31D, 0x02},
	{0x320, 0xE0},
	{0x321, 0x01},
	{0x322, 0xA0},
	{0x323, 0x02},
	{0x326, 0xE0},
	{0x327, 0x01},
	{0x32E, 0x20},
	{0x32F, 0x00},
	{0x330, 0x40},
	{0x331, 0x00},
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

unsigned char fnD2RunByPassSensor0(
	unsigned char ucResolution)
{
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor0, sizeof(D2TableBypassSensor0) / sizeof(tD2Register));
		#else  // SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_60HzVGA, sizeof(D2TableSOU_60HzVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor0_VGA, sizeof(D2TableSingleSensor0_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0D;
		#endif // SENSOR_BYPASS_PATH
		return SUCCESS;

	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor0, sizeof(D2TableBypassSensor0) / sizeof(tD2Register));
		#else  // SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_50HzVGA, sizeof(D2TableSOU_50HzVGA) / sizeof(tD2Register));
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
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor1, sizeof(D2TableBypassSensor1) / sizeof(tD2Register));
		#else  // SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_60HzVGA, sizeof(D2TableSOU_60HzVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSingleSensor1_VGA, sizeof(D2TableSingleSensor1_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0E;
		#endif // SENSOR_BYPASS_PATH
		return SUCCESS;

	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		#ifdef SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBypassSensor1, sizeof(D2TableBypassSensor1) / sizeof(tD2Register));
		#else  // SENSOR_BYPASS_PATH
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_50HzVGA, sizeof(D2TableSOU_50HzVGA) / sizeof(tD2Register));
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
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_60HzDualQVGA, sizeof(D2TableSOU_60HzDualQVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableFullSideBySide_QVGA, sizeof(D2TableFullSideBySide_QVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_QVGA, sizeof(D2TableSIU_QVGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;

	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_50HzDualQVGA, sizeof(D2TableSOU_50HzDualQVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableFullSideBySide_QVGA, sizeof(D2TableFullSideBySide_QVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_QVGA, sizeof(D2TableSIU_QVGA) / sizeof(tD2Register));
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
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_60HzVGA, sizeof(D2TableSOU_60HzVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableHalfSideBySide_VGA, sizeof(D2TableHalfSideBySide_VGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_VGA, sizeof(D2TableSIU_VGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;

	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_50HzVGA, sizeof(D2TableSOU_50HzVGA) / sizeof(tD2Register));
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
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_60Hz2xQVGA, sizeof(D2TableSOU_60Hz2xQVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableVerticalSideBySide_QVGA, sizeof(D2TableVerticalSideBySide_QVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_QVGA, sizeof(D2TableSIU_QVGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;

	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSOU_50Hz2xQVGA, sizeof(D2TableSOU_50Hz2xQVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableVerticalSideBySide_QVGA, sizeof(D2TableVerticalSideBySide_QVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_QVGA, sizeof(D2TableSIU_QVGA) / sizeof(tD2Register));
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
// DRAM buffer address for D2CalibrationTable3 shall be the same as register 0x384:0x382.
// DRAM buffer address for D2CalibrationTable4 shall be the same as register 0x388:0x386.

void fnD2TableLoad(
	unsigned char ucNewMode)
{
	if (nD2TableVerified != ucNewMode)
	{
		switch (ucNewMode)
		{
		case D2_OPERATION_ACCURATE_STITCHING:
			fnD2MoveCodeToSingleBankDRAM(D2CalibrationTable1, 0x01000L, (336/16+1)*(240/16+1)*8);
			fnD2MoveCodeToSingleBankDRAM(D2CalibrationTable2, 0x04000L, (336/16+1)*(240/16+1)*8);
			break;
		case D2_OPERATION_SCALED_STITCHING:
			fnD2MoveCodeToSingleBankDRAM(D2CalibrationTable3, 0x01000L, (336/16+1)*(480/16+1)*8);
			fnD2MoveCodeToSingleBankDRAM(D2CalibrationTable4, 0x04000L, (336/16+1)*(480/16+1)*8);
			break;
		}
	}
}

void fnD2TableVerify()
{
	if (nD2TableVerified != nD2Mode)
	{
		switch (nD2Mode)
		{
		case D2_OPERATION_ACCURATE_STITCHING:
			fnD2VerifyCodeToSingleBankDRAM(D2CalibrationTable1, 0x01000L, (336/16+1)*(240/16+1)*8);
			fnD2VerifyCodeToSingleBankDRAM(D2CalibrationTable2, 0x04000L, (336/16+1)*(240/16+1)*8);
			nD2TableVerified = D2_OPERATION_ACCURATE_STITCHING;
			break;
		case D2_OPERATION_SCALED_STITCHING:
			fnD2VerifyCodeToSingleBankDRAM(D2CalibrationTable3, 0x01000L, (336/16+1)*(480/16+1)*8);
			fnD2VerifyCodeToSingleBankDRAM(D2CalibrationTable4, 0x04000L, (336/16+1)*(480/16+1)*8);
			nD2TableVerified = D2_OPERATION_SCALED_STITCHING;
			break;
		}
	}
}

unsigned char fnD2RunAccurateStitching(
	unsigned char ucResolution)
{
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2TableLoad(D2_OPERATION_ACCURATE_STITCHING);
		fnD2WriteTable(D2TableSOU_60HzDualQVGA, sizeof(D2TableSOU_60HzDualQVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableAccurateStitch_QVGA, sizeof(D2TableAccurateStitch_QVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_QVGA, sizeof(D2TableSIU_QVGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;

	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2TableLoad(D2_OPERATION_ACCURATE_STITCHING);
		fnD2WriteTable(D2TableSOU_50HzDualQVGA, sizeof(D2TableSOU_50HzDualQVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableAccurateStitch_QVGA, sizeof(D2TableAccurateStitch_QVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_QVGA, sizeof(D2TableSIU_QVGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;

	default:
		return FAILURE;
	}
}

unsigned char fnD2RunScaledStitching(
	unsigned char ucResolution)
{
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2TableLoad(D2_OPERATION_SCALED_STITCHING);
		fnD2WriteTable(D2TableSOU_60HzVGA, sizeof(D2TableSOU_60HzVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableScaledStitch_QVGA, sizeof(D2TableScaledStitch_QVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_QVGA, sizeof(D2TableSIU_QVGA) / sizeof(tD2Register));
		ucSIU_CONTROL = 0x0F;
		return SUCCESS;

	case D2_RESOLUTION_VGA_50Hz:
		D2WriteOutputClock(SOU_VGACLK); 	// Set Host Clock in MHz
		fnD2WriteTable(D2TableBuffers, sizeof(D2TableBuffers) / sizeof(tD2Register));
		fnD2TableLoad(D2_OPERATION_SCALED_STITCHING);
		fnD2WriteTable(D2TableSOU_50HzVGA, sizeof(D2TableSOU_50HzVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableScaledStitch_QVGA, sizeof(D2TableScaledStitch_QVGA) / sizeof(tD2Register));
		fnD2WriteTable(D2TableSIU_QVGA, sizeof(D2TableSIU_QVGA) / sizeof(tD2Register));
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
		if (fnDoInitSensor(ucResolution+D2_RESOLUTION_QVGA_OFFSET) == FAILURE)
			return FAILURE;
		return fnD2RunFullSideBySide(ucResolution);

	case D2_OPERATION_HALF_SIDE_BY_SIDE:
		if (fnDoInitSensor(ucResolution) == FAILURE)
			return FAILURE;
		return fnD2RunHalfSideBySide(ucResolution);
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE

#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
	case D2_OPERATION_VERTICAL_SIDE_BY_SIDE:
		if (fnDoInitSensor(ucResolution+D2_RESOLUTION_QVGA_OFFSET) == FAILURE)
			return FAILURE;
		return fnD2RunVerticalSideBySide(ucResolution);
#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE

#ifdef SUPPORT_FUZZY_STITCHING
	case D2_OPERATION_FUZZY_STITCHING:
		return FAILURE;

	case D2_OPERATION_ACCURATE_STITCHING:
		if (fnDoInitSensor(ucResolution+D2_RESOLUTION_QVGA_OFFSET) == FAILURE)
			return FAILURE;
		return fnD2RunAccurateStitching(ucResolution);

	case D2_OPERATION_SCALED_STITCHING:
		if (fnDoInitSensor(ucResolution+D2_RESOLUTION_QVGA_OFFSET) == FAILURE)
			return FAILURE;
		return fnD2RunScaledStitching(ucResolution);
#endif // SUPPORT_FUZZY_STITCHING

	// for AWB calibration, by XYZ, 2014.01.22 - begin
	case D2_OPERATION_3A_CALIBRATION:
		return opmode_calibration(ucResolution);
	// for AWB calibration, by XYZ, 2014.01.22 - end
	}
	return FAILURE;
}

#endif // SUPPORT_D2_OPERATION
