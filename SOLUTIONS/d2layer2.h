//
// Zealtek D2 project
// D2 Library for the Second Layer D2 Operation
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// August 16, 2013.

#ifndef __D2LAYER2_H__
#define __D2LAYER2_H__

#ifdef SUPPORT_D2_LAYER2

	#define STATUS_D2_CMD_OK           0x00
	#define STATUS_D2_CMD_NOT_SENT     0x01
	#define STATUS_D2_CMD_ERROR        0x02
	#define STATUS_D2_CMD_TIMEOUT      0x03

	extern void fnD2SetSN0Reset();
	extern void fnD2SetSN1Reset();
	extern void fnD2ClrSN0Reset();
	extern void fnD2ClrSN1Reset();
	extern unsigned char fnD2SetSN0Command(
		unsigned char ucMode,
		unsigned char ucResolution);
	extern unsigned char fnD2SetSN1Command(
		unsigned char ucMode,
		unsigned char ucResolution);
	extern unsigned char fnD2GetSN0Status();
	extern unsigned char fnD2GetSN1Status();
#endif // SUPPORT_D2_LAYER2

#endif // __D2LAYER2_H__
