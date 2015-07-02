//
// Zealtek D2 project
// ZT3120@CVBS solutions
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// Nov 20, 2012.

#include "system.h"
#include "d2.h"
#include "gendef.h"
#include "sensor0.h"
#include "sensor1.h"

#ifdef SUPPORT_D2_OPERATION

	#define CLK_SENSOR  24
	#if (CLK_PLL / CLK_SENSOR >= 16)
		#error Maximum Clock Divider for CLK_SENSOR is 16.
	#endif // (CLK_PLL / CLK_SENSOR >= 16)

	#if ZT3120_DECODER == ZT3120_DECODER_ADV7180

tI2CRegister code I2CInitTableNTSC[] =
{
	//{0x00, 0x04},	// Composite decoder Channel; 0000b = auto detect PAL B/G/H/I/D, NTSC J, SECAM
	{0x00, 0x14},	// Composite decoder Channel; 0001b = auto detect PAL B/G/H/I/D, NTSC M, SECAM
	//{0x00, 0x54},	// Composite decoder Channel; 0101b = NTSC M
	//{0x04, 0x57},	// Enable SFL
	{0x04, 0xD7},	// To fix display blank issue.
	{0x17, 0x41}, // Select SH1
	{0x31, 0x02}, // Clear NEWAVMODE, SAV/EAV to suit ADV video encoders
	{0x3d, 0xa2}, // Color Kill
	{0x3e, 0x6a}, // BLM optimization
	{0x3f, 0xa0}, // BGB optimization
	{0x0e, 0x80}, // Enter hidden space
	{0x55, 0x81}, // ADC configuration
	{0x0e, 0x00}, // Enter user space
	//{0x08, 0x88}, // Contrast
};


tI2CRegister code I2CInitTablePAL[] =
{
	//{0x00, 0x04},	// Composite decoder Channel; 0000b = auto detect PAL B/G/H/I/D, NTSC J, SECAM
	{0x00, 0x14},	// Composite decoder Channel; 0001b = auto detect PAL B/G/H/I/D, NTSC M, SECAM
	//{0x00, 0x84},	// Composite decoder Channel; 1000b = PAL B/G/H/I/D
	//{0x04, 0x57},	// Enable SFL
	{0x04, 0xD7},	// To fix display blank issue.
	{0x17, 0x41}, // Select SH1
	{0x31, 0x02}, // Clear NEWAVMODE, SAV/EAV to suit ADV video encoders
	{0x3d, 0xa2}, // Color Kill
	{0x3e, 0x6a}, // BLM optimization
	{0x3f, 0xa0}, // BGB optimization
	{0x0e, 0x80}, // Enter hidden space
	{0x55, 0x81}, // ADC configuration
	{0x0e, 0x00}, // Enter user space
	//{0x08, 0x88}, // Contrast
};

	#elif ZT3120_DECODER == ZT3120_DECODER_T515B

tI2CRegister code I2CInitTableNTSC[] =
{
	{0x5F, 0x01},			//RESET
	{0x0A, 0x20},
	{0x0F, 0x00},
	{0x11, 0x0C},
	{0x18, 0x00},			//$B0,W,Y,$0000,18,00,48,83,
	{0x19, 0x0A},			//0x0A, 0x08
	//{0x19, 0x08},
	//{0x1A, 0x83},
	{0x20, 0x00},			//Video Source: NTSC
	{0x21, 0x09},
	{0x59, 0x8A},			//Video 0:Odd Fields; 1:Even Fields
	//{0x59, 0x0A},			//Video 1:Odd Fields; 0:Even Fields
	{0xD2, 0x26},
	{0xD6, 0x01},
	{0xE0, 0xB0},

	{0x38, 0x21},
	{0x39, 0xF0},
	{0x3A, 0x7C},
	{0x3B, 0x0F},
	//{0x27, 0x20},		//blue screen 0x20:auto,  0x10:enable
	{0x5F, 0x00},			//Release RESET
};


tI2CRegister code I2CInitTablePAL[] =
{
	{0x5F, 0x01},			//RESET
	{0x0A, 0x20},
	{0x0F, 0x00},
	{0x11, 0x0C},
	{0x18, 0x00},			//$B0,W,Y,$0000,18,00,48,83,
	{0x19, 0x0A},			//0x0A, 0x08
	//{0x19, 0x08},
	//{0x1A, 0x83},
	{0x20, 0x12},			//Video Source: PAL-I,B,G,H.D,N
	//{0x20, 0x18},			//Video Source: SECAM
	{0x21, 0x09},
	{0x59, 0x8A},			//Video 0:Odd Fields; 1:Even Fields
	//{0x59, 0x0A},			//Video 1:Odd Fields; 0:Even Fields
	{0xD2, 0x26},
	{0xD6, 0x01},
	{0xE0, 0xB0},
	
	{0x38, 0x2A},
	{0x39, 0x09},
	{0x3A, 0x8A},
	{0x3B, 0xCB},
	//{0x27, 0x20},		//blue screen 0x20:auto,  0x10:enable
	{0x5F, 0x00},			//Release RESET
};

	#elif ZT3120_DECODER == ZT3120_DECODER_CJC5150

tI2CRegister code I2CInitTableNTSC[] = {
	{0x00, 0x00},	// video input source selection; 
			// when set 0x00, select AIP1A and CVBS input;
			// when set 0x01, select AIP1B and CVBS input;
			// when set 0x03, select AIP1A(Luma) and AIP1B(Chroma) S-Video input;

	{0x01, 0x15},	// Analog channel controls, AGC Enable
	{0x02, 0x00},	// Operation mode controls, Normal Operation mode	
	{0x03, 0xAF},	// Miscellaneous controls, Output SCLK=27MHz, HSYNC, VSYNC, AVID, FID
			// when set 0x00, SCLK, HSYNC, VSYNC, AVID, FID are high-impedance;

	{0x04, 0xC0},	// Autoswitch mask, Autoswitch to NTSC, PAL, SECAM	
	{0x28, 0x00},	// Video standard, autoswitch mode				
	{0x06, 0x10},	// ColorKiller, Automatic color killer;
			// when set 0x50, Color killer enable, CbCr forced to zero;

	{0x0F, 0x02},	// Configuration shared pins, output FID, VSYNC, VBLK, SCLK
			// when set 0x03, output FID, VSYNC, VBLK, PCLK = 13.5MHz;

	{0x0D, 0x47},	// Output and data rates select;
			// when set 0x47(default), output 8-bit ITU-R BT.656 interface with embedded sync
			// when set 0x40, output 8-bit ITU-R BT.601 interface with discrete sync

	{0x11, 0x00},	// Active video cropping start MSB;
	{0x12, 0x00},	// Active video cropping start LSB;
	{0x13, 0x00},	// Active video cropping stop MSB;
	{0x14, 0x00},	// Active video cropping stop LSB;
			// Cropping register can set image size only when AVID(26 pin) is used for the Horizontal sync signal

	{0x16, 0x80},	// Horizontal Sync (HSYNC) Start Register;
			// Set this register can make the image right or left moving only when HSYNC(25 pin) is used 

	{0x18, 0x00},	// Vertial Blanking start;
	{0x19, 0x00},	// Vertial Blanking stop;
			// Set this register can make the image up or down moving only when VBLK(27 pin) is used for Vertial sync signal

	{0x09, 0x80},	// Luminance control; 	
	{0x0A, 0x80},	// Chroma saturation control; 	
	{0x0B, 0x00},	// Chroma hue control; 	
	{0x0C, 0x80},	// Luminance contrast control;
};

tI2CRegister code I2CInitTablePAL[] = {
	{0x00, 0x00},	// video input source selection; 
			// when set 0x00, select AIP1A and CVBS input;
			// when set 0x01, select AIP1B and CVBS input;
			// when set 0x03, select AIP1A(Luma) and AIP1B(Chroma) S-Video input;

	{0x01, 0x15},	// Analog channel controls, AGC Enable
	{0x02, 0x00},	// Operation mode controls, Normal Operation mode	
	{0x03, 0xAF},	// Miscellaneous controls, Output SCLK=27MHz, HSYNC, VSYNC, AVID, FID
			// when set 0x00, SCLK, HSYNC, VSYNC, AVID, FID are high-impedance;

	{0x04, 0xC0},	// Autoswitch mask, Autoswitch to NTSC, PAL, SECAM	
	{0x28, 0x00},	// Video standard, autoswitch mode				
	{0x06, 0x10},	// ColorKiller, Automatic color killer;
			// when set 0x50, Color killer enable, CbCr forced to zero;

	{0x0F, 0x02},	// Configuration shared pins, output FID, VSYNC, VBLK, SCLK
			// when set 0x03, output FID, VSYNC, VBLK, PCLK = 13.5MHz;

	{0x0D, 0x47},	// Output and data rates select;
			// when set 0x47(default), output 8-bit ITU-R BT.656 interface with embedded sync
			// when set 0x40, output 8-bit ITU-R BT.601 interface with discrete sync

	{0x11, 0x00},	// Active video cropping start MSB;
	{0x12, 0x00},	// Active video cropping start LSB;
	{0x13, 0x00},	// Active video cropping stop MSB;
	{0x14, 0x00},	// Active video cropping stop LSB;
			// Cropping register can set image size only when AVID(26 pin) is used for the Horizontal sync signal

	{0x16, 0x80},	// Horizontal Sync (HSYNC) Start Register;
			// Set this register can make the image right or left moving only when HSYNC(25 pin) is used 

	{0x18, 0x00},	// Vertial Blanking start;
	{0x19, 0x00},	// Vertial Blanking stop;
			// Set this register can make the image up or down moving only when VBLK(27 pin) is used for Vertial sync signal

	{0x09, 0x80},	// Luminance control; 	
	{0x0A, 0x80},	// Chroma saturation control; 	
	{0x0B, 0x00},	// Chroma hue control; 	
	{0x0C, 0x80},	// Luminance contrast control;
};

	#endif // ZT3120_DECODER

unsigned char fnDoInitSensor(
	unsigned char ucResolution)
{
	D2WriteSensorClock(24);     // Set Sensor Clock in MHz
	fnD2IdleDelay(100);         // count 100 around 12ms
	ucResetControl = 0x74;      // 40:SIU Reset, 20:IPU Reset, 10:SOU Reset, 04: DRAM Reset
	switch (ucResolution)
	{
	case D2_RESOLUTION_VGA_60Hz:
		fnI2cWriteTable0(I2CInitTableNTSC, sizeof(I2CInitTableNTSC) / sizeof(tI2CRegister));
		fnI2cWriteTable1(I2CInitTableNTSC, sizeof(I2CInitTableNTSC) / sizeof(tI2CRegister));
		break;
	case D2_RESOLUTION_VGA_50Hz:
		fnI2cWriteTable0(I2CInitTablePAL, sizeof(I2CInitTablePAL) / sizeof(tI2CRegister));
		fnI2cWriteTable1(I2CInitTablePAL, sizeof(I2CInitTablePAL) / sizeof(tI2CRegister));
		break;
	default:
		ucResetControl = 0x00;
		return FAILURE;
	}
	ucResetControl = 0x00;
	return SUCCESS;
}

void fnDoHouseKeeping()
{
	if (nD2Mode != D2_OPERATION_IDLE)
		fnD2TableVerify();
}

void fnDoInitialization()
{
#ifdef SUPPORT_D2_PIO
	// The firmware codes listed here are used to set proper controls to video sources.
	// PIO0 controls video decoder.
	// PIO1 controls video sensor.
	// The codes may not required if the controls are not implemented by hardware PCB.
	fnD2SetPIO0(0); // Set pin RESET low
	fnD2SetPIO1(0); // Set pin RESET low
	fnD2IdleDelay(100); // count 100 around 12ms
	fnD2SetPIO0(1);
	fnD2SetPIO1(1);
#endif // SUPPORT_D2_PIO
	D2DisableSensorClock();

	I2CWorkItem0.I2CConfiguration = SENSOR0_I2C_CONFIG;
	I2CWorkItem0.I2CSlaveAddress  = SENSOR0_I2C_SLAVE;
	I2CWorkItem1.I2CConfiguration = SENSOR1_I2C_CONFIG;
	I2CWorkItem1.I2CSlaveAddress  = SENSOR1_I2C_SLAVE;
}

#endif // SUPPORT_D2_OPERATION
