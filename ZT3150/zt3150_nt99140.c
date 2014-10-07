//
// Zealtek D2 project
// ZT3150@NT99140 solutions
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// Sep 11, 2012.

#include "system.h"
#include "d2.h"
#include "gendef.h"
#include "sensor0.h"
#include "sensor1.h"

#ifdef SUPPORT_D2_OPERATION

	// SUPPORT_3A_SYNC allows D2 statemachine to copy the AEC/AGC/AWB results from sensor source 0,
	// and copies to sensor source 1.
	//
	// AEC/AGC/AWB at sensor 0 remains as sensor initial setting.
	// AEC/AGC/AWB at sensor 1 is turned off when the feature is enabled.
	//
	#define TRIGGER_DISABLED      0
	#define TRIGGER_ON_SN0_VSYNC  1 // trigger on seeing VSYNC from SN0
	#define TRIGGER_ON_SN1_VSYNC  2 // trigger on seeing VSYNC from SN1
	#define TRIGGER_ON_COUNTER    3 // trigger on counter overflow
	#define TRIGGER_ANYTIME       4 // trigger without checking VSYNC
	#define SUPPORT_3A_SYNC       TRIGGER_ON_COUNTER
	//#define SUPPORT_3A_SYNC       TRIGGER_DISABLED

	//#define SENSOR_3A_SYNC_SOURCE_SN0
	#ifdef SENSOR_3A_SYNC_SOURCE_SN0
		#define CLOCK_SYNC_VALUE_ON  0x03 // 0x03 // 0x07
		#define CLOCK_SYNC_VALUE_OFF 0x00
	#else  // SENSOR_3A_SYNC_SOURCE_SN0
		#define CLOCK_SYNC_VALUE_ON  0x01 // 0x01 // 0x05
		#define CLOCK_SYNC_VALUE_OFF 0x00
	#endif // SENSOR_3A_SYNC_SOURCE_SN0

	// SENSOR_SYNC_FRAMES allows D2 to synchronize the VSYNC signals from both video sources.
	// One of the clock to video sensors holds a period of time to make VSYNC synchronization.
	//
	// SENSOR_SYNC_ALWAYS does not stops VSYNC synchronization. If this option is disabled,
	// VSYNC synchronization stops in a pre-defined period after the VSYNC synchronization starts.
	//
	//#define SENSOR_SYNC_FRAMES
	//#define SENSOR_SYNC_ALWAYS
	//
	// Programming 0x02 to register 0x3060 updates the following NT99140 registers:
	//  Integration (exposure) time, Analog and Digital gains, Red and Blue gains

	#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	enum
	{
		SENSOR_3A_SYNC_STOP,
		SENSOR_3A_SYNC_START,
		#ifdef SENSOR_SYNC_FRAMES
		SENSOR_3A_SYNC_CLOCK,
		#endif // SENSOR_SYNC_FRAMES
		SENSOR_3A_SYNC_CONFIG,
		SENSOR_3A_SYNC_AEC,
		SENSOR_3A_SYNC_AWB,
	};

	unsigned char ucSensor3ASync;
	#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)

	#define CLK_SENSOR  24
	#if (CLK_PLL / CLK_SENSOR >= 16)
		#error Maximum Clock Divider for CLK_SENSOR is 16.
	#endif // (CLK_PLL / CLK_SENSOR >= 16)

	#define SENSOR_ONCHIP_LDO
	#define DRIVING_HIGH


	// NT99140 PLL settings:
	//
	// PLL_CLK = M_CLK * (PLL_M + 1) / (2 * (PLL_N + 1) * (PLL_P + 1))
	//         = M_CLK * (0x3028[5:0] + 1) / (2 * (0x3029[5:4] + 1) * (0x3029[2:0] + 1))
	// P_CLK   = PLL_CLK / CLK_DIV
	//         = PLL_CLK / (2 ^ 0x302A[3:2])
	//
	// HD Mode Settings
	//#define MCLK_24_PCLK_74_30_FPS
	#define MCLK_24_PCLK_60_24_FPS

	// VGA Mode Settings
	//#define MCLK_24_PCLK_XX_30_FPS
	#define MCLK_24_PCLK_60_30_FPS

tI2CRegister code I2CInitTableCommon[] =
{
	#ifdef SENSOR_ONCHIP_LDO
	{0x3109, 0x84}, // Turn on NT99140 1.5V LDO
	#else  // SENSOR_ONCHIP_LDO
	{0x3109, 0x04}, // Turn off NT99140 1.5V LDO
	#endif // SENSOR_ONCHIP_LDO

	// PLL settings
	{0x3028, 0x05}, // PLL
	{0x3029, 0x02},
	{0x302A, 0x00},

	{0x32F0, 0x00}, // CbYCrY
	{0x3336, 0x14},
	// Lens Shadding Correction
	{0x3210, 0x0F},
	{0x3211, 0x0F},
	{0x3212, 0x10},
	{0x3213, 0x0F},
	{0x3214, 0x0E},
	{0x3215, 0x0E},
	{0x3216, 0x0E},
	{0x3217, 0x0E},
	{0x3218, 0x0E},
	{0x3219, 0x0E},
	{0x321A, 0x0E},
	{0x321B, 0x0E},
	{0x321C, 0x0E},
	{0x321D, 0x0E},
	{0x321E, 0x0E},
	{0x321F, 0x0E},
	{0x3220, 0x01},
	{0x3221, 0x36},
	{0x3222, 0x01},
	{0x3223, 0x32},
	{0x3224, 0x01},
	{0x3225, 0x32},
	{0x3226, 0x01},
	{0x3227, 0x2F},
	{0x3228, 0x00},
	{0x3229, 0xAF},
	{0x322A, 0x00},
	{0x322B, 0xAF},
	{0x322C, 0x00},
	{0x322D, 0xAF},
	{0x322E, 0x00},
	{0x322F, 0xAF},
	{0x3230, 0x1D},
	{0x3231, 0x00},
	{0x3232, 0x04},
	{0x3233, 0x30},
	{0x3234, 0x43},
	// Gamma
	{0x3270, 0x00},
	{0x3271, 0x0B},
	{0x3272, 0x16},
	{0x3273, 0x2B},
	{0x3274, 0x3F},
	{0x3275, 0x51},
	{0x3276, 0x72},
	{0x3277, 0x8F},
	{0x3278, 0xA7},
	{0x3279, 0xBC},
	{0x327A, 0xDC},
	{0x327B, 0xF0},
	{0x327C, 0xFA},
	{0x327D, 0xFE},
	{0x327E, 0xFF},
	// Color Correction and Transform Matrix
	{0x3302, 0x00},
	{0x3303, 0x3C},
	{0x3304, 0x00},
	{0x3305, 0xA6},
	{0x3306, 0x00},
	{0x3307, 0x1C},
	{0x3308, 0x07},
	{0x3309, 0xE1},
	{0x330A, 0x06},
	{0x330B, 0x9B},
	{0x330C, 0x01},
	{0x330D, 0x84},
	{0x330E, 0x00},
	{0x330F, 0xEF},
	{0x3310, 0x07},
	{0x3311, 0x69},
	{0x3312, 0x07},
	{0x3313, 0xA9},

	{0x32DB, 0x90},
	// Edge Enhancement and Noise Reduction
	{0x3300, 0x70},
	{0x3301, 0x40},
	// Read Mode 2:
	{0x3024, 0x00}, // PCLK, VSYNC, HSYNC
	// Calibration Control / ABLC auto black level correction
	{0x3040, 0x04},
	{0x3041, 0x02},
	{0x3042, 0xFF},
	{0x3043, 0x14},
	{0x3052, 0xD0},
	{0x3057, 0x80},
	{0x3058, 0x00},
	{0x3059, 0x2F},
	{0x305f, 0x22},
	// Luminance Accumulation Mode
	{0x32B0, 0x00},
	{0x32B1, 0x90},
	{0x32BB, 0x0B},
	{0x32BD, 0x10},
	{0x32BE, 0x05},
	{0x32BF, 0x4A},
	{0x32C0, 0x40},
	{0x32C3, 0x08},
	{0x32C5, 0x1F},
	{0x32CD, 0x01},
	{0x32D3, 0x00},
	{0x32F6, 0x0C}, // Y / CbCr adjustment enable
	{0x3324, 0x00},
	{0x3118, 0xF2},
	{0x3119, 0xF2},
	{0x311A, 0x13},
	{0x3106, 0x01},
	{0x3108, 0x55},
	{0x3105, 0x41},
	{0x3112, 0x21},
	{0x3113, 0x55},
	{0x3114, 0x05},
	{0x3343, 0xA0},
	{0x333b, 0x20},
	{0x333c, 0x28},
	// Modules Enable Controls
	{0x3200, 0x3e}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	{0x3344, 0x28}, // ?
	{0x3345, 0x30}, // ?
	{0x3346, 0x30}, // ?
	{0x3348, 0x00}, // ?
	{0x3349, 0x40}, // ?
	{0x334A, 0x30}, // ?
	{0x334B, 0x00}, // ?
	{0x334D, 0x15}, // ?
	// Driving ability
	//{0x3068, 0x01}, // 0:2mA, 1:4mA, 2:6mA, 3:8mA for I2C SCL and SDA
	#ifdef DRIVING_HIGH
	// marked following lines to lower driving current from NT99140 - by Wythe at Sep 18, 2012.
	{0x3069, 0x01}, // 0:2mA, 1:4mA, 2:6mA, 3:8mA for HSYNC, VSYNC, and D0 - D9
	{0x306A, 0x01}, // 0:2mA, 1:4mA, 2:6mA, 3:8mA for PCLK
	#endif // DRIVING_HIGH
};

tI2CRegister code I2CInitTable50HzHD[] =
{
	#ifdef MCLK_24_PCLK_74_30_FPS
	// I use the 60HzHD here.
	// Need to check NTK PM for proper setting later.
	//
	//{0x3012, 0x00}, // Default Integration Time
	//{0x3013, 0xff},
	{0x32e0, 0x05}, //  Scaler_Out_Size_X //
	{0x32e1, 0x00},
	{0x32e2, 0x02}, //  Scaler_Out_Size_Y //
	{0x32e3, 0xd0},
	// PLL settings
	{0x3028, 0x24}, // PLL_CLK = 24 * (40+1) / (2 * (2+1) * (0+1)) = 148
	{0x3029, 0x20}, //
	{0x302A, 0x04}, // P_CLK = 148 / (2^1) = 74
	{0x3022, 0x24}, // read mode 0
	{0x3023, 0x24}, // read mode 1
	// Sensor Array Active Region
	{0x3002, 0x00}, // x start
	{0x3003, 0x04},
	{0x3004, 0x00}, // y start
	{0x3005, 0x04},
	{0x3006, 0x05}, // x end  // 0x503=1283
	{0x3007, 0x03},
	{0x3008, 0x02}, // y end  // 0x2d3=723
	{0x3009, 0xd3},
	{0x300a, 0x06}, // line length // 0x683=1667
	{0x300b, 0x83},
	{0x300c, 0x02}, // frame length // 0x2E3=739
	{0x300d, 0xE3},
	{0x300e, 0x05}, // x output size // 0x500=1280
	{0x300f, 0x00},
	{0x3010, 0x02}, // y output size // 0x2d0=720
	{0x3011, 0xd0},
	// soc auto exposure
	{0x32b0, 0x00},
	{0x32b1, 0x00},
	{0x32b2, 0x01},
	{0x32b3, 0x7c},
	{0x32b4, 0x00},
	{0x32b5, 0x64},
	{0x32b6, 0x99},
	{0x32bb, 0x13},
	{0x32bc, 0x40},
	{0x32c1, 0x22},
	{0x32c2, 0xe3},
	{0x32c8, 0x6f},
	{0x32c9, 0x5c},
	{0x32c4, 0x00},
	{0x3201, 0x3F},	// enable ae/awb
	//{0x3201, 0x1F},	// disable AE/ enable AWB
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x16}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_74_30_FPS
	#ifdef MCLK_24_PCLK_60_24_FPS
	//{0x3012, 0x00}, // Default Integration Time
	//{0x3013, 0xff},
	//{0x32e0, 0x05}, //  Scaler_Out_Size_X //
	//{0x32e1, 0x00},
	//{0x32e2, 0x02}, //  Scaler_Out_Size_Y //
	//{0x32e3, 0xd0},
	// PLL settings
	{0x3028, 0x09}, // PLL_CLK = 24 * (9+1) / (2 * (0+1) * (0+1)) = 120
	{0x3029, 0x00}, //
	{0x302A, 0x04}, // P_CLK = 120 / (2^1) = 60
	{0x3022, 0x24}, // read mode 0
	{0x3023, 0x24}, // read mode 1
	// Sensor Array Active Region
	{0x3002, 0x00}, // x start
	{0x3003, 0x04},
	{0x3004, 0x00}, // y start
	{0x3005, 0x04},
	{0x3006, 0x05}, // x end  // 0x503=1283
	{0x3007, 0x03},
	{0x3008, 0x02}, // y end  // 0x2D3=723
	{0x3009, 0xD3},
	{0x300a, 0x06}, // line length // 0x67C=1660
	{0x300b, 0x7C},
	{0x300c, 0x02}, // frame length // 0x2F1=753
	{0x300d, 0xF1},
	{0x300e, 0x05}, // x output size // 0x500=1280
	{0x300f, 0x00},
	{0x3010, 0x02}, // y output size // 0x2D0=720
	{0x3011, 0xD0},
	// soc auto exposure
	{0x32b0, 0x00},
	{0x32b1, 0x00},
	{0x32b2, 0x01},
	{0x32b3, 0x7C},
	{0x32b4, 0x00},
	{0x32b5, 0x64},
	{0x32b6, 0x99},
	{0x32bb, 0x1B},
	{0x32bc, 0x40},
	{0x32c1, 0x29},
	{0x32c2, 0x69},
	{0x32c8, 0x5A},
	{0x32c9, 0x4B},
	{0x32c4, 0x00},
	{0x3201, 0x3F},	// enable ae/awb
	//{0x3201, 0x1F},	// disable AE/ enable AWB
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x16}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_60_24_FPS
};

tI2CRegister code I2CInitTable60HzHD[] =
{
	#ifdef MCLK_24_PCLK_74_30_FPS
	//{0x3012, 0x00}, // Default Integration Time
	//{0x3013, 0xff},
	{0x32e0, 0x05}, //  Scaler_Out_Size_X //
	{0x32e1, 0x00},
	{0x32e2, 0x02}, //  Scaler_Out_Size_Y //
	{0x32e3, 0xd0},
	// PLL settings
	{0x3028, 0x24}, // PLL_CLK = 24 * (40+1) / (2 * (2+1) * (0+1)) = 148
	{0x3029, 0x20}, //
	{0x302A, 0x04}, // P_CLK = 148 / (2^1) = 74
	{0x3022, 0x24}, // read mode 0
	{0x3023, 0x24}, // read mode 1
	// Sensor Array Active Region
	{0x3002, 0x00}, // x start
	{0x3003, 0x04},
	{0x3004, 0x00}, // y start
	{0x3005, 0x04},
	{0x3006, 0x05}, // x end  // 0x503=1283
	{0x3007, 0x03},
	{0x3008, 0x02}, // y end  // 0x2d3=723
	{0x3009, 0xd3},
	{0x300a, 0x06}, // line length // 0x683=1667
	{0x300b, 0x83},
	{0x300c, 0x02}, // frame length // 0x2E3=739
	{0x300d, 0xE3},
	{0x300e, 0x05}, // x output size // 0x500=1280
	{0x300f, 0x00},
	{0x3010, 0x02}, // y output size // 0x2d0=720
	{0x3011, 0xd0},
	// soc auto exposure
	{0x32b0, 0x00},
	{0x32b1, 0x00},
	{0x32b2, 0x01},
	{0x32b3, 0x7c},
	{0x32b4, 0x00},
	{0x32b5, 0x64},
	{0x32b6, 0x99},
	{0x32bb, 0x13},
	{0x32bc, 0x40},
	{0x32c1, 0x22},
	{0x32c2, 0xe3},
	{0x32c8, 0x6f},
	{0x32c9, 0x5c},
	{0x32c4, 0x00},
	{0x3201, 0x3F},	// enable ae/awb
	//{0x3201, 0x1F},	// disable AE/ enable AWB
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x16}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_74_30_FPS
	#ifdef MCLK_24_PCLK_60_24_FPS
	//{0x3012, 0x00}, // Default Integration Time
	//{0x3013, 0xff},
	//{0x32e0, 0x05}, //  Scaler_Out_Size_X //
	//{0x32e1, 0x00},
	//{0x32e2, 0x02}, //  Scaler_Out_Size_Y //
	//{0x32e3, 0xd0},
	// PLL settings
	{0x3028, 0x09}, // PLL_CLK = 24 * (9+1) / (2 * (0+1) * (0+1)) = 120
	{0x3029, 0x00}, //
	{0x302A, 0x04}, // P_CLK = 120 / (2^1) = 60
	{0x3022, 0x24}, // read mode 0
	{0x3023, 0x24}, // read mode 1
	// Sensor Array Active Region
	{0x3002, 0x00}, // x start
	{0x3003, 0x04},
	{0x3004, 0x00}, // y start
	{0x3005, 0x04},
	{0x3006, 0x05}, // x end  // 0x503=1283
	{0x3007, 0x03},
	{0x3008, 0x02}, // y end  // 0x2D3=723
	{0x3009, 0xD3},
	{0x300a, 0x06}, // line length // 0x67C=1660
	{0x300b, 0x7C},
	{0x300c, 0x02}, // frame length // 0x2F1=753
	{0x300d, 0xF1},
	{0x300e, 0x05}, // x output size // 0x500=1280
	{0x300f, 0x00},
	{0x3010, 0x02}, // y output size // 0x2D0=720
	{0x3011, 0xD0},
	// soc auto exposure
	{0x32b0, 0x00},
	{0x32b1, 0x00},
	{0x32b2, 0x01},
	{0x32b3, 0x7C},
	{0x32b4, 0x00},
	{0x32b5, 0x64},
	{0x32b6, 0x99},
	{0x32bb, 0x13},
	{0x32bc, 0x40},
	{0x32c1, 0x29},
	{0x32c2, 0x69},
	{0x32c8, 0x5A},
	{0x32c9, 0x4B},
	{0x32c4, 0x00},
	{0x3201, 0x3F},	// enable ae/awb
	//{0x3201, 0x1F},	// disable AE/ enable AWB
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x16}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_60_24_FPS
};

tI2CRegister code I2CInitTable50HzVGA[] =
{
	#ifdef MCLK_24_PCLK_XX_30_FPS
	// I use the 60HzVGA here.
	// Need to check NTK PM for proper setting later.
	//
	//{0x3012, 0x01}, // Default Integration Time
	//{0x3013, 0x75},
	//{0x301d, 0x30},
	{0x32e0, 0x02}, // Scaler_Out_Size_X // 0x280 = 640
	{0x32e1, 0x80},
	{0x32e2, 0x01}, // Scaler_Out_Size_Y // 0x1e0 = 480
	{0x32e3, 0xe0},
	{0x32e4, 0x00}, // Scaling Factor
	{0x32e5, 0x80},
	{0x32e6, 0x00},
	{0x32e7, 0x80},
	// PLL settings
	{0x302A, 0x00}, // PLL setting
	{0x3028, 0x09}, // PLL
	{0x3029, 0x00},
	{0x302A, 0x04}, // PLL run // 0x04, 0x08
	{0x3022, 0x24}, // read mode 0
	{0x3023, 0x24}, // read mode 1
	// Sensor Array Active Region
	{0x3002, 0x00}, // x start
	{0x3003, 0xa4},
	{0x3004, 0x00}, // y start
	{0x3005, 0x04},
	{0x3006, 0x04}, // x end	// 0x463 = 1123
	{0x3007, 0x63},
	{0x3008, 0x02}, // y end	// 0x2d3 = 723
	{0x3009, 0xd3},
	{0x300a, 0x05}, // line length // 0x53c = 1340
	{0x300b, 0x3c},
	{0x300c, 0x02}, // frame length // 0x2e0 = 736
	{0x300d, 0xe0},
	{0x300e, 0x03}, // x output size // 0x3c0 = 960
	{0x300f, 0xc0},
	{0x3010, 0x02}, // y output size // 0x2d0 = 720
	{0x3011, 0xd0},
	// soc auto exposure
	{0x32b0, 0x00}, // Statistics_Mode
	{0x32b1, 0x00}, // LA_Win_Wgt
	{0x32b2, 0x00}, // LA_X0
	{0x32b3, 0xdc},
	{0x32b4, 0x00}, // LA_Y0
	{0x32b5, 0x64},
	{0x32b6, 0x99}, // LA_Win_Size
	{0x32bb, 0x13}, // AE_Control_0
	{0x32bc, 0x40}, // AE targe lum
	{0x32c1, 0x2c},
	{0x32c2, 0x8c},
	{0x32c8, 0x70},
	{0x32c9, 0x5d},
	{0x32c4, 0x00},
	{0x3201, 0x7f}, // enable AE/AWB
	//{0x3201, 0x5F}, // disable AE
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_XX_30_FPS
	#ifdef MCLK_24_PCLK_60_30_FPS
	//{0x3013, 0x75},
	//{0x301d, 0x30},
	// Output Resolution
	{0x32E0, 0x02}, // Scaler_Out_Size_X // 0x280 = 640
	{0x32E1, 0x80},
	{0x32E2, 0x01}, // Scaler_Out_Size_Y // 0x1e0 = 480
	{0x32E3, 0xE0},
	{0x32E4, 0x00}, // Scaling Factor
	{0x32E5, 0x80},
	{0x32E6, 0x00},
	{0x32E7, 0x80},
	// PLL settings
	{0x3028, 0x09}, // PLL_CLK = 24 * (9+1) / (2 * (0+1) * (0+1)) = 120
	{0x3029, 0x00}, //
	{0x302A, 0x04}, // P_CLK = 120 / (2^1) = 60
	{0x3022, 0x24}, // read mode 0
	{0x3023, 0x24}, // read mode 1
	// Sensor Array Active Region
	{0x3002, 0x00}, // x start
	{0x3003, 0xA4},
	{0x3004, 0x00}, // y start
	{0x3005, 0x04},
	{0x3006, 0x04}, // x end	// 0x463 = 1123
	{0x3007, 0x63},
	{0x3008, 0x02}, // y end	// 0x2D3 = 723
	{0x3009, 0xD3},
	{0x300a, 0x05}, // line length // 0x53C = 1340
	{0x300b, 0x3C},
	{0x300c, 0x02}, // frame length // 0x2EA = 746
	{0x300d, 0xEA},
	{0x300e, 0x03}, // x output size // 0x3C0 = 960
	{0x300f, 0xC0},
	{0x3010, 0x02}, // y output size // 0x2D0 = 720
	{0x3011, 0xD0},
	// soc auto exposure
	{0x32b0, 0x00},
	{0x32b1, 0x00},
	{0x32b2, 0x00},
	{0x32b3, 0xdc},
	{0x32b4, 0x00},
	{0x32b5, 0x64},
	{0x32b6, 0x99},
	{0x32bb, 0x1b},
	{0x32bc, 0x40},
	{0x32c1, 0x2b},
	{0x32c2, 0xa9},
	{0x32c8, 0x70},
	{0x32c9, 0x5d},
	{0x32c4, 0x00},
	{0x3201, 0x7F}, // enable Scaler/AE/AWB
	//{0x3201, 0x5F}, // disable AE
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_60_30_FPS
};

tI2CRegister code I2CInitTable60HzVGA[] =
{
	#ifdef MCLK_24_PCLK_XX_30_FPS
	//{0x3012, 0x01}, // Default Integration Time
	//{0x3013, 0x75},
	//{0x301d, 0x30},
	{0x32e0, 0x02}, // Scaler_Out_Size_X // 0x280 = 640
	{0x32e1, 0x80},
	{0x32e2, 0x01}, // Scaler_Out_Size_Y // 0x1e0 = 480
	{0x32e3, 0xe0},
	{0x32e4, 0x00}, // Scaling Factor
	{0x32e5, 0x80},
	{0x32e6, 0x00},
	{0x32e7, 0x80},
	// PLL settings
	{0x3028, 0x09}, // PLL
	{0x3029, 0x00},
	{0x302A, 0x04}, // PLL run // 0x04, 0x08
	{0x3022, 0x24}, // read mode 0
	{0x3023, 0x24}, // read mode 1
	// Sensor Array Active Region
	{0x3002, 0x00}, // x start
	{0x3003, 0xa4},
	{0x3004, 0x00}, // y start
	{0x3005, 0x04},
	{0x3006, 0x04}, // x end	// 0x463 = 1123
	{0x3007, 0x63},
	{0x3008, 0x02}, // y end	// 0x2d3 = 723
	{0x3009, 0xd3},
	{0x300a, 0x05}, // line length // 0x53c = 1340
	{0x300b, 0x3c},
	{0x300c, 0x02}, // frame length // 0x2e0 = 736
	{0x300d, 0xe0},
	{0x300e, 0x03}, // x output size // 0x3c0 = 960
	{0x300f, 0xc0},
	{0x3010, 0x02}, // y output size // 0x2d0 = 720
	{0x3011, 0xd0},
	// soc auto exposure
	{0x32b0, 0x00}, // Statistics_Mode
	{0x32b1, 0x00}, // LA_Win_Wgt
	{0x32b2, 0x00}, // LA_X0
	{0x32b3, 0xdc},
	{0x32b4, 0x00}, // LA_Y0
	{0x32b5, 0x64},
	{0x32b6, 0x99}, // LA_Win_Size
	{0x32bb, 0x13}, // AE_Control_0
	{0x32bc, 0x40}, // AE targe lum
	{0x32c1, 0x2c},
	{0x32c2, 0x8c},
	{0x32c8, 0x70},
	{0x32c9, 0x5d},
	{0x32c4, 0x00},
	{0x3201, 0x7f}, // enable AE/AWB
	//{0x3201, 0x5F}, // disable AE
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_XX_30_FPS
	#ifdef MCLK_24_PCLK_60_30_FPS
	// Output Resolution
	{0x32E0, 0x02}, // Scaler_Out_Size_X // 0x280 = 640
	{0x32E1, 0x80},
	{0x32E2, 0x01}, // Scaler_Out_Size_Y // 0x1e0 = 480
	{0x32E3, 0xE0},
	{0x32E4, 0x00}, // Scaling Factor
	{0x32E5, 0x80},
	{0x32E6, 0x00},
	{0x32E7, 0x80},
	// PLL settings
	{0x3028, 0x09}, // PLL_CLK = 24 * (9+1) / (2 * (0+1) * (0+1)) = 120
	{0x3029, 0x00}, //
	{0x302A, 0x04}, // P_CLK = 120 / (2^1) = 60
	{0x3022, 0x24}, // read mode 0
	{0x3023, 0x24}, // read mode 1
	// Sensor Array Active Region
	{0x3002, 0x00}, // x start
	{0x3003, 0xA4},
	{0x3004, 0x00}, // y start
	{0x3005, 0x04},
	{0x3006, 0x04}, // x end	// 0x463 = 1123
	{0x3007, 0x63},
	{0x3008, 0x02}, // y end	// 0x2D3 = 723
	{0x3009, 0xD3},
	{0x300a, 0x05}, // line length // 0x53C = 1340
	{0x300b, 0x3C},
	{0x300c, 0x02}, // frame length // 0x2EA = 746
	{0x300d, 0xEA},
	{0x300e, 0x03}, // x output size // 0x3C0 = 960
	{0x300f, 0xC0},
	{0x3010, 0x02}, // y output size // 0x2D0 = 720
	{0x3011, 0xD0},
	// soc auto exposure
	{0x32b0, 0x00},
	{0x32b1, 0x00},
	{0x32b2, 0x00},
	{0x32b3, 0xdc},
	{0x32b4, 0x00},
	{0x32b5, 0x64},
	{0x32b6, 0x99},
	{0x32bb, 0x13},
	{0x32bc, 0x40},
	{0x32c1, 0x2b},
	{0x32c2, 0xa9},
	{0x32c8, 0x70},
	{0x32c9, 0x5d},
	{0x32c4, 0x00},
	{0x3201, 0x7F}, // enable Scaler/AE/AWB
	//{0x3201, 0x5F}, // disable AE
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_60_30_FPS
};


#ifdef SENSOR_SYNC_FRAMES
unsigned short ucVsyncCount;
#endif // SENSOR_SYNC_FRAMES

unsigned char fnDoInitSensor(
	unsigned char ucResolution)
{
#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	ucSensor3ASync = SENSOR_3A_SYNC_STOP;
#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	{
		fnD2SetPIO0(0); // Set pin RESET low
		fnD2SetPIO1(0); // Set pin RESET low
		fnD2IdleDelay(100); // count 100 around 12ms
		D2DisableSensorClock();
		fnD2IdleDelay(100); // count 100 around 12ms
		D2WriteSensorClock(CLK_SENSOR);  // Set Sensor Clock in MHz
		fnD2IdleDelay(100); // count 100 around 12ms
		fnD2SetPIO0(1); // Set pin RESET high
		fnD2SetPIO1(1); // Set pin RESET high
		fnD2IdleDelay(100); // count 100 around 12ms
		fnI2cWriteTable1(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister));
		fnI2cWriteTable0(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister));
	}
	ucResetControl = 0x74;      // 40:SIU Reset, 20:IPU Reset, 10:SOU Reset, 04: DRAM Reset
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
		fnI2cWriteTable0(I2CInitTable60HzVGA, sizeof(I2CInitTable60HzVGA) / sizeof(tI2CRegister));
		fnI2cWriteTable1(I2CInitTable60HzVGA, sizeof(I2CInitTable60HzVGA) / sizeof(tI2CRegister));
		break;
	case D2_RESOLUTION_VGA_50Hz:
		fnI2cWriteTable0(I2CInitTable50HzVGA, sizeof(I2CInitTable50HzVGA) / sizeof(tI2CRegister));
		fnI2cWriteTable1(I2CInitTable50HzVGA, sizeof(I2CInitTable50HzVGA) / sizeof(tI2CRegister));
		break;
	case D2_RESOLUTION_HD_60Hz:
		fnI2cWriteTable0(I2CInitTable60HzHD, sizeof(I2CInitTable60HzHD) / sizeof(tI2CRegister));
		fnI2cWriteTable1(I2CInitTable60HzHD, sizeof(I2CInitTable60HzHD) / sizeof(tI2CRegister));
		break;
	case D2_RESOLUTION_HD_50Hz:
		fnI2cWriteTable0(I2CInitTable50HzHD, sizeof(I2CInitTable50HzHD) / sizeof(tI2CRegister));
		fnI2cWriteTable1(I2CInitTable50HzHD, sizeof(I2CInitTable50HzHD) / sizeof(tI2CRegister));
		break;
	default:
		ucResetControl = 0x00;
		return FAILURE;
	}
	ucResetControl = 0x00;
#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	#ifdef SENSOR_SYNC_FRAMES
	ucSEN_CLK_SYNC = CLOCK_SYNC_VALUE_OFF;
	ucVsyncCount   = 100;
	#endif // SENSOR_SYNC_FRAMES
	ucSensor3ASync = SENSOR_3A_SYNC_START;
#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	return SUCCESS;
}


#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)

void fnSensorAEAWBConfig()
{
	unsigned char SensorReg1;

	#ifdef SENSOR_3A_SYNC_SOURCE_SN0
	// Disable SENSOR1 3A
	I2CREAD_SN1_REGISTER(0x3201, SensorReg1);
	SensorReg1 &= 0xCF;
	I2CWRITE_SN1_REGISTER(0x3201, SensorReg1);
	#else  // SENSOR_3A_SYNC_SOURCE_SN0
	// Disable SENSOR0 3A
	I2CREAD_SN0_REGISTER(0x3201, SensorReg1);
	SensorReg1 &= 0xCF;
	I2CWRITE_SN0_REGISTER(0x3201, SensorReg1);
	#endif // SENSOR_3A_SYNC_SOURCE_SN0
}

void fnCopySensorExposure()
{
	unsigned char SensorReg1;
	unsigned char SensorReg2;
	unsigned char SensorReg3;
	unsigned char SensorReg4;

	#ifdef SENSOR_3A_SYNC_SOURCE_SN0
	// Read SENSOR0 Exposure Time, and Gain, then write to SENSOR1.
	I2CREAD_SN0_REGISTER(0x301C, SensorReg1);  //GAIN Digital
	I2CREAD_SN0_REGISTER(0x301D, SensorReg2);  //GAIN Analog
	I2CREAD_SN0_REGISTER(0x3012, SensorReg3);  //AECH
	I2CREAD_SN0_REGISTER(0x3013, SensorReg4);  //AECL
	I2CWRITE_SN1_REGISTER(0x301C, SensorReg1); //GAIN Digital
	I2CWRITE_SN1_REGISTER(0x301D, SensorReg2); //GAIN Analog
	I2CWRITE_SN1_REGISTER(0x3012, SensorReg3); //AECH
	I2CWRITE_SN1_REGISTER(0x3013, SensorReg4); //AECL
	I2CWRITE_SN1_REGISTER(0x3060, 0x02); //Update Register
	#else  // SENSOR_3A_SYNC_SOURCE_SN0
	// Read SENSOR1 Exposure Time, and Gain, then write to SENSOR0.
	I2CREAD_SN1_REGISTER(0x301C, SensorReg1);  //GAIN Digital
	I2CREAD_SN1_REGISTER(0x301D, SensorReg2);  //GAIN Analog
	I2CREAD_SN1_REGISTER(0x3012, SensorReg3);  //AECH
	I2CREAD_SN1_REGISTER(0x3013, SensorReg4);  //AECL
	I2CWRITE_SN0_REGISTER(0x301C, SensorReg1); //GAIN Digital
	I2CWRITE_SN0_REGISTER(0x301D, SensorReg2); //GAIN Analog
	I2CWRITE_SN0_REGISTER(0x3012, SensorReg3); //AECH
	I2CWRITE_SN0_REGISTER(0x3013, SensorReg4); //AECL
	I2CWRITE_SN0_REGISTER(0x3060, 0x02); //Update Register
	#endif // SENSOR_3A_SYNC_SOURCE_SN0
}

	#ifndef SENSOR_SYNC_AEC_ONLY
void fnCopySensorWhiteBalance()
{
	unsigned char SensorReg1;
	unsigned char SensorReg2;
	unsigned char SensorReg3;
	unsigned char SensorReg4;

	#ifdef SENSOR_3A_SYNC_SOURCE_SN0
	// Read SENSOR0 Whitebalance Gain, then write to SENSOR1.
	I2CREAD_SN0_REGISTER(0x3290, SensorReg1);  //AWB RED gain H
	I2CREAD_SN0_REGISTER(0x3291, SensorReg2);  //AWB RED gain L
	I2CREAD_SN0_REGISTER(0x3296, SensorReg3);  //AWB BLUE gain H
	I2CREAD_SN0_REGISTER(0x3297, SensorReg4);  //AWB BLUE gain L
	I2CWRITE_SN1_REGISTER(0x3290, SensorReg1); //AWB RED gain H
	I2CWRITE_SN1_REGISTER(0x3291, SensorReg2); //AWB RED gain L
	I2CWRITE_SN1_REGISTER(0x3296, SensorReg3); //AWB BLUE gain H
	I2CWRITE_SN1_REGISTER(0x3297, SensorReg4); //AWB BLUE gain L
	I2CWRITE_SN1_REGISTER(0x3060, 0x02); //Update Register
	#else  // SENSOR_3A_SYNC_SOURCE_SN0
	// Read SENSOR1 Whitebalance Gain, then write to SENSOR0.
	I2CREAD_SN1_REGISTER(0x3290, SensorReg1);  //AWB RED gain H
	I2CREAD_SN1_REGISTER(0x3291, SensorReg2);  //AWB RED gain L
	I2CREAD_SN1_REGISTER(0x3296, SensorReg3);  //AWB BLUE gain H
	I2CREAD_SN1_REGISTER(0x3297, SensorReg4);  //AWB BLUE gain L
	I2CWRITE_SN0_REGISTER(0x3290, SensorReg1); //AWB RED gain H
	I2CWRITE_SN0_REGISTER(0x3291, SensorReg2); //AWB RED gain L
	I2CWRITE_SN0_REGISTER(0x3296, SensorReg3); //AWB BLUE gain H
	I2CWRITE_SN0_REGISTER(0x3297, SensorReg4); //AWB BLUE gain L
	I2CWRITE_SN0_REGISTER(0x3060, 0x02); //Update Register
	#endif // SENSOR_3A_SYNC_SOURCE_SN0
}
	#endif // SENSOR_SYNC_AEC_ONLY

#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)

void fnDoHouseKeeping()
{
#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	#if (SUPPORT_3A_SYNC == TRIGGER_ON_SN0_VSYNC)
	if ((ucSIU_CONTROL & 0x10) == 0x10) //SN0_VSYNC
		return;
	#endif // (SUPPORT_3A_SYNC == TRIGGER_ON_SN0_VSYNC)
	#if (SUPPORT_3A_SYNC == TRIGGER_ON_SN1_VSYNC)
	if ((ucSIU_CONTROL & 0x40) == 0x40) //SN1_VSYNC
		return;
	#endif // (SUPPORT_3A_SYNC == TRIGGER_ON_SN1_VSYNC)
	#if (SUPPORT_3A_SYNC == TRIGGER_ON_COUNTER)
	fnD2IdleDelay(250); // count 250 around 30ms
	#endif // (SUPPORT_3A_SYNC == TRIGGER_ON_COUNTER)

	#ifdef SENSOR_SYNC_FRAMES
	if (ucVsyncCount > 0)
	{
		ucVsyncCount--;
		return;
	}
	#endif // SENSOR_SYNC_FRAMES

	switch (ucSensor3ASync)
	{
	default:
	case SENSOR_3A_SYNC_STOP:
		break;
	case SENSOR_3A_SYNC_START:
		if ((nD2Mode == D2_OPERATION_BYPASS_SENSOR0) ||
				(nD2Mode == D2_OPERATION_BYPASS_SENSOR1) ||
				(nD2Mode == D2_OPERATION_IDLE))
		{
			ucSensor3ASync = SENSOR_3A_SYNC_STOP;
			break;
		}
	#ifdef SENSOR_SYNC_FRAMES
		ucSEN_CLK_SYNC = CLOCK_SYNC_VALUE_ON;
		ucSensor3ASync = SENSOR_3A_SYNC_CLOCK;
		ucVsyncCount   = 10;
	#else  // SENSOR_SYNC_FRAMES
		fnD2IdleDelay(25000); // count 25000 around 3000ms
		fnD2IdleDelay(25000); // count 25000 around 3000ms
		ucSensor3ASync = SENSOR_3A_SYNC_CONFIG;
	#endif // SENSOR_SYNC_FRAMES
		fnD2TableVerify();
		break;
	#ifdef SENSOR_SYNC_FRAMES
	case SENSOR_3A_SYNC_CLOCK:
		#ifndef SENSOR_SYNC_ALWAYS
		ucSEN_CLK_SYNC = CLOCK_SYNC_VALUE_OFF;
		fnD2IdleDelay(500); // count 500 around 60ms
		#endif // SENSOR_SYNC_ALWAYS
		ucSensor3ASync = SENSOR_3A_SYNC_CONFIG;
		break;
	#endif // SENSOR_SYNC_FRAMES
	case SENSOR_3A_SYNC_CONFIG:
		fnSensorAEAWBConfig();
		ucSensor3ASync = SENSOR_3A_SYNC_AEC;
		break;
	case SENSOR_3A_SYNC_AEC:
		fnCopySensorExposure();
		fnD2TableVerify();
		ucSensor3ASync = SENSOR_3A_SYNC_AWB;
		break;
	case SENSOR_3A_SYNC_AWB:
		fnCopySensorWhiteBalance();
		ucSensor3ASync = SENSOR_3A_SYNC_AEC;
		break;
	}
#else  // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	fnD2IdleDelay(250); // count 250 around 30ms
	if (nD2Mode == D2_OPERATION_ACCURATE_STITCHING)
	{
		fnD2TableVerify();
	}
#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
}

void fnDoInitialization()
{
#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	ucSensor3ASync = SENSOR_3A_SYNC_STOP;
#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)

	I2CWorkItem0.I2CConfiguration = SENSOR0_I2C_CONFIG;
	I2CWorkItem0.I2CSlaveAddress  = SENSOR0_I2C_SLAVE;
	I2CWorkItem1.I2CConfiguration = SENSOR1_I2C_CONFIG;
	I2CWorkItem1.I2CSlaveAddress  = SENSOR1_I2C_SLAVE;
}

#endif // SUPPORT_D2_OPERATION
