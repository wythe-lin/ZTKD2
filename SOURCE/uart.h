//
// Zealtek D2 project
// UART Library Header File
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// April 24, 2012.

#ifndef __UART_H__
#define __UART_H__

#ifdef DBG
extern void fnUartInitUart();
extern void fnUartTxChar(unsigned char ucChar);
extern void fnUartTxCodeString(unsigned char code *aString);
extern void fnUartTxDigit(unsigned char ucDigit);
extern void fnUartTx8bit(unsigned char ucNumber);
#endif // DBG

#endif // __UART_H__
