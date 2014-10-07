//
// Zealtek D2 project
// ZT3150@NT99050 solutions
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// Sep 17, 2012.

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
	// Programming 0x02 to register 0x3060 updates the following NT99050 registers:
	//  Integration (exposure) time, Analog and Digital gains
	//
	//#define SENSOR_SYNC_FRAMES
	//#define SENSOR_SYNC_ALWAYS

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

	#define DRIVING_ABILITY 0 // NTK99050 hardware default
	//#define DRIVING_ABILITY 1 // Tried this one for DVP97

	#define SENSOR_FRAMERATE_30
	#define SENSOR_OUTPUT_644x484
	#define PROJECT_DVP97

	//#define SENSOR_AE_SPEED 0x07 // Slowest Setting
	#define SENSOR_AE_SPEED 0x17 // Testing Setting
	//#define SENSOR_AE_SPEED 0x37 // Testing Setting
	//#define SENSOR_AE_SPEED 0x87 // NTK default Setting
	//#define SENSOR_AE_SPEED 0xF7 // Fastest Setting

	#define SENSOR_AGC_MAXGAIN 0x20
	//#define SENSOR_AGC_MAXGAIN 0x28

	// TWO_STEP_STITCHING allows D2 to get the stitching location from GPIO0,
	// and the related D2 registers are updated for far and near stitchings.
//	#define TWO_STEP_STITCHING
	#if defined(TWO_STEP_STITCHING) && !defined(SUPPORT_FUZZY_STITCHING)
		#error SUPPORT_FUZZY_STITCHING must be enabled for TWO_STEP_STITCHING
	#endif // defined(TWO_STEP_STITCHING) && !defined(SUPPORT_FUZZY_STITCHING)

tI2CRegister code I2CInitTableCommon[] =
{
#if 1
//I2CInitTable 20140403
{0x3100,0x23},
{0x3102,0x21},
{0x3106,0x07},
{0x3108,0x00},
{0x3113,0x4F},
{0x303E,0x04},
{0x303F,0x02},
{0x3040,0xFF},
{0x3041,0x08},
{0x3051,0xE0},
{0x3044,0x00},
{0x3045,0x00},
{0x3046,0x00},
{0x3047,0x00},
{0x3048,0x00},
{0x3049,0x00},
{0x304A,0x00},
{0x304B,0x00},
{0x3290,0x01},
{0x3291,0x77},
{0x3296,0x01},
{0x3297,0x5F},
{0x3270,0x08},
{0x3271,0x14},
{0x3272,0x20},
{0x3273,0x36},
{0x3274,0x4B},
{0x3275,0x5D},
{0x3276,0x7E},
{0x3277,0x98},
{0x3278,0xAC},
{0x3279,0xBD},
{0x327A,0xD4},
{0x327B,0xE5},
{0x327C,0xF0},
{0x327D,0xF9},
{0x327E,0xFF},
{0x3210,0x20},
{0x3211,0x20},
{0x3212,0x20},
{0x3213,0x20},
{0x3214,0x16},
{0x3215,0x16},
{0x3216,0x16},
{0x3217,0x16},
{0x321C,0x18},
{0x321D,0x18},
{0x321E,0x18},
{0x321F,0x18},
{0x3220,0x00},
{0x3221,0xA1},
{0x3222,0x00},
{0x3223,0xA1},
{0x3224,0x00},
{0x3225,0xA1},
{0x3226,0x79},
{0x3227,0x79},
{0x3228,0x79},
{0x3229,0x30},
{0x322A,0xFF},
{0x322B,0x01},
{0x322C,0x04},
{0x322D,0x02},
{0x3302,0x00},
{0x3303,0x4C},
{0x3304,0x00},
{0x3305,0x96},
{0x3306,0x00},
{0x3307,0x1D},
{0x3308,0x07},
{0x3309,0xD8},
{0x330A,0x07},
{0x330B,0x27},
{0x330C,0x01},
{0x330D,0x02},
{0x330E,0x00},
{0x330F,0xBC},
{0x3310,0x07},
{0x3311,0x6B},
{0x3312,0x07},
{0x3313,0xD9},
{0x3251,0x03},
{0x3252,0xFF},
{0x3253,0x00},
{0x3254,0x03},
{0x3255,0xFF},
{0x3256,0x00},
{0x3257,0x40},
{0x32C4,0x3a},
{0x32F6,0x4F},
{0x32F9,0x84},
{0x32FA,0x48},
{0x3338,0x2A},
{0x3339,0x77},
{0x333A,0x77},
{0x3363,0x37},
{0x3331,0x06},
{0x3332,0xFF},
{0x3360,0x08},
{0x3361,0x30},
{0x3362,0x74},
{0x3364,0x90},
{0x3365,0x80},
{0x3366,0x6F},
{0x3367,0x38},
{0x3368,0x88},
{0x3369,0x78},
{0x336A,0x68},
{0x336B,0x58},
{0x336C,0x55},
{0x336E,0x1A},
{0x336F,0x16},
{0x3370,0x28},
{0x3371,0x20},
{0x3372,0x30},
{0x3373,0x3F},
{0x3374,0x3f},
{0x3375,0x0A},
{0x3376,0x1A},
{0x3377,0x28},
{0x3378,0x38},
{0x3326,0x03},
{0x3327,0x04},
{0x3328,0x04},
{0x3329,0x02},
{0x332A,0x02},
{0x332B,0x1E},
{0x332C,0x1E},
{0x332d,0x00},
{0x332e,0x1F},
{0x332f,0x00},
{0x33E0,0x01},
{0x301E,0x10},
{0x3102,0x23},
#else
	{0x3100,0x23},
	{0x3102,0x21},
	{0x3106,0x07},
	{0x3108,0x00},
	{0x3113,0x4F},
	{0x303E,0x04},
	{0x303F,0x02},
	{0x3040,0xFF},
	{0x3041,0x08},
	{0x3051,0xE0},
	{0x3044,0x00},
	{0x3045,0x00},
	{0x3046,0x00},
	{0x3047,0x00},
	{0x3048,0x00},
	{0x3049,0x00},
	{0x304A,0x00},
	{0x304B,0x00},
	{0x3290,0x01},
	{0x3291,0x77},
	{0x3296,0x01},
	{0x3297,0x5F},
	{0x3270,0x08},   //Gamma begin
	{0x3271,0x14},
	{0x3272,0x20},
	{0x3273,0x36},
	{0x3274,0x4B},
	{0x3275,0x5D},
	{0x3276,0x7E},
	{0x3277,0x98},
	{0x3278,0xAC},
	{0x3279,0xBD},
	{0x327A,0xD4},
	{0x327B,0xE5},
	{0x327C,0xF0},
	{0x327D,0xF9},
	{0x327E,0xFF},   //Gamma end
	#ifdef PROJECT_DVP97
	{0x3210,0x2C},//0x20}, // 20121127 by NTK Cliff   //LSC begin
	{0x3211,0x30},//0x24}, // 20121127 by NTK Cliff
	{0x3212,0x2C},//0x24}, // 20121127 by NTK Cliff
	{0x3213,0x2C},//0x20}, // 20121127 by NTK Cliff
	{0x3214,0x18},
	{0x3215,0x18},
	{0x3216,0x18},
	{0x3217,0x18},
	{0x321C,0x18},
	{0x321D,0x18},
	{0x321E,0x18},
	{0x321F,0x18},
	{0x3220,0x00},
	{0x3221,0xA8},
	{0x3222,0x00},
	{0x3223,0xA8},
	{0x3224,0x00},
	{0x3225,0xA8},
	{0x3226,0x80},
	{0x3227,0x80},
	{0x3228,0x80},
	{0x3229,0x30},
	{0x322A,0xFF},//0xCF}, // 20121127 by NTK Cliff
	{0x322B,0x01},
	{0x322C,0x04},
	{0x322D,0x02},   //LSC end
	{0x3302,0x00},   //CC begin
	{0x3303,0x4C},
	{0x3304,0x00},
	{0x3305,0x96},
	{0x3306,0x00},
	{0x3307,0x1D},
	{0x3308,0x07},
	{0x3309,0xD9},
	{0x330A,0x07},
	{0x330B,0x20},
	{0x330C,0x01},
	{0x330D,0x08},
	{0x330E,0x00},
	{0x330F,0xB8},
	{0x3310,0x07},
	{0x3311,0x73},
	{0x3312,0x07},
	{0x3313,0xD6},   //CC end
	#else  // PROJECT_DVP97
	{0x3210,0x0D},   //LSC begin
	{0x3211,0x0F},
	{0x3212,0x0D},
	{0x3213,0x0D},
	{0x3214,0x04},
	{0x3215,0x05},
	{0x3216,0x04},
	{0x3217,0x04},
	{0x321C,0x04},
	{0x321D,0x05},
	{0x321E,0x04},
	{0x321F,0x03},
	{0x3220,0x00},
	{0x3221,0xA0},
	{0x3222,0x00},
	{0x3223,0xA0},
	{0x3224,0x00},
	{0x3225,0xA0},
	{0x3226,0x80},
	{0x3227,0x88},
	{0x3228,0x88},
	{0x3229,0x30},
	{0x322A,0xCF},
	{0x322B,0x07},
	{0x322C,0x04},
	{0x322D,0x02},   //LSC end
	{0x3302,0x00},   //CC begin
	{0x3303,0x4F},
	{0x3304,0x00},
	{0x3305,0xA0},
	{0x3306,0x00},
	{0x3307,0x12},
	{0x3308,0x07},
	{0x3309,0xDD},
	{0x330A,0x06},
	{0x330B,0xF1},
	{0x330C,0x01},
	{0x330D,0x32},
	{0x330E,0x00},
	{0x330F,0xF8},
	{0x3310,0x07},
	{0x3311,0x0F},
	{0x3312,0x07},
	{0x3313,0xF9},   //CC end
	#endif // PROJECT_DVP97
	{0x3257,0x50},
	{0x32C4,0x3a},   //IQ begin
	{0x32F6,0x4F},
	{0x32F9,0x84},
	{0x32FA,0x48},
	{0x3338,0x2A},
	{0x3339,0x77},
	{0x333A,0x77},
	{0x3363,0x37},
	{0x3331,0x08},
	{0x3332,0x60},
	{0x3360,0x08},
	{0x3361,0x30},
	{0x3362,0x74},
	{0x3364,0x90},
	{0x3365,0x80},
	{0x3366,0x6F},
	{0x3367,0x38},
	{0x3368,0x88},
	{0x3369,0x78},
	{0x336A,0x68},
	{0x336B,0x58},
	{0x336E,0x1A},
	{0x336F,0x16},
	{0x3370,0x28},
	{0x3371,0x20},
	{0x3372,0x30},
	{0x3373,0x3F},
	{0x3374,0x3f},
	{0x3375,0x0A},
	{0x3376,0x18},
	{0x3377,0x20},
	{0x3378,0x30},
	{0x3326,0x02},
	{0x3327,0x04},
	{0x3328,0x04},
	{0x3329,0x02},
	{0x332A,0x02},
	{0x332B,0x1E},
	{0x332C,0x1E},
	{0x332d,0x00},
	{0x332e,0x1F},
	{0x332f,0x00},   //IQ end
	{0x33E0,0x01},   //BDLC checks every frames.
	// The Exposure Time and Gains are updated on one of the following two cases:
	{0x301E,0x10},   //Updated on VSYNC after the writing 0x02 to register 0x3060.
	//{0x301E,0x00},   //Updated on the writing 0x02 to register 0x3060.
	// Driving ability
	//{0x3068, 0x01}, // 0:4mA, 1:8mA, 2:12mA, 3:16mA for I2C SCL and SDA
	{0x3069, DRIVING_ABILITY}, // 0:4mA, 1:8mA, 2:12mA, 3:16mA for HSYNC, VSYNC, and D0 - D9
	{0x306A, DRIVING_ABILITY}, // 0:4mA, 1:8mA, 2:12mA, 3:16mA for PCLK
#endif // for C_Y
};

tI2CRegister code I2CInitTable60HzVGA[] =
{
#if 1
//I2CInitTable60HzVGA    20140409
{0x32BF,0x04},
{0x32C0,0x48},
{0x32C1,0x48},
{0x32C2,0x48},
{0x32C3,0x00},
{0x32C4,0x30}, //Max Gain
{0x32C5,0x20},
{0x32C6,0x20},
{0x32C7,0x00},
{0x32C8,0xF8},
{0x32C9,0x48},
{0x32CA,0x68},
{0x32CB,0x68},
{0x32CC,0x68},
{0x32CD,0x68},
{0x32D0,0x01},
{0x3200,0x3E},
{0x3201,0x0F},	// AE/AWB off
{0x302A,0x00},
{0x302B,0x0B},
{0x302C,0x00},
{0x302D,0x02},
{0x3022,0x24},
{0x3023,0x24},
{0x3002,0x00},
{0x3003,0x02},
{0x3004,0x00},
{0x3005,0x02},
{0x3006,0x02},
{0x3007,0x85},
{0x3008,0x01},
{0x3009,0xE5},
{0x300A,0x03},
{0x300B,0x28},
{0x300C,0x03},
{0x300D,0xDE},
{0x300E,0x02},
{0x300F,0x84},
{0x3010,0x01},
{0x3011,0xE4},
{0x32B8,0x3B},
{0x32B9,0x2D},
{0x32BB,0x17}, //AE Speed
{0x32BC,0x34},
{0x32BD,0x38},
{0x32BE,0x30},
{0x3201,0x3F},
{0x320A,0x01},
{0x3021,0x06},
{0x3060,0x01},
#else
#ifdef SENSOR_FRAMERATE_30
	// Clock to sensor 24MHz, Clock to ZT3150 48MHz, Frame rate 30fps
	#ifdef SENSOR_OUTPUT_644x484
	{0x32BF,0x04},
	{0x32C0,0x48}, //0x50},
	{0x32C1,0x48}, //0x50},
	{0x32C2,0x48}, //0x50},
	{0x32C3,0x00},
	{0x32C4,SENSOR_AGC_MAXGAIN},
	{0x32C5,0x20},
	{0x32C6,0x20},
	{0x32C7,0x00},
	{0x32C8,0xF8},
	{0x32C9,0x48}, //0x50},
	{0x32CA,0x68}, //0x70},
	{0x32CB,0x68}, //0x70},
	{0x32CC,0x68}, //0x70},
	{0x32CD,0x68}, //0x70},
	{0x32D0,0x01},
	{0x3200,0x3E},
	{0x3201,0x0F},	// AE/AWB off
	{0x302A,0x00},
	{0x302B,0x0B},
	{0x302C,0x00},
	{0x302D,0x02},
	{0x3022,0x24},
	{0x3023,0x24},
	{0x3002,0x00}, //0x00},
	{0x3003,0x02}, //0x00},
	{0x3004,0x00}, //0x00},
	{0x3005,0x02}, //0x00},
	{0x3006,0x02}, //0x02},
	{0x3007,0x85}, //0x83},
	{0x3008,0x01}, //0x01},
	{0x3009,0xE5}, //0xE3},
	{0x300A,0x03},
	{0x300B,0x28},
	{0x300C,0x03},
	{0x300D,0xDE},
	{0x300E,0x02},
	{0x300F,0x84},
	{0x3010,0x01},
	{0x3011,0xE4},
	{0x32B8,0x3B},
	{0x32B9,0x2D},
	{0x32BB,SENSOR_AE_SPEED},
	{0x32BC,0x34},
	{0x32BD,0x38},
	{0x32BE,0x30},
	{0x3201,0x3F},
	{0x320A,0x01},
	{0x3021,0x06},
	{0x3060,0x01},
	#else  // SENSOR_OUTPUT_644x484
	{0x32BF,0x04},
	{0x32C0,0x60},
	{0x32C1,0x5F},
	{0x32C2,0x5F},
	{0x32C3,0x00},
	{0x32C4,SENSOR_AGC_MAXGAIN},
	{0x32C5,0x20},
	{0x32C6,0x20},
	{0x32C7,0x00},
	{0x32C8,0xBE},
	{0x32C9,0x5F},
	{0x32CA,0x7F},
	{0x32CB,0x7F},
	{0x32CC,0x7F},
	{0x32CD,0x80},
	{0x32D0,0x01},
	{0x3200,0x3E},
	{0x3201,0x0F},	// AE/AWB off
	{0x302A,0x00},
	{0x302B,0x0B},
	{0x302C,0x00},
	{0x302D,0x02},
	{0x3022,0x24},
	{0x3023,0x24},
	{0x3002,0x00},
	{0x3003,0x02},
	{0x3004,0x00},
	{0x3005,0x02},
	{0x3006,0x02},
	{0x3007,0x81},
	{0x3008,0x01},
	{0x3009,0xE3},//0xE1},
	{0x300A,0x04},
	{0x300B,0x1E},
	{0x300C,0x02},
	{0x300D,0xF7},
	{0x300E,0x02},
	{0x300F,0x80},
	{0x3010,0x01},
	{0x3011,0xE2},//0xE0},
	{0x32B8,0x3B},
	{0x32B9,0x2D},
	{0x32BB,SENSOR_AE_SPEED},
	{0x32BC,0x34},
	{0x32BD,0x38},
	{0x32BE,0x30},
	{0x3201,0x3F},	// AE/AWB on
	{0x320A,0x5E},
	{0x3021,0x06},
	{0x3060,0x01},
	#endif // SENSOR_OUTPUT_644x484
#else  // SENSOR_FRAMERATE_30
	// Clock to sensor 24MHz, Clock to ZT3150 24MHz, Frame rate 15fps
	{0x32BF,0x04},	// AEC & AGC controls start
	{0x32C0,0x6A},
	{0x32C1,0x6A},
	{0x32C2,0x6A},
	{0x32C3,0x00},
	{0x32C4,SENSOR_AGC_MAXGAIN},
	{0x32C5,0x20},
	{0x32C6,0x20},
	{0x32C7,0x00},
	{0x32C8,0x5F},
	{0x32C9,0x6A},
	{0x32CA,0x8A},
	{0x32CB,0x8A},
	{0x32CC,0x8A},
	{0x32CD,0x8A},
	{0x32D0,0x01},	// AEC & AGC controls end
	{0x3200,0x3E},
	{0x3201,0x0F},	// AE/AWB off
	{0x302A,0x00},
	{0x302B,0x09},
	{0x302C,0x00},
	{0x302D,0x04},
	{0x3022,0x24},
	{0x3023,0x24},
	{0x3002,0x00},
	{0x3003,0x02},
	{0x3004,0x00},
	{0x3005,0x02},
	{0x3006,0x02},
	{0x3007,0x81},
	{0x3008,0x01},
	{0x3009,0xE3},//0xE1},
	{0x300A,0x04},
	{0x300B,0x1E},
	{0x300C,0x02},
	{0x300D,0xF7},
	{0x300E,0x02},
	{0x300F,0x80},
	{0x3010,0x01},
	{0x3011,0xE2},//0xE0},
	{0x32B8,0x3B},	// AEC & AGC controls start
	{0x32B9,0x2D},
	{0x32BB,SENSOR_AE_SPEED},
	{0x32BC,0x34},
	{0x32BD,0x38},
	{0x32BE,0x30},	// AEC & AGC controls end
	{0x3201,0x3F},	// AE/AWB on
	{0x320A,0x5E},
	{0x3021,0x06},
	{0x3060,0x01},
#endif // SENSOR_FRAMERATE_30
#endif // for C_Y
};

tI2CRegister code I2CInitTable50HzVGA[] =
{
#if 1
//I2CInitTable50HzVGA  20140409
{0x32BF,0x04},
{0x32C0,0x48},
{0x32C1,0x48},
{0x32C2,0x48},
{0x32C3,0x00},
{0x32C4,0x30}, //Max Gain
{0x32C5,0x20},
{0x32C6,0x20},
{0x32C7,0x40},
{0x32C8,0x29},
{0x32C9,0x48},
{0x32CA,0x68},
{0x32CB,0x68},
{0x32CC,0x68},
{0x32CD,0x68},
{0x32D0,0x01},
{0x3200,0x3E},
{0x3201,0x0F},
{0x302A,0x00},
{0x302B,0x0B},
{0x302C,0x00},
{0x302D,0x02},
{0x3022,0x24},
{0x3023,0x24},
{0x3002,0x00},
{0x3003,0x02},
{0x3004,0x00},
{0x3005,0x02},
{0x3006,0x02},
{0x3007,0x85},
{0x3008,0x01},
{0x3009,0xE5},
{0x300A,0x03},
{0x300B,0x28},
{0x300C,0x03},
{0x300D,0xDE},
{0x300E,0x02},
{0x300F,0x84},
{0x3010,0x01},
{0x3011,0xE4},
{0x32B8,0x3B},
{0x32B9,0x2D},
{0x32BB,0x17}, //AE Speed
{0x32BC,0x34},
{0x32BD,0x38},
{0x32BE,0x30},
{0x3201,0x3F},
{0x320A,0x01},
{0x3021,0x06},
{0x3060,0x01},
#else
#ifdef SENSOR_FRAMERATE_30
	#ifdef SENSOR_OUTPUT_644x484
	{0x32BF,0x04},
	{0x32C0,0x48}, //0x50},
	{0x32C1,0x48}, //0x50},
	{0x32C2,0x48}, //0x50},
	{0x32C3,0x00},
	{0x32C4,SENSOR_AGC_MAXGAIN},
	{0x32C5,0x20},
	{0x32C6,0x20},
	{0x32C7,0x40},
	{0x32C8,0x29},
	{0x32C9,0x48}, //0x50},
	{0x32CA,0x68}, //0x70},
	{0x32CB,0x68}, //0x70},
	{0x32CC,0x68}, //0x70},
	{0x32CD,0x68}, //0x70},
	{0x32D0,0x01},
	{0x3200,0x3E},
	{0x3201,0x0F},
	{0x302A,0x00},
	{0x302B,0x0B},
	{0x302C,0x00},
	{0x302D,0x02},
	{0x3022,0x24},
	{0x3023,0x24},
	{0x3002,0x00}, //0x00},
	{0x3003,0x02}, //0x00},
	{0x3004,0x00}, //0x00},
	{0x3005,0x02}, //0x00},
	{0x3006,0x02}, //0x02},
	{0x3007,0x85}, //0x83},
	{0x3008,0x01}, //0x01},
	{0x3009,0xE5}, //0xE3},
	{0x300A,0x03},
	{0x300B,0x28},
	{0x300C,0x03},
	{0x300D,0xDE},
	{0x300E,0x02},
	{0x300F,0x84},
	{0x3010,0x01},
	{0x3011,0xE4},
	{0x32B8,0x3B},
	{0x32B9,0x2D},
	{0x32BB,SENSOR_AE_SPEED},
	{0x32BC,0x34},
	{0x32BD,0x38},
	{0x32BE,0x30},
	{0x3201,0x3F},
	{0x320A,0x01},
	{0x3021,0x06},
	{0x3060,0x01},
	#else  // SENSOR_OUTPUT_644x484
	// Clock to sensor 24MHz, Clock to ZT3150 48MHz, Frame rate 30fps
	{0x32BF,0x04},
	{0x32C0,0x5A},
	{0x32C1,0x5A},
	{0x32C2,0x5A},
	{0x32C3,0x00},
	{0x32C4,SENSOR_AGC_MAXGAIN},
	{0x32C5,0x20},
	{0x32C6,0x20},
	{0x32C7,0x00},
	{0x32C8,0xE4},
	{0x32C9,0x5A},
	{0x32CA,0x7A},
	{0x32CB,0x7A},
	{0x32CC,0x7A},
	{0x32CD,0x7A},
	{0x32D0,0x01},
	{0x3200,0x3E},
	{0x3201,0x0F},	// AE/AWB off
	{0x302A,0x00},
	{0x302B,0x0B},
	{0x302C,0x00},
	{0x302D,0x02},
	{0x3022,0x24},
	{0x3023,0x24},
	{0x3002,0x00},
	{0x3003,0x02},
	{0x3004,0x00},
	{0x3005,0x02},
	{0x3006,0x02},
	{0x3007,0x85},//0x81},
	{0x3008,0x01},
	{0x3009,0xE3},//0xE1},
	{0x300A,0x04},
	{0x300B,0x1E},
	{0x300C,0x02},
	{0x300D,0xF7},
	{0x300E,0x02},
	{0x300F,0x84},//0x80},
	{0x3010,0x01},
	{0x3011,0xE2},//0xE0},
	{0x32B8,0x3B},
	{0x32B9,0x2D},
	{0x32BB,SENSOR_AE_SPEED},
	{0x32BC,0x34},
	{0x32BD,0x38},
	{0x32BE,0x30},
	{0x3201,0x3F},	// AE/AWB on
	{0x320A,0x5E},
	{0x3021,0x06},
	{0x3060,0x01},
	#endif // SENSOR_OUTPUT_644x484
#else  // SENSOR_FRAMERATE_30
	// Clock to sensor 24MHz, Clock to ZT3150 24MHz, Frame rate 15fps
	{0x32BF,0x04},	// AEC & AGC controls start
	{0x32C0,0x6A},
	{0x32C1,0x6A},
	{0x32C2,0x6A},
	{0x32C3,0x00},
	{0x32C4,SENSOR_AGC_MAXGAIN},
	{0x32C5,0x20},
	{0x32C6,0x20},
	{0x32C7,0x00},
	{0x32C8,0x72},
	{0x32C9,0x6A},
	{0x32CA,0x8A},
	{0x32CB,0x8A},
	{0x32CC,0x8A},
	{0x32CD,0x8A},
	{0x32D0,0x01},	// AEC & AGC controls end
	{0x3200,0x3E},
	{0x3201,0x0F},	// AE/AWB off
	{0x302A,0x00},
	{0x302B,0x09},
	{0x302C,0x00},
	{0x302D,0x04},
	{0x3022,0x24},
	{0x3023,0x24},
	{0x3002,0x00},
	{0x3003,0x02},
	{0x3004,0x00},
	{0x3005,0x02},
	{0x3006,0x02},
	{0x3007,0x85},//0x81},
	{0x3008,0x01},
	{0x3009,0xE3},//0xE1},
	{0x300A,0x04},
	{0x300B,0x1E},
	{0x300C,0x02},
	{0x300D,0xF7},
	{0x300E,0x02},
	{0x300F,0x84},//0x80},
	{0x3010,0x01},
	{0x3011,0xE2},//0xE0},
	{0x32B8,0x3B},	// AEC & AGC controls start
	{0x32B9,0x2D},
	{0x32BB,SENSOR_AE_SPEED},
	{0x32BC,0x34},
	{0x32BD,0x38},
	{0x32BE,0x30},	// AEC & AGC controls end
	{0x3201,0x3F},	// AE/AWB on
	{0x320A,0x5E},
	{0x3021,0x06},
	{0x3060,0x01},
#endif // SENSOR_FRAMERATE_30
#endif // for C_Y
};

#ifdef TWO_STEP_STITCHING

#define IO_DEBOUNCE_COUNT 4

unsigned char xdata SiuOffset _at_ 0x023B;
unsigned char xdata SiuUpdate _at_ 0x0203;
unsigned char ucIOCount, ucIOState;

void fnGoFarLocation()
{
	SiuOffset = 0;
	SiuUpdate = 2;
	SiuUpdate = 0;
}

void fnGoNearLocation()
{
	SiuOffset = 12;//16;
	SiuUpdate = 2;
	SiuUpdate = 0;
}

#endif // TWO_STEP_STITCHING

#ifdef SENSOR_SYNC_FRAMES

unsigned short ucVsyncCount;

#endif // SENSOR_SYNC_FRAMES

#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
void fnCopySensorExposure();
void fnCopySensorWhiteBalance();
#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)

unsigned char fnDoInitSensor(
	unsigned char ucResolution)
{
#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	ucSensor3ASync = SENSOR_3A_SYNC_STOP;
#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	{
#ifndef TWO_STEP_STITCHING
		fnD2SetPIO0(0); // Set pin RESET low
#endif // TWO_STEP_STITCHING
		fnD2SetPIO1(0); // Set pin RESET low
		fnD2IdleDelay(200); // count 100 around 12ms
#if 0 // Not disable NT99050. Sensor Clock (Suggested by Salix Allen)
		D2DisableSensorClock();
#endif
		fnD2IdleDelay(200); // count 100 around 12ms
		D2WriteSensorClock(CLK_SENSOR);  // Set Sensor Clock in MHz
		fnD2IdleDelay(200); // count 100 around 12ms
#ifndef TWO_STEP_STITCHING
		fnD2SetPIO0(1); // Set pin RESET high
#endif // TWO_STEP_STITCHING
		fnD2SetPIO1(1); // Set pin RESET high
		fnD2IdleDelay(200); // count 100 around 12ms
		fnI2cWriteTable1(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister));
		fnI2cWriteTable0(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister));
	}
	ucResetControl = 0x74;               // 40:SIU Reset, 20:IPU Reset, 10:SOU Reset, 04: DRAM Reset
#ifdef SUPPORT_D2_PAN
	switch (ucResolution & (~D2_RESOLUTION_PAN))
#else  // SUPPORT_D2_PAN
	switch (ucResolution)
#endif // SUPPORT_D2_PAN
	{
	case D2_RESOLUTION_VGA_60Hz:
		fnI2cWriteTable1(I2CInitTable60HzVGA, sizeof(I2CInitTable60HzVGA) / sizeof(tI2CRegister));
		fnI2cWriteTable0(I2CInitTable60HzVGA, sizeof(I2CInitTable60HzVGA) / sizeof(tI2CRegister));
		break;
	case D2_RESOLUTION_VGA_50Hz:
		fnI2cWriteTable1(I2CInitTable50HzVGA, sizeof(I2CInitTable50HzVGA) / sizeof(tI2CRegister));
		fnI2cWriteTable0(I2CInitTable50HzVGA, sizeof(I2CInitTable50HzVGA) / sizeof(tI2CRegister));
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
	fnCopySensorWhiteBalance();
	fnCopySensorExposure();
#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
#ifdef TWO_STEP_STITCHING
	ucIOState = 0xF0;
#endif // TWO_STEP_STITCHING
	return SUCCESS;
}

#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)

void fnSensorAEAWBConfig()
{
	unsigned char SensorReg1;

	// Set AE in slave sensor OFF.
	#ifdef SENSOR_3A_SYNC_SOURCE_SN0
	I2CREAD_SN1_REGISTER(0x3201, SensorReg1);
	SensorReg1 &= 0xDF; // Disable SENSOR1 AE
	I2CWRITE_SN1_REGISTER(0x3201, SensorReg1);
	I2CWRITE_SN1_REGISTER(0x329B, 0x01); // Enable AWB Limit
	#else  // SENSOR_3A_SYNC_SOURCE_SN0
	I2CREAD_SN0_REGISTER(0x3201, SensorReg1);
	SensorReg1 &= 0xDF; // Disable SENSOR0 AE
	I2CWRITE_SN0_REGISTER(0x3201, SensorReg1);
	I2CWRITE_SN0_REGISTER(0x329B, 0x01); // Enable AWB Limit
	#endif // SENSOR_3A_SYNC_SOURCE_SN0
}

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
	#else  // SENSOR_3A_SYNC_SOURCE_SN0
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
	#endif // SENSOR_3A_SYNC_SOURCE_SN0
}

void fnCopySensorWhiteBalance()
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
	// Read SENSOR0 Whitebalance Gain, then write to SENSOR1 Gain Limits.
	I2CREAD_SN0_REGISTER(0x3290, SensorReg1);  //AWB RED gain H
	I2CREAD_SN0_REGISTER(0x3291, SensorReg2);  //AWB RED gain L
	I2CREAD_SN0_REGISTER(0x3296, SensorReg3);  //AWB BLUE gain H
	I2CREAD_SN0_REGISTER(0x3297, SensorReg4);  //AWB BLUE gain L
	while ((SensorReg1 != SensorDataA) || (SensorReg2 != SensorDataB) ||
		(SensorReg3 != SensorDataC) || (SensorReg4 != SensorDataD))
	{
		SensorDataA = SensorReg1;
		SensorDataB = SensorReg2;
		SensorDataC = SensorReg3;
		SensorDataD = SensorReg4;
		I2CREAD_SN0_REGISTER(0x3290, SensorReg1);  //AWB RED gain H
		I2CREAD_SN0_REGISTER(0x3291, SensorReg2);  //AWB RED gain L
		I2CREAD_SN0_REGISTER(0x3296, SensorReg3);  //AWB BLUE gain H
		I2CREAD_SN0_REGISTER(0x3297, SensorReg4);  //AWB BLUE gain L
	}
	I2CWRITE_SN1_REGISTER(0x32A1, SensorReg1); //AWB RED gain Lower Limit H
	I2CWRITE_SN1_REGISTER(0x32A2, SensorReg2); //AWB RED gain Lower Limit L
	I2CWRITE_SN1_REGISTER(0x32A3, SensorReg1); //AWB RED gain Upper Limit H
	I2CWRITE_SN1_REGISTER(0x32A4, SensorReg2); //AWB RED gain Upper Limit L
	I2CWRITE_SN1_REGISTER(0x32A5, SensorReg3); //AWB BLUE gain Lower Limit H
	I2CWRITE_SN1_REGISTER(0x32A6, SensorReg4); //AWB BLUE gain Lower Limit L
	I2CWRITE_SN1_REGISTER(0x32A7, SensorReg3); //AWB BLUE gain Upper Limit H
	I2CWRITE_SN1_REGISTER(0x32A8, SensorReg4); //AWB BLUE gain Upper Limit L
	#else  // SENSOR_3A_SYNC_SOURCE_SN0
	// Read SENSOR1 Whitebalance Gain, then write to SENSOR0 Gain Limits.
	I2CREAD_SN1_REGISTER(0x3290, SensorReg1);  //AWB RED gain H
	I2CREAD_SN1_REGISTER(0x3291, SensorReg2);  //AWB RED gain L
	I2CREAD_SN1_REGISTER(0x3296, SensorReg3);  //AWB BLUE gain H
	I2CREAD_SN1_REGISTER(0x3297, SensorReg4);  //AWB BLUE gain L
	while ((SensorReg1 != SensorDataA) || (SensorReg2 != SensorDataB) ||
		(SensorReg3 != SensorDataC) || (SensorReg4 != SensorDataD))
	{
		SensorDataA = SensorReg1;
		SensorDataB = SensorReg2;
		SensorDataC = SensorReg3;
		SensorDataD = SensorReg4;
		I2CREAD_SN1_REGISTER(0x3290, SensorReg1);  //AWB RED gain H
		I2CREAD_SN1_REGISTER(0x3291, SensorReg2);  //AWB RED gain L
		I2CREAD_SN1_REGISTER(0x3296, SensorReg3);  //AWB BLUE gain H
		I2CREAD_SN1_REGISTER(0x3297, SensorReg4);  //AWB BLUE gain L
	}
	I2CWRITE_SN0_REGISTER(0x32A1, SensorReg1); //AWB RED gain Lower Limit H
	I2CWRITE_SN0_REGISTER(0x32A2, SensorReg2); //AWB RED gain Lower Limit L
	I2CWRITE_SN0_REGISTER(0x32A3, SensorReg1); //AWB RED gain Upper Limit H
	I2CWRITE_SN0_REGISTER(0x32A4, SensorReg2); //AWB RED gain Upper Limit L
	I2CWRITE_SN0_REGISTER(0x32A5, SensorReg3); //AWB BLUE gain Lower Limit H
	I2CWRITE_SN0_REGISTER(0x32A6, SensorReg4); //AWB BLUE gain Lower Limit L
	I2CWRITE_SN0_REGISTER(0x32A7, SensorReg3); //AWB BLUE gain Upper Limit H
	I2CWRITE_SN0_REGISTER(0x32A8, SensorReg4); //AWB BLUE gain Upper Limit L
	#endif // SENSOR_3A_SYNC_SOURCE_SN0
}

#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)

void fnDoHouseKeeping()
{
#ifdef TWO_STEP_STITCHING
	if (nD2Mode == D2_OPERATION_ACCURATE_STITCHING)
	{
		if (ucIOState == fnD2GetPIO0()) // Check GPIO state
		{
			if (ucIOCount < IO_DEBOUNCE_COUNT)
				ucIOCount++;
			else
			{
				ucIOCount = 0;

//				if (ucIOState)
//					fnGoNearLocation();
//				else
//					fnGoFarLocation();
				if (ucIOState)
					fnGoFarLocation();
				else
					fnGoNearLocation();
			}
		}
		else
		{
			ucIOState = fnD2GetPIO0();
			ucIOCount = 0;
		}
	}
#endif // TWO_STEP_STITCHING
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
		ucVsyncCount   = 100;
	#else  // SENSOR_SYNC_FRAMES
		//fnD2IdleDelay(2500); // count 2500 around 300ms
	#endif // SENSOR_SYNC_FRAMES
		ucSensor3ASync = SENSOR_3A_SYNC_CONFIG;
		//fnD2TableVerify();
		break;
	#ifdef SENSOR_SYNC_FRAMES
	case SENSOR_3A_SYNC_CLOCK:
		#ifndef SENSOR_SYNC_ALWAYS
		fnD2IdleDelay(500); // count 5000 around 600ms
		ucSEN_CLK_SYNC = CLOCK_SYNC_VALUE_OFF;
		#endif // SENSOR_SYNC_ALWAYS
		ucSensor3ASync = SENSOR_3A_SYNC_AEC;
		break;
	#endif // SENSOR_SYNC_FRAMES
	case SENSOR_3A_SYNC_CONFIG:
		fnSensorAEAWBConfig();
	#ifdef SENSOR_SYNC_FRAMES
		ucSensor3ASync = SENSOR_3A_SYNC_CLOCK;
	#else  // SENSOR_SYNC_FRAMES
		ucSensor3ASync = SENSOR_3A_SYNC_AEC;
	#endif // SENSOR_SYNC_FRAMES
		break;
	case SENSOR_3A_SYNC_AEC:
		fnCopySensorExposure();
		//fnD2TableVerify();
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

#ifdef SUPPORT_D2_PAN
	fnPanInitial();
	#if 0
	fnSetOperatingMode(D2_OPERATION_ACCURATE_STITCHING, D2_RESOLUTION_PAN+D2_RESOLUTION_VGA_60Hz);
	nD2Mode       = D2_OPERATION_ACCURATE_STITCHING;
	nD2Resolution = D2_RESOLUTION_VGA_60Hz;
	#endif
#else  // SUPPORT_D2_PAN
	#if 0
	fnSetOperatingMode(D2_OPERATION_ACCURATE_STITCHING, D2_RESOLUTION_VGA_60Hz);
	nD2Mode       = D2_OPERATION_ACCURATE_STITCHING;
	nD2Resolution = D2_RESOLUTION_VGA_60Hz;
	#endif
#endif // SUPPORT_D2_PAN
}

#endif // SUPPORT_D2_OPERATION
