//
// Zealtek D2 project
// Sensor 1 Library Header File
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 21, 2012.

#ifndef __SENSOR_1_H__
#define __SENSOR_1_H__

#include "sensor.h"

#ifdef I2C_INTERFACE

	#ifdef SUPPORT_D2_OPERATION
		#ifndef SENSOR1_REGISTER_ADDRESS
			#error  SENSOR1_REGISTER_ADDRESS shall be either 1 or 2
			#define SENSOR1_REGISTER_ADDRESS 1
		#endif // SENSOR1_REGISTER_ADDRESS
		#ifndef SENSOR1_REGISTER_VALUE
			#error  SENSOR1_REGISTER_VALUE shall be either 1 or 2
			#define SENSOR1_REGISTER_VALUE   1
		#endif // SENSOR1_REGISTER_VALUE

		#ifdef SENSOR1_EQ_SENSOR0
			#include "sensor0.h"
			extern tI2CRegister  I2CRegister1;
		#else  // SENSOR1_EQ_SENSOR0
			typedef struct _I2CRegister1
			{
				#if (SENSOR1_REGISTER_ADDRESS == 2)
					unsigned short I2CRegAddress;
				#else  // SENSOR1_REGISTER_ADDRESS == 2
					unsigned char  I2CRegAddress;
				#endif // SENSOR1_REGISTER_ADDRESS == 2
				#if SENSOR1_REGISTER_VALUE == 2
					unsigned short I2CRegValue;
				#else  // SENSOR1_REGISTER_VALUE == 2
					unsigned char  I2CRegValue;
				#endif // SENSOR1_REGISTER_VALUE == 2
			} tI2CRegister1;
		#endif // SENSOR1_EQ_SENSOR0

		#ifdef SENSOR1_EQ_SENSOR0
	extern unsigned char fnI2cWriteTable1(tI2CRegister code *i2cTable, unsigned short usLength);
		#else  // SENSOR1_EQ_SENSOR0
	extern unsigned char fnI2cWriteTable1(tI2CRegister1 code *i2cTable, unsigned short usLength);
		#endif // SENSOR1_EQ_SENSOR0
	#endif // SUPPORT_D2_OPERATION

	#define I2CWRITE_SN1_REGISTER(a,v) { I2CWorkItem1.I2CRegAddress = (a); I2CWorkItem1.I2CRegValue = (v); fnI2cWriteRegisterI2c1(); }
	#define I2CREAD_SN1_REGISTER(a,v)  { I2CWorkItem1.I2CRegAddress = (a); fnI2cReadRegisterI2c1();  (v) = I2CWorkItem1.I2CRegValue; }

	extern unsigned char fnI2cReadI2c1(unsigned char *i2cStruct);
	extern unsigned char fnI2cWriteI2c1(unsigned char *i2cStruct);
	extern unsigned char fnI2cReadRegisterI2c1();
	extern unsigned char fnI2cWriteRegisterI2c1();

#endif // I2C_INTERFACE

#endif // __SENSOR_1_H__
