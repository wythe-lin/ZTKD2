//
// Zealtek D2 project
// ZT3150@OV7725 solutions
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// Sep 10, 2012.

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
	//#define SUPPORT_3A_SYNC       TRIGGER_ON_SN1_VSYNC
	//#define SUPPORT_3A_SYNC       TRIGGER_DISABLED

	//#define SENSOR_3A_SYNC_SOURCE_SN0
	#ifdef SENSOR_3A_SYNC_SOURCE_SN0
		#define CLOCK_SYNC_VALUE_ON  0x03
		#define CLOCK_SYNC_VALUE_OFF 0x00
	#else  // SENSOR_3A_SYNC_SOURCE_SN0
		#define CLOCK_SYNC_VALUE_ON  0x01
		#define CLOCK_SYNC_VALUE_OFF 0x00
	#endif // SENSOR_3A_SYNC_SOURCE_SN0

	// SENSOR_SYNC_FRAMES allows D2 to synchronize the VSYNC signals from both video sources.
	// One of the clock to video sensors holds a period of time to make VSYNC synchronization.
	//
	// SENSOR_SYNC_ALWAYS does not stops VSYNC synchronization. If this option is disabled,
	// VSYNC synchronization stops in a pre-defined period after the VSYNC synchronization starts.
	//
	// SENSOR_LAEC allows OV7725 sensor exposure time be set to less than 1 line.
	//
	#define SENSOR_SYNC_FRAMES
	#define SENSOR_SYNC_ALWAYS
	#define SENSOR_LAEC

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
//	#define CLK_SENSOR  48
	#if (CLK_PLL / CLK_SENSOR >= 16)
		#error Maximum Clock Divider for CLK_SENSOR is 16.
	#endif // (CLK_PLL / CLK_SENSOR >= 16)

	#if ZT3150_SOLUTION == ZT3150_SOLUTION_CCTV
		#define D2_RESOLUTION_QVGA_60Hz (D2_RESOLUTION_VGA_60Hz+D2_RESOLUTION_QVGA_OFFSET)
		#define D2_RESOLUTION_QVGA_50Hz (D2_RESOLUTION_VGA_50Hz+D2_RESOLUTION_QVGA_OFFSET)
	#endif // ZT3150_SOLUTION == ZT3150_SOLUTION_CCTV


tI2CRegister code ov7725_rst[] = {
	{0x12, 0x80},
};

tI2CRegister code I2CInitTableCommon[] = {
	{0x3d, 0x03},	// DC offset for analog process
	{0x17, 0x22},	// HSTART
	{0x18, 0xa4},	// HSIZE
	{0x19, 0x07},	// VSTRT
	{0x1a, 0xf0},	// VSIZE
	{0x32, 0x00},	// HREF
	{0x29, 0xa0},	// HOutSize
	{0x2c, 0xf0},	// VOutSize
	{0x2a, 0x00},	// dummy pixel
	{0x11, 0x01},	// 00/01/03/07 for 60/30/15/7.5fps

	{0x0c, 0x00},	// [7]: flip, [6]: mirror

	{0x42, 0x7f},
	{0x4d, 0x09},
	{0x63, 0xe0},  // AWB control
	{0x64, 0xff},  // DSP_Ctrl1
	{0x65, 0x20},  // DSP_Ctrl2
	{0x66, 0x00},  // DSP_Ctrl3
	{0x67, 0x48},  // DSP_Ctrl4

	{0x13, 0xf0},
#if ZT3150_SOLUTION == ZT3150_SOLUTION_CCTV
	{0x0d, 0x41},	// 0x51/0x61/0x71 for different AEC/AGC window
#else
	{0x0d, 0x41},	// for 24MHz input clock, 0x51/0x61/0x71 for different AEC/AGC window
#endif
	{0x0f, 0xc5},	// COM6
	{0x14, 0x11},	// 4x maximum gain
	{0x22, 0x7f},	// ff/7f/3f/1f for 60/30/15/7.5fps
	{0x23, 0x03},	// 01/03/07/0f for 60/30/15/7.5fps
	{0x24, 0x40},	// AEW
	{0x25, 0x30},	// AEB
	{0x26, 0xa1},	// VPT
	{0x2b, 0x00},

	{0x6b, 0xaa},
	{0x13, 0xff},

	{0x90, 0x05},	// EDGE1
	{0x91, 0x01},   // DNSOFF
	{0x92, 0x03},   // EDGE2
	{0x93, 0x00},   // EDGE3

	{0x94, 0xb0},   // color matrix, MTX1
	{0x95, 0x9d},   // color matrix, MTX2
	{0x96, 0x13},   // color matrix, MTX3
	{0x97, 0x16},   // color matrix, MTX4
	{0x98, 0x7b},   // color matrix, MTX5
	{0x99, 0x91},   // color matrix, MTX6
	{0x9a, 0x1e},   // color matrix control

	{0x9b, 0x08},   // brightness
	{0x9c, 0x20},   // contrast
	{0x9e, 0x81},	// UV adjustment controls
	{0xa6, 0x06},	// Special Digital Effect (SDE) Control, contrast/brightness/saturation/hue

	// Gamma
	{0x7e, 0x0c},	// GAM1
	{0x7f, 0x16},	// GAM2
	{0x80, 0x2a},	// GAM3
	{0x81, 0x4e},	// GAM4
	{0x82, 0x61},	// GAM5
	{0x83, 0x6f},	// GAM6
	{0x84, 0x7b},	// GAM7
	{0x85, 0x86},	// GAM8
	{0x86, 0x8e},	// GAM9
	{0x87, 0x97},	// GAM10
	{0x88, 0xa4},	// GAM11
	{0x89, 0xaf},	// GAM12
	{0x8a, 0xc5},	// GAM13
	{0x8b, 0xd7},	// GAM14
	{0x8c, 0xe8},	// GAM15
	{0x8d, 0x20},	// SLOP

	// Lens Correction, should be tuned with real camera module
	{0x4a, 0x10},	// LC_RADI
	{0x49, 0x10},	// LC_COEF
	{0x4b, 0x14},	// LC_COEFB
	{0x4c, 0x17},	// LC_COEFR
	{0x46, 0x05},	// Bit[0]: Lens correction enable

//	// AWB Correction
//	{0x69, 0x5c},
//	{0x6a, 0x11},
//	{0x6b, 0xa2},
//	{0x6c, 0x01},
//	{0x6d, 0x50},
//	{0x6e, 0x80},
//	{0x6f, 0x80},
//	{0x70, 0x0f},
//	{0x71, 0x00},
//	{0x72, 0x00},
//	{0x73, 0x0f},
//	{0x74, 0x0f},
//	{0x75, 0xff},
//	{0x76, 0x00},
//	{0x77, 0x10},
//	{0x78, 0x10},
//	{0x79, 0x70},
//	{0x7a, 0x70},
//	{0x7b, 0xf0},
//	{0x7c, 0xf0},
//	{0x7d, 0xf0},

	{0x0e, 0x55/*0x65*/},
};

tI2CRegister code I2CInitTable50HzVGA[] =
{
	//{0x2B,0x92},
	// for 25 fps, 50Hz
	{0x33, 0x66},
	{0x22, 0x99},
	{0x23, 0x03},

	{0xa0, 0x02},
	{0xa1, 0x50},
	{0xa2, 0x40},
};

tI2CRegister code I2CInitTable60HzVGA[] =
{
	//{0x2B,0x00},
	// for 30 fps, 60Hz
	{0x33, 0x00},
	{0x22, 0x7F},
	{0x23, 0x03},

	{0xa0, 0x02},
	{0xa1, 0x50},
	{0xa2, 0x40},
};

#if ZT3150_SOLUTION == ZT3150_SOLUTION_CCTV
tI2CRegister code I2CInitTable50HzQVGA[] =
{
	//{0x2B,0x92},
	// for 25 fps, 50Hz
	{0x33, 0x66},
	{0x22, 0x99},
	{0x23, 0x03},
	//320x240
	{0x29, 0x50},
	{0x2c, 0x79},  //default = 78, for more line by jim 20080813
	{0x65, 0x2f},
	{0xa0, 0x05},
	{0xa1, 0x40},
	{0xa2, 0x40},
};

tI2CRegister code I2CInitTable60HzQVGA[] =
{
	{0x0d ,0x00},  //AEC/AGC window
	//{0x2B,0x00},
	// for 30 fps, 60Hz
	{0x33, 0x00},
	{0x22, 0x7F},
	{0x23, 0x03},
	//320x240
	{0x29, 0x50},
	{0x2c, 0x79},  //default = 78, for more line by jim 20080813
	{0x65, 0x2f},
	{0xa0, 0x05},
	{0xa1, 0x40},
	{0xa2, 0x40},
};
#endif // ZT3150_SOLUTION == ZT3150_SOLUTION_CCTV


#ifdef SENSOR_SYNC_FRAMES
unsigned short ucVsyncCount;
#endif // SENSOR_SYNC_FRAMES

unsigned char fnDoInitSensor(
	unsigned char ucResolution)
{
#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	ucSensor3ASync = SENSOR_3A_SYNC_STOP;
#endif // (SUPPORT_3A_SYNC > TRIGGER_DISABLED)

	fnD2SetPIO0(0);			// Set pin RESET low, by xyz 2014.09.01
	fnD2SetPIO1(0);			// Set pin RESET low, by xyz 2014.09.01

	D2WriteSensorClock(CLK_SENSOR);  // Set Sensor Clock in MHz
	fnD2IdleDelay(100);              // count 100 around 12ms

	fnD2SetPIO0(1);			// Set pin RESET high, by xyz 2014.09.01
	fnD2SetPIO1(1);			// Set pin RESET high, by xyz 2014.09.01
	fnD2IdleDelay(10);		// count 10 around 1.2ms, by xyz 2014.09.01

	fnI2cWriteTable0(ov7725_rst, sizeof(ov7725_rst) / sizeof(tI2CRegister));
	fnI2cWriteTable1(ov7725_rst, sizeof(ov7725_rst) / sizeof(tI2CRegister));
	fnD2IdleDelay(50);		// count 50 around 6ms, by xyz 2014.09.30

	ucResetControl = 0x74;           // 40:SIU Reset, 20:IPU Reset, 10:SOU Reset, 04: DRAM Reset
#ifdef SUPPORT_D2_PAN
	switch (ucResolution & (~D2_RESOLUTION_PAN))
#else  // SUPPORT_D2_PAN
	switch (ucResolution)
#endif // SUPPORT_D2_PAN
	{
	case D2_RESOLUTION_VGA_60Hz:
		fnI2cWriteTable1(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister));
		fnI2cWriteTable1(I2CInitTable60HzVGA, sizeof(I2CInitTable60HzVGA) / sizeof(tI2CRegister));
		fnI2cWriteTable0(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister));
		fnI2cWriteTable0(I2CInitTable60HzVGA, sizeof(I2CInitTable60HzVGA) / sizeof(tI2CRegister));
		break;
	case D2_RESOLUTION_VGA_50Hz:
		fnI2cWriteTable1(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister));
		fnI2cWriteTable1(I2CInitTable50HzVGA, sizeof(I2CInitTable50HzVGA) / sizeof(tI2CRegister));
		fnI2cWriteTable0(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister));
		fnI2cWriteTable0(I2CInitTable50HzVGA, sizeof(I2CInitTable50HzVGA) / sizeof(tI2CRegister));
		break;
#if ZT3150_SOLUTION == ZT3150_SOLUTION_CCTV
	case D2_RESOLUTION_QVGA_60Hz:
		fnI2cWriteTable1(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister));
		fnI2cWriteTable1(I2CInitTable60HzQVGA, sizeof(I2CInitTable60HzQVGA) / sizeof(tI2CRegister));
		fnI2cWriteTable0(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister));
		fnI2cWriteTable0(I2CInitTable60HzQVGA, sizeof(I2CInitTable60HzQVGA) / sizeof(tI2CRegister));
		break;
	case D2_RESOLUTION_QVGA_50Hz:
		fnI2cWriteTable1(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister));
		fnI2cWriteTable1(I2CInitTable50HzQVGA, sizeof(I2CInitTable50HzQVGA) / sizeof(tI2CRegister));
		fnI2cWriteTable0(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister));
		fnI2cWriteTable0(I2CInitTable50HzQVGA, sizeof(I2CInitTable50HzQVGA) / sizeof(tI2CRegister));
		break;
#endif // ZT3150_SOLUTION == ZT3150_SOLUTION_CCTV
	default:
		ucResetControl = 0x00;
		return FAILURE;
	}
	ucResetControl = 0x00;
#if (SUPPORT_3A_SYNC > TRIGGER_DISABLED)
	#ifdef SENSOR_SYNC_FRAMES
	ucSEN_CLK_SYNC = CLOCK_SYNC_VALUE_OFF;
	ucVsyncCount   = 10;
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
	I2CREAD_SN1_REGISTER(0x13, SensorReg1);
	SensorReg1 &= 0xF8;
	I2CWRITE_SN1_REGISTER(0x13, SensorReg1);
	#else  // SENSOR_3A_SYNC_SOURCE_SN0
	// Disable SENSOR0 3A
	I2CREAD_SN0_REGISTER(0x13, SensorReg1);
	SensorReg1 &= 0xF8;
	I2CWRITE_SN0_REGISTER(0x13, SensorReg1);
	#endif // SENSOR_3A_SYNC_SOURCE_SN0
}

void fnCopySensorExposure()
{
	unsigned char SensorReg1;
	unsigned char SensorReg2;
	unsigned char SensorReg3;
	#ifdef SENSOR_LAEC
	unsigned char SensorReg4;
	#endif // SENSOR_LAEC
	unsigned char SensorDataA = 1;
	unsigned char SensorDataB = 1;
	unsigned char SensorDataC = 1;
	#ifdef SENSOR_LAEC
	unsigned char SensorDataD = 1;
	#endif // SENSOR_LAEC

	#ifdef SENSOR_3A_SYNC_SOURCE_SN0
	// Read SENSOR0 Exposure Time, and Gain, then write to SENSOR1.
	I2CREAD_SN0_REGISTER(0x00, SensorReg1);  //GAIN
	I2CREAD_SN0_REGISTER(0x08, SensorReg2);  //AECH
	I2CREAD_SN0_REGISTER(0x10, SensorReg3);  //AEC
		#ifdef SENSOR_LAEC
	I2CREAD_SN0_REGISTER(0x1F, SensorReg4);  //LAEC
		#endif // SENSOR_LAEC
	while ((SensorReg1 != SensorDataA) || (SensorReg2 != SensorDataB) || (SensorReg3 != SensorDataC)
		#ifdef SENSOR_LAEC
		|| (SensorReg4 != SensorDataD)
		#endif // SENSOR_LAEC
		)
	{
		SensorDataA = SensorReg1;
		SensorDataB = SensorReg2;
		SensorDataC = SensorReg3;
		#ifdef SENSOR_LAEC
		SensorDataD = SensorReg4;
		#endif // SENSOR_LAEC
		I2CREAD_SN0_REGISTER(0x00, SensorReg1);  //GAIN
		I2CREAD_SN0_REGISTER(0x08, SensorReg2);  //AECH
		I2CREAD_SN0_REGISTER(0x10, SensorReg3);  //AEC
		#ifdef SENSOR_LAEC
		I2CREAD_SN0_REGISTER(0x1F, SensorReg4);  //LAEC
		#endif // SENSOR_LAEC
	}
	I2CWRITE_SN1_REGISTER(0x00, SensorReg1); //GAIN
	I2CWRITE_SN1_REGISTER(0x08, SensorReg2); //AECH
	I2CWRITE_SN1_REGISTER(0x10, SensorReg3); //AEC
		#ifdef SENSOR_LAEC
	I2CWRITE_SN1_REGISTER(0x1F, SensorReg4); //LAEC
		#endif // SENSOR_LAEC
	#else  // SENSOR_3A_SYNC_SOURCE_SN0
	// Read SENSOR1 Exposure Time, and Gain, then write to SENSOR0.
	I2CREAD_SN1_REGISTER(0x00, SensorReg1);  //GAIN
	I2CREAD_SN1_REGISTER(0x08, SensorReg2);  //AECH
	I2CREAD_SN1_REGISTER(0x10, SensorReg3);  //AEC
		#ifdef SENSOR_LAEC
	I2CREAD_SN1_REGISTER(0x1F, SensorReg4);  //LAEC
		#endif // SENSOR_LAEC
	while ((SensorReg1 != SensorDataA) || (SensorReg2 != SensorDataB) || (SensorReg3 != SensorDataC)
		#ifdef SENSOR_LAEC
		|| (SensorReg4 != SensorDataD)
		#endif // SENSOR_LAEC
		)
	{
		SensorDataA = SensorReg1;
		SensorDataB = SensorReg2;
		SensorDataC = SensorReg3;
		#ifdef SENSOR_LAEC
		SensorDataD = SensorReg4;
		#endif // SENSOR_LAEC
		I2CREAD_SN1_REGISTER(0x00, SensorReg1);  //GAIN
		I2CREAD_SN1_REGISTER(0x08, SensorReg2);  //AECH
		I2CREAD_SN1_REGISTER(0x10, SensorReg3);  //AEC
		#ifdef SENSOR_LAEC
		I2CREAD_SN1_REGISTER(0x1F, SensorReg4);  //LAEC
		#endif // SENSOR_LAEC
	}
	I2CWRITE_SN0_REGISTER(0x00, SensorReg1); //GAIN
	I2CWRITE_SN0_REGISTER(0x08, SensorReg2); //AECH
	I2CWRITE_SN0_REGISTER(0x10, SensorReg3); //AEC
		#ifdef SENSOR_LAEC
	I2CWRITE_SN0_REGISTER(0x1F, SensorReg4); //LAEC
		#endif // SENSOR_LAEC
	#endif // SENSOR_3A_SYNC_SOURCE_SN0
}

void fnCopySensorWhiteBalance()
{
	unsigned char SensorReg1;
	unsigned char SensorReg2;
	unsigned char SensorReg3;
	unsigned char SensorDataA = 1;
	unsigned char SensorDataB = 1;
	unsigned char SensorDataC = 1;

	#ifdef SENSOR_3A_SYNC_SOURCE_SN0
	// Read SENSOR0 Whitebalance Gain, then write to SENSOR1.
	I2CREAD_SN0_REGISTER(0x01, SensorReg1);  //BLUE
	I2CREAD_SN0_REGISTER(0x02, SensorReg2);  //RED
	I2CREAD_SN0_REGISTER(0x03, SensorReg3);  //GREEN
	while ((SensorReg1 != SensorDataA) || (SensorReg2 != SensorDataB)
		 || (SensorReg3 != SensorDataC))
	{
		SensorDataA = SensorReg1;
		SensorDataB = SensorReg2;
		SensorDataC = SensorReg3;
		I2CREAD_SN0_REGISTER(0x01, SensorReg1);  //BLUE
		I2CREAD_SN0_REGISTER(0x02, SensorReg2);  //RED
		I2CREAD_SN0_REGISTER(0x03, SensorReg3);  //GREEN
	}
	I2CWRITE_SN1_REGISTER(0x01, SensorReg1); //BLUE
	I2CWRITE_SN1_REGISTER(0x02, SensorReg2); //RED
	I2CWRITE_SN1_REGISTER(0x03, SensorReg3); //GREEN
	#else  // SENSOR_3A_SYNC_SOURCE_SN0
	// Read SENSOR1 Whitebalance Gain, then write to SENSOR0.
	I2CREAD_SN1_REGISTER(0x01, SensorReg1);  //BLUE
	I2CREAD_SN1_REGISTER(0x02, SensorReg2);  //RED
	I2CREAD_SN1_REGISTER(0x03, SensorReg3);  //GREEN
	while ((SensorReg1 != SensorDataA) || (SensorReg2 != SensorDataB)
		 || (SensorReg3 != SensorDataC))
	{
		SensorDataA = SensorReg1;
		SensorDataB = SensorReg2;
		SensorDataC = SensorReg3;
		I2CREAD_SN1_REGISTER(0x01, SensorReg1);  //BLUE
		I2CREAD_SN1_REGISTER(0x02, SensorReg2);  //RED
		I2CREAD_SN1_REGISTER(0x03, SensorReg3);  //GREEN
	}
	I2CWRITE_SN0_REGISTER(0x01, SensorReg1); //BLUE
	I2CWRITE_SN0_REGISTER(0x02, SensorReg2); //RED
	I2CWRITE_SN0_REGISTER(0x03, SensorReg3); //GREEN
	#endif // SENSOR_3A_SYNC_SOURCE_SN0
}

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
		ucSensor3ASync = SENSOR_3A_SYNC_CONFIG;
	#endif // SENSOR_SYNC_FRAMES
		fnD2TableVerify();
		break;
	#ifdef SENSOR_SYNC_FRAMES
	case SENSOR_3A_SYNC_CLOCK:
		#ifndef SENSOR_SYNC_ALWAYS
		ucSEN_CLK_SYNC = CLOCK_SYNC_VALUE_OFF;
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

#ifdef SUPPORT_D2_PAN
	fnPanInitial();
#endif // SUPPORT_D2_PAN
}

#endif // SUPPORT_D2_OPERATION
