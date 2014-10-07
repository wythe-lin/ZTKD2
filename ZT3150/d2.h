//
// Zealtek D2 project
// D2 Library Header File
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 23, 2012.

#ifndef __D2_H__
#define __D2_H__

extern unsigned char xdata ucResetControl;
extern unsigned char xdata ucResetConfig;
extern unsigned char xdata ucSEN_CLK_DIV;
extern unsigned char xdata ucSEN_CLK_WIDTH;
extern unsigned char xdata ucSOU_CLK_DIV;
extern unsigned char xdata ucSOU_CLK_WIDTH;
extern unsigned char xdata ucSEN_CLK_SYNC;
extern unsigned char xdata ucSIU_CONTROL;

extern unsigned char xdata Port4Mux;
extern unsigned char xdata Port4Enable;
extern unsigned char xdata Port4Value;
extern unsigned char xdata Port6Mux;
extern unsigned char xdata Port6Enable;
extern unsigned char xdata Port6Value;
extern unsigned char xdata Port8Mux;
extern unsigned char xdata Port8Enable;
extern unsigned char xdata Port8Value;
extern unsigned char xdata Port10Mux;

	#ifdef FPGA_VERIFY
#define D2DisableSensorClock() { }
#define D2WriteSensorClock(v)  { }
#define D2WriteOutputClock(v)  { ucSOU_CLK_DIV = ((CLK_PLL/(v)-1)); ucSOU_CLK_WIDTH = (CLK_PLL/(v)/2); }
	#else  // FPGA_VERIFY
#define D2DisableSensorClock() { ucSEN_CLK_DIV = 0; ucSEN_CLK_WIDTH = 0; }
#define D2WriteSensorClock(v)  { ucSEN_CLK_DIV = ((CLK_PLL/(v)-1) | 0x80); ucSEN_CLK_WIDTH = ((CLK_PLL/(v)/2) | 0x80); }
#define D2WriteOutputClock(v)  { ucSOU_CLK_DIV = ((CLK_PLL/(v)-1)); ucSOU_CLK_WIDTH = (CLK_PLL/(v)/2); }
	#endif // FPGA_VERIFY

	typedef union
	{
		struct
		{
			unsigned char HighByte;
			unsigned char LowByte;
		} Bytes;
		unsigned short wWord;
		unsigned char xdata *pRegister;
	} D2_REG_UNION;

	typedef struct _tD2Register
	{
		unsigned short D2RegAddress;
		unsigned char  D2RegValue;
	} tD2Register;

	extern void fnD2Init();
	extern void fnD2WriteTable(tD2Register code *pD2Table, unsigned short usLength);

#ifdef SUPPORT_D2_PIO
	extern void fnD2SetPIO0(unsigned char ucLevel);
	extern unsigned char fnD2GetPIO0();
	extern void fnD2SetPIO1(unsigned char ucLevel);
	extern unsigned char fnD2GetPIO1();
#endif // SUPPORT_D2_PIO

#ifdef SUPPORT_D2_OPERATION
	extern void fnD2IdleDelay(unsigned short usDelay);
	extern void fnD2MoveCodeToDRAM(unsigned char code *ucTableData, unsigned long ulAddress, unsigned short usLength);
	extern void fnD2MoveCodeToSingleBankDRAM(unsigned char code *ucTableData, unsigned long ulAddress, unsigned short usLength);
	extern void fnD2VerifyCodeToSingleBankDRAM(unsigned char code *ucTableData, unsigned long ulAddress, unsigned short usLength);
#endif // SUPPORT_D2_OPERATION

#endif // __D2_H__
