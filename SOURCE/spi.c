//
// Zealtek D2 project
// SPI Library
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 11, 2012.

#include "system.h"

#ifdef SPI_INTERFACE

#include "spi.h"

//#define SPI_DBG

// DEBUG_SDRAM_VERIFY is the debug firmware that is used to
// cover the hardware board level issue on SDRAM interface.
// For normal operation, this option shall be turned off.
#define DEBUG_SDRAM_VERIFY

//
// SPI_CHECK_SPI_STATUS checks the SPI Flash Memory Status on command WREN.
// SPI_FAST_READ speeds up the time on reading data from SPI Flash Memory.
// SPI_BLOCK_ERASE issues the SPI_CMD_BE instead of SPI_CMD_CE on erasing SPI Flash Memory.
//
//#define SPI_CHECK_SPI_STATUS
//#define SPI_FAST_READ
#define SPI_BLOCK_ERASE

// SPI_PAGE_SIZE has to be 32 bytes for the following SPI Flash Memory models:
// MX25L5121E
//
// SPI_PAGE_SIZE can be 256 bytes for the following SPI Flash Memory models:
// MX25L512E
//
#define SPI_PAGE_SIZE 32
//#define SPI_PAGE_SIZE 256
#define SPI_MAX_BUFFER_SIZE 16384
#define SPI_DEF_BUFFER_ADDR 0xA000
//
// SPI Command List
// For SPI Flash Memories that require different SPI command ID, add options here.
//
#define SPI_CMD_WREN		0x06	// Command : Write enable
#define SPI_CMD_WRDI		0x04	// Command : Write disable
#define SPI_CMD_RDID		0x9F	// Command : Read ID
#define SPI_CMD_RDSR		0x05	// Command : Read Status register
#define SPI_CMD_WRSR		0x01	// Command : Write Status register
#define SPI_CMD_READ		0x03	// Command : Read Data
#define SPI_CMD_FASTREAD	0x0B	// Command : Read Data in high speed

#define SPI_CMD_CE			0xC7	// Command : Chip Erase
#define SPI_CMD_BE			0xD8	// Command : Block/Sector Erase, 10000H bytes erase
#define SPI_CMD_SE			0x20	// Command : Sector Erase, 1000H bytes erase, for MXIC only
#define SPI_CMD_PP			0x02	// Command : Page Program, 100H bytes.

#ifdef SPI_ESMT
#define SPI_CMD_AAI			0xAD	// Command : Auto Address Increment Word Program, once 2 bytes.
#define SPI_CMD_EBSY		0x70	// Command : SO as Ready signal while AAI.
#endif // SPI_ESMT

//
// SPI Status Flags
// For SPI Flash Memories that report different SPI status, add options here.
//
#define SPI_SR_WP       0x80  // Write Protect to Status Register
#define SPI_SR_BP       0x1C  // Block Protect (BP0, BP1, and BP2)
#define SPI_SR_WEL      0x02  // Write Enable
#define SPI_SR_WIP      0x01  // Write In Process

unsigned char xdata SpiCS        _at_ 0x0600; // SPI CS#
unsigned char xdata SpiCmdStart  _at_ 0x0602; // SPI Command Start
//
// Settings to SPI CONFIGURATION
#define SPI_CONF_READ        0x00
#define SPI_CONF_WRITE       0x02
#define SPI_CONF_NONE        0x00	//0x04
#define SPI_CONF_ONE_BYTE    0x00
#define SPI_CONF_TWO_BYTES   0x10
#define SPI_CONF_THREE_BYTES 0x20
#define SPI_CONF_FOUR_BYTES  0x30
unsigned char xdata SpiConfig    _at_ 0x0604; // SPI CONFIGURATION
//
// Settings to SPI CONFIGURATION
#define SPI_MODE             0x03
#define SPI_MSB              0x00
#define SPI_LSB              0x04
unsigned char xdata SpiMode      _at_ 0x0605; // SPI MODE

unsigned char xdata SpiCommand   _at_ 0x0606; // SPI CMD
unsigned char xdata SpiParam1    _at_ 0x0607; // SPI PARA1
unsigned char xdata SpiParam2    _at_ 0x0608; // SPI PARA2
unsigned char xdata SpiParam3    _at_ 0x0609; // SPI PARA3

unsigned char xdata SpiTxData    _at_ 0x060E; // SPI TX DATA
unsigned char xdata SpiRxData    _at_ 0x0610; // SPI RX DATA
unsigned char xdata SpiStatus    _at_ 0x0612; // SPI STATUS
unsigned char xdata SpiWidth     _at_ 0x0614; // SPI WIDTH

#define SpiStart()   { SpiCS = 0x00; SpiCmdStart = 0x01; }
#define SpiStop()    { SpiCmdStart = 0x00; SpiCS = 0x01; }

#define SPI_TIMEOUT_COUNT       100

unsigned long  ulSpiAddr;
unsigned short usSpiLength;
unsigned char xdata *pBuffer;


#define CODE_AT_SPI  0
#define CODE_AT_SRAM 1

unsigned char xdata d2CodeSpace     _at_ 0x0025; // Code Space

/**
* @fn     void fnSpiInit()
* @brief  Initialize SPI Library.
* @retval none
*/
void fnSpiInit()
{
	SpiMode     = SPI_MODE+SPI_MSB;
	//SpiWidth    = 0x00;
	pBuffer     = SPI_DEF_BUFFER_ADDR; //default buffer address from XDATA SPI_DEF_BUFFER_ADDR.
	ulSpiAddr   = 0L;     //default SPI Flash memory from 0x00000000.
	usSpiLength = SPI_MAX_BUFFER_SIZE; //default SPI buffer length SPI_MAX_BUFFER_SIZE.

#ifdef DBG
	if (d2CodeSpace == CODE_AT_SRAM)
	{
		unsigned char code sramString[] = "@SRAM\n\r";

		fnUartTxCodeString(sramString);
	}
	else
	{
		unsigned char code spiString[] = "@SPI\n\r";

		fnUartTxCodeString(spiString);
	}
#endif // DBG
}

/**
* @fn     void fnSpiWaitIdle()
* @brief  Delay some time.
* @retval none
*/
void fnSpiWaitIdle(unsigned short nCount)
{
	unsigned short nLoop;
	for (nLoop=0; nLoop<nCount; nLoop++)
		;
}		

/**
* @fn     void fnSpiWaitComplete()
* @brief  Wait until SPI process completion.
* @retval none
*/
unsigned char fnSpiWaitComplete()
{
	unsigned char ucStatus;
	unsigned char ucCount;

	ucStatus = SpiStatus;	// Dummy Read
	for (ucCount = 0; ucCount < SPI_TIMEOUT_COUNT; ucCount++)
	{
		if (SpiStatus == 0x00)
			return SUCCESS;
	}
#ifdef DBG
	//fnUartTxChar('F');
	//fnUartTx8bit(ucCount);
#endif // DBG
	return FAILURE;
}

/**
* @fn     void fnSpiCmdWrsr()
* @brief  Write SPI Status to SPI slave.
* @retval none
*/
void fnSpiCmdWrsr(
	unsigned char ucValue)
{
	SpiConfig  = SPI_CONF_WRITE+SPI_CONF_TWO_BYTES;
	//SpiMode    = SPI_MODE+SPI_MSB;
	SpiCommand = SPI_CMD_WRSR;
	SpiParam1  = ucValue;
	SpiStart();
	fnSpiWaitComplete();
	SpiStop();
}

/**
* @fn     void fnSpiCmdRdsr()
* @brief  Read SPI Status from SPI slave.
* @retval Content in SPI Status Register
*/
unsigned char fnSpiCmdRdsr()
{
	unsigned char ucSR;

	fnSpiWaitIdle(100);
	SpiConfig  = SPI_CONF_READ+SPI_CONF_ONE_BYTE;
	//SpiMode    = SPI_MODE+SPI_MSB;
	SpiCommand = SPI_CMD_RDSR;
	SpiStart();
	fnSpiWaitComplete();
	ucSR = SpiRxData; // Dummy Read
	fnSpiWaitComplete();
	ucSR = SpiRxData; // Real Read
#ifdef DBG
	fnUartTxChar('R');
	fnUartTx8bit(ucSR);
#else  // DBG
	fnSpiWaitIdle(10);
#endif // DBG
	SpiStop();
	return ucSR;
}


/**
* @fn     void fnSpiCmdWren()
* @brief  Issue SPI command Write Enable.
* @retval none
*/
void fnSpiCmdWren()
{
#ifdef SPI_CHECK_SPI_STATUS
	unsigned char ucLoop;

	ucLoop = 0;
	while (!(fnSpiCmdRdsr() & SPI_SR_WEL) && ucLoop++ < SPI_TIMEOUT_COUNT)
#endif // SPI_CHECK_SPI_STATUS
	{
		SpiConfig  = SPI_CONF_NONE+SPI_CONF_ONE_BYTE;
		//SpiMode    = SPI_MODE+SPI_MSB;
		SpiCommand = SPI_CMD_WREN;
		SpiStart();
		fnSpiWaitComplete();
		SpiStop();
	}
}


/**
* @fn     void fnSpiWriteData()
* @brief  Write at most SPI_MAX_BUFFER_SIZE data to SPI Flash Memory.
* @retval none
*/
void fnSpiWriteData()
{
	unsigned char xdata *pData;
	unsigned short usPageWriteSize;
	unsigned short idx;
	unsigned short usCount;
	unsigned short usRemainSize;

	usRemainSize = (usSpiLength > SPI_MAX_BUFFER_SIZE) ? SPI_MAX_BUFFER_SIZE : usSpiLength;
	pData = pBuffer;
	usPageWriteSize = SPI_PAGE_SIZE - (ulSpiAddr & (SPI_PAGE_SIZE-1));
	fnSpiWaitIdle(1000);

	if ((fnSpiCmdRdsr() & SPI_SR_BP) != 0)
	{
		fnSpiCmdWren();
		fnSpiCmdWrsr(0x00);
		fnSpiWaitIdle(8000);
		while ((fnSpiCmdRdsr() & SPI_SR_WIP))
			fnSpiWaitIdle(100);
	}

	while (usRemainSize > 0)
	{
		if (usRemainSize <= usPageWriteSize)
			usPageWriteSize = usRemainSize;
		fnSpiCmdWren();

		//page write
		SpiConfig  = SPI_CONF_WRITE+SPI_CONF_FOUR_BYTES;
		//SpiMode    = SPI_MODE+SPI_MSB;
		SpiCommand = SPI_CMD_PP;
		SpiParam1  = GETDWORD2NDMSB(ulSpiAddr);
		SpiParam2  = GETDWORD2NDLSB(ulSpiAddr);
		SpiParam3  = GETDWORDLSB(ulSpiAddr);
		SpiStart();
		fnSpiWaitComplete();

		for (idx = 0; idx < usPageWriteSize; idx++)
		{
#ifdef SPI_DBG
			if (idx%32==0)
			{
				fnUartTxChar('\n');
				fnUartTxChar('\r');
				fnUartTx8bit(' ');
				fnUartTx8bit(' ');
				fnUartTxChar(':');
			}
			fnUartTxChar(' ');
			fnUartTx8bit(*pData);
#endif // SPI_DBG
			SpiTxData = *pData;
			pData++;
			fnSpiWaitIdle(32);
			//fnSpiWaitComplete();
		}
		fnSpiWaitIdle(100);
		SpiStop();

		usRemainSize   -= usPageWriteSize;
		ulSpiAddr      += usPageWriteSize;
		usPageWriteSize = SPI_PAGE_SIZE;

		for (usCount = 0; usCount < SPI_TIMEOUT_COUNT; usCount++)
		{
			if ((fnSpiCmdRdsr() & SPI_SR_WIP) == 0)
				break;
		}
		fnSpiWaitIdle(100);
	}
}

/**
* @fn     void fnSpiReadData()
* @brief  Read at most SPI_MAX_BUFFER_SIZE data from SPI Flash Memory.
* @retval none
*/
void fnSpiReadData()
{
	unsigned char xdata *pData;
	unsigned char ucValue;
	unsigned short usRemainSize;

	usRemainSize = (usSpiLength > SPI_MAX_BUFFER_SIZE) ? SPI_MAX_BUFFER_SIZE : usSpiLength;
	pData = pBuffer;

	SpiConfig  = SPI_CONF_READ+SPI_CONF_FOUR_BYTES;
	//SpiMode    = SPI_MODE+SPI_MSB;
	SpiCommand = SPI_CMD_READ;
	SpiParam1  = GETDWORD2NDMSB(ulSpiAddr);
	SpiParam2  = GETDWORD2NDLSB(ulSpiAddr);
	SpiParam3  = GETDWORDLSB(ulSpiAddr);
	SpiStart();
	fnSpiWaitComplete();
	ucValue = SpiRxData; // Dummy Read

	while (usRemainSize > 0)
	{
		fnSpiWaitComplete();
		ucValue = SpiRxData; // Real Read

#ifdef SPI_DBG
		if (usRemainSize%32 == 0)
		{
			fnUartTxChar('\n');
			fnUartTxChar('\r');
			fnUartTx8bit(GETDWORD2NDLSB(ulSpiAddr));
			fnUartTx8bit(GETDWORDLSB(ulSpiAddr));
			fnUartTxChar(':');
		}
		fnUartTxChar(' ');
		fnUartTx8bit(ucValue);
		ulSpiAddr++;
#endif // SPI_DBG
		*pData = ucValue;
#ifdef DEBUG_SDRAM_VERIFY
		while (*pData != ucValue)
			*pData = ucValue;
#endif // DEBUG_SDRAM_VERIFY
		pData++;
		usRemainSize--;
	}
	SpiStop();
}

/**
* @fn     void fnSpiEraseData()
* @brief  Erase one block memory in SPI Flash Memory.
*         Block size is defined by SPI FLash Memory Datasheets
* @retval none
*/
void fnSpiEraseData()
{
	fnSpiWaitIdle(1000);
	if ((fnSpiCmdRdsr() & SPI_SR_BP) != 0)
	{
		fnSpiCmdWren();
		fnSpiCmdWrsr(0x00);
		fnSpiWaitIdle(8000);
		while ((fnSpiCmdRdsr() & SPI_SR_WIP))
			fnSpiWaitIdle(100);
	}
	fnSpiCmdWren();
	if ((fnSpiCmdRdsr() & SPI_SR_WEL) == 0)
		return;
#ifdef SPI_BLOCK_ERASE
	{
		SpiConfig  = SPI_CONF_NONE+SPI_CONF_FOUR_BYTES;
		//SpiMode    = SPI_MODE+SPI_MSB;
		SpiCommand = SPI_CMD_BE;
		SpiParam1  = GETDWORD2NDMSB(ulSpiAddr);
		SpiParam2  = GETDWORD2NDLSB(ulSpiAddr);
		SpiParam3  = 0x00;//GETDWORDLSB(ulSpiAddr);
		SpiStart();
		fnSpiWaitComplete();
		SpiStop();
	}
#else  // SPI_BLOCK_ERASE
	{
		SpiConfig  = SPI_CONF_NONE+SPI_CONF_ONE_BYTE;
		//SpiMode    = SPI_MODE+SPI_MSB;
		SpiCommand = SPI_CMD_CE;
		SpiStart();
		fnSpiWaitComplete();
		SpiStop();
	}
#endif // SPI_BLOCK_ERASE
	while ((fnSpiCmdRdsr() & SPI_SR_WIP))
		fnSpiWaitIdle(2000);
}

#endif // SPI_INTERFACE
