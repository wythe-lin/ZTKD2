//
// Zealtek D2 project
// ZT3150@NT99141 solutions
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

	// Compiler Options for Testing Codes requested by Wythe.
	#define CHECK_SDRAM_BUFFERS

	// SENSOR_DENOISE_COLOR enables the D2 Firmware to check the sensor gain from sensor,
	// and decreases the color noise from sensor on dark mode.
	// NTK K.H. @ 2013-03-06
	//
	// SENSOR_CHECK_DAY_NIGHT enables the D2 Firmware to check the luminance value from sensor,
	// and know if the sensor is running at day or night modes.
	// NTK K.H. @ 2012-12-26
	//
	// SENSOR_SYNC_FRAMES_NT99141 enables the VSYNC signals from both video sources.
	// NTK99141 hardware synchronization is used on synchronization.
	// NTK K.H. @ 2012-11-26
	//
	#define SENSOR_DENOISE_COLOR // Enabled on A6 project
	//#define SENSOR_CHECK_DAY_NIGHT
//	#define SENSOR_SYNC_FRAMES_NT99141
	#ifdef FPGA_VERIFY
		#undef SENSOR_SYNC_FRAMES_NT99141
	#endif // FPGA_VERIFY
	//
	// Programming 0x02 to register 0x3060 updates the following NT99141 registers:
	//  Integration (exposure) time, Analog and Digital gains

	#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	enum
	{
		SENSOR_3A_SYNC_STOP,
		SENSOR_3A_SYNC_START,
		SENSOR_3A_SYNC_CONFIG,
		SENSOR_3A_SYNC_AEC,
		SENSOR_3A_CHECK_MODE,
		SENSOR_3A_SYNC_AWB,
		SENSOR_3A_CALIBRATION,		// for AWB calibration, by XYZ, 2014.01.22
	};

	unsigned char ucSensor3ASync;
	#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)

	// for AWB calibration, by XYZ, 2014.01.22 - begain
	unsigned short code	cwr _at_ 0x3fd0;
	unsigned short code	cwb _at_ 0x3fd2;
	// for AWB calibration, by XYZ, 2014.01.22 - end

	#define CLK_SENSOR  24
	#if (CLK_PLL / CLK_SENSOR >= 16)
		#error Maximum Clock Divider for CLK_SENSOR is 16.
	#endif // (CLK_PLL / CLK_SENSOR >= 16)

	//#define SENSOR_ONCHIP_LDO
	#define DRIVING_ABILITY 1 // for customer target boards
	//#define DRIVING_ABILITY 2 // for NTK sensor daughter boards

	//#define SENSOR_CC 0
	//#define SENSOR_CC 1
	#define SENSOR_CC 2
	//#define SENSOR_AE_SPEED 0x07 // Slowest Setting
	#define SENSOR_AE_SPEED 0x17 // NTK Setting for MCLK_24_PCLK_72_24_FPS
	//#define SENSOR_AE_SPEED 0x37 // NTK Setting for MCLK_24_PCLK_72_24_FPS
	//#define SENSOR_AE_SPEED 0x37 // NTK Setting for MCLK_24_PCLK_80_24_FPS
	//#define SENSOR_AE_SPEED 0x87 // NTK Setting for MCLK_24_PCLK_80_24_FPS
	//#define SENSOR_AE_SPEED 0xF7 // Fastest Setting

	//#define SENSOR_AGC_GAIN 0x20 // Maximum AGC Gain
	//#define SENSOR_AGC_GAIN 0x28 // Maximum AGC Gain, by NTK K.H. 2013/01/09 6x
	//#define SENSOR_AGC_GAIN 0x2A // Maximum AGC Gain, by NTK K.H. 2012/12/03
	#define SENSOR_AGC_GAIN 0x2B // Maximum AGC Gain, by NTK K.H. 2013/01/31 7x
	//#define SENSOR_AGC_GAIN 0x30 // Maximum AGC Gain, by NTK K.H. 2012/12/20 8x

	// NT99141 PLL settings:
	//
	// PLL_CLK = M_CLK * (PLL_M + 1) / (2 * (PLL_N + 1) * (PLL_P + 1))
	//         = M_CLK * (0x3028[5:0] + 1) / (2 * (0x3029[5:4] + 1) * (0x3029[2:0] + 1))
	// P_CLK   = PLL_CLK / CLK_DIV
	//         = PLL_CLK / (2 ^ 0x302A[3:2])
	//
	// HD Mode Settings
	#define MCLK_24_PCLK_80_24_FPS
	//#define MCLK_24_PCLK_72_24_FPS
	//#define MCLK_24_PCLK_60_24_FPS
	#define SENSOR_CROP_960x544

	// VGA Mode Settings
	//#define MCLK_24_PCLK_XX_30_FPS
	#define MCLK_24_PCLK_60_30_FPS

	// lens choice
	#define SUYINOPT		0
	#define OTHERSOPT		1
	#define OPT_LENS		SUYINOPT


tI2CRegister code I2CInitTableCommon[] =
{
	#ifdef SENSOR_ONCHIP_LDO
	{0x3109, 0x84}, // Turn on NT99141 1.5V LDO
	#else  // SENSOR_ONCHIP_LDO
	{0x3109, 0x04}, // Turn off NT99141 1.5V LDO
	#endif // SENSOR_ONCHIP_LDO

	{0x3040, 0x04},
	{0x3041, 0x02},
	{0x3042, 0xFF},
	{0x3043, 0x08},
	{0x3052, 0xE0},
	{0x305F, 0x33},
	{0x3100, 0x07},
	{0x3106, 0x03},
	{0x3108, 0x00},
	{0x3110, 0x22},
	{0x3111, 0x57},
	{0x3112, 0x22},
	{0x3113, 0x55},
	{0x3114, 0x05},
	{0x3135, 0x00},
	{0x32F0, 0x01},
	{0x3290, 0x01},
	{0x3291, 0xA0},
	{0x3296, 0x01},
	{0x3297, 0x73},
	// AWB settings
	{0x3250, 0xC0},
	{0x3251, 0x00},
	{0x3252, 0xDF},
	{0x3253, 0x85},
	{0x3254, 0x00},
	{0x3255, 0xEB},
	{0x3256, 0x81},
	{0x3257, 0x40},
	{0x329B, 0x01},
	{0x32A1, 0x00},
	{0x32A2, 0xA0},
	{0x32A3, 0x01},
	{0x32A4, 0xA0},
	{0x32A5, 0x01},
	{0x32A6, 0x18},
	{0x32A7, 0x01},
	{0x32A8, 0xE0},

	{0x32F0, 0x00}, // CbYCrY
	{0x3336, 0x14},
	// Lens Shadding Correction
	{0x3210, 0x16},
	{0x3211, 0x19},
	{0x3212, 0x16},
	{0x3213, 0x14},
	{0x3214, 0x15},
	{0x3215, 0x18},
	{0x3216, 0x15},
	{0x3217, 0x14},
	{0x3218, 0x15},
	{0x3219, 0x18},
	{0x321A, 0x15},
	{0x321B, 0x14},
	{0x321C, 0x14},
	{0x321D, 0x17},
	{0x321E, 0x14},
	{0x321F, 0x12},
	{0x3231, 0x74},
	{0x3232, 0xC4},
	// Gamma
	{0x3270, 0x00},
	{0x3271, 0x0C},
	{0x3272, 0x18},
	{0x3273, 0x32},
	{0x3274, 0x44},
	{0x3275, 0x54},
	{0x3276, 0x70},
	{0x3277, 0x88},
	{0x3278, 0x9D},
	{0x3279, 0xB0},
	{0x327A, 0xCF},
	{0x327B, 0xE2},
	{0x327C, 0xEF},
	{0x327D, 0xF7},
	{0x327E, 0xFF},
	//{0x3060, 0x01}, // Update Registers
	// AE Window weighting table
	// W0  W1  W2  W3  0x32B0:b76 b54 b32 b10
	// W4  W5  W6  W7  0x32B1:b76 b54 b32 b10
	// W8  W9  W10 W11 0x32B1:b76 b54 b32 b10
	// W12 W13 W14 W15 0x32B1:b76 b54 b32 b10
	#if 0 // NTK K.H. Modified on Jan 24, 2013
	#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
		#ifdef SENSOR_3A_SYNC_SOURCE_SN0
	{0x32B0, 0x5B}, // W0  = 1, W1  = 1, W2  = 2, W3  = 3
	{0x32B1, 0x5B}, // W4  = 1, W5  = 1, W6  = 2, W7  = 3
	{0x32B2, 0x5B}, // W8  = 1, W9  = 1, W10 = 2, W11 = 3
	{0x32B3, 0x5B}, // W12 = 1, W13 = 1, W14 = 2, W15 = 3
		#else  // SENSOR_3A_SYNC_SOURCE_SN0
	{0x32B0, 0xE5}, // W0  = 3, W1  = 2, W2  = 1, W3  = 1
	{0x32B1, 0xE5}, // W4  = 3, W5  = 2, W6  = 1, W7  = 1
	{0x32B2, 0xE5}, // W8  = 3, W9  = 2, W10 = 1, W11 = 1
	{0x32B3, 0xE5}, // W12 = 3, W13 = 2, W14 = 1, W15 = 1
		#endif // SENSOR_3A_SYNC_SOURCE_SN0
	#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	#endif // NTK K.H. Modified on Jan 24, 2013
	// Color Correction and Transform Matrix
	#if SENSOR_CC == 0
	{0x3302, 0x00},
	{0x3303, 0x40},
	{0x3304, 0x00},
	{0x3305, 0x96},
	{0x3306, 0x00},
	{0x3307, 0x29},
	{0x3308, 0x07},
	{0x3309, 0xBA},
	{0x330A, 0x06},
	{0x330B, 0xF5},
	{0x330C, 0x01},
	{0x330D, 0x51},
	{0x330E, 0x01},
	{0x330F, 0x30},
	{0x3310, 0x07},
	{0x3311, 0x16},
	{0x3312, 0x07},
	{0x3313, 0xBA},
	#elif SENSOR_CC == 1
	{0x3302, 0x00},
	{0x3303, 0x4C},
	{0x3304, 0x00},
	{0x3305, 0x96},
	{0x3306, 0x00},
	{0x3307, 0x1D},
	{0x3308, 0x07},
	{0x3309, 0xE8},
	{0x330A, 0x06},
	{0x330B, 0x98},
	{0x330C, 0x01},
	{0x330D, 0x80},
	{0x330E, 0x00},
	{0x330F, 0xCC},
	{0x3310, 0x07},
	{0x3311, 0x7A},
	{0x3312, 0x07},
	{0x3313, 0xBB},
	#elif SENSOR_CC == 2
	{0x3302, 0x00},
	{0x3303, 0x4C},
	{0x3304, 0x00},
	{0x3305, 0x96},
	{0x3306, 0x00},
	{0x3307, 0x1D},
	{0x3308, 0x07},
	{0x3309, 0xC8},
	{0x330A, 0x06},
	{0x330B, 0xF8},
	{0x330C, 0x01},
	{0x330D, 0x41},
	{0x330E, 0x01},
	{0x330F, 0x10},
	{0x3310, 0x07},
	{0x3311, 0x05},
	{0x3312, 0x07},
	{0x3313, 0xEB},
	#endif // SENSOR_CC
	//////
	{0x3326, 0x02},
	{0x3327, 0x0A},
	{0x3328, 0x0A},
	{0x3329, 0x06},
	{0x332A, 0x06},
	{0x332B, 0x1C},
	{0x332C, 0x1C},
	{0x332D, 0x00},
	{0x332E, 0x1D},
	{0x332F, 0x1F},
	{0x32F6, 0x0F},
	{0x32F9, 0x42},
	{0x32FA, 0x24},
	{0x3325, 0x4A},
	{0x3330, 0x00},
	{0x3331, 0x0A},
	{0x3332, 0xFF},
	{0x3338, 0x30},
	{0x3339, 0x84},
	{0x333A, 0x48},
	{0x333F, 0x07},
	{0x3360, 0x10},
	{0x3361, 0x18},
	{0x3362, 0x1f},
	{0x3363, 0x37},
	{0x3364, 0x80},
	{0x3365, 0x80},
	{0x3366, 0x68},
	{0x3367, 0x60},
	#if 1 // NTK K.H. Modified on Jan 31, 2013
	{0x3368, 0x90},
	{0x3369, 0x80},
	{0x336A, 0x70},
	{0x336B, 0x60},
	#endif // NTK K.H. Modified on Jan 31, 2013
	#if 0 // NTK K.H. Modified on Jan 23, 2013
	{0x3368, 0x60},
	{0x3369, 0x50},
	{0x336A, 0x50},
	{0x336B, 0x50},
	#endif // NTK K.H. Modified on Jan 23, 2013
	#if 0 // NTK K.H. Modified on Jan 14, 2013
	{0x3368, 0x50},
	{0x3369, 0x40},
	{0x336A, 0x30},
	{0x336B, 0x30},
	#endif // NTK K.H. Modified on Jan 14, 2013
	#if 0  // NTK Initial Setting
	{0x3368, 0x30},
	{0x3369, 0x28},
	{0x336A, 0x20},
	{0x336B, 0x10},
	#endif // NTK Initial Setting
	{0x336C, 0x00},
	{0x336D, 0x20},
	{0x336E, 0x1C},
	{0x336F, 0x18},
	#if 1 // NTK K.H. Modified on Jan 31, 2013
	{0x3370, 0x18},
	{0x3371, 0x28},
	{0x3372, 0x2C},
	{0x3373, 0x30},
	{0x3374, 0x30},
	{0x3375, 0x0C},
	{0x3376, 0x12},
	{0x3377, 0x18},
	{0x3378, 0x1C},
	#endif // NTK K.H. Modified on Jan 31, 2013
	#if 0 // NTK K.H. Modified on Jan 23, 2013
	{0x3370, 0x18},
	{0x3371, 0x38},
	{0x3372, 0x3C},
	{0x3373, 0x3F},
	{0x3374, 0x3C},
	{0x3378, 0x30},
	#endif // NTK K.H. Modified on Jan 23, 2013
	#if 0 // NTK K.H. Modified on Jan 14, 2013
	{0x3370, 0x12},
	{0x3371, 0x38},
	{0x3372, 0x3C},
	{0x3373, 0x3F},
	{0x3374, 0x3F},
	{0x3378, 0x30},
	#endif // NTK K.H. Modified on Jan 14, 2013
	#if 0  // NTK Initial Setting
	{0x3370, 0x10},
	{0x3371, 0x38},
	{0x3372, 0x3C},
	{0x3373, 0x3F},
	{0x3374, 0x3F},
	{0x3378, 0x50},
	#endif // NTK Initial Setting
	{0x338A, 0x34},
	{0x338B, 0x7F},
	{0x338C, 0x10},
	{0x338D, 0x23},
	{0x338E, 0x7F},
	{0x338F, 0x14},
	// Driving ability
	//{0x3068, 0x01}, // 0:2mA, 1:4mA, 2:6mA, 3:8mA for I2C SCL and SDA
	// marked following lines to lower driving current from NT99141 - by Wythe at Sep 18, 2012.
	{0x3069, DRIVING_ABILITY}, // 0:2mA, 1:4mA, 2:6mA, 3:8mA for HSYNC, VSYNC, and D0 - D9
	{0x306A, DRIVING_ABILITY}, // 0:2mA, 1:4mA, 2:6mA, 3:8mA for PCLK
};

tI2CRegister code I2CInitTable50HzHD[] =
{
	#ifdef MCLK_24_PCLK_80_24_FPS
	// Not important item, use the same settings as MCLK_24_PCLK_72_24_FPS. - 20130109
	//{0x3012, 0x00}, // Default Integration Time
	//{0x3013, 0xff},
	// 50 Hz Anti-Flicker
	{0x32BF, 0x60},
	{0x32C0, 0x60},
	{0x32C1, 0x60},
	{0x32C2, 0x60},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x10},
	{0x32C6, 0x18},
	{0x32C7, 0x00},
	{0x32C8, 0xD8},
	{0x32C9, 0x60},
	{0x32CA, 0x70},
	{0x32CB, 0x70},
	{0x32CC, 0x78},
	{0x32CD, 0x78},
	{0x32DB, 0x7A},
	// Modules Enable Controls
	{0x3200, 0x3E}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	// Output Resolution
	//{0x32e0, 0x05}, // Scaler_Out_Size_X //
	//{0x32e1, 0x00},
	//{0x32e2, 0x02}, // Scaler_Out_Size_Y //
	//{0x32e3, 0xd0},
	// PLL settings
	{0x3028, 0x0B}, // PLL_CLK = 24 * (11+1) / (2 * (0+1) * (0+1)) = 144
	{0x3029, 0x00}, //
	{0x302A, 0x04}, // P_CLK = 144 / (2^1) = 72
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
	{0x300A, 0x06}, // line length // 0x67C=1660
	{0x300B, 0x83},
	{0x300C, 0x03}, // frame length // 0x2F1=753
	{0x300D, 0x83},
	{0x300E, 0x05}, // x output size // 0x500=1280
	{0x300F, 0x00},
	{0x3010, 0x02}, // y output size // 0x2D0=720
	{0x3011, 0xD0},
	// soc auto exposure
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x3F},	// enable ae/awb
	//{0x3201, 0x1F},	// disable AE/ enable AWB
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_80_24_FPS
	#ifdef MCLK_24_PCLK_72_24_FPS
	//{0x3012, 0x00}, // Default Integration Time
	//{0x3013, 0xff},
	// 50 Hz Anti-Flicker
	{0x32BF, 0x60},
	{0x32C0, 0x60},
	{0x32C1, 0x60},
	{0x32C2, 0x60},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x10},
	{0x32C6, 0x18},
	{0x32C7, 0x00},
	{0x32C8, 0xD8},
	{0x32C9, 0x60},
	{0x32CA, 0x70},
	{0x32CB, 0x70},
	{0x32CC, 0x78},
	{0x32CD, 0x78},
	{0x32DB, 0x7A},
	// Modules Enable Controls
	{0x3200, 0x3E}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	// Output Resolution
	//{0x32e0, 0x05}, // Scaler_Out_Size_X //
	//{0x32e1, 0x00},
	//{0x32e2, 0x02}, // Scaler_Out_Size_Y //
	//{0x32e3, 0xd0},
	// PLL settings
	{0x3028, 0x0B}, // PLL_CLK = 24 * (11+1) / (2 * (0+1) * (0+1)) = 144
	{0x3029, 0x00}, //
	{0x302A, 0x04}, // P_CLK = 144 / (2^1) = 72
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
	{0x300A, 0x06}, // line length // 0x67C=1660
	{0x300B, 0x83},
	{0x300C, 0x03}, // frame length // 0x2F1=753
	{0x300D, 0x83},
	{0x300E, 0x05}, // x output size // 0x500=1280
	{0x300F, 0x00},
	{0x3010, 0x02}, // y output size // 0x2D0=720
	{0x3011, 0xD0},
	// soc auto exposure
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x3F},	// enable ae/awb
	//{0x3201, 0x1F},	// disable AE/ enable AWB
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_72_24_FPS
	#ifdef MCLK_24_PCLK_60_24_FPS
	//{0x3012, 0x00}, // Default Integration Time
	//{0x3013, 0xff},
	// 50 Hz Anti-Flicker
	{0x32BF, 0x60},
	{0x32C0, 0x64},
	{0x32C1, 0x63},
	{0x32C2, 0x63},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x10},
	{0x32C6, 0x18},
	{0x32C7, 0x00},
	{0x32C8, 0xB4},
	{0x32C9, 0x63},
	{0x32CA, 0x73},
	{0x32CB, 0x73},
	{0x32CC, 0x7B},
	{0x32CD, 0x7C},
	{0x32DB, 0x76},
	// Modules Enable Controls
	{0x3200, 0x3E}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	// Output Resolution
	//{0x32e0, 0x05}, // Scaler_Out_Size_X //
	//{0x32e1, 0x00},
	//{0x32e2, 0x02}, // Scaler_Out_Size_Y //
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
	{0x300A, 0x06}, // line length // 0x67C=1660
	{0x300B, 0x83},
	{0x300C, 0x03}, // frame length // 0x2F1=753
	{0x300D, 0x83},
	{0x300E, 0x05}, // x output size // 0x500=1280
	{0x300F, 0x00},
	{0x3010, 0x02}, // y output size // 0x2D0=720
	{0x3011, 0xD0},
	// soc auto exposure
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x3F},	// enable ae/awb
	//{0x3201, 0x1F},	// disable AE/ enable AWB
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_60_24_FPS
};

tI2CRegister code I2CInitTable60HzHD[] =
{
	#ifdef MCLK_24_PCLK_80_24_FPS
	// Not important item, use the same settings as MCLK_24_PCLK_72_24_FPS. - 20130109
	//{0x3012, 0x00}, // Default Integration Time
	//{0x3013, 0xff},
	// 60 Hz Anti-Flicker
	{0x32BF, 0x60},
	{0x32C0, 0x64},
	{0x32C1, 0x63},
	{0x32C2, 0x63},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x10},
	{0x32C6, 0x18},
	{0x32C7, 0x00},
	{0x32C8, 0xB4},
	{0x32C9, 0x63},
	{0x32CA, 0x73},
	{0x32CB, 0x73},
	{0x32CC, 0x7B},
	{0x32CD, 0x7C},
	{0x32DB, 0x76},
	// Modules Enable Controls
	{0x3200, 0x3E}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	// Output Resolution
	//{0x32e0, 0x05}, // Scaler_Out_Size_X //
	//{0x32e1, 0x00},
	//{0x32e2, 0x02}, // Scaler_Out_Size_Y //
	//{0x32e3, 0xd0},
	// PLL settings
	{0x3028, 0x0B}, // PLL_CLK = 24 * (11+1) / (2 * (0+1) * (0+1)) = 144
	{0x3029, 0x00}, //
	{0x302A, 0x04}, // P_CLK = 144 / (2^1) = 72
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
	{0x300A, 0x06}, // line length // 0x67C=1660
	{0x300B, 0x83},
	{0x300C, 0x03}, // frame length // 0x2F1=753
	{0x300D, 0x83},
	{0x300E, 0x05}, // x output size // 0x500=1280
	{0x300F, 0x00},
	{0x3010, 0x02}, // y output size // 0x2D0=720
	{0x3011, 0xD0},
	// soc auto exposure
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x3F},	// enable ae/awb
	//{0x3201, 0x1F},	// disable AE/ enable AWB
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_80_24_FPS
	#ifdef MCLK_24_PCLK_72_24_FPS
	//{0x3012, 0x00}, // Default Integration Time
	//{0x3013, 0xff},
	// 60 Hz Anti-Flicker
	{0x32BF, 0x60},
	{0x32C0, 0x64},
	{0x32C1, 0x63},
	{0x32C2, 0x63},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x10},
	{0x32C6, 0x18},
	{0x32C7, 0x00},
	{0x32C8, 0xB4},
	{0x32C9, 0x63},
	{0x32CA, 0x73},
	{0x32CB, 0x73},
	{0x32CC, 0x7B},
	{0x32CD, 0x7C},
	{0x32DB, 0x76},
	// Modules Enable Controls
	{0x3200, 0x3E}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	// Output Resolution
	//{0x32e0, 0x05}, // Scaler_Out_Size_X //
	//{0x32e1, 0x00},
	//{0x32e2, 0x02}, // Scaler_Out_Size_Y //
	//{0x32e3, 0xd0},
	// PLL settings
	{0x3028, 0x0B}, // PLL_CLK = 24 * (11+1) / (2 * (0+1) * (0+1)) = 144
	{0x3029, 0x00}, //
	{0x302A, 0x04}, // P_CLK = 144 / (2^1) = 72
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
	{0x300A, 0x06}, // line length // 0x67C=1660
	{0x300B, 0x83},
	{0x300C, 0x03}, // frame length // 0x2F1=753
	{0x300D, 0x83},
	{0x300E, 0x05}, // x output size // 0x500=1280
	{0x300F, 0x00},
	{0x3010, 0x02}, // y output size // 0x2D0=720
	{0x3011, 0xD0},
	// soc auto exposure
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x3F},	// enable ae/awb
	//{0x3201, 0x1F},	// disable AE/ enable AWB
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_72_24_FPS
	#ifdef MCLK_24_PCLK_60_24_FPS
	//{0x3012, 0x00}, // Default Integration Time
	//{0x3013, 0xff},
	// 60 Hz Anti-Flicker
	{0x32BF, 0x60},
	{0x32C0, 0x80},
	{0x32C1, 0x7F},
	{0x32C2, 0x7F},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x20},
	{0x32C6, 0x20},
	{0x32C7, 0x00},
	{0x32C8, 0x97},
	{0x32C9, 0x7F},
	{0x32CA, 0x9F},
	{0x32CB, 0x9F},
	{0x32CC, 0x9F},
	{0x32CD, 0xA0},
	{0x32DB, 0x72},
	// Modules Enable Controls
	{0x3200, 0x3E}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	// Output Resolution
	//{0x32e0, 0x05}, // Scaler_Out_Size_X //
	//{0x32e1, 0x00},
	//{0x32e2, 0x02}, // Scaler_Out_Size_Y //
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
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x3F},	// enable ae/awb
	//{0x3201, 0x1F},	// disable AE/ enable AWB
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_60_24_FPS
};

#ifdef SUPPORT_SENSOR_SCALING
tI2CRegister code I2CInitTable50HzScaledHD[] =
{
#if OPT_LENS == SUYINOPT
	//YUYV_960x544_24.01_24.01_Fps_50hz
	{0x32BF, 0x60},
	{0x32C0, 0x50},
	{0x32C1, 0x50},
	{0x32C2, 0x50},
	{0x32C3, 0x00},
	{0x32C4, 0x2B},
	{0x32C5, 0x20},
	{0x32C6, 0x20},
	{0x32C7, 0x00},
	{0x32C8, 0xC8},
	{0x32C9, 0x50},
	{0x32CA, 0x70},
	{0x32CB, 0x70},
	{0x32CC, 0x70},
	{0x32CD, 0x70},
	{0x32DB, 0x79},
	{0x32E0, 0x03},
	{0x32E1, 0xC0},
	{0x32E2, 0x02},
	{0x32E3, 0x20},
	{0x32E4, 0x00},
	{0x32E5, 0x55},
	{0x32E6, 0x00},
	{0x32E7, 0x33},
	{0x3200, 0x3E},
	{0x3201, 0x0F},
	{0x3028, 0x13},
	{0x3029, 0x20},
	{0x302A, 0x00},
	{0x3022, 0x24},
	{0x3023, 0x24},
	{0x3002, 0x00},
	{0x3003, 0x04},
	{0x3004, 0x00},
	{0x3005, 0x26},
	{0x3006, 0x05},
	{0x3007, 0x03},
	{0x3008, 0x02},
	{0x3009, 0xB1},
	{0x300A, 0x07},
	{0x300B, 0xD0},
	{0x300C, 0x03},
	{0x300D, 0x41},
	{0x300E, 0x05},
	{0x300F, 0x00},
	{0x3010, 0x02},
	{0x3011, 0x8C},
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, 0x87},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x7F},
	{0x3021, 0x06},
	{0x320A, 0x80},
	{0x32b1, 0xe5},
	{0x32b2, 0xe5},
	{0x32b3, 0xe5},
	{0x32b4, 0xe5},
	{0x3060, 0x01},
#else
	#ifdef MCLK_24_PCLK_80_24_FPS
	{0x32BF, 0x60},
	{0x32C0, 0x60},
	{0x32C1, 0x60},
	{0x32C2, 0x60},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x20},
	{0x32C6, 0x20},
	{0x32C7, 0x00},
	{0x32C8, 0xC8},
	{0x32C9, 0x60},
	{0x32CA, 0x80},
	{0x32CB, 0x80},
	{0x32CC, 0x80},
	{0x32CD, 0x80},
	{0x32DB, 0x79},
	// Output Resolution
		#ifdef SENSOR_CROP_960x544
	{0x32E0, 0x03}, // Scaler_Out_Size_X // 0x3C0 = 960
	{0x32E1, 0xC0},
	{0x32E2, 0x02}, // Scaler_Out_Size_Y // 0x220 = 544
	{0x32E3, 0x20},
		#else  // SENSOR_CROP_960x544
	{0x32E0, 0x03}, // Scaler_Out_Size_X // 0x3C0 = 960
	{0x32E1, 0xC0},
	{0x32E2, 0x02}, // Scaler_Out_Size_Y // 0x258 = 600
	{0x32E3, 0x58},
		#endif // SENSOR_CROP_960x544
	{0x32E4, 0x00}, // Scaling Factor
	{0x32E5, 0x55},
	{0x32E6, 0x00},
	{0x32E7, 0x33},
	// Modules Enable Controls
	{0x3200, 0x3E}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	// PLL settings
	{0x3028, 0x13}, // PLL_CLK = 24 * (19+1) / (2 * (2+1) * (0+1)) = 80
	{0x3029, 0x20}, //
	{0x302A, 0x00}, // P_CLK = 80 / (2^0) = 80
	{0x3022, 0x24}, // read mode 0
	{0x3023, 0x24}, // read mode 1
	// Sensor Array Active Region
		#ifdef SENSOR_CROP_960x544
	{0x3002, 0x00}, // x start
	{0x3003, 0x04},
	{0x3004, 0x00}, // y start // 4+34 = 38 = 0x26
	{0x3005, 0x26},
	{0x3006, 0x05}, // x end  // 0x503=1283
	{0x3007, 0x03},
	{0x3008, 0x02}, // y end  // 723-34 = 689 = 0x2B1
	{0x3009, 0xB1},
		#else  // SENSOR_CROP_960x544
	{0x3002, 0x00}, // x start
	{0x3003, 0x04},
	{0x3004, 0x00}, // y start
	{0x3005, 0x04},
	{0x3006, 0x05}, // x end  // 0x503=1283
	{0x3007, 0x03},
	{0x3008, 0x02}, // y end  // 0x2D3=723
	{0x3009, 0xD3},
		#endif // SENSOR_CROP_960x544
	{0x300A, 0x07}, // line length // 0x7D0=2000
	{0x300B, 0xD0},
	{0x300C, 0x03}, // frame length // 0x341=833
	{0x300D, 0x41},
		#ifdef SENSOR_CROP_960x544
	{0x300E, 0x05}, // x output size // 0x500=1280
	{0x300F, 0x00},
	{0x3010, 0x02}, // y output size // 0x28C=652
	{0x3011, 0x8C},
		#else  // SENSOR_CROP_960x544
	{0x300E, 0x05}, // x output size // 0x500=1280
	{0x300F, 0x00},
	{0x3010, 0x02}, // y output size // 0x2D0=720
	{0x3011, 0xD0},
		#endif // SENSOR_CROP_960x544
	// soc auto exposure
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x7F},	// enable scaler/ae/awb
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_80_24_FPS
	#ifdef MCLK_24_PCLK_72_24_FPS
	//{0x3012, 0x00}, // Default Integration Time
	//{0x3013, 0xff},
	// 50 Hz Anti-Flicker
		#if 1  // NTK_KH_20130925
	{0x32BF, 0x60},
	{0x32C0, 0x60},
	{0x32C1, 0x60},
	{0x32C2, 0x60},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x20},
	{0x32C6, 0x20},
	{0x32C7, 0x00},
	{0x32C8, 0xD8},
	{0x32C9, 0x60},
	{0x32CA, 0x80},
	{0x32CB, 0x80},
	{0x32CC, 0x80},
	{0x32CD, 0x80},
	{0x32DB, 0x7A},
		#endif // NTK_KH_20130925
		#if 0  // NTK_KH_Original
	{0x32BF, 0x60},
	{0x32C0, 0x60},
	{0x32C1, 0x60},
	{0x32C2, 0x60},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x20},
	{0x32C6, 0x20},
	{0x32C7, 0x00},
	{0x32C8, 0xB4},
	{0x32C9, 0x60},
	{0x32CA, 0x80},
	{0x32CB, 0x80},
	{0x32CC, 0x80},
	{0x32CD, 0x80},
	{0x32DB, 0x76},
		#endif // NTK_KH_Original
		#if 0  // NTK_KH_20120103
	{0x32BF, 0x60},
	{0x32C0, 0x60},
	{0x32C1, 0x60},
	{0x32C2, 0x60},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x10},
	{0x32C6, 0x18},
	{0x32C7, 0x00},
	{0x32C8, 0xD8},
	{0x32C9, 0x60},
	{0x32CA, 0x70},
	{0x32CB, 0x70},
	{0x32CC, 0x78},
	{0x32CD, 0x78},
	{0x32DB, 0x7A},
		#endif // NTK_KH_20120103
	// Modules Enable Controls
	{0x3200, 0x3E}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	// Output Resolution
	{0x32E0, 0x03}, // Scaler_Out_Size_X //
	{0x32E1, 0xC0},
	{0x32E2, 0x02}, // Scaler_Out_Size_Y //
	{0x32E3, 0x20},
	{0x32E4, 0x00}, // Scaling Factor
	{0x32E5, 0x55},
	{0x32E6, 0x00},
	{0x32E7, 0x33}, //{0x32E7, 0x53},
	// PLL settings
	{0x3028, 0x0B}, // PLL_CLK = 24 * (11+1) / (2 * (0+1) * (0+1)) = 144
	{0x3029, 0x00}, //
	{0x302A, 0x04}, // P_CLK = 144 / (2^1) = 72
	{0x3022, 0x24}, // read mode 0
	{0x3023, 0x24}, // read mode 1
	// Sensor Array Active Region
		#ifdef SENSOR_CROP_960x544
	{0x3002, 0x00}, // x start
	{0x3003, 0x04},
	{0x3004, 0x00}, // y start // 4+34 = 38 = 0x26
	{0x3005, 0x26},
	{0x3006, 0x05}, // x end  // 0x503=1283
	{0x3007, 0x03},
	{0x3008, 0x02}, // y end  // 723-34 = 689 = 0x2B1
	{0x3009, 0xB1},
		#else  // SENSOR_CROP_960x544
	{0x3002, 0x00}, // x start
	{0x3003, 0x04},
	{0x3004, 0x00}, // y start
	{0x3005, 0x04},
	{0x3006, 0x05}, // x end  // 0x503=1283
	{0x3007, 0x03},
	{0x3008, 0x02}, // y end  // 0x2D3=723
	{0x3009, 0xD3},
		#endif // SENSOR_CROP_960x544
		#if 0  // NTK_KH_20120103
	{0x300A, 0x07}, // line length // 0x7D0=2000
	{0x300B, 0xD0},
	{0x300C, 0x02}, // frame length // 0x2EE=750
	{0x300D, 0xFE},
		#else  // NTK_KH_20120103
	{0x300A, 0x06}, // line length // 0x683=1667
	{0x300B, 0x83},
	{0x300C, 0x03}, // frame length // 0x383=899
	{0x300D, 0x83},
		#endif // NTK_KH_20120103
		#ifdef SENSOR_CROP_960x544
	{0x300E, 0x05}, // x output size // 0x500=1280
	{0x300F, 0x00},
	{0x3010, 0x02}, // y output size // 0x28C=652
	{0x3011, 0x8C},
		#else  // SENSOR_CROP_960x544
	{0x300E, 0x05}, // x output size // 0x500=1280
	{0x300F, 0x00},
	{0x3010, 0x02}, // y output size // 0x2D0=720
	{0x3011, 0xD0},
		#endif // SENSOR_CROP_960x544
	// soc auto exposure
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x7F},	// enable scaler/ae/awb
	//{0x3201, 0x1F},	// disable AE/ enable AWB
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_72_24_FPS
	#ifdef MCLK_24_PCLK_60_24_FPS
	//{0x3012, 0x00}, // Default Integration Time
	//{0x3013, 0xff},
	// 50 Hz Anti-Flicker
	{0x32BF, 0x60},
	{0x32C0, 0x64},
	{0x32C1, 0x63},
	{0x32C2, 0x63},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x10},
	{0x32C6, 0x18},
	{0x32C7, 0x00},
	{0x32C8, 0xB4},
	{0x32C9, 0x63},
	{0x32CA, 0x73},
	{0x32CB, 0x73},
	{0x32CC, 0x7B},
	{0x32CD, 0x7C},
	{0x32DB, 0x76},
	// Modules Enable Controls
	{0x3200, 0x3E}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	// Output Resolution
	{0x32E0, 0x03}, // Scaler_Out_Size_X //
	{0x32E1, 0xC0},
	{0x32E2, 0x02}, // Scaler_Out_Size_Y //
	{0x32E3, 0x20},
	{0x32E4, 0x00}, // Scaling Factor
	{0x32E5, 0x55},
	{0x32E6, 0x00},
	{0x32E7, 0x53},
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
	{0x300A, 0x06}, // line length // 0x67C=1660
	{0x300B, 0x83},
	{0x300C, 0x03}, // frame length // 0x2F1=753
	{0x300D, 0x83},
	{0x300E, 0x05}, // x output size // 0x500=1280
	{0x300F, 0x00},
	{0x3010, 0x02}, // y output size // 0x2D0=720
	{0x3011, 0xD0},
	// soc auto exposure
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x3F},	// enable ae/awb
	//{0x3201, 0x1F},	// disable AE/ enable AWB
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_60_24_FPS
#endif // OPT_LENS
};

tI2CRegister code I2CInitTable60HzScaledHD[] =
{
#if OPT_LENS == SUYINOPT
	//YUYV_960x544_24.01_24.01_Fps_60hz
	{0x32BF, 0x60},
	{0x32C0, 0x54},
	{0x32C1, 0x54},
	{0x32C2, 0x54},
	{0x32C3, 0x00},
	{0x32C4, 0x2B},
	{0x32C5, 0x20},
	{0x32C6, 0x20},
	{0x32C7, 0x00},
	{0x32C8, 0xA7},
	{0x32C9, 0x54},
	{0x32CA, 0x74},
	{0x32CB, 0x74},
	{0x32CC, 0x74},
	{0x32CD, 0x74},
	{0x32DB, 0x74},
	{0x32E0, 0x03},
	{0x32E1, 0xC0},
	{0x32E2, 0x02},
	{0x32E3, 0x20},
	{0x32E4, 0x00},
	{0x32E5, 0x55},
	{0x32E6, 0x00},
	{0x32E7, 0x33},
	{0x3200, 0x3E},
	{0x3201, 0x0F},
	{0x3028, 0x13},
	{0x3029, 0x20},
	{0x302A, 0x00},
	{0x3022, 0x24},
	{0x3023, 0x24},
	{0x3002, 0x00},
	{0x3003, 0x04},
	{0x3004, 0x00},
	{0x3005, 0x26},
	{0x3006, 0x05},
	{0x3007, 0x03},
	{0x3008, 0x02},
	{0x3009, 0xB1},
	{0x300A, 0x07},
	{0x300B, 0xD0},
	{0x300C, 0x03},
	{0x300D, 0x41},
	{0x300E, 0x05},
	{0x300F, 0x00},
	{0x3010, 0x02},
	{0x3011, 0x8C},
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, 0x87},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x7F},
	{0x3021, 0x06},
	{0x320A, 0x80},
	{0x32b1, 0xe5},
	{0x32b2, 0xe5},
	{0x32b3, 0xe5},
	{0x32b4, 0xe5},
	{0x3060, 0x01},
#else
	#ifdef MCLK_24_PCLK_80_24_FPS
	{0x32BF, 0x60},
	{0x32C0, 0x64},
	{0x32C1, 0x64},
	{0x32C2, 0x64},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x20},
	{0x32C6, 0x20},
	{0x32C7, 0x00},
	{0x32C8, 0xA7},
	{0x32C9, 0x64},
	{0x32CA, 0x84},
	{0x32CB, 0x84},
	{0x32CC, 0x84},
	{0x32CD, 0x84},
	{0x32DB, 0x74},
	// Output Resolution
		#ifdef SENSOR_CROP_960x544
	{0x32E0, 0x03}, // Scaler_Out_Size_X // 0x3C0 = 960
	{0x32E1, 0xC0},
	{0x32E2, 0x02}, // Scaler_Out_Size_Y // 0x220 = 544
	{0x32E3, 0x20},
		#else  // SENSOR_CROP_960x544
	{0x32E0, 0x03}, // Scaler_Out_Size_X // 0x3C0 = 960
	{0x32E1, 0xC0},
	{0x32E2, 0x02}, // Scaler_Out_Size_Y // 0x258 = 600
	{0x32E3, 0x58},
		#endif // SENSOR_CROP_960x544
	{0x32E4, 0x00}, // Scaling Factor
	{0x32E5, 0x55},
	{0x32E6, 0x00},
	{0x32E7, 0x33},
	// Modules Enable Controls
	{0x3200, 0x3E}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	// PLL settings
	{0x3028, 0x13}, // PLL_CLK = 24 * (19+1) / (2 * (2+1) * (0+1)) = 80
	{0x3029, 0x20}, //
	{0x302A, 0x00}, // P_CLK = 80 / (2^0) = 80
	{0x3022, 0x24}, // read mode 0
	{0x3023, 0x24}, // read mode 1
	// Sensor Array Active Region
		#ifdef SENSOR_CROP_960x544
	{0x3002, 0x00}, // x start
	{0x3003, 0x04},
	{0x3004, 0x00}, // y start // 4+34 = 38 = 0x26
	{0x3005, 0x26},
	{0x3006, 0x05}, // x end  // 0x503=1283
	{0x3007, 0x03},
	{0x3008, 0x02}, // y end  // 723-34 = 689 = 0x2B1
	{0x3009, 0xB1},
		#else  // SENSOR_CROP_960x544
	{0x3002, 0x00}, // x start
	{0x3003, 0x04},
	{0x3004, 0x00}, // y start
	{0x3005, 0x04},
	{0x3006, 0x05}, // x end  // 0x503=1283
	{0x3007, 0x03},
	{0x3008, 0x02}, // y end  // 0x2D3=723
	{0x3009, 0xD3},
		#endif // SENSOR_CROP_960x544
	{0x300A, 0x07}, // line length // 0x7D0=2000
	{0x300B, 0xD0},
	{0x300C, 0x03}, // frame length // 0x341=833
	{0x300D, 0x41},
		#ifdef SENSOR_CROP_960x544
	{0x300E, 0x05}, // x output size // 0x500=1280
	{0x300F, 0x00},
	{0x3010, 0x02}, // y output size // 0x28C=652
	{0x3011, 0x8C},
		#else  // SENSOR_CROP_960x544
	{0x300E, 0x05}, // x output size // 0x500=1280
	{0x300F, 0x00},
	{0x3010, 0x02}, // y output size // 0x2D0=720
	{0x3011, 0xD0},
		#endif // SENSOR_CROP_960x544
	// soc auto exposure
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x7F},	// enable scaler/ae/awb
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_80_24_FPS
	#ifdef MCLK_24_PCLK_72_24_FPS
	//{0x3012, 0x00}, // Default Integration Time
	//{0x3013, 0xff},
	// 60 Hz Anti-Flicker
		#if 1  // NTK_KH_20130925
	{0x32BF, 0x60},
	{0x32C0, 0x64},
	{0x32C1, 0x64},
	{0x32C2, 0x64},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x20},
	{0x32C6, 0x20},
	{0x32C7, 0x00},
	{0x32C8, 0xB4},
	{0x32C9, 0x64},
	{0x32CA, 0x84},
	{0x32CB, 0x84},
	{0x32CC, 0x84},
	{0x32CD, 0x84},
	{0x32DB, 0x76},
		#endif // NTK_KH_20130925
		#if 0  // NTK_KH_Original
	{0x32BF, 0x60},
	{0x32C0, 0x64},
	{0x32C1, 0x64},
	{0x32C2, 0x64},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x20},
	{0x32C6, 0x20},
	{0x32C7, 0x00},
	{0x32C8, 0x96},
	{0x32C9, 0x64},
	{0x32CA, 0x84},
	{0x32CB, 0x84},
	{0x32CC, 0x84},
	{0x32CD, 0x84},
	{0x32DB, 0x72},
		#endif // NTK_KH_Original
		#if 0  // NTK_KH_20120103
	{0x32BF, 0x60},
	{0x32C0, 0x64},
	{0x32C1, 0x63},
	{0x32C2, 0x63},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x10},
	{0x32C6, 0x18},
	{0x32C7, 0x00},
	{0x32C8, 0xB4},
	{0x32C9, 0x63},
	{0x32CA, 0x73},
	{0x32CB, 0x73},
	{0x32CC, 0x7B},
	{0x32CD, 0x7C},
	{0x32DB, 0x76},
		#endif // NTK_KH_20120103
	// Modules Enable Controls
	{0x3200, 0x3E}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	// Output Resolution
	{0x32E0, 0x03}, // Scaler_Out_Size_X //
	{0x32E1, 0xC0},
	{0x32E2, 0x02}, // Scaler_Out_Size_Y //
	{0x32E3, 0x20},
	{0x32E4, 0x00}, // Scaling Factor
	{0x32E5, 0x55},
	{0x32E6, 0x00},
	{0x32E7, 0x33}, //{0x32E7, 0x53},
	// PLL settings
	{0x3028, 0x0B}, // PLL_CLK = 24 * (11+1) / (2 * (0+1) * (0+1)) = 144
	{0x3029, 0x00}, //
	{0x302A, 0x04}, // P_CLK = 144 / (2^1) = 72
	{0x3022, 0x24}, // read mode 0
	{0x3023, 0x24}, // read mode 1
	// Sensor Array Active Region
		#ifdef SENSOR_CROP_960x544
	{0x3002, 0x00}, // x start
	{0x3003, 0x04},
	{0x3004, 0x00}, // y start // 4+34 = 38 = 0x26
	{0x3005, 0x26},
	{0x3006, 0x05}, // x end  // 0x503=1283
	{0x3007, 0x03},
	{0x3008, 0x02}, // y end  // 723-34 = 689 = 0x2B1
	{0x3009, 0xB1},
		#else  // SENSOR_CROP_960x544
	{0x3002, 0x00}, // x start
	{0x3003, 0x04},
	{0x3004, 0x00}, // y start
	{0x3005, 0x04},
	{0x3006, 0x05}, // x end  // 0x503=1283
	{0x3007, 0x03},
	{0x3008, 0x02}, // y end  // 0x2D3=723
	{0x3009, 0xD3},
		#endif // SENSOR_CROP_960x544
		#if 0  // NTK_KH_20120103
	{0x300A, 0x07}, // line length // 0x7D0=2000
	{0x300B, 0xD0},
	{0x300C, 0x02}, // frame length // 0x2EE=750
	{0x300D, 0xFE},
		#else  // NTK_KH_20120103
	{0x300A, 0x06}, // line length // 0x683=1667
	{0x300B, 0x83},
	{0x300C, 0x03}, // frame length // 0x383=899
	{0x300D, 0x83},
		#endif // NTK_KH_20120103
		#ifdef SENSOR_CROP_960x544
	{0x300E, 0x05}, // x output size // 0x500=1280
	{0x300F, 0x00},
	{0x3010, 0x02}, // y output size // 0x28C=652
	{0x3011, 0x8C},
		#else  // SENSOR_CROP_960x544
	{0x300E, 0x05}, // x output size // 0x500=1280
	{0x300F, 0x00},
	{0x3010, 0x02}, // y output size // 0x2D0=720
	{0x3011, 0xD0},
		#endif // SENSOR_CROP_960x544
	// soc auto exposure
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x7F},	// enable scaler/ae/awb
	//{0x3201, 0x1F},	// disable AE/ enable AWB
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_72_24_FPS
	#ifdef MCLK_24_PCLK_60_24_FPS
	//{0x3012, 0x00}, // Default Integration Time
	//{0x3013, 0xff},
	// 60 Hz Anti-Flicker
	{0x32BF, 0x60},
	{0x32C0, 0x80},
	{0x32C1, 0x7F},
	{0x32C2, 0x7F},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x20},
	{0x32C6, 0x20},
	{0x32C7, 0x00},
	{0x32C8, 0x97},
	{0x32C9, 0x7F},
	{0x32CA, 0x9F},
	{0x32CB, 0x9F},
	{0x32CC, 0x9F},
	{0x32CD, 0xA0},
	{0x32DB, 0x72},
	// Modules Enable Controls
	{0x3200, 0x3E}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	// Output Resolution
	{0x32E0, 0x03}, // Scaler_Out_Size_X //
	{0x32E1, 0xC0},
	{0x32E2, 0x02}, // Scaler_Out_Size_Y //
	{0x32E3, 0x20},
	{0x32E4, 0x00}, // Scaling Factor
	{0x32E5, 0x55},
	{0x32E6, 0x00},
	{0x32E7, 0x53},
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
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x3F},	// enable ae/awb
	//{0x3201, 0x1F},	// disable AE/ enable AWB
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_60_24_FPS
#endif // OPT_LENS
};
#endif // SUPPORT_SENSOR_SCALING

tI2CRegister code I2CInitTable50HzVGA[] =
{
	#ifdef MCLK_24_PCLK_XX_30_FPS
	//{0x3012, 0x01}, // Default Integration Time
	//{0x3013, 0x75},
	//{0x301d, 0x30},
	// 50 Hz Anti-Flicker
	{0x32BF, 0x60},
	{0x32C0, 0x5A},
	{0x32C1, 0x5A},
	{0x32C2, 0x5A},
	{0x32C3, 0x04},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x10},
	{0x32C6, 0x18},
	{0x32C7, 0x00},
	{0x32C8, 0xE0},
	{0x32C9, 0x5E},
	{0x32CA, 0x6A},
	{0x32CB, 0x6A},
	{0x32CC, 0x72},
	{0x32CD, 0x72},
	{0x32DB, 0x7B},
	// Output Resolution
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
	{0x300c, 0x02}, // frame length // 0x2ea = 746
	{0x300d, 0xea},
	{0x300e, 0x03}, // x output size // 0x3c0 = 960
	{0x300f, 0xc0},
	{0x3010, 0x02}, // y output size // 0x2d0 = 720
	{0x3011, 0xd0},
	// soc auto exposure
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x7f}, // enable AE/AWB
	//{0x3201, 0x5F}, // disable AE
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_XX_30_FPS
	#ifdef MCLK_24_PCLK_60_30_FPS
	//{0x3012, 0x01}, // Default Integration Time
	//{0x3013, 0x75},
	// 50 Hz Anti-Flicker
	{0x32BF, 0x60},
	{0x32C0, 0x5A},
	{0x32C1, 0x5A},
	{0x32C2, 0x5A},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x20},
	{0x32C6, 0x20},
	{0x32C7, 0x00},
	{0x32C8, 0xE0},
	{0x32C9, 0x5A},
	{0x32CA, 0x7A},
	{0x32CB, 0x7A},
	{0x32CC, 0x7A},
	{0x32CD, 0x7A},
	{0x32DB, 0x7B},
	// Output Resolution
	{0x32E0, 0x02}, // Scaler_Out_Size_X // 0x280 = 640
	{0x32E1, 0x80},
	{0x32E2, 0x01}, // Scaler_Out_Size_Y // 0x1e0 = 480
	{0x32E3, 0xE0},
	{0x32E4, 0x00}, // Scaling Factor
	{0x32E5, 0x80},
	{0x32E6, 0x00},
	{0x32E7, 0x80},
	// Modules Enable Controls
	{0x3200, 0x3E}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	// PLL settings
		#if 0
	{0x3028, 0x04}, // PLL_CLK = 24 * (4+1) / (2 * (0+1) * (0+1)) = 60
	{0x3029, 0x00}, //
	{0x302A, 0x00}, // P_CLK = 60 / (2^0) = 60
		#else
	{0x3028, 0x09}, // PLL_CLK = 24 * (9+1) / (2 * (0+1) * (0+1)) = 120
	{0x3029, 0x00}, //
	{0x302A, 0x04}, // P_CLK = 120 / (2^1) = 60
		#endif
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
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x7F}, // enable Scaler/AE/AWB
	//{0x3201, 0x5F}, // disable AE
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06},
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_60_30_FPS
};

tI2CRegister code I2CInitTable60HzVGA[] =
{
	#ifdef MCLK_24_PCLK_XX_30_FPS
	//{0x3012, 0x01}, // Default Integration Time
	//{0x3013, 0x75},
	//{0x301d, 0x30},
	// 60 Hz Anti-Flicker
	{0x32BF, 0x60},
	{0x32C0, 0x60},
	{0x32C1, 0x5F},
	{0x32C2, 0x5F},
	{0x32C3, 0x04},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x10},
	{0x32C6, 0x18},
	{0x32C7, 0x00},
	{0x32C8, 0xBB},
	{0x32C9, 0x63},
	{0x32CA, 0x6F},
	{0x32CB, 0x6F},
	{0x32CC, 0x77},
	{0x32CD, 0x78},
	{0x32DB, 0x77},
	// Output Resolution
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
	{0x300c, 0x02}, // frame length // 0x2ea = 746
	{0x300d, 0xea},
	{0x300e, 0x03}, // x output size // 0x3c0 = 960
	{0x300f, 0xc0},
	{0x3010, 0x02}, // y output size // 0x2d0 = 720
	{0x3011, 0xd0},
	// soc auto exposure
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x7f}, // enable Scaler/AE/AWB
	//{0x3201, 0x5F}, // disable AE
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06}, // Output Streaming
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_XX_30_FPS
	#ifdef MCLK_24_PCLK_60_30_FPS
	//{0x3012, 0x01}, // Default Integration Time
	//{0x3013, 0x75},
	// 60 Hz Anti-Flicker
	{0x32BF, 0x60},
	{0x32C0, 0x60},
	{0x32C1, 0x5F},
	{0x32C2, 0x5F},
	{0x32C3, 0x00},
	{0x32C4, SENSOR_AGC_GAIN},
	{0x32C5, 0x20},
	{0x32C6, 0x20},
	{0x32C7, 0x00},
	{0x32C8, 0xBB},
	{0x32C9, 0x5F},
	{0x32CA, 0x7F},
	{0x32CB, 0x7F},
	{0x32CC, 0x7F},
	{0x32CD, 0x80},
	{0x32DB, 0x77},
	// Output Resolution
	{0x32E0, 0x02}, // Scaler_Out_Size_X // 0x280 = 640
	{0x32E1, 0x80},
	{0x32E2, 0x01}, // Scaler_Out_Size_Y // 0x1e0 = 480
	{0x32E3, 0xE0},
	{0x32E4, 0x00}, // Scaling Factor
	{0x32E5, 0x80},
	{0x32E6, 0x00},
	{0x32E7, 0x80},
	// Modules Enable Controls
	{0x3200, 0x3E}, // enables
	{0x3201, 0x0F}, // Noise Reduction en, Edge Enhance en, Color Correction en, Special Effect en
	// PLL settings
		#if 0
	{0x3028, 0x04}, // PLL_CLK = 24 * (4+1) / (2 * (0+1) * (0+1)) = 60
	{0x3029, 0x00}, //
	{0x302A, 0x00}, // P_CLK = 60 / (2^0) = 60
		#else
	{0x3028, 0x09}, // PLL_CLK = 24 * (9+1) / (2 * (0+1) * (0+1)) = 120
	{0x3029, 0x00}, //
	{0x302A, 0x04}, // P_CLK = 120 / (2^1) = 60
		#endif
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
	{0x32B8, 0x3F},
	{0x32B9, 0x31},
	{0x32BB, SENSOR_AE_SPEED},
	{0x32BC, 0x38},
	{0x32BD, 0x3C},
	{0x32BE, 0x34},
	{0x3201, 0x7F}, // enable Scaler/AE/AWB
	//{0x3201, 0x5F}, // disable AE
	//{0x3025, 0x02}, // test pattern
	{0x3021, 0x06},
	{0x3060, 0x01},
	#endif // MCLK_24_PCLK_60_30_FPS
};


	#ifdef CHECK_SDRAM_BUFFERS

#if 1
unsigned char ucFrozenCount0;
unsigned char ucFrozenCount1;
unsigned char ucFrozenCount2;
unsigned char ucFrozenCount3;
unsigned char ucFrozenCount4;
unsigned char ucFrozenCount5;
#else
unsigned char xdata ucFrozenCount0 _at_ 0x7000;
unsigned char xdata ucFrozenCount1 _at_ 0x7001;
unsigned char xdata ucFrozenCount2 _at_ 0x7002;
unsigned char xdata ucFrozenCount3 _at_ 0x7003;
unsigned char xdata ucFrozenCount4 _at_ 0x7004;
unsigned char xdata ucFrozenCount5 _at_ 0x7005;
#endif
unsigned char xdata ucSRAMControl _at_ 0x0027;

#define SDRAM_BUFFER_ADDRESS0 0x010000
#define SDRAM_BUFFER_ADDRESS1 0x1D2000
#define SDRAM_BUFFER_ADDRESS2 0x394000
#define SDRAM_BUFFER_ADDRESS3 0x556000
#define SDRAM_BUFFER_ADDRESS4 0x718000
#define SDRAM_BUFFER_ADDRESS5 0x8DA000

#define SDRAM_BUFFER_VGAPOINT0 640L*2L*300L+200L*2L
#define SDRAM_BUFFER_VGAPOINT1 640L*2L*300L+400L*2L
#define SDRAM_BUFFER_HDPOINT0  960L*2L*370L+200L*2L
#define SDRAM_BUFFER_HDPOINT1  960L*2L*390L+700L*2L

#define FRAME_DATA_CHECKLENGTH 16
#define COUNTER_BEFORE_RESTERT 5

extern unsigned char xdata ucMemBank0;
extern unsigned char xdata ucMemBank1;

unsigned char fnD2CheckDataFromDRAM(
	unsigned char xdata *ucDataBuffer,
	unsigned long ulAddress,
	unsigned short usLength)
{
	D2_REG_UNION D2REG;
	unsigned short idx;
	unsigned char ucCmpResult = SUCCESS;

	for (idx = 0; idx < usLength; idx++, ulAddress++)
	{
		ucMemBank0  = (unsigned char)(ulAddress >> 15);
		ucMemBank1  = (unsigned char)(ulAddress >> 23);
		D2REG.wWord = (unsigned short)(ulAddress|0x8000);
		if (ucDataBuffer[idx] != *D2REG.pRegister)
			ucCmpResult = FAILURE;
		ucDataBuffer[idx] = *D2REG.pRegister;
	}
	return ucCmpResult;
}

void fnInitSDRAMBuffers()
{
	ucSRAMControl = 1;
	ucFrozenCount0 = ucFrozenCount1 = ucFrozenCount2 =
	ucFrozenCount3 = ucFrozenCount4 = ucFrozenCount5 = 0;
}

void fnD2RestartSensor()
{
	unsigned char SensorRegA, SensorRegB, SensorRegC, SensorRegD;
	unsigned char SensorRegE, SensorRegF, SensorRegG, SensorRegH;

	I2CREAD_SN0_REGISTER(0x3290, SensorRegA);  //AWB RED gain H
	I2CREAD_SN0_REGISTER(0x3291, SensorRegB);  //AWB RED gain L
	I2CREAD_SN0_REGISTER(0x3296, SensorRegC);  //AWB BLUE gain H
	I2CREAD_SN0_REGISTER(0x3297, SensorRegD);  //AWB BLUE gain L
	I2CREAD_SN0_REGISTER(0x301C, SensorRegE);  //GAIN Digital
	I2CREAD_SN0_REGISTER(0x301D, SensorRegF);  //GAIN Analog
	I2CREAD_SN0_REGISTER(0x3012, SensorRegG);  //AECH
	I2CREAD_SN0_REGISTER(0x3013, SensorRegH);  //AECL
	fnSetOperatingMode(nD2Mode, nD2Resolution);
	I2CWRITE_SN0_REGISTER(0x3290, SensorRegA);  //AWB RED gain H
	I2CWRITE_SN0_REGISTER(0x3291, SensorRegB);  //AWB RED gain L
	I2CWRITE_SN0_REGISTER(0x3296, SensorRegC);  //AWB BLUE gain H
	I2CWRITE_SN0_REGISTER(0x3297, SensorRegD);  //AWB BLUE gain L
	I2CWRITE_SN1_REGISTER(0x3290, SensorRegA);  //AWB RED gain H
	I2CWRITE_SN1_REGISTER(0x3291, SensorRegB);  //AWB RED gain L
	I2CWRITE_SN1_REGISTER(0x3296, SensorRegC);  //AWB BLUE gain H
	I2CWRITE_SN1_REGISTER(0x3297, SensorRegD);  //AWB BLUE gain L
	I2CWRITE_SN0_REGISTER(0x301C, SensorRegE);  //GAIN Digital
	I2CWRITE_SN0_REGISTER(0x301D, SensorRegF);  //GAIN Analog
	I2CWRITE_SN0_REGISTER(0x3012, SensorRegG);  //AECH
	I2CWRITE_SN0_REGISTER(0x3013, SensorRegH);  //AECL
	I2CWRITE_SN1_REGISTER(0x301C, SensorRegE);  //GAIN Digital
	I2CWRITE_SN1_REGISTER(0x301D, SensorRegF);  //GAIN Analog
	I2CWRITE_SN1_REGISTER(0x3012, SensorRegG);  //AECH
	I2CWRITE_SN1_REGISTER(0x3013, SensorRegH);  //AECL
	I2CWRITE_SN0_REGISTER(0x3060, 0x02); //Update Register
	I2CWRITE_SN1_REGISTER(0x3060, 0x02); //Update Register
}

void fnTraceSDRAMBuffers()
{
	unsigned char xdata *pFrameBuffer;
	unsigned char ucResult0, ucResult1;

	pFrameBuffer = 0x7010; // SRAM buffer
	switch (nD2Resolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	case D2_RESOLUTION_VGA_50Hz:
		ucResult0 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS0+SDRAM_BUFFER_VGAPOINT0, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucResult1 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS0+SDRAM_BUFFER_VGAPOINT1, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucFrozenCount0 = ((ucResult0 == SUCCESS) && (ucResult1 == SUCCESS))? ucFrozenCount0+1 : 0;

		ucResult0 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS1+SDRAM_BUFFER_VGAPOINT0, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucResult1 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS1+SDRAM_BUFFER_VGAPOINT1, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucFrozenCount1 = ((ucResult0 == SUCCESS) && (ucResult1 == SUCCESS))? ucFrozenCount1+1 : 0;

		ucResult0 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS2+SDRAM_BUFFER_VGAPOINT0, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucResult1 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS2+SDRAM_BUFFER_VGAPOINT1, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucFrozenCount2 = ((ucResult0 == SUCCESS) && (ucResult1 == SUCCESS))? ucFrozenCount2+1 : 0;

		ucResult0 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS3+SDRAM_BUFFER_VGAPOINT0, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucResult1 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS3+SDRAM_BUFFER_VGAPOINT1, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucFrozenCount3 = ((ucResult0 == SUCCESS) && (ucResult1 == SUCCESS))? ucFrozenCount3+1 : 0;

		ucResult0 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS4+SDRAM_BUFFER_VGAPOINT0, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucResult1 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS4+SDRAM_BUFFER_VGAPOINT1, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucFrozenCount4 = ((ucResult0 == SUCCESS) && (ucResult1 == SUCCESS))? ucFrozenCount4+1 : 0;

		ucResult0 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS5+SDRAM_BUFFER_VGAPOINT0, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucResult1 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS5+SDRAM_BUFFER_VGAPOINT1, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucFrozenCount5 = ((ucResult0 == SUCCESS) && (ucResult1 == SUCCESS))? ucFrozenCount5+1 : 0;
		break;
	case D2_RESOLUTION_HD_60Hz:
	case D2_RESOLUTION_HD_50Hz:
		ucResult0 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS0+SDRAM_BUFFER_HDPOINT0, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucResult1 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS0+SDRAM_BUFFER_HDPOINT1, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucFrozenCount0 = ((ucResult0 == SUCCESS) && (ucResult1 == SUCCESS))? ucFrozenCount0+1 : 0;

		ucResult0 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS1+SDRAM_BUFFER_HDPOINT0, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucResult1 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS1+SDRAM_BUFFER_HDPOINT1, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucFrozenCount1 = ((ucResult0 == SUCCESS) && (ucResult1 == SUCCESS))? ucFrozenCount1+1 : 0;

		ucResult0 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS2+SDRAM_BUFFER_HDPOINT0, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucResult1 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS2+SDRAM_BUFFER_HDPOINT1, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucFrozenCount2 = ((ucResult0 == SUCCESS) && (ucResult1 == SUCCESS))? ucFrozenCount2+1 : 0;

		ucResult0 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS3+SDRAM_BUFFER_HDPOINT0, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucResult1 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS3+SDRAM_BUFFER_HDPOINT1, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucFrozenCount3 = ((ucResult0 == SUCCESS) && (ucResult1 == SUCCESS))? ucFrozenCount3+1 : 0;

		ucResult0 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS4+SDRAM_BUFFER_HDPOINT0, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucResult1 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS4+SDRAM_BUFFER_HDPOINT1, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucFrozenCount4 = ((ucResult0 == SUCCESS) && (ucResult1 == SUCCESS))? ucFrozenCount4+1 : 0;

		ucResult0 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS5+SDRAM_BUFFER_HDPOINT0, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucResult1 = fnD2CheckDataFromDRAM(pFrameBuffer, SDRAM_BUFFER_ADDRESS5+SDRAM_BUFFER_HDPOINT1, FRAME_DATA_CHECKLENGTH);
		pFrameBuffer += FRAME_DATA_CHECKLENGTH;
		ucFrozenCount5 = ((ucResult0 == SUCCESS) && (ucResult1 == SUCCESS))? ucFrozenCount5+1 : 0;
		break;
	}
	if ((ucFrozenCount0 >= COUNTER_BEFORE_RESTERT) || (ucFrozenCount1 >= COUNTER_BEFORE_RESTERT) ||
		(ucFrozenCount2 >= COUNTER_BEFORE_RESTERT) || (ucFrozenCount3 >= COUNTER_BEFORE_RESTERT) ||
		(ucFrozenCount4 >= COUNTER_BEFORE_RESTERT) || (ucFrozenCount5 > COUNTER_BEFORE_RESTERT))
	{
		ucFrozenCount0 = ucFrozenCount1 = ucFrozenCount2 =
		ucFrozenCount3 = ucFrozenCount4 = ucFrozenCount5 = 0;
		fnD2RestartSensor();
	}
}
	#endif // CHECK_SDRAM_BUFFERS

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
#ifdef SENSOR_SYNC_FRAMES_NT99141
		I2CWorkItem0.I2CSlaveAddress  = SENSOR0_I2C_SLAVE;
		I2CWorkItem1.I2CSlaveAddress  = SENSOR1_I2C_SLAVE;
		I2CWRITE_SN0_REGISTER(0x3F00, 0x01);
		I2CWRITE_SN1_REGISTER(0x3F00, 0x01);
		I2CWorkItem0.I2CSlaveAddress  = 0xFA;
		I2CWorkItem1.I2CSlaveAddress  = 0xFA;
		I2CWRITE_SN0_REGISTER(0x4000, 0x01);
		I2CWRITE_SN1_REGISTER(0x4000, 0x01);
		I2CWRITE_SN0_REGISTER(0x4005, 0x01);
		I2CWRITE_SN1_REGISTER(0x4005, 0x01);
#endif // SENSOR_SYNC_FRAMES_NT99141
		fnI2cWriteTable1(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister));
		fnI2cWriteTable0(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister));
	}
	ucResetControl = 0x74;      // 40:SIU Reset, 20:IPU Reset, 10:SOU Reset, 04: DRAM Reset

	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
	#ifdef SUPPORT_SENSOR_SCALING
	case D2_RESOLUTION_VGA_60Hz+D2_RESOLUTION_SCALED_HD_OFFSET:
	#endif // SUPPORT_SENSOR_SCALING
		fnI2cWriteTable0(I2CInitTable60HzVGA, sizeof(I2CInitTable60HzVGA) / sizeof(tI2CRegister));
		fnI2cWriteTable1(I2CInitTable60HzVGA, sizeof(I2CInitTable60HzVGA) / sizeof(tI2CRegister));
		break;
	case D2_RESOLUTION_VGA_50Hz:
	#ifdef SUPPORT_SENSOR_SCALING
	case D2_RESOLUTION_VGA_50Hz+D2_RESOLUTION_SCALED_HD_OFFSET:
	#endif // SUPPORT_SENSOR_SCALING
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
	#ifdef SUPPORT_SENSOR_SCALING
	case D2_RESOLUTION_HD_60Hz+D2_RESOLUTION_SCALED_HD_OFFSET:
		fnI2cWriteTable0(I2CInitTable60HzScaledHD, sizeof(I2CInitTable60HzScaledHD) / sizeof(tI2CRegister));
		fnI2cWriteTable1(I2CInitTable60HzScaledHD, sizeof(I2CInitTable60HzScaledHD) / sizeof(tI2CRegister));
		break;
	case D2_RESOLUTION_HD_50Hz+D2_RESOLUTION_SCALED_HD_OFFSET:
		fnI2cWriteTable0(I2CInitTable50HzScaledHD, sizeof(I2CInitTable50HzScaledHD) / sizeof(tI2CRegister));
		fnI2cWriteTable1(I2CInitTable50HzScaledHD, sizeof(I2CInitTable50HzScaledHD) / sizeof(tI2CRegister));
		break;
	#endif // SUPPORT_SENSOR_SCALING
	default:
		ucResetControl = 0x00;
		return FAILURE;
	}
	ucResetControl = 0x00;
#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	ucSensor3ASync = SENSOR_3A_SYNC_START;
#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)

	//I2CWRITE_SN0_REGISTER(0x3022, 0x24+0x03); // Sensor 0: Vertical + Horizontal Flips
	//I2CWRITE_SN0_REGISTER(0x3022, 0x24); // Sensor 0: No Flips
	//I2CWRITE_SN1_REGISTER(0x3022, 0x24+0x02); // Sensor 1: Horizontal Mirrors

#ifdef SENSOR_SYNC_FRAMES_NT99141
	// NTK K.H. 2012-11-26, may not work well when I2C bus is seperated.
	I2CWRITE_SN0_REGISTER(0x4001, 0x01);
	I2CWRITE_SN1_REGISTER(0x4001, 0x01);
	I2CWRITE_SN0_REGISTER(0x4003, 0x01);
	I2CWRITE_SN1_REGISTER(0x4003, 0x01);
#endif // SENSOR_SYNC_FRAMES_NT99141
#ifdef CHECK_SDRAM_BUFFERS
	fnInitSDRAMBuffers();
#endif // CHECK_SDRAM_BUFFERS
	return SUCCESS;
}


#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)

void fnSensorAEAWBConfig()
{
	unsigned char SensorReg1;

	#ifdef SENSOR_3A_SYNC_SOURCE_SN0
	// Disable SENSOR1 AEC/AGC, and keep AEB enabled
	I2CREAD_SN1_REGISTER(0x3201, SensorReg1);
	SensorReg1 &= 0xDF;
	I2CWRITE_SN1_REGISTER(0x3201, SensorReg1);
	I2CWRITE_SN1_REGISTER(0x329B, 0x01); // Enable AWB Limit
	// Force the ExposureTime and Gains are updated on writting 0x02 to register 0x3060.
	// Suggested by NTK K.H. 2012-11-22
	I2CWRITE_SN1_REGISTER(0x301E, 0x12); // NTK K.H. 2012-11-22
	I2CWRITE_SN0_REGISTER(0x301E, 0x12); // NTK K.H. 2012-12-17
	//I2CWRITE_SN1_REGISTER(0x3053, 0x90); // NTK K.H. 2012-11-22, removed on 2012-11-28
	#else  // SENSOR_3A_SYNC_SOURCE_SN0
	// Disable SENSOR0 AEC/AGC, and keep AEB enabled
	I2CREAD_SN0_REGISTER(0x3201, SensorReg1);
	SensorReg1 &= 0xDF;
	I2CWRITE_SN0_REGISTER(0x3201, SensorReg1);
	I2CWRITE_SN0_REGISTER(0x329B, 0x01); // Enable AWB Limit
	// Force the ExposureTime and Gains are updated on writting 0x02 to register 0x3060.
	// Suggested by NTK K.H. 2012-11-22
	I2CWRITE_SN0_REGISTER(0x301E, 0x12); // NTK K.H. 2012-11-22
	I2CWRITE_SN1_REGISTER(0x301E, 0x12); // NTK K.H. 2012-12-17
	//I2CWRITE_SN0_REGISTER(0x3053, 0x90); // NTK K.H. 2012-11-22, removed on 2012-11-28
	#endif // SENSOR_3A_SYNC_SOURCE_SN0
	#ifdef SENSOR_SYNC_FRAMES_NT99141
	// NTK K.H. 2012-11-26, may not work well when I2C bus is seperated.
	I2CWRITE_SN0_REGISTER(0x4001, 0x01);
	I2CWRITE_SN1_REGISTER(0x4001, 0x01);
	I2CWRITE_SN0_REGISTER(0x4003, 0x01);
	I2CWRITE_SN1_REGISTER(0x4003, 0x01);
	#endif // SENSOR_SYNC_FRAMES_NT99141
}

	#ifdef SENSOR_CHECK_DAY_NIGHT
#define SENSOR_OB_MIN  8
#define SENSOR_OB_MAX 16

void fnCheckSensorDayNight()
{
	unsigned char SensorLuminance;
	unsigned char SensorOBValue;

	#ifdef SENSOR_3A_SYNC_SOURCE_SN0
	I2CREAD_SN0_REGISTER(0x32BA, SensorLuminance);  //Y average
	I2CREAD_SN0_REGISTER(0x3053, SensorOBValue);    //Optical black multiplier
	#else  // SENSOR_3A_SYNC_SOURCE_SN0
	I2CREAD_SN1_REGISTER(0x32BA, SensorLuminance);  //Y average
	I2CREAD_SN1_REGISTER(0x3053, SensorOBValue);    //Optical black multiplier
	#endif // SENSOR_3A_SYNC_SOURCE_SN0
	if (SensorLuminance <= 0x10)
	{
		if (SensorOBValue > SENSOR_OB_MIN)
		{
			SensorOBValue --;
			I2CWRITE_SN0_REGISTER(0x3053, SensorOBValue); //Optical black multiplier
			I2CWRITE_SN1_REGISTER(0x3053, SensorOBValue); //Optical black multiplier
		}
	}
	if (SensorLuminance >= 0x28)
	{
		if (SensorOBValue < SENSOR_OB_MAX)
		{
			SensorOBValue ++;
			I2CWRITE_SN0_REGISTER(0x3053, SensorOBValue); //Optical black multiplier
			I2CWRITE_SN1_REGISTER(0x3053, SensorOBValue); //Optical black multiplier
		}
	}
}
	#endif // SENSOR_CHECK_DAY_NIGHT

	#ifdef SENSOR_DENOISE_COLOR
void fnCheckColorNoise()
{
	unsigned char ucSensorGain;

	#ifdef SENSOR_3A_SYNC_SOURCE_SN0
	I2CREAD_SN0_REGISTER(0x301D, ucSensorGain);  //Current Gain
	#else  // SENSOR_3A_SYNC_SOURCE_SN0
	I2CREAD_SN1_REGISTER(0x301D, ucSensorGain);  //Current Gain
	#endif // SENSOR_3A_SYNC_SOURCE_SN0
	if (ucSensorGain < 0x3B)
	{ // Denoise Normal
		I2CWRITE_SN0_REGISTER(0x32F9, 0x42);
		I2CWRITE_SN0_REGISTER(0x32FA, 0x24);
		I2CWRITE_SN1_REGISTER(0x32F9, 0x42);
		I2CWRITE_SN1_REGISTER(0x32FA, 0x24);
	}
	else
	{ // Denoise Strong
		I2CWRITE_SN0_REGISTER(0x32F9, 0x84);
		I2CWRITE_SN0_REGISTER(0x32FA, 0x48);
		I2CWRITE_SN1_REGISTER(0x32F9, 0x84);
		I2CWRITE_SN1_REGISTER(0x32FA, 0x48);
	}
}
	#endif // SENSOR_DENOISE_COLOR

void fnCopySensorExposure()
{
	unsigned char SensorReg1;
	unsigned char SensorReg2;
	unsigned char SensorReg3;
	unsigned char SensorReg4;
	unsigned char SensorDataA;
	unsigned char SensorDataB;
	unsigned char SensorDataC;
	unsigned char SensorDataD;

	SensorDataA = SensorDataB = SensorDataC = SensorDataD = 1;

#ifdef SENSOR_3A_SYNC_SOURCE_SN0
	/* NTK K.H. 2014.04.10 - begin */
	{
		unsigned char	SENSOR_OB_AVG_R_H, SENSOR_OB_AVG_R_L;
		unsigned char	SENSOR_OB_AVG_Gr_H, SENSOR_OB_AVG_Gr_L;
		unsigned char	SENSOR_OB_AVG_Gb_H, SENSOR_OB_AVG_Gb_L;
		unsigned char	SENSOR_OB_AVG_B_H, SENSOR_OB_AVG_B_L;
		unsigned short	SENSOR_OB_AVG_R, SENSOR_OB_AVG_Gr, SENSOR_OB_AVG_Gb, SENSOR_OB_AVG_B, tmp;
		unsigned short	Sensor_OB_Th = 0x200;
		unsigned short	gOB_AVG = 0;		// 目前的

		I2CREAD_SN0_REGISTER(0x3044, SENSOR_OB_AVG_R_H);
		I2CREAD_SN0_REGISTER(0x3045, SENSOR_OB_AVG_R_L);
		I2CREAD_SN0_REGISTER(0x3046, SENSOR_OB_AVG_Gr_H);
		I2CREAD_SN0_REGISTER(0x3047, SENSOR_OB_AVG_Gr_L);
		I2CREAD_SN0_REGISTER(0x3048, SENSOR_OB_AVG_Gb_H);
		I2CREAD_SN0_REGISTER(0x3049, SENSOR_OB_AVG_Gb_L);
		I2CREAD_SN0_REGISTER(0x304A, SENSOR_OB_AVG_B_H);
		I2CREAD_SN0_REGISTER(0x304B, SENSOR_OB_AVG_B_L);

		SENSOR_OB_AVG_R    = (SENSOR_OB_AVG_R_H  << 8) | SENSOR_OB_AVG_R_L;
		SENSOR_OB_AVG_Gr   = (SENSOR_OB_AVG_Gr_H << 8) | SENSOR_OB_AVG_Gr_L;
		SENSOR_OB_AVG_Gb   = (SENSOR_OB_AVG_Gb_H << 8) | SENSOR_OB_AVG_Gb_L;
		SENSOR_OB_AVG_B    = (SENSOR_OB_AVG_B_H  << 8) | SENSOR_OB_AVG_B_L;

		gOB_AVG = (SENSOR_OB_AVG_R + SENSOR_OB_AVG_Gr + SENSOR_OB_AVG_Gb + SENSOR_OB_AVG_B) >> 2;
		if (gOB_AVG > 0x210) {
			I2CREAD_SN1_REGISTER(0x3053, tmp);
			tmp |= 0x80;
			I2CWRITE_SN1_REGISTER(0x3053, tmp);
		}
		if (gOB_AVG < 0x1F0) {
			I2CREAD_SN1_REGISTER(0x3053, tmp);
			tmp &= 0x7F;
			I2CWRITE_SN1_REGISTER(0x3053, tmp);
		}
	}
	/* NTK K.H. 2014.04.10 - end */

	// Read SENSOR0 Exposure Time, and Gain, then write to SENSOR1.
	I2CREAD_SN0_REGISTER(0x301C, SensorReg1);  //GAIN Digital
	I2CREAD_SN0_REGISTER(0x301D, SensorReg2);  //GAIN Analog
	I2CREAD_SN0_REGISTER(0x3012, SensorReg3);  //AECH
	I2CREAD_SN0_REGISTER(0x3013, SensorReg4);  //AECL
	while ((SensorReg1 != SensorDataA) || (SensorReg2 != SensorDataB) ||
		(SensorReg3 != SensorDataC) || (SensorReg4 != SensorDataD))
	{
		SensorDataA = SensorReg1;
		SensorDataB = SensorReg2;
		SensorDataC = SensorReg3;
		SensorDataD = SensorReg4;
		I2CREAD_SN0_REGISTER(0x301C, SensorReg1);  //GAIN Digital
		I2CREAD_SN0_REGISTER(0x301D, SensorReg2);  //GAIN Analog
		I2CREAD_SN0_REGISTER(0x3012, SensorReg3);  //AECH
		I2CREAD_SN0_REGISTER(0x3013, SensorReg4);  //AECL
	}
	I2CWRITE_SN1_REGISTER(0x301C, SensorReg1); //GAIN Digital
	I2CWRITE_SN1_REGISTER(0x301D, SensorReg2); //GAIN Analog
	I2CWRITE_SN1_REGISTER(0x3012, SensorReg3); //AECH
	I2CWRITE_SN1_REGISTER(0x3013, SensorReg4); //AECL
	I2CWRITE_SN1_REGISTER(0x3060, 0x02); //Update Register
	// NTK K.H. 2012-12-17 : modification start
	I2CREAD_SN0_REGISTER(0x32D2, SensorReg1);
	if (SensorReg1 & 0x80)
	{
		I2CWRITE_SN0_REGISTER(0x3060, 0x02);
	}
	// NTK K.H. 2012-12-17 : modification end
#else  // SENSOR_3A_SYNC_SOURCE_SN0
	/* NTK K.H. 2014.04.10 - begin */
	{
		unsigned char	SENSOR_OB_AVG_R_H, SENSOR_OB_AVG_R_L;
		unsigned char	SENSOR_OB_AVG_Gr_H, SENSOR_OB_AVG_Gr_L;
		unsigned char	SENSOR_OB_AVG_Gb_H, SENSOR_OB_AVG_Gb_L;
		unsigned char	SENSOR_OB_AVG_B_H, SENSOR_OB_AVG_B_L;
		unsigned short	SENSOR_OB_AVG_R, SENSOR_OB_AVG_Gr, SENSOR_OB_AVG_Gb, SENSOR_OB_AVG_B, tmp;
		unsigned short	Sensor_OB_Th = 0x200;
		unsigned short	gOB_AVG = 0;     // 目前的

		I2CREAD_SN1_REGISTER(0x3044, SENSOR_OB_AVG_R_H);
		I2CREAD_SN1_REGISTER(0x3045, SENSOR_OB_AVG_R_L);
		I2CREAD_SN1_REGISTER(0x3046, SENSOR_OB_AVG_Gr_H);
		I2CREAD_SN1_REGISTER(0x3047, SENSOR_OB_AVG_Gr_L);
		I2CREAD_SN1_REGISTER(0x3048, SENSOR_OB_AVG_Gb_H);
		I2CREAD_SN1_REGISTER(0x3049, SENSOR_OB_AVG_Gb_L);
		I2CREAD_SN1_REGISTER(0x304A, SENSOR_OB_AVG_B_H);
		I2CREAD_SN1_REGISTER(0x304B, SENSOR_OB_AVG_B_L);

		SENSOR_OB_AVG_R    = (SENSOR_OB_AVG_R_H  << 8) | SENSOR_OB_AVG_R_L;
		SENSOR_OB_AVG_Gr   = (SENSOR_OB_AVG_Gr_H << 8) | SENSOR_OB_AVG_Gr_L;
		SENSOR_OB_AVG_Gb   = (SENSOR_OB_AVG_Gb_H << 8) | SENSOR_OB_AVG_Gb_L;
		SENSOR_OB_AVG_B    = (SENSOR_OB_AVG_B_H  << 8) | SENSOR_OB_AVG_B_L;

		gOB_AVG = (SENSOR_OB_AVG_R + SENSOR_OB_AVG_Gr + SENSOR_OB_AVG_Gb + SENSOR_OB_AVG_B) >> 2;
		if (gOB_AVG > 0x210) {
			I2CREAD_SN0_REGISTER(0x3053, tmp);
			tmp |= 0x80;
			I2CWRITE_SN0_REGISTER(0x3053, tmp);
		}
		if (gOB_AVG < 0x1F0) {
			I2CREAD_SN0_REGISTER(0x3053, tmp);
			tmp &= 0x7F;
			I2CWRITE_SN0_REGISTER(0x3053, tmp);
		}
	}
	/* NTK K.H. 2014.04.10 - end */

	// Read SENSOR1 Exposure Time, and Gain, then write to SENSOR0.
	I2CREAD_SN1_REGISTER(0x301C, SensorReg1);  //GAIN Digital
	I2CREAD_SN1_REGISTER(0x301D, SensorReg2);  //GAIN Analog
	I2CREAD_SN1_REGISTER(0x3012, SensorReg3);  //AECH
	I2CREAD_SN1_REGISTER(0x3013, SensorReg4);  //AECL
	while ((SensorReg1 != SensorDataA) || (SensorReg2 != SensorDataB) ||
		(SensorReg3 != SensorDataC) || (SensorReg4 != SensorDataD))
	{
		SensorDataA = SensorReg1;
		SensorDataB = SensorReg2;
		SensorDataC = SensorReg3;
		SensorDataD = SensorReg4;
		I2CREAD_SN1_REGISTER(0x301C, SensorReg1);  //GAIN Digital
		I2CREAD_SN1_REGISTER(0x301D, SensorReg2);  //GAIN Analog
		I2CREAD_SN1_REGISTER(0x3012, SensorReg3);  //AECH
		I2CREAD_SN1_REGISTER(0x3013, SensorReg4);  //AECL
	}
	I2CWRITE_SN0_REGISTER(0x301C, SensorReg1); //GAIN Digital
	I2CWRITE_SN0_REGISTER(0x301D, SensorReg2); //GAIN Analog
	I2CWRITE_SN0_REGISTER(0x3012, SensorReg3); //AECH
	I2CWRITE_SN0_REGISTER(0x3013, SensorReg4); //AECL
	I2CWRITE_SN0_REGISTER(0x3060, 0x02); //Update Register
	// NTK K.H. 2012-12-17 : modification start
	I2CREAD_SN1_REGISTER(0x32D2, SensorReg1);
	if (SensorReg1 & 0x80)
	{
		I2CWRITE_SN1_REGISTER(0x3060, 0x02);
	}
	// NTK K.H. 2012-12-17 : modification end
#endif // SENSOR_3A_SYNC_SOURCE_SN0
}

typedef union {
	struct {
		unsigned char	c1;
		unsigned char	c0;
	} u8;
	unsigned short		u16;
} awb16;

#ifdef SENSOR_3A_SYNC_SOURCE_SN0
#define awb_rd_src		I2CREAD_SN0_REGISTER
#define awb_wr_dest		I2CWRITE_SN1_REGISTER
#else  // SENSOR_3A_SYNC_SOURCE_SN0
#define awb_rd_src		I2CREAD_SN1_REGISTER
#define awb_wr_dest		I2CWRITE_SN0_REGISTER
#endif // SENSOR_3A_SYNC_SOURCE_SN0

void fnCopySensorWhiteBalance()
{
	awb16	red, blue;
	awb16	rgain, bgain;
	awb16	wr, wb;

	rgain.u16 = bgain.u16 = 1;
	red.u16   = blue.u16  = 0;

	// read SOURCE white balance gain, then write to DESTINATION gain limits.
	while ((rgain.u16 != red.u16) || (bgain.u16 != blue.u16)) {
		awb_rd_src(0x3290, red.u8.c1);	// AWB RED  gain H
		awb_rd_src(0x3291, red.u8.c0);	// AWB RED  gain L
		awb_rd_src(0x3296, blue.u8.c1);	// AWB BLUE gain H
		awb_rd_src(0x3297, blue.u8.c0);	// AWB BLUE gain L
		rgain.u16 = red.u16;
		bgain.u16 = blue.u16;
	}

	wr.u16 = cwr;
	wb.u16 = cwb;
	if ((wr.u16 == 0) && (wb.u16 == 0)) {
		awb_wr_dest(0x32A1, rgain.u8.c1);	// AWB RED  gain Lower Limit H
		awb_wr_dest(0x32A2, rgain.u8.c0);	// AWB RED  gain Lower Limit L
		awb_wr_dest(0x32A3, rgain.u8.c1);	// AWB RED  gain Upper Limit H
		awb_wr_dest(0x32A4, rgain.u8.c0);	// AWB RED  gain Upper Limit L
		awb_wr_dest(0x32A5, bgain.u8.c1);	// AWB BLUE gain Lower Limit H
		awb_wr_dest(0x32A6, bgain.u8.c0);	// AWB BLUE gain Lower Limit L
		awb_wr_dest(0x32A7, bgain.u8.c1);	// AWB BLUE gain Upper Limit H
		awb_wr_dest(0x32A8, bgain.u8.c0);	// AWB BLUE gain Upper Limit L
	} else {
		red.u16  = ((rgain.u16 * wr.u16) >> 7);
		blue.u16 = ((bgain.u16 * wb.u16) >> 7);

		awb_wr_dest(0x32A1, red.u8.c1);		// AWB RED  gain Lower Limit H
		awb_wr_dest(0x32A2, red.u8.c0);		// AWB RED  gain Lower Limit L
		awb_wr_dest(0x32A3, red.u8.c1);		// AWB RED  gain Upper Limit H
		awb_wr_dest(0x32A4, red.u8.c0);		// AWB RED  gain Upper Limit L
		awb_wr_dest(0x32A5, blue.u8.c1);	// AWB BLUE gain Lower Limit H
		awb_wr_dest(0x32A6, blue.u8.c0);	// AWB BLUE gain Lower Limit L
		awb_wr_dest(0x32A7, blue.u8.c1);	// AWB BLUE gain Upper Limit H
		awb_wr_dest(0x32A8, blue.u8.c0);	// AWB BLUE gain Upper Limit L
	}
}

#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)

#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
unsigned short ucVsyncCount;
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
		#ifdef CHECK_SDRAM_BUFFERS
	if (nD2Mode == D2_OPERATION_ACCURATE_STITCHING)
		fnTraceSDRAMBuffers();
	else
		#endif // CHECK_SDRAM_BUFFERS
		fnD2IdleDelay(200); // count 200 around 24ms
	#endif // (SUPPORT_3A_SYNC == TRIGGER_ON_COUNTER)

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
		ucVsyncCount   = 100;
		ucSensor3ASync = SENSOR_3A_SYNC_CONFIG;
		fnD2TableVerify();
		break;
	case SENSOR_3A_SYNC_CONFIG:
		if (ucVsyncCount > 0)
		{
			ucVsyncCount--;
			return;
		}
		fnSensorAEAWBConfig();
		ucVsyncCount   = 5;
		ucSensor3ASync = SENSOR_3A_SYNC_AEC;
		break;
	case SENSOR_3A_SYNC_AEC:
		fnCopySensorExposure();
		fnD2TableVerify();
		ucSensor3ASync = SENSOR_3A_SYNC_AWB;
		break;
	case SENSOR_3A_SYNC_AWB:
		fnCopySensorWhiteBalance();
		#if defined(SENSOR_CHECK_DAY_NIGHT) || defined(SENSOR_DENOISE_COLOR)
		if (ucVsyncCount > 0)
		{
			ucVsyncCount--;
			return;
		}
		ucSensor3ASync = SENSOR_3A_CHECK_MODE;
		#else  // defined(SENSOR_CHECK_DAY_NIGHT) || defined(SENSOR_DENOISE_COLOR)
		ucSensor3ASync = SENSOR_3A_SYNC_AEC;
		#endif // defined(SENSOR_CHECK_DAY_NIGHT) || defined(SENSOR_DENOISE_COLOR)
		break;
	#if defined(SENSOR_CHECK_DAY_NIGHT) || defined(SENSOR_DENOISE_COLOR)
	case SENSOR_3A_CHECK_MODE:
		#ifdef SENSOR_CHECK_DAY_NIGHT
		fnCheckSensorDayNight();
		#endif // SENSOR_CHECK_DAY_NIGHT
		#ifdef SENSOR_DENOISE_COLOR
		fnCheckColorNoise();
		#endif // SENSOR_DENOISE_COLOR
		ucVsyncCount   = 30; // at least 2 times in a second
		ucSensor3ASync = SENSOR_3A_SYNC_AEC;
		break;
	#endif // defined(SENSOR_CHECK_DAY_NIGHT) || defined(SENSOR_DENOISE_COLOR)

	// for AWB calibration, by XYZ, 2014.01.22 - begin
	case SENSOR_3A_CALIBRATION:

		ucSensor3ASync = SENSOR_3A_SYNC_STOP;
		break;
	// for AWB calibration, by XYZ, 2014.01.22 - end

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


// for AWB calibration, by XYZ, 2014.01.22 - begin
void nt99141_3a_calibration(unsigned char mode)
{
	if (mode) {
		ucSensor3ASync = SENSOR_3A_CALIBRATION;
	} else {
		ucSensor3ASync = SENSOR_3A_SYNC_START;
	}
}
// for AWB calibration, by XYZ, 2014.01.22 - begin

#endif // SUPPORT_D2_OPERATION
