//
// Zealtek D2 project
// ZT3150 Solution Header File
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 23, 2012.

#ifndef __ZT3150_H__
#define __ZT3150_H__

	// System Clock definition
	//#define CLK_SDRAM 108 //MHz
	//#define CLK_SDRAM 120 //MHz
	//#define CLK_SDRAM 160 //MHz
	//#define CLK_SDRAM 162 //MHz

	// Input Clock definition
	//#define CLK_XIN    12 //MHz
	//#define CLK_XIN    24 //MHz
	//#define CLK_XIN    48 //MHz

	// Sensor 0 definitions
	//#define SENSOR0_I2C_CONFIG (I2C_REGISTER_ADDRESS_BYTE+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
	//#define SENSOR0_I2C_SLAVE     0x42     // Sensor 0 slave address
	//#define SENSOR0_REGISTER_ADDRESS 1     // Sensor 0 one-byte register address
	//#define SENSOR0_REGISTER_VALUE   1     // Sensor 0 one-byte register value
	//#define SENSOR0_REGISTER_ADDRESS 2     // Sensor 0 two-byte register address
	//#define SENSOR0_REGISTER_VALUE   2     // Sensor 0 two-byte register value

	// Sensor 1 definitions
	//#define SENSOR1_I2C_CONFIG (I2C_REGISTER_ADDRESS_BYTE+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
	//#define SENSOR1_I2C_SLAVE     0x42     // Sensor 1 slave address
	//#define SENSOR1_REGISTER_ADDRESS 1     // Sensor 1 one-byte register address
	//#define SENSOR1_REGISTER_VALUE   1     // Sensor 1 one-byte register value
	//#define SENSOR1_REGISTER_ADDRESS 2     // Sensor 1 two-byte register address
	//#define SENSOR1_REGISTER_VALUE   2     // Sensor 1 two-byte register value

	// SDRAM_8MB_4x16 is used for solutions whose SDRAM size is 8MB 4x16.
	// Otherwise, SDRAM size is 16MB 8x16.
	//#define SDRAM_8MB_4x16

	// SUPPORT_SENSOR_SCALING is used for sensors that can down scale from HD resolution.
	// On HD accurating switching operation mode, the output resolution is 1920x544.
	// If the sensor can down scale to 960x544, the SDRAM bandwidth is smaller.
	// The generated video output frame rate can go up to 24fps.
	// If the sensor does not support the down scaling, input 1280x720 images are put to
	// SDRAM, and the SDRAM bandwidth is higher, the calibration table is also bigger.
	// The generated video output frame rate becomes less than 24fps.
	//#define SUPPORT_SENSOR_SCALING

	// FPGA_VERIFY is used to verify firmware codes on FPGA environment.
	// This is enabled on finding out the reason of hardware failures.
	//#define FPGA_VERIFY

////////////////////////////////////////////////////////////////////////////////
//  Start of ZT3150 solutions
////////////////////////////////////////////////////////////////////////////////
//#define ZT3150_OV7725
//#define ZT3150_OV7725_CCTV
//#define ZT3150_OV7725_SCALED
//#define ZT3150_NT99050
//#define ZT3150_NT99140
//#define ZT3150_NT99141
//#define ZT3150_ZT3150
#define ZT3150_ZT3150HD
////////////////////////////////////////////////////////////////////////////////
//  End of ZT3150 solutions
////////////////////////////////////////////////////////////////////////////////

///// Supporting List of Video Sensors
#define ZT3150_SENSOR_NONE		0
#define ZT3150_SENSOR_OV7725		1
#define ZT3150_SENSOR_NT99050		2
#define ZT3150_SENSOR_NT99140		3
#define ZT3150_SENSOR_NT99141		4
#define ZT3150_SENSOR_ZT3150		5
#define ZT3150_SENSOR_ZT3150HD		6

///// Supporting List of Video solutions
#define ZT3150_SOLUTION_NONE		0
#define ZT3150_SOLUTION_PATTERN		1
#define ZT3150_SOLUTION_VGA		2
#define ZT3150_SOLUTION_HD		3
#define ZT3150_SOLUTION_CCTV		4
#define ZT3150_SOLUTION_D2VGA		5
#define ZT3150_SOLUTION_D2HD		6

///// Supporting List of Calibration Types
#define ZT3150_CALIBRATION_NONE		0
#define ZT3150_CALIBRATION_VGA		1
#define ZT3150_CALIBRATION_HD		2
#define ZT3150_CALIBRATION_DUAL		3
#define ZT3150_CALIBRATION_SCALED	4
#define ZT3150_CALIBRATION_D2VGA	5
#define ZT3150_CALIBRATION_D2HD		6

///// Supporting List of Image Stitching Size
#define STITCHING_SIZE_1280x480		0
#define STITCHING_SIZE_1280x496		1
#define STITCHING_SIZE_640x240		2
#define STITCHING_SIZE_640x256		3
#define STITCHING_SIZE_640x480		4
#define STITCHING_SIZE_640x496		5
#define STITCHING_SIZE_1920x544		6
#define STITCHING_SIZE_1920x560		7
#define STITCHING_SIZE_1920x496		8
#define STITCHING_SIZE_2560x496		9
#define STITCHING_SIZE_1920x1080	10

//#message "ZT3150 - OV7725"
#ifdef ZT3150_OV7725
    #define SUPPORT_D2_PAN			// Support Panorama Adjustment on stitching mode
    #define CLK_SDRAM			120	//MHz
    #define CLK_XIN			12	//MHz
    #define ZT3150_SENSOR		ZT3150_SENSOR_OV7725
    #define ZT3150_SOLUTION		ZT3150_SOLUTION_VGA
    #define ZT3150_CALITYPE		ZT3150_CALIBRATION_VGA
//    #define IMAGE_STITCHING_SIZE	STITCHING_SIZE_1280x480
    #define IMAGE_STITCHING_SIZE	STITCHING_SIZE_1280x496
    #define YUVSENSOR
    #define SDRAM_8MB_4x16
#endif // ZT3150_OV7725

//#message "ZT3150 - OV7725 (scaled)"
#ifdef ZT3150_OV7725_SCALED
    #define CLK_SDRAM			120	//MHz
    #define CLK_XIN			12	//MHz
    #define ZT3150_SENSOR		ZT3150_SENSOR_OV7725
    #define ZT3150_SOLUTION		ZT3150_SOLUTION_VGA
    #define ZT3150_CALITYPE		ZT3150_CALIBRATION_SCALED
//    #define IMAGE_STITCHING_SIZE	STITCHING_SIZE_640x240
    #define IMAGE_STITCHING_SIZE	STITCHING_SIZE_640x256
//    #define IMAGE_STITCHING_SIZE	STITCHING_SIZE_640x480
//    #define IMAGE_STITCHING_SIZE	STITCHING_SIZE_640x496
    #define YUVSENSOR
    #define SDRAM_8MB_4x16
#endif // ZT3150_OV7725_SCALED

//#message "ZT3150 - OV7725 (CCTV)"
#ifdef ZT3150_OV7725_CCTV
//    #define CLK_SDRAM			108	//MHz
//    #define CLK_SDRAM			162	//MHz
//    #define CLK_XIN			27	//MHz
    #define CLK_SDRAM			120	//MHz
    #define CLK_XIN			48	//MHz
    #define ZT3150_SENSOR		ZT3150_SENSOR_OV7725
    #define ZT3150_SOLUTION		ZT3150_SOLUTION_CCTV
    #define ZT3150_CALITYPE		ZT3150_CALIBRATION_DUAL
    #define YUVSENSOR
    #define CONTROLLER_SONIX_CCTV
    #define D2_RESOLUTION_QVGA_OFFSET	8 //Must be larger than 4
    #define SDRAM_8MB_4x16
#endif // ZT3150_OV7725_CCTV

//#message "ZT3150 - NT99050"
#ifdef ZT3150_NT99050
    #define SUPPORT_D2_PAN			// Support Panorama Adjustment on stitching mode
    #define CLK_SDRAM			160	//MHz
    #define CLK_XIN			12	//MHz
    #define ZT3150_SENSOR		ZT3150_SENSOR_NT99050
    #define ZT3150_SOLUTION		ZT3150_SOLUTION_VGA
    #define ZT3150_CALITYPE		ZT3150_CALIBRATION_VGA
//    #define IMAGE_STITCHING_SIZE	STITCHING_SIZE_1280x480
    #define IMAGE_STITCHING_SIZE	STITCHING_SIZE_1280x496
    #define YUVSENSOR
    #define SDRAM_8MB_4x16
#endif // ZT3150_NT99050

//#message "ZT3150 - NT99140"
#ifdef ZT3150_NT99140
    #define CLK_SDRAM			160	//MHz
    #define CLK_XIN			12	//MHz
    #define ZT3150_SENSOR		ZT3150_SENSOR_NT99140
    #define ZT3150_SOLUTION		ZT3150_SOLUTION_HD
    #define ZT3150_CALITYPE		ZT3150_CALIBRATION_HD
    #define IMAGE_STITCHING_SIZE	STITCHING_SIZE_1920x544
    #define YUVSENSOR
#endif // ZT3150_NT99140

//#message "ZT3150 - NT99141"
#ifdef ZT3150_NT99141
    #define SUPPORT_SENSOR_SCALING		// Support Scaled HD from sensors on stitching mode
    #define CLK_SDRAM			160	//MHz
    #define CLK_XIN			12	//MHz
    #define ZT3150_SENSOR		ZT3150_SENSOR_NT99141
    #define ZT3150_SOLUTION		ZT3150_SOLUTION_HD
    #define ZT3150_CALITYPE		ZT3150_CALIBRATION_HD
//    #define IMAGE_STITCHING_SIZE	STITCHING_SIZE_1920x544
    #define IMAGE_STITCHING_SIZE	STITCHING_SIZE_1920x560
    #define YUVSENSOR
#endif // ZT3150_NT99141

//#message "ZT3150 - ZT3150"
#ifdef ZT3150_ZT3150
//    #define SUPPORT_OUTPUT_SCALING
    #define CLK_SDRAM			160	//MHz
    #define CLK_XIN			12	//MHz
    #define ZT3150_SENSOR		ZT3150_SENSOR_ZT3150
    #define ZT3150_SOLUTION		ZT3150_SOLUTION_D2VGA
    #define ZT3150_CALITYPE		ZT3150_CALIBRATION_D2VGA
    #ifdef SUPPORT_OUTPUT_SCALING
        #define IMAGE_STITCHING_SIZE	STITCHING_SIZE_1920x496
    #else
        #define IMAGE_STITCHING_SIZE	STITCHING_SIZE_2560x496
    #endif
    #define YUVSENSOR
    #define SUPPORT_D2_LAYER2
#endif

//#message "ZT3150 - ZT3150HD"
#ifdef ZT3150_ZT3150HD
//    #define SUPPORT_SENSOR_SCALING		// Support Scaled HD from sensors on stitching mode
    #define CLK_SDRAM			160	//MHz
    #define CLK_XIN			24	//MHz
    #define ZT3150_SENSOR		ZT3150_SENSOR_ZT3150HD
    #define ZT3150_SOLUTION		ZT3150_SOLUTION_D2HD
    #define ZT3150_CALITYPE		ZT3150_CALIBRATION_D2VGA
    #define IMAGE_STITCHING_SIZE	STITCHING_SIZE_1920x1080
    #define YUVSENSOR
    #define SUPPORT_D2_LAYER2
#endif // ZT3150_ZT3150HD

#ifdef SUPPORT_SENSOR_SCALING
    #define D2_RESOLUTION_SCALED_HD_OFFSET	8	//Must be larger than 4
#endif // SUPPORT_SENSOR_SCALING


// SENSOR0 of ZT3150 solutions should be one of the supported video sensors.
// SENSOR1 of ZT3150 solutions should be one of the supported video sensors.
//
#if ZT3150_SENSOR == ZT3150_SENSOR_NONE
// ZT3150_PATTERN does not require sensor interfaces.
// No I2C slave devices are required.
// These compiler options are defined in order not to see compiler errors.
# define SENSOR0_REGISTER_ADDRESS		1	// Sensor 0 one-byte register address
# define SENSOR0_REGISTER_VALUE			1	// Sensor 0 one-byte register value
# define SENSOR0_NACK_ON_I2C_READ
# define SENSOR1_REGISTER_ADDRESS		1	// Sensor 1 one-byte register address
# define SENSOR1_REGISTER_VALUE			1	// Sensor 1 one-byte register value
# define SENSOR1_NACK_ON_I2C_READ
#elif ZT3150_SENSOR == ZT3150_SENSOR_OV7725
# define SENSOR0_I2C_CONFIG			(I2C_REGISTER_ADDRESS_BYTE+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
# define SENSOR0_I2C_SLAVE			0x42	// Sensor 0 slave address
# define SENSOR0_REGISTER_ADDRESS		1	// Sensor 0 one-byte register address
# define SENSOR0_REGISTER_VALUE			1	// Sensor 0 one-byte register value
# define SENSOR0_NACK_ON_I2C_READ
# define SENSOR1_I2C_CONFIG			(I2C_REGISTER_ADDRESS_BYTE+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
# define SENSOR1_I2C_SLAVE			0x42	// Sensor 1 slave address
# define SENSOR1_REGISTER_ADDRESS		1	// Sensor 1 one-byte register address
# define SENSOR1_REGISTER_VALUE			1	// Sensor 1 one-byte register value
# define SENSOR1_NACK_ON_I2C_READ
#elif ZT3150_SENSOR == ZT3150_SENSOR_NT99050
# define SENSOR0_I2C_CONFIG			(I2C_REGISTER_ADDRESS_WORD+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
# define SENSOR0_I2C_SLAVE			0x42	// Sensor 0 slave address
# define SENSOR0_REGISTER_ADDRESS		2	// Sensor 0 two-byte register address
# define SENSOR0_REGISTER_VALUE			1	// Sensor 0 one-byte register value
# define SENSOR0_NACK_ON_I2C_READ
# define SENSOR1_I2C_CONFIG			(I2C_REGISTER_ADDRESS_WORD+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
# define SENSOR1_I2C_SLAVE			0x42	// Sensor 1 slave address
# define SENSOR1_REGISTER_ADDRESS		2	// Sensor 1 two-byte register address
# define SENSOR1_REGISTER_VALUE			1	// Sensor 1 one-byte register value
# define SENSOR1_NACK_ON_I2C_READ
#elif ZT3150_SENSOR == ZT3150_SENSOR_NT99140
# define SENSOR0_I2C_CONFIG			(I2C_REGISTER_ADDRESS_WORD+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
# define SENSOR0_I2C_SLAVE			0x54	// Sensor 0 slave address
# define SENSOR0_REGISTER_ADDRESS		2	// Sensor 0 two-byte register address
# define SENSOR0_REGISTER_VALUE			1	// Sensor 0 one-byte register value
# define SENSOR0_NACK_ON_I2C_READ
# define SENSOR1_I2C_CONFIG			(I2C_REGISTER_ADDRESS_WORD+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
# define SENSOR1_I2C_SLAVE			0x54	// Sensor 1 slave address
# define SENSOR1_REGISTER_ADDRESS		2	// Sensor 1 two-byte register address
# define SENSOR1_REGISTER_VALUE			1	// Sensor 1 one-byte register value
# define SENSOR1_NACK_ON_I2C_READ
#elif ZT3150_SENSOR == ZT3150_SENSOR_NT99141
# define SENSOR0_I2C_CONFIG			(I2C_REGISTER_ADDRESS_WORD+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
# define SENSOR0_I2C_SLAVE			0x54	// Sensor 0 slave address
# define SENSOR0_REGISTER_ADDRESS		2	// Sensor 0 two-byte register address
# define SENSOR0_REGISTER_VALUE			1	// Sensor 0 one-byte register value
# define SENSOR0_NACK_ON_I2C_READ
# define SENSOR1_I2C_CONFIG			(I2C_REGISTER_ADDRESS_WORD+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
# define SENSOR1_I2C_SLAVE			0x54	// Sensor 1 slave address
# define SENSOR1_REGISTER_ADDRESS		2	// Sensor 1 two-byte register address
# define SENSOR1_REGISTER_VALUE			1	// Sensor 1 one-byte register value
# define SENSOR1_NACK_ON_I2C_READ
#elif ZT3150_SENSOR == ZT3150_SENSOR_ZT3150
# define SENSOR0_I2C_CONFIG			(I2C_REGISTER_ADDRESS_WORD+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
# define SENSOR0_I2C_SLAVE			0x04	// Sensor slave address
# define SENSOR0_REGISTER_ADDRESS		2	// Sensor two-byte register address
# define SENSOR0_REGISTER_VALUE			1	// Sensor one-byte register value
# define SENSOR0_NACK_ON_I2C_READ
# define SENSOR1_I2C_CONFIG			(I2C_REGISTER_ADDRESS_WORD+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
# define SENSOR1_I2C_SLAVE			0x04	// Sensor slave address
# define SENSOR1_REGISTER_ADDRESS		2	// Sensor two-byte register address
# define SENSOR1_REGISTER_VALUE			1	// Sensor one-byte register value
# define SENSOR1_NACK_ON_I2C_READ
#elif ZT3150_SENSOR == ZT3150_SENSOR_ZT3150HD
# define SENSOR0_I2C_CONFIG			(I2C_REGISTER_ADDRESS_WORD+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
# define SENSOR0_I2C_SLAVE			0x04	// Sensor slave address
# define SENSOR0_REGISTER_ADDRESS		2	// Sensor two-byte register address
# define SENSOR0_REGISTER_VALUE			1	// Sensor one-byte register value
# define SENSOR0_NACK_ON_I2C_READ
# define SENSOR1_I2C_CONFIG			(I2C_REGISTER_ADDRESS_WORD+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
# define SENSOR1_I2C_SLAVE			0x04	// Sensor slave address
# define SENSOR1_REGISTER_ADDRESS		2	// Sensor two-byte register address
# define SENSOR1_REGISTER_VALUE			1	// Sensor one-byte register value
# define SENSOR1_NACK_ON_I2C_READ
#endif // ZT3150_SENSOR

#endif // __ZT3150_H__
