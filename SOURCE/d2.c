//
// Zealtek D2 project
// D2 Library
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 23, 2012.
	#include <intrins.h>
	#include "system.h"
	#include "sdram.h"
	#include "d2.h"

	// DEBUG_RESET_CONFIGURATION is the debug firmware that is used to
	// cover the hardware board level issue on HSN_VSYNC pin.
	// For normal operation, this option shall be turned off.
	#define DEBUG_RESET_CONFIGURATION
	#ifdef SPI_INTERFACE
		#undef DEBUG_RESET_CONFIGURATION
	#endif // SPI_INTERFACE

//
// D2 Initialization Setting
//
unsigned char xdata ucPIO_HSN        _at_ 0x0019;
unsigned char xdata ucMemBank0       _at_ 0x0020;
unsigned char xdata ucMemBank1       _at_ 0x0021;
unsigned char xdata ucResetControl   _at_ 0x0026;
unsigned char xdata ucResetConfig    _at_ 0x0028;
unsigned char xdata ucSDRControl     _at_ 0x0100;
unsigned char xdata ucI2CMASK        _at_ 0x7FFF;

unsigned char xdata ucSEN_CLK_DIV    _at_ 0x000A;
unsigned char xdata ucSEN_CLK_WIDTH  _at_ 0x000B;
unsigned char xdata ucSOU_CLK_DIV    _at_ 0x0007;
unsigned char xdata ucSOU_CLK_WIDTH  _at_ 0x002B;
unsigned char xdata ucSEN_CLK_SYNC   _at_ 0x0030;
unsigned char xdata ucSIU_CONTROL    _at_ 0x0200;

unsigned char xdata Port4Mux         _at_ 0x0014; // MUX
unsigned char xdata Port4Enable      _at_ 0x0509; // output enable
unsigned char xdata Port4Value       _at_ 0x0508; // output value
unsigned char xdata Port5Mux         _at_ 0x0015; // MUX
unsigned char xdata Port5Enable      _at_ 0x050B; // output enable
unsigned char xdata Port5Value       _at_ 0x050A; // output value
unsigned char xdata Port6Mux         _at_ 0x0016; // MUX
unsigned char xdata Port6Enable      _at_ 0x050D; // output enable
unsigned char xdata Port6Value       _at_ 0x050C; // output value
unsigned char xdata Port7Mux         _at_ 0x0017; // MUX
unsigned char xdata Port7Enable      _at_ 0x050F; // output enable
unsigned char xdata Port7Value       _at_ 0x050E; // output value
unsigned char xdata Port8Mux         _at_ 0x0018; // MUX
unsigned char xdata Port8Enable      _at_ 0x0511; // output enable
unsigned char xdata Port8Value       _at_ 0x0510; // output value
unsigned char xdata Port10Mux        _at_ 0x001A; // MUX


typedef union {
	struct {
		unsigned char	c3;
		unsigned char	c2;
		unsigned char	c1;
		unsigned char	c0;
	} u8;
	struct {
		unsigned short	c1;
		unsigned short	c0;
	} u16;
	unsigned long		u32;
} addrline;


#ifdef FPGA_VERIFY

tD2Register code D2TableSDRAM[] =
{
	SDRAM_CONFIGURATION
	SDRAM_TIMING
	{0x00A, 0x83}, // Fixed Clock to sensors
	{0x00B, 0x82}, // Fixed Clock to sensors
};

#else  // FPGA_VERIFY

#if (CLK_SDRAM == 162) && (CLK_XIN == 27)

tD2Register code D2TableSDRAM[] =
{
	#ifdef DEBUG_RESET_CONFIGURATION
	{0x028, 0x0B}, // Force SDRAM enable
	{0x010, 0xFF}, // MUX SDRAM enable
	{0x011, 0xFF}, // MUX SDRAM enable
	{0x012, 0xFF}, // MUX SDRAM enable
	{0x013, 0xFF}, // MUX SDRAM enable
	{0x014, 0xFF}, // MUX SDRAM enable
	{0x026, 0x00}, // Release SDRAM RESET
	#endif // DEBUG_RESET_CONFIGURATION
	{0x003, 0x00}, // Isolate PLL
	// PLL clk = Xin * PLL_M / PLL_N / (PLL_K + 1)
	//         =  27 *  72   /  4    /  1
	//         = 486MHz
	{0x000, 0x48}, // PLL_M
	{0x001, 0x04}, // PLL_N
	{0x002, 0x00}, // PLL_K
	// SDRAM clk = PLL clk / (SDRAM_DIV + 1)
	//           =  486MHz /   3
	//           =  162MHz
	{0x005, 0x82}, // SDRAM_DIV (max 16)+ clock enable
	{0x02A, 0x01}, // SDRAM_WIDTH
	SDRAM_CONFIGURATION
	SDRAM_TIMING
	// MAIN clk  = SDRAN clk / (MCLK_DIV + 1)
	//           =  162MHz   /   2
	//           =   81MHz
	{0x006, 0x01}, // MCLK_DIV
	// SPI clk   = MAIN  clk / (SPI_DIV + 1)
	//           =   81MHz   /  3
	//           =   27MHz
	{0x008, 0x02}, // SPI_DIV
	{0x009, 0x01}, // SPI_WIDTH
	{0x004, 0x04},
	{0x003, 0x04}, // Use PLL
};

#endif // (CLK_SDRAM == 162) && (CLK_XIN == 27)

#if (CLK_SDRAM == 160) && (CLK_XIN == 12)

tD2Register code D2TableSDRAM[] =
{
	#ifdef DEBUG_RESET_CONFIGURATION
	{0x028, 0x0B}, // Force SDRAM enable
	{0x010, 0xFF}, // MUX SDRAM enable
	{0x011, 0xFF}, // MUX SDRAM enable
	{0x012, 0xFF}, // MUX SDRAM enable
	{0x013, 0xFF}, // MUX SDRAM enable
	{0x014, 0xFF}, // MUX SDRAM enable
	{0x026, 0x00}, // Release SDRAM RESET
	#endif // DEBUG_RESET_CONFIGURATION
	{0x003, 0x00}, // Isolate PLL
	// PLL clk = Xin * PLL_M / PLL_N / (PLL_K + 1)
	//         =  12 *  80   /  3    /  1
	//         = 320MHz
	{0x000, 0x50}, // PLL_M
	{0x001, 0x03}, // PLL_N
	{0x002, 0x00}, // PLL_K
	// SDRAM clk = PLL clk / (SDRAM_DIV + 1)
	//           =  320MHz /   2
	//           =  160MHz
	{0x005, 0x81}, // SDRAM_DIV (max 16)+ clock enable
	{0x02A, 0x01}, // SDRAM_WIDTH
	SDRAM_CONFIGURATION
	SDRAM_TIMING
	// MAIN clk  = SDRAN clk / (MCLK_DIV + 1)
	//           =  160MHz   /   2
	//           =   80MHz
	{0x006, 0x01}, // MCLK_DIV
	// SPI clk   = MAIN  clk / (SPI_DIV + 1)
	//           =   80MHz   /  3
	//           =   26MHz
	{0x008, 0x02}, // SPI_DIV
	{0x009, 0x01}, // SPI_WIDTH
	{0x004, 0x04},
	{0x003, 0x04}, // Use PLL
};

#endif // (CLK_SDRAM == 160) && (CLK_XIN == 12)

#if (CLK_SDRAM == 160) && (CLK_XIN == 24)

tD2Register code D2TableSDRAM[] =
{
	#ifdef DEBUG_RESET_CONFIGURATION
	{0x028, 0x0B}, // Force SDRAM enable
	{0x010, 0xFF}, // MUX SDRAM enable
	{0x011, 0xFF}, // MUX SDRAM enable
	{0x012, 0xFF}, // MUX SDRAM enable
	{0x013, 0xFF}, // MUX SDRAM enable
	{0x014, 0xFF}, // MUX SDRAM enable
	{0x026, 0x00}, // Release SDRAM RESET
	#endif // DEBUG_RESET_CONFIGURATION
	{0x003, 0x00}, // Isolate PLL
	// PLL clk = Xin * PLL_M / PLL_N / (PLL_K + 1)
	//         =  24 *  40   /  3    /  1
	//         = 320MHz
	{0x000, 0x28}, // PLL_M
	{0x001, 0x03}, // PLL_N
	{0x002, 0x00}, // PLL_K
	// SDRAM clk = PLL clk / (SDRAM_DIV + 1)
	//           =  320MHz /   2
	//           =  160MHz
	{0x005, 0x81}, // SDRAM_DIV (max 16)+ clock enable
	{0x02A, 0x01}, // SDRAM_WIDTH
	SDRAM_CONFIGURATION
	SDRAM_TIMING
	// MAIN clk  = SDRAN clk / (MCLK_DIV + 1)
	//           =  160MHz   /   2
	//           =   80MHz
	{0x006, 0x01}, // MCLK_DIV
	// SPI clk   = MAIN  clk / (SPI_DIV + 1)
	//           =   80MHz   /  3
	//           =   26MHz
	{0x008, 0x02}, // SPI_DIV
	{0x009, 0x01}, // SPI_WIDTH
	{0x004, 0x04},
	{0x003, 0x04}, // Use PLL
};

#endif // (CLK_SDRAM == 160) && (CLK_XIN == 24)

#if (CLK_SDRAM == 160) && (CLK_XIN == 48)

tD2Register code D2TableSDRAM[] =
{
	#ifdef DEBUG_RESET_CONFIGURATION
	{0x028, 0x0B}, // Force SDRAM enable
	{0x010, 0xFF}, // MUX SDRAM enable
	{0x011, 0xFF}, // MUX SDRAM enable
	{0x012, 0xFF}, // MUX SDRAM enable
	{0x013, 0xFF}, // MUX SDRAM enable
	{0x014, 0xFF}, // MUX SDRAM enable
	{0x026, 0x00}, // Release SDRAM RESET
	#endif // DEBUG_RESET_CONFIGURATION
	{0x003, 0x00}, // Isolate PLL
	// PLL clk = Xin * PLL_M / PLL_N / (PLL_K + 1)
	//         =  48 *  20   /  3    /  1
	//         = 320MHz
	{0x000, 0x14}, // PLL_M
	{0x001, 0x03}, // PLL_N
	{0x002, 0x00}, // PLL_K
	// SDRAM clk = PLL clk / (SDRAM_DIV + 1)
	//           =  320MHz /   2
	//           =  160MHz
	{0x005, 0x81}, // SDRAM_DIV (max 16)+ clock enable
	{0x02A, 0x01}, // SDRAM_WIDTH
	SDRAM_CONFIGURATION
	SDRAM_TIMING
	// MAIN clk  = SDRAN clk / (MCLK_DIV + 1)
	//           =  160MHz   /   2
	//           =   80MHz
	{0x006, 0x01}, // MCLK_DIV
	// SPI clk   = MAIN  clk / (SPI_DIV + 1)
	//           =   80MHz   /  3
	//           =   26MHz
	{0x008, 0x02}, // SPI_DIV
	{0x009, 0x01}, // SPI_WIDTH
	{0x004, 0x04},
	{0x003, 0x04}, // Use PLL
};

#endif // (CLK_SDRAM == 160) && (CLK_XIN == 48)

#if (CLK_SDRAM == 120) && (CLK_XIN == 12)

tD2Register code D2TableSDRAM[] =
{
	#ifdef DEBUG_RESET_CONFIGURATION
	{0x028, 0x0B}, // Force SDRAM enable
	{0x010, 0xFF}, // MUX SDRAM enable
	{0x011, 0xFF}, // MUX SDRAM enable
	{0x012, 0xFF}, // MUX SDRAM enable
	{0x013, 0xFF}, // MUX SDRAM enable
	{0x014, 0xFF}, // MUX SDRAM enable
	{0x026, 0x00}, // Release SDRAM RESET
	#endif // DEBUG_RESET_CONFIGURATION
	{0x003, 0x00}, // Isolate PLL
	// PLL clk = Xin * PLL_M / PLL_N / (PLL_K + 1)
	//         =  12 *  40   /  2    /  1
	//         = 240MHz
	{0x000, 0x28}, // PLL_M
	{0x001, 0x02}, // PLL_N
	{0x002, 0x00}, // PLL_K
	// SDRAM clk = PLL clk / (SDRAM_DIV + 1)
	//           =  240MHz /   2
	//           =  120MHz
	{0x005, 0x81}, // SDRAM_DIV (max 16)+ clock enable
	{0x02A, 0x01}, // SDRAM_WIDTH
	SDRAM_CONFIGURATION
	SDRAM_TIMING
	// MAIN clk  = SDRAN clk / (MCLK_DIV + 1)
	//           =  120MHz   /   1
	//           =  120MHz
	{0x006, 0x00}, // MCLK_DIV
	// SPI clk   = MAIN  clk / (SPI_DIV + 1)
	//           =  120MHz   /  5
	//           =   24MHz
	{0x008, 0x04}, // SPI_DIV
	{0x009, 0x02}, // SPI_WIDTH
	{0x004, 0x04},
	{0x003, 0x04}, // Use PLL
};

#endif // (CLK_SDRAM == 120) && (CLK_XIN == 12)

#if (CLK_SDRAM == 120) && (CLK_XIN == 24)

tD2Register code D2TableSDRAM[] =
{
	#ifdef DEBUG_RESET_CONFIGURATION
	{0x028, 0x0B}, // Force SDRAM enable
	{0x010, 0xFF}, // MUX SDRAM enable
	{0x011, 0xFF}, // MUX SDRAM enable
	{0x012, 0xFF}, // MUX SDRAM enable
	{0x013, 0xFF}, // MUX SDRAM enable
	{0x014, 0xFF}, // MUX SDRAM enable
	{0x026, 0x00}, // Release SDRAM RESET
	#endif // DEBUG_RESET_CONFIGURATION
	{0x003, 0x00}, // Isolate PLL
	// PLL clk = Xin * PLL_M / PLL_N / (PLL_K + 1)
	//         =  24 *  20   /  2    /  1
	//         = 240MHz
	{0x000, 0x14}, // PLL_M
	{0x001, 0x02}, // PLL_N
	{0x002, 0x00}, // PLL_K
	// SDRAM clk = PLL clk / (SDRAM_DIV + 1)
	//           =  240MHz /   2
	//           =  120MHz
	{0x005, 0x81}, // SDRAM_DIV (max 16)+ clock enable
	{0x02A, 0x01}, // SDRAM_WIDTH
	SDRAM_CONFIGURATION
	SDRAM_TIMING
	// MAIN clk  = SDRAN clk / (MCLK_DIV + 1)
	//           =  120MHz   /   1
	//           =  120MHz
	{0x006, 0x00}, // MCLK_DIV
	// SPI clk   = MAIN  clk / (SPI_DIV + 1)
	//           =  120MHz   /  5
	//           =   24MHz
	{0x008, 0x04}, // SPI_DIV
	{0x009, 0x02}, // SPI_WIDTH
	{0x004, 0x04},
	{0x003, 0x04}, // Use PLL
};

#endif // (CLK_SDRAM == 120) && (CLK_XIN == 24)

#if (CLK_SDRAM == 120) && (CLK_XIN == 48)

tD2Register code D2TableSDRAM[] =
{
	#ifdef DEBUG_RESET_CONFIGURATION
	{0x028, 0x0B}, // Force SDRAM enable
	{0x010, 0xFF}, // MUX SDRAM enable
	{0x011, 0xFF}, // MUX SDRAM enable
	{0x012, 0xFF}, // MUX SDRAM enable
	{0x013, 0xFF}, // MUX SDRAM enable
	{0x014, 0xFF}, // MUX SDRAM enable
	{0x026, 0x00}, // Release SDRAM RESET
	#endif // DEBUG_RESET_CONFIGURATION
	{0x003, 0x00}, // Isolate PLL
	// PLL clk = Xin * PLL_M / PLL_N / (PLL_K + 1)
	//         =  48 *  10   /  2    /  1
	//         = 240MHz
	{0x000, 0x0A}, // PLL_M
	{0x001, 0x02}, // PLL_N
	{0x002, 0x00}, // PLL_K
	// SDRAM clk = PLL clk / (SDRAM_DIV + 1)
	//           =  240MHz /   2
	//           =  120MHz
	{0x005, 0x81}, // SDRAM_DIV (max 16)+ clock enable
	{0x02A, 0x01}, // SDRAM_WIDTH
	SDRAM_CONFIGURATION
	SDRAM_TIMING
	// MAIN clk  = SDRAN clk / (MCLK_DIV + 1)
	//           =  120MHz   /   1
	//           =  120MHz
	{0x006, 0x00}, // MCLK_DIV
	// SPI clk   = MAIN  clk / (SPI_DIV + 1)
	//           =  120MHz   /  5
	//           =   24MHz
	{0x008, 0x04}, // SPI_DIV
	{0x009, 0x02}, // SPI_WIDTH
	{0x004, 0x04},
	{0x003, 0x04}, // Use PLL
};

#endif // (CLK_SDRAM == 120) && (CLK_XIN == 48)

#if (CLK_SDRAM == 108) && (CLK_XIN == 27)

tD2Register code D2TableSDRAM[] =
{
	#ifdef DEBUG_RESET_CONFIGURATION
	{0x028, 0x0B}, // Force SDRAM enable
	{0x010, 0xFF}, // MUX SDRAM enable
	{0x011, 0xFF}, // MUX SDRAM enable
	{0x012, 0xFF}, // MUX SDRAM enable
	{0x013, 0xFF}, // MUX SDRAM enable
	{0x014, 0xFF}, // MUX SDRAM enable
	{0x026, 0x00}, // Release SDRAM RESET
	#endif // DEBUG_RESET_CONFIGURATION
	{0x003, 0x00}, // Isolate PLL
	// PLL clk = Xin * PLL_M / PLL_N / (PLL_K + 1)
	//         =  27 *  32   /  2    /  1
	//         = 432MHz
	{0x000, 0x20}, // PLL_M
	{0x001, 0x02}, // PLL_N
	{0x002, 0x00}, // PLL_K
	// SDRAM clk = PLL clk / (SDRAM_DIV + 1)
	//           =  432MHz /   4
	//           =  108MHz
	{0x005, 0x83}, // SDRAM_DIV (max 16)+ clock enable
	{0x02A, 0x02}, // SDRAM_WIDTH
	SDRAM_CONFIGURATION
	SDRAM_TIMING
	// MAIN clk  = SDRAN clk / (MCLK_DIV + 1)
	//           =  108MHz   /   1
	//           =  108MHz
	{0x006, 0x00}, // MCLK_DIV
	// SPI clk   = MAIN  clk / (SPI_DIV + 1)
	//           =  108MHz   /  4
	//           =   27MHz
	{0x008, 0x03}, // SPI_DIV
	{0x009, 0x02}, // SPI_WIDTH
	{0x004, 0x04},
	{0x003, 0x04}, // Use PLL
};

#endif // (CLK_SDRAM == 108) && (CLK_XIN == 27)

#endif // FPGA_VERIFY

void fnD2Init()
{
	ucPIO_HSN = 0xFF; // Enable HSN_SCL and HSN_SDA
	ucI2CMASK = 0x00; // Disable HW I2C Splitter.

	fnD2WriteTable(D2TableSDRAM, sizeof(D2TableSDRAM) / sizeof(tD2Register));
	while (ucSDRControl != 0)
		;
#if ((CLK_SDRAM == 120) || (CLK_SDRAM == 160))
	#ifdef SPI_INTERFACE
	D2DisableSensorClock(); // Disable Sensor Clock
	D2WriteOutputClock(24); // Set Host Clock in MHz
	#else  // SPI_INTERFACE
	D2WriteSensorClock(24); // Set Sensor Clock in MHz
	D2WriteOutputClock(24); // Set Host Clock in MHz
	#endif // SPI_INTERFACE
#elif ((CLK_SDRAM == 108) || (CLK_SDRAM == 162))
	#ifdef SPI_INTERFACE
	D2DisableSensorClock(); // Disable Sensor Clock
	D2WriteOutputClock(27); // Set Host Clock in MHz
	#else  // SPI_INTERFACE
	D2WriteSensorClock(27); // Set Sensor Clock in MHz
	D2WriteOutputClock(27); // Set Host Clock in MHz
	#endif // SPI_INTERFACE
#else
	#error CLK_SDRAM is not defined properly.
#endif // (CLK_SDRAM)

	Port10Mux = 0xFF;	// Enable Sensor Pixel Clock gpio
}

void fnD2WriteTable(
	tD2Register code *pD2Table, 
	unsigned short usLength)
{
	D2_REG_UNION D2REG;
	unsigned short idx;

	for (idx = 0; idx < usLength; idx ++)
	{
		D2REG.wWord  = pD2Table[idx].D2RegAddress;
		*D2REG.pRegister = pD2Table[idx].D2RegValue;
	#ifdef DBG
		//fnUartTxChar('D');
		//fnUartTxChar('2');
		//fnUartTxChar('R');
		//fnUartTxChar(':');
		//fnUartTx8bit(D2REG.Bytes.HighByte);
		//fnUartTx8bit(D2REG.Bytes.LowByte);
		//fnUartTxChar('=');
		//fnUartTx8bit(pD2Table[idx].D2RegValue);
		//fnUartTxChar('\n');
		//fnUartTxChar('\r');
	#endif // DBG
	}
}

#ifdef SUPPORT_D2_PIO

// D2_PIO0 is assigned to GPIO8.1
// D2_PIO1 is assigned to GPIO8.2

#define D2_PIO0_EN                (Port8Mux    &= 0xFD)
#define D2_PIO0_OUT               (Port8Enable |= 0x02)
#define D2_PIO0_IN                (Port8Enable &= 0xFD)
#define D2_PIO0_HIGH              (Port8Value  |= 0x02)
#define D2_PIO0_LOW               (Port8Value  &= 0xFD)
#define D2_PIO0_DATA              (Port8Value  &  0x02)

#define D2_PIO1_EN                (Port8Mux    &= 0xFB)
#define D2_PIO1_OUT               (Port8Enable |= 0x04)
#define D2_PIO1_IN                (Port8Enable &= 0xFB)
#define D2_PIO1_HIGH              (Port8Value  |= 0x04)
#define D2_PIO1_LOW               (Port8Value  &= 0xFB)
#define D2_PIO1_DATA              (Port8Value  &  0x04)


/**
* @fn     fnD2SetPIO0()
* @brief  PIO0 CONTROL
* @retval none
*/
void fnD2SetPIO0(
	unsigned char ucLevel)
{
	if (ucLevel)
		D2_PIO0_HIGH;
	else
		D2_PIO0_LOW;
	D2_PIO0_OUT;
	D2_PIO0_EN;
}

/**
* @fn     fnD2GetPIO0()
* @brief  PIO0 STATUS
* @retval PIO0 input signal
*/
unsigned char fnD2GetPIO0()
{
	D2_PIO0_IN;
	D2_PIO0_EN;
	return (D2_PIO0_DATA? (1 << 0): 0);
}

/**
* @fn     fnD2SetPIO1()
* @brief  PIO1 CONTROL
* @retval none
*/
void fnD2SetPIO1(
	unsigned char ucLevel)
{
	if (ucLevel)
		D2_PIO1_HIGH;
	else
		D2_PIO1_LOW;
	D2_PIO1_OUT;
	D2_PIO1_EN;
}

/**
* @fn     fnD2GetPIO1()
* @brief  PIO1 STATUS
* @retval PIO1 input signal
*/
unsigned char fnD2GetPIO1()
{
	D2_PIO1_IN;
	D2_PIO1_EN;
	return (D2_PIO1_DATA? (1 << 1): 0);
}

#endif // SUPPORT_D2_PIO

#ifdef SUPPORT_D2_OPERATION

void fnD2IdleDelay(unsigned short usDelay)
{
	unsigned short	idx;

	for (idx=0; idx<usDelay; idx++)
		;
}

void fnD2MoveCodeToDRAM(
	unsigned char code *ucTableData,
	unsigned long ulAddress,
	unsigned short usLength)
{
	D2_REG_UNION	D2REG;
	unsigned short	idx;
	addrline	addr;

	for (idx = 0; idx < usLength; idx++, ulAddress++)
	{
		addr.u32    = (ulAddress << 1UL);
		ucMemBank0  = addr.u8.c2;
//		ucMemBank0  = (unsigned char)(ulAddress >> 15);
		D2REG.wWord = (unsigned short)(ulAddress|0x8000);
		*D2REG.pRegister = ucTableData[idx];
	}
}

void fnD2MoveCodeToSingleBankDRAM(unsigned char code *ucTableData, unsigned long ulAddress, unsigned short usLength)
{
	D2_REG_UNION	D2REG;
	addrline	addr;

	addr.u32    = (ulAddress << 1UL);
	ucMemBank0  = addr.u8.c2;
//	ucMemBank0  = (unsigned char)(ulAddress >> 15);
	D2REG.wWord = (unsigned short) (ulAddress|0x8000);
	for (; usLength != 0; usLength--, D2REG.wWord++, ucTableData++) {
//		do {
			*D2REG.pRegister = *ucTableData;
//		} while (*D2REG.pRegister != *ucTableData);
	}
}

void fnD2VerifyCodeToSingleBankDRAM(
	unsigned char code *ucTableData,
	unsigned long ulAddress,
	unsigned short usLength)
{
	D2_REG_UNION	D2REG;
	addrline	addr;

	addr.u32    = (ulAddress << 1UL);
	ucMemBank0  = addr.u8.c2;
//	ucMemBank0  = (unsigned char)(ulAddress >> 15);
	D2REG.wWord = (unsigned short)(ulAddress|0x8000);
	for (; usLength != 0; usLength--, D2REG.wWord++, ucTableData++)
	{
		while (*D2REG.pRegister != *ucTableData)
			*D2REG.pRegister = *ucTableData;
	}
}

#endif // SUPPORT_D2_OPERATION
