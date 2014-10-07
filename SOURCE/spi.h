//
// Zealtek D2 project
// SPI Library Header File
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 11, 2012.

#ifndef __SPI_H__
#define __SPI_H__

#ifdef SPI_INTERFACE

#include "gendef.h"

extern unsigned long  ulSpiAddr;
extern unsigned short usSpiLength;
extern unsigned char xdata *pBuffer;

#define fnSpiSetSpiAddress(ucAddrLsb,ucAddr2nd,ucAddrMsb)	\
{                                                         \
	SETDWORDLSB(ulSpiAddr, ucAddrLsb);                      \
	SETDWORD2NDLSB(ulSpiAddr, ucAddr2nd);                   \
	SETDWORD2NDMSB(ulSpiAddr, ucAddrMsb);                   \
}

#define fnSpiGetSpiAddress(ucAddrLsb,ucAddr2nd,ucAddrMsb)	\
{                                                         \
	ucAddrLsb  = GETDWORDLSB(ulSpiAddr);                    \
	ucAddr2nd  = GETDWORD2NDLSB(ulSpiAddr);                 \
	ucAddrMsb  = GETDWORD2NDMSB(ulSpiAddr);                 \
}

#define fnSpiSetBufferLength(ucLengthLsb,ucLengthMsb)	    \
{                                                         \
	SETWORDLSB(usSpiLength, ucLengthLsb);                   \
	SETWORDMSB(usSpiLength, ucLengthMsb);                   \
}

#define fnSpiGetBufferLength(ucLengthLsb,ucLengthMsb)	    \
{                                                         \
	ucLengthLsb  = GETWORDLSB(usSpiLength);                 \
	ucLengthMsb  = GETWORDMSB(usSpiLength);                 \
}

#define fnSpiSetBufferAddress(ucAddressLsb,ucAddressMsb)  \
{                                                         \
	SETWORDLSB(pBuffer, ucAddressLsb);                      \
	SETWORDMSB(pBuffer, ucAddressMsb);                      \
}

#define fnSpiGetBufferAddress(ucAddressLsb,ucAddressMsb)  \
{                                                         \
	ucAddressLsb  = GETWORDLSB(pBuffer);                    \
	ucAddressMsb  = GETWORDMSB(pBuffer);                    \
}

extern void fnSpiInit();
extern unsigned char fnSpiCmdRdsr();
extern void fnSpiCmdWren();
extern void fnSpiWriteData();
extern void fnSpiReadData();
extern void fnSpiEraseData();

#endif // SPI_INTERFACE

#endif // __SPI_H__
