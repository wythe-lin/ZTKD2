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


#ifdef I2C_INTERFACE
//
// SN0_SCL is assigned to GPIO4.5 (SN0_D0)
// SN0_SDA is assigned to GPIO4.6 (SN0_D1)
//
//
// SN1_SCL is assigned to GPIO6.3 (SN1_D0)
// SN1_SDA is assigned to GPIO6.4 (SN1_D1)
//

#define SN0_SCL_OUT_SN0_SDA_OUT   (Port4Enable |= 0x60)
#define SN0_SDA_OUT               (Port4Enable |= 0x40)
#define SN0_SDA_IN                (Port4Enable &= 0xBF)
#define SN0_SCL_HIGH_SN0_SDA_HIGH	(Port4Value  |= 0x60)
#define SN0_SCL_HIGH              (Port4Value  |= 0x20)
#define SN0_SCL_LOW               (Port4Value  &= 0xDF)
#define SN0_SDA_HIGH              (Port4Value  |= 0x40)
#define SN0_SDA_LOW               (Port4Value  &= 0xBF)
#define SN0_SDA_VALUE             (Port4Value  &= 0x40)

#define SN1_SCL_OUT_SN1_SDA_OUT   (Port6Enable |= 0x18)
#define SN1_SDA_OUT               (Port6Enable |= 0x10)
#define SN1_SDA_IN                (Port6Enable &= 0xEF)
#define SN1_SCL_HIGH_SN1_SDA_HIGH	(Port6Value  |= 0x18)
#define SN1_SCL_HIGH              (Port6Value  |= 0x08)
#define SN1_SCL_LOW               (Port6Value  &= 0xF7)
#define SN1_SDA_HIGH              (Port6Value  |= 0x10)
#define SN1_SDA_LOW               (Port6Value  &= 0xEF)
#define SN1_SDA_VALUE             (Port6Value  &= 0x10)


void fnDelayCycleCounter(unsigned char ucCounter)
{
	unsigned char i;

	for (i=0; i<ucCounter; i++)
		;

}

//========================================================================

/**
* @fn     fnI2cInitI2c0()
* @brief  Video source 0 I2C intialization
* @retval none
*/
void fnI2cInitI2c0()
{
	SN0_SCL_HIGH_SN0_SDA_HIGH;
	SN0_SCL_OUT_SN0_SDA_OUT;
	Port4Mux &= 0x9F;
}

/**
* @fn     fnI2cStartI2c0()
* @brief  Video source 0 I2C START
* @retval none
*/
void fnI2cStartI2c0()
{
	SN0_SCL_HIGH_SN0_SDA_HIGH;
	SN0_SCL_OUT_SN0_SDA_OUT;
	SN0_SDA_LOW;

	fnDelayCycleCounter(34);
}

/**
* @fn     fnI2cReStartI2c0()
* @brief  Video source 0 I2C RE-START
* @retval none
*/
void fnI2cReStartI2c0()
{
	SN0_SCL_HIGH;
	fnDelayCycleCounter(17);
	SN0_SDA_HIGH;
	fnDelayCycleCounter(34);
	SN0_SDA_LOW;
	fnDelayCycleCounter(17);
}

/**
* @fn     fnI2cStopI2c0()
* @brief  Video source 0 I2C STOP
* @retval none
*/
void fnI2cStopI2c0()
{
	// SCL prior state = LOW

	fnDelayCycleCounter(4);

	SN0_SDA_LOW;
	fnDelayCycleCounter(34);

	SN0_SCL_HIGH;
	fnDelayCycleCounter(34);

	SN0_SDA_HIGH;
	fnDelayCycleCounter(34);
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
		fnDelayCycleCounter(20);
		if (ucValue&0x80)
			SN0_SDA_HIGH;
		else
			SN0_SDA_LOW;
		fnDelayCycleCounter(14);
		SN0_SCL_HIGH;
		fnDelayCycleCounter(34);

		ucValue = ucValue<<1;
	}
	// Check I2C ACK from I2C Slave
	SN0_SCL_LOW;			// Ack SCL st. L - 1
	fnDelayCycleCounter(34);

	SN0_SDA_IN;
	SN0_SCL_HIGH;			// Ack SCL st. H - 2
	fnDelayCycleCounter(34);

	ucAck = SN0_SDA_VALUE;	// read Ack
	fnDelayCycleCounter(34);
	SN0_SCL_LOW;			// Ack SCL st. L - 3
	SN0_SDA_OUT;

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
		ucValue = ucValue<<1;
		SN0_SCL_LOW;
		fnDelayCycleCounter(34);
		SN0_SCL_HIGH;
		fnDelayCycleCounter(20);
		if (SN0_SDA_VALUE)
			ucValue |= 1;
		fnDelayCycleCounter(14);
	}
	// Send I2C ACK to I2C Slave
	SN0_SCL_LOW;			// reply N-Ack to stop read SCL St. L - 1
	fnDelayCycleCounter(17);
	SN0_SDA_OUT;
	SN0_SDA_LOW;
	fnDelayCycleCounter(17);
	SN0_SCL_HIGH;			// reply N-Ack to stop read SCL St. H - 2
	fnDelayCycleCounter(34);
	SN0_SCL_LOW;			// reply N-Ack to stop read SCL St. L - 2
	fnDelayCycleCounter(17);

	return ucValue;
}

//========================================================================

/**
* @fn     fnI2cInitI2c1()
* @brief  Video source 1 I2C intialization
* @retval none
*/
void fnI2cInitI2c1()
{
	SN1_SCL_HIGH_SN1_SDA_HIGH;
	SN1_SCL_OUT_SN1_SDA_OUT;
	Port6Mux &= 0xE7;
}

/**
* @fn     fnI2cStartI2c1()
* @brief  Video source 1 I2C START
* @retval none
*/
void fnI2cStartI2c1()
{
	SN1_SCL_HIGH_SN1_SDA_HIGH;
	SN1_SCL_OUT_SN1_SDA_OUT;
	SN1_SDA_LOW;

	fnDelayCycleCounter(34);
}

/**
* @fn     fnI2cReStartI2c1()
* @brief  Video source 1 I2C RE-START
* @retval none
*/
void fnI2cReStartI2c1()
{
	SN1_SCL_HIGH;
	fnDelayCycleCounter(17);
	SN1_SDA_HIGH;
	fnDelayCycleCounter(34);
	SN1_SDA_LOW;
	fnDelayCycleCounter(17);
}

/**
* @fn     fnI2cStopI2c1()
* @brief  Video source 1 I2C STOP
* @retval none
*/
void fnI2cStopI2c1()
{
	// SCL prior state = LOW

	fnDelayCycleCounter(4);

	SN1_SDA_LOW;
	fnDelayCycleCounter(34);

	SN1_SCL_HIGH;
	fnDelayCycleCounter(34);

	SN1_SDA_HIGH;
	fnDelayCycleCounter(34);
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
		fnDelayCycleCounter(20);

		if (ucValue&0x80)
			SN1_SDA_HIGH;
		else
			SN1_SDA_LOW;
		fnDelayCycleCounter(14);
		SN1_SCL_HIGH;
		fnDelayCycleCounter(34);

		ucValue = ucValue<<1;
	}
	// Check I2C ACK from I2C Slave
	SN1_SCL_LOW;			// Ack SCL st. L - 1
	SN1_SDA_IN;
	fnDelayCycleCounter(34);

	SN1_SCL_HIGH;			// Ack SCL st. H - 2
	fnDelayCycleCounter(34);

	ucAck = SN1_SDA_VALUE;	// read Ack

	SN1_SCL_LOW;			// Ack SCL st. L - 3
	fnDelayCycleCounter(34);
	SN1_SDA_OUT;
	SN1_SDA_HIGH;

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
		ucValue = ucValue<<1;
		SN1_SCL_LOW;
		fnDelayCycleCounter(34);
		SN1_SCL_HIGH;
		fnDelayCycleCounter(20);

		if (SN1_SDA_VALUE)
			ucValue |= 1;

		fnDelayCycleCounter(14);
	}
	// Send I2C ACK to I2C Slave
	SN1_SCL_LOW;			// reply N-Ack to stop read SCL St. L - 1
	fnDelayCycleCounter(17);
	SN1_SDA_OUT;
	SN1_SDA_HIGH;
	fnDelayCycleCounter(17);
	SN1_SCL_HIGH;			// reply N-Ack to stop read SCL St. H - 2
	fnDelayCycleCounter(34);
	SN1_SCL_LOW;			// reply N-Ack to stop read SCL St. L - 2
	fnDelayCycleCounter(17);

	return ucValue;
}

#endif // I2C_INTERFACE

