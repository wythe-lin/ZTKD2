//
// Zealtek D2 project
// I2C Library
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// April 25, 2012.

#include "system.h"
#include "d2.h"
#include "i2c.h"
#include <intrins.h>

#ifdef I2C_INTERFACE

	#define D2_I2C0_OPENDRAIN
	#define D2_I2C1_OPENDRAIN
	#define I2C_DELAY ;

	#ifdef D2_I2C0_ENABLE
//
// SN0_SCL is assigned to GPIO4.5 (SN0_D0)
// SN0_SDA is assigned to GPIO4.6 (SN0_D1)
//
		#ifdef D2_I2C0_OPENDRAIN
#define SN0_SCL_OUT_SN0_SDA_OUT   ;
#define SN0_SCL_IN_SN0_SDA_IN     ;
#define SN0_SDA_OUT               (Port4Value  &= 0xBF)
#define SN0_SDA_IN                (Port4Enable &= 0xBF)
#define SN0_SCL_LOW_SN0_SDA_LOW   (Port4Enable |= 0x60)
#define SN0_SCL_HIGH_SN0_SDA_HIGH (Port4Enable &= 0x9F)
#define SN0_SCL_LOW               (Port4Enable |= 0x20)
#define SN0_SCL_HIGH              (Port4Enable &= 0xDF)
#define SN0_SDA_LOW               (Port4Enable |= 0x40)
#define SN0_SDA_HIGH              (Port4Enable &= 0xBF)
#define SN0_SDA_VALUE             (Port4Value  &= 0x40)
		#else  // D2_I2C0_OPENDRAIN
#define SN0_SCL_OUT_SN0_SDA_OUT   (Port4Enable |= 0x60)
#define SN0_SCL_IN_SN0_SDA_IN     (Port4Enable &= 0x9F)
#define SN0_SDA_OUT               (Port4Enable |= 0x40)
#define SN0_SDA_IN                (Port4Enable &= 0xBF)
#define SN0_SCL_HIGH_SN0_SDA_HIGH	(Port4Value  |= 0x60)
#define SN0_SCL_LOW_SN0_SDA_LOW   (Port4Value  &= 0x9F)
#define SN0_SCL_HIGH              (Port4Value  |= 0x20)
#define SN0_SCL_LOW               (Port4Value  &= 0xDF)
#define SN0_SDA_HIGH              (Port4Value  |= 0x40)
#define SN0_SDA_LOW               (Port4Value  &= 0xBF)
#define SN0_SDA_VALUE             (Port4Value  &= 0x40)
		#endif // D2_I2C0_OPENDRAIN

	#endif // D2_I2C0_ENABLE
	#ifdef D2_I2C1_ENABLE
//
// SN1_SCL is assigned to GPIO6.3 (SN1_D0)
// SN1_SDA is assigned to GPIO6.4 (SN1_D1)
//
		#ifdef D2_I2C1_OPENDRAIN
#define SN1_SCL_OUT_SN1_SDA_OUT   ;
#define SN1_SCL_IN_SN1_SDA_IN     ;
#define SN1_SDA_OUT               (Port6Value  &= 0xEF)
#define SN1_SDA_IN                (Port6Enable &= 0xEF)
#define SN1_SCL_LOW_SN1_SDA_LOW   (Port6Enable |= 0x18)
#define SN1_SCL_HIGH_SN1_SDA_HIGH (Port6Enable &= 0xE7)
#define SN1_SCL_LOW               (Port6Enable |= 0x08)
#define SN1_SCL_HIGH              (Port6Enable &= 0xF7)
#define SN1_SDA_LOW               (Port6Enable |= 0x10)
#define SN1_SDA_HIGH              (Port6Enable &= 0xEF)
#define SN1_SDA_VALUE             (Port6Value  &= 0x10)
		#else  // D2_I2C1_OPENDRAIN
#define SN1_SCL_OUT_SN1_SDA_OUT   (Port6Enable |= 0x18)
#define SN1_SCL_IN_SN1_SDA_IN     (Port6Enable &= 0xE7)
#define SN1_SDA_OUT               (Port6Enable |= 0x10)
#define SN1_SDA_IN                (Port6Enable &= 0xEF)
#define SN1_SCL_HIGH_SN1_SDA_HIGH	(Port6Value  |= 0x18)
#define SN1_SCL_LOW_SN1_SDA_LOW   (Port6Value  &= 0xE7)
#define SN1_SCL_HIGH              (Port6Value  |= 0x08)
#define SN1_SCL_LOW               (Port6Value  &= 0xF7)
#define SN1_SDA_HIGH              (Port6Value  |= 0x10)
#define SN1_SDA_LOW               (Port6Value  &= 0xEF)
#define SN1_SDA_VALUE             (Port6Value  &= 0x10)
		#endif // D2_I2C1_OPENDRAIN

	#endif // D2_I2C1_ENABLE

//========================================================================

	#ifdef D2_I2C0_ENABLE

/**
* @fn     fnI2cInitI2c0()
* @brief  Video source 0 I2C intialization
* @retval none
*/
void fnI2cInitI2c0()
{
		#ifdef D2_I2C0_OPENDRAIN
	Port4Value  &= 0x9F;
	Port4Enable &= 0x9F;
	Port4Mux    &= 0x9F;
		#else  // D2_I2C0_OPENDRAIN
	SN0_SCL_HIGH_SN0_SDA_HIGH;
	SN0_SCL_OUT_SN0_SDA_OUT;
	Port4Mux    &= 0x9F;
		#endif // D2_I2C0_OPENDRAIN
}

/**
* @fn     fnI2cStartI2c0()
* @brief  Video source 0 I2C START
* @retval none
*/
void fnI2cStartI2c0()
{
	SN0_SCL_HIGH_SN0_SDA_HIGH;
	//SN0_SCL_OUT_SN0_SDA_OUT;
	I2C_DELAY;
	SN0_SDA_LOW;
	I2C_DELAY;
}

/**
* @fn     fnI2cReStartI2c0()
* @brief  Video source 0 I2C RE-START
* @retval none
*/
void fnI2cReStartI2c0()
{
	SN0_SCL_HIGH;
	I2C_DELAY;
	SN0_SDA_HIGH;
	I2C_DELAY;
	SN0_SDA_LOW;
	I2C_DELAY;
}

/**
* @fn     fnI2cStopI2c0()
* @brief  Video source 0 I2C STOP
* @retval none
*/
void fnI2cStopI2c0()
{
	// SCL prior state = LOW
	SN0_SDA_LOW;
	I2C_DELAY;
	I2C_DELAY;
	SN0_SCL_HIGH;
	I2C_DELAY;
	I2C_DELAY;
	SN0_SDA_HIGH;
	//SN0_SCL_IN_SN0_SDA_IN;
	I2C_DELAY;
}

/**
* @fn     fnI2cWriteDataI2c0()
* @brief  Video source 0 I2C DATA WRITE (8 bits in total),
*         then check the replied I2C ACK.
* @retval ACK (0) or NACK (1) from I2C Slave
*/
unsigned char fnI2cWriteDataI2c0(unsigned char ucValue)
{
	unsigned char idx, ucAck;

	// Write 8-bit I2C Data to I2C Slave
	for (idx=0; idx<8; idx++)
	{
		SN0_SCL_LOW;

		if (ucValue&0x80)
			SN0_SDA_HIGH;
		else
			SN0_SDA_LOW;
		I2C_DELAY;

		SN0_SCL_HIGH;
		I2C_DELAY;
		ucValue = ucValue<<1;
	}
	// Check I2C ACK from I2C Slave
	SN0_SCL_LOW;			// Ack SCL st. L - 1
	I2C_DELAY;
	SN0_SDA_IN;
	I2C_DELAY;
	_nop_();
	_nop_();
	_nop_();

	SN0_SCL_HIGH;			// Ack SCL st. H - 2
	I2C_DELAY;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	ucAck = SN0_SDA_VALUE;	// read Ack
	SN0_SCL_LOW;			// Ack SCL st. L - 3
	SN0_SDA_OUT;
	I2C_DELAY;

	return ucAck;
}

/**
* @fn     fnI2cReadDataI2c0()
* @brief  Video source 0 I2C DATA READ (8 bits in total),
*         then send out the I2C NAK.
* @retval 8-bit read data
*/
unsigned char fnI2cReadDataI2c0()
{
	unsigned char idx, ucValue;

	ucValue = 0;
	SN0_SDA_IN;
	// Read 8-bit I2C Data from I2C Slave
	for (idx=0; idx<8; idx++)
	{
		SN0_SCL_LOW;
		I2C_DELAY;
		ucValue = ucValue<<1;
		SN0_SCL_HIGH;
		I2C_DELAY;
		_nop_();
		_nop_();
		_nop_();

		if (SN0_SDA_VALUE)
			ucValue |= 1;
	}
	// Send I2C ACK to I2C Slave
	SN0_SCL_LOW;			// reply N-Ack to stop read SCL St. L - 1
	I2C_DELAY;
	SN0_SDA_OUT;
	#ifdef SENSOR0_NACK_ON_I2C_READ
	SN0_SDA_HIGH;
	#else  // SENSOR0_NACK_ON_I2C_READ
	SN0_SDA_LOW;
	#endif // SENSOR0_NACK_ON_I2C_READ
	I2C_DELAY;
	SN0_SCL_HIGH;			// reply N-Ack to stop read SCL St. H - 2
	I2C_DELAY;
	_nop_();
	_nop_();
	_nop_();
	SN0_SCL_LOW;			// reply N-Ack to stop read SCL St. L - 3
	I2C_DELAY;

	return ucValue;
}

	#endif // D2_I2C0_ENABLE

//========================================================================

	#ifdef D2_I2C1_ENABLE

/**
* @fn     fnI2cInitI2c1()
* @brief  Video source 1 I2C intialization
* @retval none
*/
void fnI2cInitI2c1()
{
		#ifdef D2_I2C1_OPENDRAIN
	Port6Value  &= 0xE7;
	Port6Enable &= 0xE7;
	Port6Mux    &= 0xE7;
		#else  // D2_I2C1_OPENDRAIN
	SN1_SCL_HIGH_SN1_SDA_HIGH;
	SN1_SCL_OUT_SN1_SDA_OUT;
	Port6Mux    &= 0xE7;
		#endif // D2_I2C1_OPENDRAIN
}

/**
* @fn     fnI2cStartI2c1()
* @brief  Video source 1 I2C START
* @retval none
*/
void fnI2cStartI2c1()
{
	SN1_SCL_HIGH_SN1_SDA_HIGH;
	//SN1_SCL_OUT_SN1_SDA_OUT;
	I2C_DELAY;
	SN1_SDA_LOW;
	I2C_DELAY;
}

/**
* @fn     fnI2cReStartI2c1()
* @brief  Video source 1 I2C RE-START
* @retval none
*/
void fnI2cReStartI2c1()
{
	SN1_SCL_HIGH;
	I2C_DELAY;
	SN1_SDA_HIGH;
	I2C_DELAY;
	SN1_SDA_LOW;
	I2C_DELAY;
}

/**
* @fn     fnI2cStopI2c1()
* @brief  Video source 1 I2C STOP
* @retval none
*/
void fnI2cStopI2c1()
{
	// SCL prior state = LOW
	SN1_SDA_LOW;
	I2C_DELAY;
	I2C_DELAY;
	SN1_SCL_HIGH;
	I2C_DELAY;
	I2C_DELAY;
	SN1_SDA_HIGH;
	//SN1_SCL_IN_SN1_SDA_IN;
	I2C_DELAY;
}

/**
* @fn     fnI2cWriteDataI2c1()
* @brief  Video source 1 I2C DATA WRITE (8 bits in total),
*         then check the replied I2C ACK.
* @retval ACK (0) or NACK (1) from I2C Slave
*/
unsigned char fnI2cWriteDataI2c1(unsigned char ucValue)
{
	unsigned char idx, ucAck;

	// Write 8-bit I2C Data to I2C Slave
	for (idx=0; idx<8; idx++)
	{
		SN1_SCL_LOW;

		if (ucValue&0x80)
			SN1_SDA_HIGH;
		else
			SN1_SDA_LOW;
		I2C_DELAY;

		SN1_SCL_HIGH;
		I2C_DELAY;
		ucValue = ucValue<<1;
	}
	// Check I2C ACK from I2C Slave
	SN1_SCL_LOW;			// Ack SCL st. L - 1
	I2C_DELAY;
	SN1_SDA_IN;
	I2C_DELAY;
	_nop_();
	_nop_();
	_nop_();

	SN1_SCL_HIGH;			// Ack SCL st. H - 2
	I2C_DELAY;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	ucAck = SN1_SDA_VALUE;	// read Ack
	SN1_SCL_LOW;			// Ack SCL st. L - 3
	SN1_SDA_OUT;
	I2C_DELAY;

	return ucAck;
}

/**
* @fn     fnI2cReadDataI2c1()
* @brief  Video source 1 I2C DATA READ (8 bits in total),
*         then send out the I2C NAK.
* @retval 8-bit read data
*/
unsigned char fnI2cReadDataI2c1()
{
	unsigned char idx, ucValue;

	ucValue = 0;
	SN1_SDA_IN;
	// Read 8-bit I2C Data from I2C Slave
	for (idx=0; idx<8; idx++)
	{
		SN1_SCL_LOW;
		I2C_DELAY;
		ucValue = ucValue<<1;
		SN1_SCL_HIGH;
		I2C_DELAY;
		_nop_();
		_nop_();
		_nop_();

		if (SN1_SDA_VALUE)
			ucValue |= 1;
	}
	// Send I2C ACK to I2C Slave
	SN1_SCL_LOW;			// reply N-Ack to stop read SCL St. L - 1
	I2C_DELAY;
	SN1_SDA_OUT;
	#ifdef SENSOR1_NACK_ON_I2C_READ
	SN1_SDA_HIGH;
	#else  // SENSOR1_NACK_ON_I2C_READ
	SN1_SDA_LOW;
	#endif // SENSOR1_NACK_ON_I2C_READ
	I2C_DELAY;
	SN1_SCL_HIGH;			// reply N-Ack to stop read SCL St. H - 2
	I2C_DELAY;
	_nop_();
	_nop_();
	_nop_();
	SN1_SCL_LOW;			// reply N-Ack to stop read SCL St. L - 3
	I2C_DELAY;

	return ucValue;
}

	#endif // D2_I2C1_ENABLE

#endif // I2C_INTERFACE

