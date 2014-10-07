//
// Zealtek D2 project
// D2 Library for the Second Layer D2 Operation
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// August 16, 2013.

#include "system.h"
#include "d2.h"
#include "sensor0.h"
#include "sensor1.h"
#include "i2c.h"
#include "d2layer2.h"

#ifndef D2_I2C0_ENABLE
	#error D2_I2C0_ENABLE and D2_I2C1_ENABLE shall be both enabled.
#endif // D2_I2C0_ENABLE
#ifndef D2_I2C1_ENABLE
	#error D2_I2C0_ENABLE and D2_I2C1_ENABLE shall be both enabled.
#endif // D2_I2C1_ENABLE

#ifdef SUPPORT_D2_LAYER2

// D2_LAYER2_SET_POWER_STRAP is used to set proper strapping values to layer 2 devices.
// This option is expected to not enabled by Wythe.
// More tests are required before disabling this option.
#define D2_LAYER2_SET_POWER_STRAP

#define D2_TIMEOUT_COUNT	10000
#define D2_ENABLING_COUNT	10000

// Provide proper GPIO settings to second layer D2 device.
//
// For SN0_RESET, GPIO8.1, 0 = reset, 1 = normal.
// For SN0_D2,    GPIO4.7, 0 = sensor bypass source SIU0, 1 = SIU1.
// For SN0_D3,    GPIO5.0, 1 = sensor not bypass.
// For SN0_D4,    GPIO5.1, 0 = I2C slave address 0x04, 1 = 0x06.
// For SN0_D5,    GPIO5.2, 0 = D2 not testing mode.
// For SN0_D6,    GPIO5.3, 0 = D2 MBIST disable.
// For SN0_VSYNC, GPIO5.7, 1 = D2 SPI enable, 0 = disable.
// For SN0_HSYNC, GPIO6.0, 1 = D2 SDRAM enable, 0 = disable.
#define BIT_SN0_RESET		(1<<1)
#define BIT_SN0_D2		(1<<7)
#define BIT_SN0_D3		(1<<0)
#define BIT_SN0_D4		(1<<1)
#define BIT_SN0_D5		(1<<2)
#define BIT_SN0_D6		(1<<3)
#define BIT_SN0_VSYNC		(1<<7)
#define BIT_SN0_HSYNC		(1<<0)

//
// For SN1_RESET, GPIO8.2, 0 = reset, 1 = normal.
// For SN1_D2,    GPIO6.5, 0 = sensor bypass source SIU0, 1 = SIU1.
// For SN1_D3,    GPIO6.6, 1 = sensor not bypass.
// For SN1_D4,    GPIO6.7, 0 = I2C slave address 0x04, 1 = 0x06.
// For SN1_D5,    GPIO7.0, 0 = D2 not testing mode.
// For SN1_D6,    GPIO7.1, 0 = D2 MBIST disable.
// For SN1_VSYNC, GPIO7.5, 1 = D2 SPI enable, 0 = disable.
// For SN1_HSYNC, GPIO7.6, 1 = D2 SDRAM enable, 0 = disable.
#define BIT_SN1_RESET		(1<<2)
#define BIT_SN1_D2		(1<<5)
#define BIT_SN1_D3		(1<<6)
#define BIT_SN1_D4		(1<<7)
#define BIT_SN1_D5		(1<<0)
#define BIT_SN1_D6		(1<<2)
#define BIT_SN1_VSYNC		(1<<5)
#define BIT_SN1_HSYNC		(1<<6)

void fnD2SetSN0Reset(void)
{
	Port8Value  &= 0xFF - BIT_SN0_RESET; // output low.
	Port8Enable |= BIT_SN0_RESET; // output enable.
	Port8Mux    &= 0xFF - BIT_SN0_RESET;
#ifdef D2_LAYER2_SET_POWER_STRAP
	// I2C slave address = 0x04, SPI enabled, SDRAM enabled.
	Port5Value  &= 0xFF - (BIT_SN0_D6 + BIT_SN0_D5 + BIT_SN0_D4);
	Port5Value  |= BIT_SN0_VSYNC + BIT_SN0_D3;
	Port5Enable |= BIT_SN0_VSYNC + BIT_SN0_D6 + BIT_SN0_D5 + BIT_SN0_D4 + BIT_SN0_D3;
	Port6Value  |= BIT_SN0_HSYNC;
	Port6Enable |= BIT_SN0_HSYNC;
	// Make SN0 pin as GPIO output pins.
	Port5Mux    &= 0xFF - (BIT_SN0_VSYNC + BIT_SN0_D6 + BIT_SN0_D5 + BIT_SN0_D4 + BIT_SN0_D3);
	Port6Mux    &= 0xFF - BIT_SN0_HSYNC;
#endif
}

void fnD2ClrSN0Reset(void)
{
	Port8Mux    &= 0xFF - BIT_SN0_RESET;
	Port8Value  |= BIT_SN0_RESET; // output high.
	Port8Enable |= BIT_SN0_RESET; // output enable.
#ifdef D2_LAYER2_SET_POWER_STRAP
	{
		unsigned char ucLoop;
		for (ucLoop = 100; ucLoop > 0; ucLoop--)
			;

		// Restore SN0 normal functions.
		Port5Mux    |= BIT_SN0_VSYNC + BIT_SN0_D6 + BIT_SN0_D5 + BIT_SN0_D4 + BIT_SN0_D3;
		Port6Mux    |= BIT_SN0_HSYNC;
	}
#endif
}

void fnD2SetSN1Reset(void)
{
	Port8Value  &= 0xFF - BIT_SN1_RESET; // output low.
	Port8Enable |= BIT_SN1_RESET; // output enable.
	Port8Mux    &= 0xFF - BIT_SN1_RESET;
#ifdef D2_LAYER2_SET_POWER_STRAP
	// I2C slave address = 0x04, SPI enabled, SDRAM enabled.
	Port6Value  &= 0xFF - BIT_SN1_D4;
	Port6Value  |= BIT_SN1_D3;
	Port6Enable |= BIT_SN1_D4 + BIT_SN1_D3;
	Port7Value  &= 0xFF - (BIT_SN1_D6 + BIT_SN1_D5);
	Port7Value  |= BIT_SN1_HSYNC + BIT_SN1_VSYNC;
	Port7Enable |= BIT_SN1_HSYNC + BIT_SN1_VSYNC + BIT_SN1_D6 + BIT_SN1_D5;
	// Make SN1 pin as GPIO output pins.
	Port6Mux    &= 0xFF - (BIT_SN1_D4 + BIT_SN1_D3);
	Port7Mux    &= 0xFF - (BIT_SN1_HSYNC + BIT_SN1_VSYNC + BIT_SN1_D6 + BIT_SN1_D5);
#endif
}

void fnD2ClrSN1Reset(void)
{
	Port8Value  |= BIT_SN1_RESET; // output high.
	Port8Enable |= BIT_SN1_RESET; // output enable.
	Port8Mux    &= 0xFF - BIT_SN1_RESET;
#ifdef D2_LAYER2_SET_POWER_STRAP
	{
		unsigned char ucLoop;
		for (ucLoop = 100; ucLoop > 0; ucLoop--)
			;

		// Restore SN1 normal functions.
		Port6Mux |= BIT_SN1_D4 + BIT_SN1_D3;
		Port7Mux |= BIT_SN1_HSYNC + BIT_SN1_VSYNC + BIT_SN1_D6 + BIT_SN1_D5;
	}
#endif
}

unsigned char fnD2SetSN0Command(unsigned char ucMode, unsigned char ucResolution)
{
	unsigned char  ucCmdAck;

	I2CREAD_SN0_REGISTER(0x0080, ucCmdAck);
	if (ucCmdAck & 0x80) {			// 0x80 : Ack Command
		I2CWRITE_SN0_REGISTER(0x0081, ucMode);
		I2CWRITE_SN0_REGISTER(0x0082, ucResolution);
		I2CWRITE_SN0_REGISTER(0x0080, CMD_D2_SET_OPERATING_MODE);
		return STATUS_D2_CMD_OK;
	} else {
		return STATUS_D2_CMD_NOT_SENT;
	}
}

unsigned char fnD2SetSN1Command(unsigned char ucMode, unsigned char ucResolution)
{
	unsigned char  ucCmdAck;

	I2CREAD_SN1_REGISTER(0x0080, ucCmdAck);
	if (ucCmdAck & 0x80) {			// 0x80 : Ack Command
		I2CWRITE_SN1_REGISTER(0x81, ucMode);
		I2CWRITE_SN1_REGISTER(0x82, ucResolution);
		I2CWRITE_SN1_REGISTER(0x80, CMD_D2_SET_OPERATING_MODE);
		return STATUS_D2_CMD_OK;
	} else {
		return STATUS_D2_CMD_NOT_SENT;
	}
}

unsigned char fnD2GetSN0Status(void)
{
	unsigned char  ucCmdAck;
	unsigned short usCount = 0;

	while (usCount < D2_TIMEOUT_COUNT) {
		usCount++;
		I2CREAD_SN0_REGISTER(0x0080, ucCmdAck);
		if (ucCmdAck&0x80)
			break;
	}

	if (usCount >= D2_TIMEOUT_COUNT) {
		return STATUS_D2_CMD_TIMEOUT;
	}
	if (ucCmdAck & 0x40) {
		return STATUS_D2_CMD_ERROR;
	}
	return STATUS_D2_CMD_OK;
}

unsigned char fnD2GetSN1Status(void)
{
	unsigned char  ucCmdAck;
	unsigned short usCount = 0;

	while (usCount < D2_TIMEOUT_COUNT) {
		usCount++;
		I2CREAD_SN1_REGISTER(0x0080, ucCmdAck);
		if (ucCmdAck&0x80)
			break;
	}

	if (usCount >= D2_TIMEOUT_COUNT) {
		return STATUS_D2_CMD_TIMEOUT;
	}
	if (ucCmdAck & 0x40) {
		return STATUS_D2_CMD_ERROR;
	}
	return STATUS_D2_CMD_OK;
}

#endif // SUPPORT_D2_LAYER2
