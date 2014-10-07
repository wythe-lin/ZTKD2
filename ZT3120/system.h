//
// Zealtek D2 project
// System Header File
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// April 28, 2012.

#ifndef __SYSTEM_H__
#define __SYSTEM_H__


#ifdef ZT3120
	#define I2C_INTERFACE
	#define SUPPORT_D2_PIO
	#define SUPPORT_D2_OPERATION
	#define SUPPORT_HORIZONTAL_SIDE_BY_SIDE
	#include "zt3120.h"
#endif // ZT3120

#ifdef I2C_INTERFACE
	//#define SUPPORT_I2C_ERROR_COUNT
#endif // I2C_INTERFACE

#define SUCCESS  0
#define FAILURE  1


//
// Clock Settings: INPUT, PLL, SDRAM, MAIN, SPI
//
#ifndef CLK_SDRAM
	//#define CLK_SDRAM 108 //MHz
	#define CLK_SDRAM 120 //MHz
	//#define CLK_SDRAM 160 //MHz
#endif // CLK_SDRAM
#ifndef CLK_XIN
	#if (CLK_SDRAM == 108) || (CLK_SDRAM == 162)
		#define CLK_XIN 27 //MHz
	#else  // (CLK_SDRAM == 108) || (CLK_SDRAM == 162)
		#define CLK_XIN 12 //MHz
	#endif // (CLK_SDRAM == 108) || (CLK_SDRAM == 162)
#endif // CLK_XIN
#if (CLK_SDRAM == 120)
	#define CLK_MAIN  120 //MHz
	#define CLK_PLL   240 //MHz
	#define CLK_SPI    24 //MHz
#elif (CLK_SDRAM == 160)
	#define CLK_MAIN   80 //MHz
	#define CLK_PLL   320 //MHz
	#define CLK_SPI    24 //MHz
#elif (CLK_SDRAM == 108)
	#define CLK_MAIN  108 //MHz
	#define CLK_PLL   432 //MHz
	#define CLK_SPI    27 //MHz
#elif (CLK_SDRAM == 162)
	#define CLK_MAIN   81 //MHz
	#define CLK_PLL   486 //MHz
	#define CLK_SPI    27 //MHz
#else
	#error CLK_SDRAM is not defined properly. 
#endif // (CLK_SDRAM)

	//
	// Mailbox status and acknowledgement
	//
	#define CMD_ACK                    0x80
	#define CMD_ERROR                  0x40
	//
	// Mailbox command list
	//
	#define CMD_D2_GET_OPERATING_MODE  0x00
	#define CMD_D2_SET_OPERATING_MODE  0x01
#ifdef I2C_INTERFACE
	#define CMD_D2_WRITE_SN0_CONF      0x02
	#define CMD_D2_WRITE_SN1_CONF      0x03
	#define CMD_D2_READ_SN0_REG        0x04
	#define CMD_D2_WRITE_SN0_REG       0x05
	#define CMD_D2_READ_SN1_REG        0x06
	#define CMD_D2_WRITE_SN1_REG       0x07

	#ifdef SUPPORT_I2C_ERROR_COUNT
	#define CMD_D2_GET_I2C_ERROR_CNT   0x08
	#define CMD_D2_SET_I2C_ERROR_CNT   0x09
	#endif // SUPPORT_I2C_ERROR_COUNT
#endif // I2C_INTERFACE

#ifdef SUPPORT_D2_PIO
	#define CMD_D2_GET_PIO             0x18
	#define CMD_D2_SET_PIO             0x19
#endif // SUPPORT_D2_PIO

#ifdef SUPPORT_D2_OPERATION
	//
	// D2 operating Modes
	//
		#define D2_OPERATION_IDLE                    0
		#define D2_OPERATION_BYPASS_SENSOR0          1
		#define D2_OPERATION_BYPASS_SENSOR1          2
	#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
		#define D2_OPERATION_HORIZONTAL_SIDE_BY_SIDE 3
		#define D2_OPERATION_HALF_SIDE_BY_SIDE       4
	#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE
	#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
		#define D2_OPERATION_VERTICAL_SIDE_BY_SIDE   5
	#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE
		#define D2_OPERATION_3A_CALIBRATION	   255		// for AWB calibration, by XYZ, 2014.01.22

	//
	// D2 operating Resolutions
	//
		#define D2_RESOLUTION_NONE        0
		#define D2_RESOLUTION_VGA_60Hz    1
		#define D2_RESOLUTION_VGA_50Hz    2
		#define D2_RESOLUTION_HD_60Hz     3
		#define D2_RESOLUTION_HD_50Hz     4

	extern unsigned char fnDoInitSensor(
		unsigned char ucResolution);
	extern unsigned char fnSetOperatingMode(
		unsigned char ucMode,
		unsigned char ucResolution);
	extern void fnDoHouseKeeping();
	extern void fnDoInitialization();
	extern void fnD2TableVerify();

	extern unsigned char nD2Mode;
	extern unsigned char nD2Resolution;
	extern unsigned char nD2TableVerified;
#endif // SUPPORT_D2_OPERATION

#endif // __SYSTEM_H__
