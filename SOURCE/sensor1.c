//
// Zealtek D2 project
// Sensor 1 I2C Library
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 21, 2012.

#include "system.h"
#include "i2c.h"
#include "gendef.h"
#include "sensor1.h"

#ifdef I2C_INTERFACE

	#define SENSOR_REGISTER_ADDRESS SENSOR1_REGISTER_ADDRESS
	#define SENSOR_REGISTER_VALUE   SENSOR1_REGISTER_VALUE

	#define I2CWorkItem             I2CWorkItem1
#ifndef SENSOR1_EQ_SENSOR0
	#define tI2CRegister            tI2CRegister1
#endif // SENSOR1_EQ_SENSOR0

	#define fnI2cStartI2c           fnI2cStartI2c1
	#define fnI2cReStartI2c         fnI2cReStartI2c1
	#define fnI2cStopI2c            fnI2cStopI2c1
	#define fnI2cReadDataI2c        fnI2cReadDataI2c1
	#define fnI2cWriteDataI2c       fnI2cWriteDataI2c1

	#define fnI2cWriteTable         fnI2cWriteTable1
	#define fnI2cWriteRegisterI2c   fnI2cWriteRegisterI2c1
	#define fnI2cReadRegisterI2c    fnI2cReadRegisterI2c1
	#define fnI2cReadI2c            fnI2cReadI2c1
	#define fnI2cWriteI2c           fnI2cWriteI2c1

	#include "sensor.c"

#endif // I2C_INTERFACE
