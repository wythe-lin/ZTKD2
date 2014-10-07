//
// Zealtek D2 project
// Sensor 0 I2C Library
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 21, 2012.

#include "system.h"
#include "i2c.h"
#include "gendef.h"
#include "sensor0.h"

#ifdef I2C_INTERFACE

	#define SENSOR_REGISTER_ADDRESS SENSOR0_REGISTER_ADDRESS
	#define SENSOR_REGISTER_VALUE   SENSOR0_REGISTER_VALUE

	#define I2CWorkItem             I2CWorkItem0
	#define I2CRegister             I2CRegister0
#ifndef SENSOR1_EQ_SENSOR0
	#define tI2CRegister            tI2CRegister0
#endif // SENSOR1_EQ_SENSOR0

	#define fnI2cStartI2c           fnI2cStartI2c0
	#define fnI2cReStartI2c         fnI2cReStartI2c0
	#define fnI2cStopI2c            fnI2cStopI2c0
	#define fnI2cReadDataI2c        fnI2cReadDataI2c0
	#define fnI2cWriteDataI2c       fnI2cWriteDataI2c0

	#define fnI2cWriteTable         fnI2cWriteTable0
	#define fnI2cWriteRegisterI2c   fnI2cWriteRegisterI2c0
	#define fnI2cReadRegisterI2c    fnI2cReadRegisterI2c0
	#define fnI2cReadI2c            fnI2cReadI2c0
	#define fnI2cWriteI2c           fnI2cWriteI2c0

	#include "sensor.c"

#endif // I2C_INTERFACE
