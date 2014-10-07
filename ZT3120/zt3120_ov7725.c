//
// Zealtek D2 project
// ZT3120@OV7725 solutions
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

	// SENSOR_LAEC allows OV7725 sensor exposure time be set to less than 1 line.
	//
	#define SENSOR_LAEC

	#define CLK_SENSOR  24
	#if (CLK_PLL / CLK_SENSOR >= 16)
		#error Maximum Clock Divider for CLK_SENSOR is 16.
	#endif // (CLK_PLL / CLK_SENSOR >= 16)


	#if ZT3120_DECODER == ZT3120_DECODER_ADV7180

tI2CRegister0 code I2CInitTableNTSC[] =
{
	//{0x00, 0x04},	// Composite decoder Channel; 0000b = auto detect PAL B/G/H/I/D, NTSC J, SECAM
	//{0x00, 0x14},	// Composite decoder Channel; 0001b = auto detect PAL B/G/H/I/D, NTSC M, SECAM
	{0x00, 0x54},	// Composite decoder Channel; 0101b = NTSC M
	//{0x04, 0x57},	// Enable SFL
	{0x04, 0xD7},	// To fix display blank issue.
	{0x17, 0x41}, // Select SH1
	{0x31, 0x02}, // Clear NEWAVMODE, SAV/EAV to suit ADV video encoders
	{0x3d, 0xa2}, // Color Kill
	{0x3e, 0x6a}, // BLM optimization
	{0x3f, 0xa0}, // BGB optimization
	{0x0e, 0x80}, // Enter hidden space
	{0x55, 0x81}, // ADC configuration
	{0x0e, 0x00}, // Enter user space
	//{0x08, 0x88}, // Contrast
};


tI2CRegister0 code I2CInitTablePAL[] =
{
	//{0x00, 0x04},	// Composite decoder Channel; 0000b = auto detect PAL B/G/H/I/D, NTSC J, SECAM
	//{0x00, 0x14},	// Composite decoder Channel; 0001b = auto detect PAL B/G/H/I/D, NTSC M, SECAM
	{0x00, 0x84},	// Composite decoder Channel; 1000b = PAL B/G/H/I/D
	//{0x04, 0x57},	// Enable SFL
	{0x04, 0xD7},	// To fix display blank issue.
	{0x17, 0x41}, // Select SH1
	{0x31, 0x02}, // Clear NEWAVMODE, SAV/EAV to suit ADV video encoders
	{0x3d, 0xa2}, // Color Kill
	{0x3e, 0x6a}, // BLM optimization
	{0x3f, 0xa0}, // BGB optimization
	{0x0e, 0x80}, // Enter hidden space
	{0x55, 0x81}, // ADC configuration
	{0x0e, 0x00}, // Enter user space
	//{0x08, 0x88}, // Contrast
};

	#elif ZT3120_DECODER == ZT3120_DECODER_T515B

tI2CRegister0 code I2CInitTableNTSC[] =
{
	{0x5F, 0x01},			//RESET
	{0x0A, 0x20},
	{0x0F, 0x00},
	{0x11, 0x0C},
	{0x18, 0x00},			//$B0,W,Y,$0000,18,00,48,83,
	{0x19, 0x0A},			//0x0A, 0x08
	//{0x19, 0x08},
	//{0x1A, 0x83},
	{0x20, 0x00},			//Video Source: NTSC
	{0x21, 0x09},
	{0x59, 0x8A},			//Video 0:Odd Fields; 1:Even Fields
	//{0x59, 0x0A},			//Video 1:Odd Fields; 0:Even Fields
	{0xD2, 0x26},
	{0xD6, 0x01},
	{0xE0, 0xB0},

	{0x38, 0x21},
	{0x39, 0xF0},
	{0x3A, 0x7C},
	{0x3B, 0x0F},
	//{0x27, 0x20},		//blue screen 0x20:auto,  0x10:enable
	{0x5F, 0x00},			//Release RESET
};


tI2CRegister0 code I2CInitTablePAL[] =
{
	{0x5F, 0x01},			//RESET
	{0x0A, 0x20},
	{0x0F, 0x00},
	{0x11, 0x0C},
	{0x18, 0x00},			//$B0,W,Y,$0000,18,00,48,83,
	{0x19, 0x0A},			//0x0A, 0x08
	//{0x19, 0x08},
	//{0x1A, 0x83},
	{0x20, 0x12},			//Video Source: PAL-I,B,G,H.D,N
	//{0x20, 0x18},			//Video Source: SECAM
	{0x21, 0x09},
	{0x59, 0x8A},			//Video 0:Odd Fields; 1:Even Fields
	//{0x59, 0x0A},			//Video 1:Odd Fields; 0:Even Fields
	{0xD2, 0x26},
	{0xD6, 0x01},
	{0xE0, 0xB0},
	
	{0x38, 0x2A},
	{0x39, 0x09},
	{0x3A, 0x8A},
	{0x3B, 0xCB},
	//{0x27, 0x20},		//blue screen 0x20:auto,  0x10:enable
	{0x5F, 0x00},			//Release RESET
};

	#endif // ZT3120_DECODER


tI2CRegister1 code I2CInitTableCommon[] =
{
	{0x12, 0x80},

	{0x3d, 0x03},  //DC offset for analog process
	{0x17, 0x22},  //HSTART
	{0x18, 0xa4},  //HSIZE
	{0x19, 0x07},  //VSTRT	;default = 7, for more line by jim 20080813
	{0x1a, 0xf1},  //VSIZE	;default = f0, for more line by jim 20080813
	{0x32, 0x00},  //HREF
	{0x29, 0xa0},  //HOutSize
	{0x2c, 0xf0},  //VOutSize
	{0x2a, 0x00},  //dummy pixel
	{0x11, 0x40},  //external clock

	{0x3E, 0x60},  // ABLC, ADBLC enabled, active on changing gain.
	//{0x3E, 0x00},  // ABLC, ADBLC disabled.

	////{0x42, 0x7f},  //BLC blue channel
	{0x64, 0xff},  //DSP_Ctrl1
	{0x65, 0x20},  //DSP_Ctrl2
	{0x66, 0x00},  //DSP_Ctrl3
	{0x67, 0x48},  //DSP_Ctrl4
	//{0x0c, 0xC0},  //bit 7 is flip, bit 6 is mirror
	{0x0c, 0x00},  //bit 7 is flip, bit 6 is mirror
	{0x13, 0xf0},

	// AWB Controls
	{0x63, 0xe0},  //AWB control Byte 0
	{0x6b, 0xaa},  //AWBCtrl3

	// AWB Gain Range
	////{0x4d, 0x09},  //Fix Pre-Gain Amplifier
	#if 1  // ECL_SETTINGS
	#else  // ECL_SETTINGS
	{0x7b, 0xf0},  //R gain range
	{0x7c, 0xf5},  //G gain range
	{0x7d, 0xf3},  //B gain range
	#endif // ECL_SETTINGS

	{0x00, 0x04},  //Default AGC gain
	{0x01, 0x65},  //Default BLUE gain
	{0x02, 0x5a},  //Default RED gain
	{0x03, 0x40},  //Default GREEN gain
	// Lens Correction
	#if 1  // ECL_SETTINGS
	{0x46, 0x05}, //Bit[0]: Lens correction enable
	{0x49, 0x10}, //LC_COEF
	{0x4a, 0x10}, //LC_RADI
	{0x4b, 0x10}, //LC_COEFB
	{0x4c, 0x17}, //LC_COEFR
	#else  // ECL_SETTINGS
	{0x46, 0x05},  //
	{0x49, 0x18},  //
	{0x4a, 0x00},  //
	{0x4b, 0x17},  //
	{0x4c, 0x20},  //
	#endif // ECL_SETTINGS

	// Edge (Sharpness) Control
	#if 0  // ECL_SETTINGS
	{0x8f, 0x03}, //EDGE Magnitude
	{0x90, 0x02}, //EDGE Threshold
	{0x92, 0x03}, //EDGE Upper Limit
	{0x93, 0x01}, //EDGE Lower Limit
	#else  // ECL_SETTINGS
	{0x90, 0x05},  //EDGE1
	{0x92, 0x03},  //EDGE2
	{0x93, 0x00},  //EDGE3
	//{0x92, 0x02},  //Edge Upper; remove 近锅絬
	//{0x93, 0x02},  //Edge Lower; 羪隔Аっ
	#endif // ECL_SETTINGS

	// Noise Control
	#if 0  // ECL_SETTINGS
	{0x8e, 0x00}, //Denoise Threshold
	{0x91, 0x00}, //Denoise Offset
	#else  // ECL_SETTINGS
	{0x91, 0x01},  //Denoise Offset
	#endif // ECL_SETTINGS

	// Color Matrix
	#if 1  // ECL_SETTINGS
	{0x94, 0xb0},  //MTX1
	{0x95, 0x9d},  //MTX2
	{0x96, 0x13},  //MTX3
	{0x97, 0x16},  //MTX4
	{0x98, 0x7b},  //MTX5
	{0x99, 0x91},  //MTX6
	{0x9a, 0x1e},  //TMX_Ctrl
	#else  // ECL_SETTINGS
	{0x94, 0x75},  // For 羪场熬 20081125 by Tom
	{0x95, 0x70},  // For 羪场熬 20081125 by Tom
	{0x96, 0x05},  // For 羪场熬 20081125 by Tom
	{0x97, 0x22},  // For 羪场熬 20081125 by Tom
	//{0x97, 0x28},  //for 羪︹ㄎ
	{0x98, 0x63},  // For 羪场熬 20081125 by Tom
	{0x99, 0x85},  // For 羪场熬 20081125 by Tom
	{0x9a, 0x1e},  //TMX_Ctrl
	#endif // ECL_SETTINGS

	//{0x0D ,0x10}, //[5:4]AEC/AGC window: 1/2 Window
	//{0x0D ,0x20}, //[5:4]AEC/AGC window: 1/4 Window (ず璉いみ獹)
	{0x0D ,0x30}, //[5:4]AEC/AGC window: Lower 2/3 Window
	{0x0F, 0xC5},
	{0x24, 0x40}, //Y Upper on stable operation region
	{0x25, 0x30}, //Y Lower on stable operation region
	{0x26, 0xA1}, //Y range on fast mode operation region
	//{0x26, 0xB2}, //Y range on fast mode operation region

	// AGC Configuration
	//{0x14, 0x01},		//[6:4] AEC gain ceiling: max 2x
	//{0x14, 0x11},		//[6:4] AEC gain ceiling: max 4x
	//{0x14, 0x21},		//[6:4] AEC gain ceiling: max 8x
	{0x14, 0x31},		//[6:4] AEC gain ceiling: max 16x

	//{0x0E, 0x41},  //Night mode OFF
	{0x0E, 0xF5},  //Night mode ON
	#ifdef SENSOR_LAEC
	//{0x13, 0xff},  //AEC, AWB, AGC, Banding ON
	{0x13, 0xdf},  //AEC, AWB, AGC, LAEC ON; Banding OFF
	#else  // SENSOR_LAEC
	//{0x13, 0xf7},  //AEC, AWB, AGC, Banding ON
	{0x13, 0xd7},  //AEC, AWB, AGC ON; LAEC, Banding OFF
	#endif // SENSOR_LAEC

	#if 1  // ECL_SETTINGS
	#else  // ECL_SETTINGS
	// Brightness and Contrast Controls
	{0x9B, 0x00},  //05 ;brightness
	{0x9C, 0x25},  //Y contrast
	//{0x9b, 0x08},  //brightness
	//{0x9b, 0x2f},  //brightness
	//{0x9c, 0x20},  //contrast normalized by 0x20
	//{0x9c, 0x1c},  //contrast normalized

	// Saturation Controls
	{0xA7, 0x46},  //;43 ;32;U saturation
	{0xA8, 0x4A},  //;43 ;3F;V saturation
	//{0xa7, 0x4a},
	//{0xa8, 0x30},
	#endif // ECL_SETTINGS

	// UV adjustment controls
	{0x9e, 0x81},  //auto UV adjust Ctrl 0

	// contrast/brightness/saturation/hue Control
	//{0xa6, 0x04},  //Contrast/brightness enabled
	{0xA6, 0x06},  //contrast/brightness/saturation enabled

	// Gamma
	{0x7e, 0x0C},  //GAM1
	{0x7f, 0x16},  //GAM2
	{0x80, 0x2A},  //GAM3
	{0x81, 0x4E},  //GAM4
	{0x82, 0x61},  //GAM5
	{0x83, 0x6F},  //GAM6
	{0x84, 0x7B},  //GAM7
	{0x85, 0x86},  //GAM8
	{0x86, 0x8E},  //GAM9
	{0x87, 0x97},  //GAM10
	{0x88, 0xA4},  //GAM11
	{0x89, 0xAF},  //GAM12
	{0x8a, 0xC5},  //GAM13
	{0x8b, 0xD7},  //GAM14
	{0x8c, 0xE8},  //GAM15
	{0x8d, 0x20},  //SLOP

	// AWB Corrections for Lens
	#if 0
	// For 垂环 ss3820ra lens awb - 20081125a by Tom
	{0x69, 0x9E},  // 5c
	{0x6A, 0x11},  // 11
	{0x6B, 0x2D},  // aa
	{0x6C, 0x09},  // 1
	{0x6D, 0x50},  // 4
	{0x6E, 0x4B},  // 80
	{0x6F, 0x60},  // 80
	{0x70, 0x0B},  // f
	{0x71, 0xBF},  // 0
	{0x72, 0xE3},  // 0
	{0x73, 0x15},  // f
	{0x74, 0x14},  // f
	{0x75, 0xFF},  // ff
	{0x76, 0x00},  // 0
	{0x77, 0x11},  // 10
	{0x79, 0x3E},  // 70
	{0x78, 0x25},  // 10
	{0x7A, 0x41},  // 70
	{0x7B, 0xF0},  //
	{0x7C, 0xF0},  //
	{0x7D, 0xF0},  //
	#endif
	#if 0
	// For yellowish when large Blue color issue - 20081205 by Tom
	{0x69, 0x9E},
	{0x6A, 0x11},
	{0x6B, 0x2D},
	{0x6C, 0x09},
	{0x6D, 0x50},
	{0x6E, 0x5C},
	{0x6F, 0x60},
	{0x70, 0x11},
	{0x71, 0xBF},
	{0x72, 0x6C},
	{0x73, 0x11},
	{0x74, 0x11},
	{0x75, 0xFF},
	{0x76, 0x00},
	{0x77, 0x41},
	{0x78, 0x25},
	{0x79, 0x5C},
	{0x7A, 0x41},
	{0x7B, 0xF0},
	{0x7C, 0xF0},
	{0x7D, 0xF0},
	#endif
};

tI2CRegister1 code I2CInitTable50HzVGA[] =
{
	//{0x2B,0x92},
	// for 25 fps, 50Hz
	{0x33, 0x66},
	{0x22, 0x99},
	{0x23, 0x03},
	//640x480
	{0x29, 0xa0},
	{0x2c, 0xf1},  //default = f0, for more line by jim 20080813
	{0x65, 0x20},
	{0xa0, 0x02},
	{0xa1, 0x50},
	{0xa2, 0x40},
};

tI2CRegister1 code I2CInitTable60HzVGA[] =
{
	//{0x2B,0x00},
	// for 30 fps, 60Hz
	{0x33, 0x00},
	{0x22, 0x7F},
	{0x23, 0x03},
	//640x480
	{0x29, 0xa0},
	{0x2c, 0xf1},  //default = f0, for more line by jim 20080813
	{0x65, 0x20},
	{0xa0, 0x02},
	{0xa1, 0x50},
	{0xa2, 0x40},
};


unsigned char fnDoInitSensor(
	unsigned char ucResolution)
{
	D2WriteSensorClock(CLK_SENSOR); // Set Sensor Clock in MHz
	fnD2IdleDelay(100);             // count 100 around 12ms
	ucResetControl = 0x74;          // 40:SIU Reset, 20:IPU Reset, 10:SOU Reset, 04: DRAM Reset
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
		fnI2cWriteTable0(I2CInitTableNTSC, sizeof(I2CInitTableNTSC) / sizeof(tI2CRegister0));
		fnI2cWriteTable1(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister1));
		fnI2cWriteTable1(I2CInitTable60HzVGA, sizeof(I2CInitTable60HzVGA) / sizeof(tI2CRegister1));
		break;
	case D2_RESOLUTION_VGA_50Hz:
		fnI2cWriteTable0(I2CInitTablePAL, sizeof(I2CInitTablePAL) / sizeof(tI2CRegister0));
		fnI2cWriteTable1(I2CInitTableCommon, sizeof(I2CInitTableCommon) / sizeof(tI2CRegister1));
		fnI2cWriteTable1(I2CInitTable50HzVGA, sizeof(I2CInitTable50HzVGA) / sizeof(tI2CRegister1));
		break;
	default:
		ucResetControl = 0x00;
		return FAILURE;
	}
	ucResetControl = 0x00;
	return SUCCESS;
}

void fnDoHouseKeeping()
{
	if (nD2Mode != D2_OPERATION_IDLE)
		fnD2TableVerify();
}

void fnDoInitialization()
{
#ifdef SUPPORT_D2_PIO
	// The firmware codes listed here are used to set proper controls to video sources.
	// PIO0 controls video decoder.
	// PIO1 controls video sensor.
	// The codes may not required if the controls are not implemented by hardware PCB.
	//fnD2SetPIO0(1);
	//fnD2SetPIO1(1);
#endif // SUPPORT_D2_PIO
	D2DisableSensorClock();

	I2CWorkItem0.I2CConfiguration = SENSOR0_I2C_CONFIG;
	I2CWorkItem0.I2CSlaveAddress  = SENSOR0_I2C_SLAVE;
	I2CWorkItem1.I2CConfiguration = SENSOR1_I2C_CONFIG;
	I2CWorkItem1.I2CSlaveAddress  = SENSOR1_I2C_SLAVE;
}

#endif // SUPPORT_D2_OPERATION
