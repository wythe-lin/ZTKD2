//
// Zealtek D2 project
// I2C Library Header File
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// April 25, 2012.

#ifndef __I2C_H__
#define __I2C_H__

#include "system.h"

#ifdef I2C_INTERFACE

	#define D2_I2C0_ENABLE
	#define D2_I2C1_ENABLE

	#ifdef D2_I2C0_ENABLE

	extern void fnI2cInitI2c0();
	extern void fnI2cStartI2c0();
	extern void fnI2cReStartI2c0();
	extern void fnI2cStopI2c0();
	extern unsigned char fnI2cWriteDataI2c0(unsigned char ucValue);
	extern unsigned char fnI2cReadDataI2c0();

	#endif // D2_I2C0_ENABLE
	#ifdef D2_I2C1_ENABLE

	extern void fnI2cInitI2c1();
	extern void fnI2cStartI2c1();
	extern void fnI2cReStartI2c1();
	extern void fnI2cStopI2c1();
	extern unsigned char fnI2cWriteDataI2c1(unsigned char ucValue);
	extern unsigned char fnI2cReadDataI2c1();

	#endif // D2_I2C1_ENABLE

#endif // I2C_INTERFACE

#endif // __I2C_H__
