//
// Zealtek D2 project
// Sensor Header File
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// June 23, 2012.

#ifndef __SENSOR_H__
#define __SENSOR_H__

#ifdef I2C_INTERFACE

	#define I2C_REGISTER_ADDRESS_WORD 0x01
	#define I2C_REGISTER_ADDRESS_BYTE 0x00
	#define I2C_REGISTER_VALUE_WORD   0x02
	#define I2C_REGISTER_VALUE_BYTE   0x00
	#define I2C_RESTART_READ          0x04
	#define I2C_STOP_START_READ       0x00

	typedef struct _tI2CWorkItem
	{
		unsigned char  I2CConfiguration;
		unsigned char  I2CSlaveAddress;
		unsigned short I2CRegAddress;
		unsigned short I2CRegValue;
	} tI2CWorkItem;

	#ifdef SUPPORT_I2C_ERROR_COUNT
	extern unsigned long I2CErrorCount;
	#endif // SUPPORT_I2C_ERROR_COUNT
	extern tI2CWorkItem  I2CWorkItem0;
	extern tI2CWorkItem  I2CWorkItem1;

#endif // I2C_INTERFACE

#endif // __SENSOR_H__
