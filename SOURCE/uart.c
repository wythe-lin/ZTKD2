//
// Zealtek D2 project
// UART Library
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// April 24, 2012.

#include "system.h"
#include "uart.h"
#include <reg51.h>

#ifdef DBG

unsigned char xdata UartMux  _at_ 0x0040;

/**
* @fn     void fnUartInitUart()
* @brief  8031 internal Uart0 intialization  
* @retval none
*/
void fnUartInitUart()
{
	unsigned char ucEAState = EA;

	// Enable UART interface for D2 8051
	UartMux  |= 0x02; //D2 8051 MUX

	SCON = 0x52;  // Set Uart as mode 1 , receive enable
	TMOD = 0x20;  // Set timer 1 clock by t1 clock and 8bit counter with auto-reload
#if (CLK_MAIN==80)
	TH1 = 0x7E;  //timer reload value
#elif (CLK_MAIN==120)
	TH1 = 0x3D;  //timer reload value
#endif // (CLK_MAIN)
	TF1 = 0;
	TR1 = 1;  // Enable timer 1
	EA  = 1 & ucEAState; // Enable global interrupt (restore back)
	//ES = 1; // Enable serial port interrupt
}

/**
* @fn     void fnUartTxChar(unsigned char ucTransmittChar)
* @brief  Transmit one character via UART0   
* @param  ucTransmittChar : charater to be transmitted  
* @retval none
*/
void fnUartTxChar(unsigned char ucTransmitChar)
{
	TI = 0;
	SBUF = ucTransmitChar; // Send char by RBR
	while (!TI)
		;
	TI = 0;
}

void fnUartTxCodeString(unsigned char code *aString)
{
	while (*aString != 0)
	{
		fnUartTxChar(*aString);
		aString++;
	}
}

#if 0
void fnUartTxString(unsigned char *aString)
{
	while (*aString != 0)
	{
		fnUartTxChar(*aString);
		aString++;
	}
}
#endif

void fnUartTxDigit(unsigned char ucDigit)
{
	if (ucDigit <= 9)
		fnUartTxChar(ucDigit+'0');
	else
		fnUartTxChar(ucDigit-10+'A');
}

void fnUartTx8bit(unsigned char ucNumber)
{
	fnUartTxDigit((ucNumber>>4)&0x0F);
	fnUartTxDigit((ucNumber&0x0F));
}

#endif // DBG
