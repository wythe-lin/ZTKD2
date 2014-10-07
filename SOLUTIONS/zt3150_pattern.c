//
// Zealtek D2 project
// ZT3150 pattern generated solutions
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 22, 2012.

#include "system.h"
#include "d2.h"


// Compiler option SOU_CLK_40MHz:
//
// It is suggested to run SOU_CLK_48MHz for dual_VGA
// For unknown reason, image quality is acceptable for SOU_CLK_40MHz.
// When the root cause is found, run run SOU_CLK_48MHz.
//
#define SOU_CLK_40MHz


// SOU_PATTERN_ENABLE enables the fixed color pattern generator to video destination.
// SOU_PATTERN_1ST_COLOR defines the first shown color on the generated pattern.
// SOU_PATTERN_COUNTER is used to define the changing ratio of the generated pattern color.
//
#define SOU_PATTERN_ENABLE  8
#define SOU_PATTERN_DISABLE 0
#define SOU_PATTERN_1ST_COLOR 6
//
//#define SOU_PATTERN_COUNTER  10000L // around 1/2 seconds changing ratio
#define SOU_PATTERN_COUNTER  20000L // around 1 second changing ratio
//#define SOU_PATTERN_COUNTER 100000L // around 5 seconds changing ratio

#ifdef SUPPORT_D2_OPERATION

#define CLK_SENSOR  24
#define CLK_HOST    24

#if (CLK_PLL / CLK_SENSOR >= 16)
	#error Maximum Clock Divider for CLK_SENSOR is 16.
#endif // (CLK_PLL / CLK_SENSOR >= 16)
#if (CLK_PLL / CLK_HOST >= 16)
	#error Maximum Clock Divider for CLK_HOST is 16.
#endif // (CLK_PLL / CLK_HOST >= 16)


tD2Register code D2TableSOU_VGA[] =
{
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x02},
	{0x414, 0xE0},
	{0x415, 0x01},
	{0x41B, 0xA0},
	{0x41C, 0x05},
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x05},
	{0x425, 0x46},
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

tD2Register code D2TableSOU_HD[] =
{
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x00},
	{0x413, 0x05},
	{0x414, 0xD0},
	{0x415, 0x02},
	{0x41B, 0xF0},
	{0x41C, 0x0A},
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x0A},
	{0x425, 0xE4},
	{0x426, 0x02},
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

tD2Register code D2TableSOU_DualVGA[] =
{
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x00},
	{0x413, 0x05},
	{0x414, 0xE0},
	{0x415, 0x01},
	{0x41B, 0x40},
	{0x41C, 0x0B},
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x0A},
	#ifdef SOU_CLK_40MHz
	{0x425, 0xE4}, // SOU_CLK 40MHz
	{0x426, 0x01}, // SOU_CLK 40MHz
	#else // SOU_CLK_40MHz
	{0x425, 0x46}, // SOU_CLK 48MHz
	{0x426, 0x02}, // SOU_CLK 48MHz
	#endif // SOU_CLK_40MHz
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

tD2Register code D2TableSOU_DualHD[] =
{
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x00},
	{0x413, 0x0A},
	{0x414, 0xD0},
	{0x415, 0x02},
	{0x41B, 0xE0},
	{0x41C, 0x15},
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x14},
	{0x425, 0xE4},
	{0x426, 0x02},
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

#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
tD2Register code D2TableSOU_2xVGA[] =
{
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x02},
	{0x414, 0xC0},
	{0x415, 0x03},
	{0x41B, 0xA0},
	{0x41C, 0x05},
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x05},
	{0x425, 0x8C},
	{0x426, 0x04},
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

tD2Register code D2TableSOU_2xHD[] =
{
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x00},
	{0x413, 0x05},
	{0x414, 0xA0},
	{0x415, 0x05},
	{0x41B, 0xF0},
	{0x41C, 0x0A},
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x0A},
	{0x425, 0xC8},
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
#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE

#ifdef SUPPORT_FUZZY_STITCHING
tD2Register code D2TableSOU_1920x544[] =
{
	// SOU Configuration
	{0x400, 0x08},
	{0x410, 0x01},
	// SOU Windowing Parameters
	{0x412, 0x80},
	{0x413, 0x07},
	{0x414, 0x20},
	{0x415, 0x02},
	{0x41B, 0xA8},
	{0x41C, 0x11},
	{0x41D, 0x04},
	{0x41E, 0x00},
	{0x41F, 0x44},
	{0x420, 0x00},
	{0x421, 0x44},
	{0x422, 0x00},
	{0x423, 0x43},
	{0x424, 0x0F},
	{0x425, 0x58},
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
#endif // SUPPORT_FUZZY_STITCHING

// for AWB calibration, by XYZ, 2014.01.22 - begin
unsigned char opmode_calibration(unsigned char act)
{
	act = act;
	return SUCCESS;
}
// for AWB calibration, by XYZ, 2014.01.22 - end

unsigned char xdata SouPattern _at_ 0x041A; // SOU PATTERN
unsigned long ulSouCount;

unsigned char fnSetOperatingMode(
	unsigned char ucMode,
	unsigned char ucResolution)
{
	SouPattern = ucMode|SOU_PATTERN_ENABLE; // Enable SOU Pattern
	ulSouCount = 0;

	switch (ucMode)
	{
	case D2_OPERATION_IDLE:
		D2WriteOutputClock(12); // Set Host Clock in MHz
		return SUCCESS;

	case D2_OPERATION_BYPASS_SENSOR0:
	case D2_OPERATION_BYPASS_SENSOR1:
#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
	case D2_OPERATION_HALF_SIDE_BY_SIDE:
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE
		if (ucResolution <= 2)
		{
			D2WriteOutputClock(24); 		// Set Host Clock in MHz
			fnD2WriteTable(D2TableSOU_VGA, sizeof(D2TableSOU_VGA) / sizeof(tD2Register));
		}
		else
		{
			D2WriteOutputClock(60); 		// Set Host Clock in MHz
			fnD2WriteTable(D2TableSOU_HD, sizeof(D2TableSOU_HD) / sizeof(tD2Register));
		}
		return SUCCESS;

#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
	case D2_OPERATION_VERTICAL_SIDE_BY_SIDE:
		if (ucResolution <= 2)
		{
			D2WriteOutputClock(48); 		// Set Host Clock in MHz
			fnD2WriteTable(D2TableSOU_2xVGA, sizeof(D2TableSOU_2xVGA) / sizeof(tD2Register));
		}
		else
		{
			D2WriteOutputClock(60); 		// Set Host Clock in MHz
			fnD2WriteTable(D2TableSOU_2xHD, sizeof(D2TableSOU_2xHD) / sizeof(tD2Register));
		}
		return SUCCESS;
#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE

	case D2_OPERATION_HORIZONTAL_SIDE_BY_SIDE:
		if (ucResolution <= 2)
		{
	#ifdef SOU_CLK_40MHz
			D2WriteOutputClock(40); 		// Set Host Clock in MHz
	#else // SOU_CLK_40MHz
			D2WriteOutputClock(48); 		// Set Host Clock in MHz
	#endif // SOU_CLK_40MHz
			fnD2WriteTable(D2TableSOU_DualVGA, sizeof(D2TableSOU_DualVGA) / sizeof(tD2Register));
		}
		else
		{
			D2WriteOutputClock(60); 		// Set Host Clock in MHz
			fnD2WriteTable(D2TableSOU_DualHD, sizeof(D2TableSOU_DualHD) / sizeof(tD2Register));
		}
		return SUCCESS;
#ifdef SUPPORT_FUZZY_STITCHING
	case D2_OPERATION_ACCURATE_STITCHING:
		if (ucResolution <= 2)
		{
	#ifdef SOU_CLK_40MHz
			D2WriteOutputClock(40); 		// Set Host Clock in MHz
	#else // SOU_CLK_40MHz
			D2WriteOutputClock(48); 		// Set Host Clock in MHz
	#endif // SOU_CLK_40MHz
			fnD2WriteTable(D2TableSOU_DualVGA, sizeof(D2TableSOU_DualVGA) / sizeof(tD2Register));
		}
		else
		{
			D2WriteOutputClock(60); 		// Set Host Clock in MHz
			fnD2WriteTable(D2TableSOU_1920x544, sizeof(D2TableSOU_1920x544) / sizeof(tD2Register));
		}
		return SUCCESS;
#endif // SUPPORT_FUZZY_STITCHING

	// for AWB calibration, by XYZ, 2014.01.22 - begin
	case D2_OPERATION_3A_CALIBRATION:
		return opmode_calibration(ucResolution);
	// for AWB calibration, by XYZ, 2014.01.22 - end
	}
	return FAILURE;
}

void fnDoHouseKeeping()
{
	if (ulSouCount >= SOU_PATTERN_COUNTER)
	{ // Periodically Change the Pattern Color
		ulSouCount = 0;
		SouPattern = ((SouPattern & 0x07) + 1) | SOU_PATTERN_ENABLE;
	}
	else
	{
		ulSouCount++;
	}
}

void fnDoInitialization()
{
	SouPattern = SOU_PATTERN_1ST_COLOR+SOU_PATTERN_ENABLE; // Enable SOU Pattern
	ulSouCount = 0;
	D2DisableSensorClock();
}

#endif // SUPPORT_D2_OPERATION
