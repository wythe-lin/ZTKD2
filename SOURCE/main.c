//
// Zealtek D2 project
// 8051 Firmware inside Zealtek D2
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// April 23, 2012.

	#include "system.h"
	#include "d2.h"
#ifdef I2C_INTERFACE
	#include "i2c.h"
	#ifdef D2_I2C0_ENABLE
		#include "sensor0.h"
	#endif // D2_I2C0_ENABLE
	#ifdef D2_I2C1_ENABLE
		#include "sensor1.h"
	#endif // D2_I2C1_ENABLE
	#ifdef SUPPORT_I2C_ERROR_COUNT
	#include "gendef.h"
	unsigned long I2CErrorCount;
	#endif // SUPPORT_I2C_ERROR_COUNT
#endif // I2C_INTERFACE
#ifdef SPI_INTERFACE
	#include "spi.h"
#endif // SPI_INTERFACE

#ifdef SUPPORT_D2_OPERATION
	// Constant Information at fixed location for Zealtek Projects.
	// Check out the location on ZT3120 and ZT3150 project files.
	unsigned char code aSize = sizeof(__DATE__)+3;
	unsigned char code aSignature[15] = "ZT " __DATE__;
	unsigned char code *pSignature;

	unsigned char nD2TableVerified;
#endif // SUPPORT_D2_OPERATION

unsigned char xdata ucCmd   _at_ 0x0080;
unsigned char xdata ucPara1 _at_ 0x0081;
unsigned char xdata ucPara2 _at_ 0x0082;
unsigned char xdata ucPara3 _at_ 0x0083;

unsigned char nD2Mode;
unsigned char nD2Resolution;

void fnProcess()
{
	if (ucCmd & CMD_ACK)
		return;

	switch (ucCmd)
	{
		case CMD_D2_SET_OPERATING_MODE:
#ifdef SUPPORT_D2_OPERATION
			if (fnSetOperatingMode(ucPara1, ucPara2) == SUCCESS)
			{
				nD2Mode       = ucPara1; // Operating Mode
				nD2Resolution = ucPara2; // Operating Resolution
				ucCmd  |= CMD_ACK;
			}
			else
#endif // SUPPORT_D2_OPERATION
				ucCmd  |= CMD_ACK+CMD_ERROR;
			break;
		case CMD_D2_GET_OPERATING_MODE:
			ucPara1 = nD2Mode;       // Operating Mode
			ucPara2 = nD2Resolution; // Operating Resolution
			ucCmd  |= CMD_ACK;
			break;
#ifdef I2C_INTERFACE
	#ifdef D2_I2C0_ENABLE
		case CMD_D2_WRITE_SN0_CONF:
			I2CWorkItem0.I2CConfiguration = ucPara1;
			I2CWorkItem0.I2CSlaveAddress  = ucPara2;
			ucCmd  |= CMD_ACK;
			break;
	#endif // D2_I2C0_ENABLE
	#ifdef D2_I2C1_ENABLE
		case CMD_D2_WRITE_SN1_CONF:
			I2CWorkItem1.I2CConfiguration = ucPara1;
			I2CWorkItem1.I2CSlaveAddress  = ucPara2;
			ucCmd  |= CMD_ACK;
			break;
	#endif // D2_I2C1_ENABLE
	#ifdef D2_I2C0_ENABLE
		case CMD_D2_READ_SN0_REG:
			if (fnI2cReadI2c0(&ucPara1) == SUCCESS)
				ucCmd  |= CMD_ACK;
			else
				ucCmd  |= CMD_ACK+CMD_ERROR;
			break;
		case CMD_D2_WRITE_SN0_REG:
			if (fnI2cWriteI2c0(&ucPara1) == SUCCESS)
				ucCmd  |= CMD_ACK;
			else
				ucCmd  |= CMD_ACK+CMD_ERROR;
			break;
	#endif // D2_I2C0_ENABLE
	#ifdef D2_I2C1_ENABLE
		case CMD_D2_READ_SN1_REG:
			if (fnI2cReadI2c1(&ucPara1) == SUCCESS)
				ucCmd  |= CMD_ACK;
			else
				ucCmd  |= CMD_ACK+CMD_ERROR;
			break;
		case CMD_D2_WRITE_SN1_REG:
			if (fnI2cWriteI2c1(&ucPara1) == SUCCESS)
				ucCmd  |= CMD_ACK;
			else
				ucCmd  |= CMD_ACK+CMD_ERROR;
			break;
	#endif // D2_I2C1_ENABLE
	#ifdef SUPPORT_I2C_ERROR_COUNT
		case CMD_D2_GET_I2C_ERROR_CNT:
			ucPara1 = GETDWORDLSB(I2CErrorCount);
			ucPara2 = GETDWORD2NDLSB(I2CErrorCount);
			ucPara3 = GETDWORD2NDMSB(I2CErrorCount);
			ucCmd  |= CMD_ACK;
			break;
		case CMD_D2_SET_I2C_ERROR_CNT:
			SETDWORDLSB(I2CErrorCount, ucPara1);
			SETDWORD2NDLSB(I2CErrorCount, ucPara2);
			SETDWORD2NDMSB(I2CErrorCount, ucPara3);
			ucCmd  |= CMD_ACK;
			break;
	#endif // SUPPORT_I2C_ERROR_COUNT
#endif // I2C_INTERFACE
#ifdef SPI_INTERFACE
		case CMD_D2_GET_FLASH_ADDRESS:
			fnSpiGetSpiAddress(ucPara1, ucPara2, ucPara3);
			ucCmd  |= CMD_ACK;
			break;
		case CMD_D2_SET_FLASH_ADDRESS:
			fnSpiSetSpiAddress(ucPara1, ucPara2, ucPara3);
			ucCmd  |= CMD_ACK;
			break;
		case CMD_D2_READ_FLASH_MEMORY:
			fnSpiSetBufferLength(ucPara1, ucPara2);
			fnSpiReadData();
			ucPara1 = fnSpiCmdRdsr();
			ucCmd  |= CMD_ACK;
			break;
		case CMD_D2_WRITE_FLASH_MEMORY:
			fnSpiSetBufferLength(ucPara1, ucPara2);
			fnSpiWriteData();
			ucPara1 = fnSpiCmdRdsr();
			ucCmd  |= CMD_ACK;
			break;
		case CMD_D2_ERASE_FLASH_MEMORY:
			fnSpiEraseData();
			ucPara1 = fnSpiCmdRdsr();
			ucCmd  |= CMD_ACK;
			break;
		case CMD_D2_GET_BUFFER_ADDRESS:
			fnSpiGetBufferAddress(ucPara1, ucPara2);
			ucCmd  |= CMD_ACK;
			break;
		case CMD_D2_SET_BUFFER_ADDRESS:
			fnSpiSetBufferAddress(ucPara1, ucPara2);
			ucCmd  |= CMD_ACK;
			break;
#endif // SPI_INTERFACE
#ifdef SUPPORT_D2_PIO
		case CMD_D2_GET_PIO:
			ucPara1 = fnD2GetPIO0() | fnD2GetPIO1();
			ucCmd  |= CMD_ACK;
			break;
		case CMD_D2_SET_PIO:
			fnD2SetPIO0(ucPara1 & (1 << 0));
			fnD2SetPIO1(ucPara1 & (1 << 1));
			ucCmd  |= CMD_ACK;
			break;
#endif // SUPPORT_D2_PIO
#ifdef SUPPORT_D2_PAN
		case CMD_D2_PAN_ADJUST:
			if (fnPanSetValue(ucPara1, ucPara2, ucPara3) == SUCCESS)
				ucCmd  |= CMD_ACK;
			else
				ucCmd  |= CMD_ACK+CMD_ERROR;
			break;
#endif // SUPPORT_D2_PAN
		default:
			ucCmd  |= CMD_ACK+CMD_ERROR;
			break;
	}
}

#ifdef SUPPORT_D2_OPERATION
void fnMain()
#else  // SUPPORT_D2_OPERATION
void main() 
#endif // SUPPORT_D2_OPERATION
{
	fnD2Init();

#ifdef I2C_INTERFACE
	#ifdef D2_I2C0_ENABLE
	fnI2cInitI2c0();
	#endif // D2_I2C0_ENABLE
	#ifdef D2_I2C1_ENABLE
	fnI2cInitI2c1();
	#endif // D2_I2C1_ENABLE
	#ifdef SUPPORT_I2C_ERROR_COUNT
	I2CErrorCount = 0L;
	#endif // SUPPORT_I2C_ERROR_COUNT
#endif // I2C_INTERFACE

#ifdef DBG
	{
		unsigned char code aString[] = "ZEALTEK D2 UART\n\r";

		fnUartInitUart();
		fnUartTxCodeString(aString);
	}
#endif // DBG

#ifdef SPI_INTERFACE
	fnSpiInit();
#endif // SPI_INTERFACE

	nD2Mode          = 0;
	nD2Resolution    = 0;
#ifdef SUPPORT_D2_OPERATION
	pSignature       = aSignature;
	nD2TableVerified = 0;
	fnDoInitialization();
#endif // SUPPORT_D2_OPERATION

	ucCmd = CMD_D2_GET_OPERATING_MODE; // first cmd to be executed.

	while (1)
	{
		fnProcess();
#ifdef SUPPORT_D2_OPERATION
		fnDoHouseKeeping();
#endif // SUPPORT_D2_OPERATION
	}
}
