//
// Zealtek D2 project
// ZT3120 Solution Header File
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 23, 2012.

#ifndef __ZT3120_H__
#define __ZT3120_H__

	// System Clock definition
	//#define CLK_SDRAM 120 //MHz
	//#define CLK_SDRAM 160 //MHz

	// Sensor 0 definitions
	//#define SENSOR0_I2C_CONFIG (I2C_REGISTER_ADDRESS_BYTE+I2C_REGISTER_VALUE_BYTE+I2C_RESTART_READ)
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
	#define SDRAM_8MB_4x16

////////////////////////////////////////////////////////////////////////////////
//  Start of ZT3120 solutions
////////////////////////////////////////////////////////////////////////////////
//#define ZT3120_T515B_T515B
#define ZT3120_ADV7180_ADV7180
//#define ZT3120_T515B_OV7725
//#define ZT3120_ADV7180_OV7725
//#define ZT3120_T515B_NT99140
//#define ZT3120_T515B_NT99141
//#define ZT3120_ADV7180_NT99141
////////////////////////////////////////////////////////////////////////////////
//  End of ZT3120 solutions
////////////////////////////////////////////////////////////////////////////////

///// Supporting List of Video Decoders
#define ZT3120_DECODER_NONE       0
#define ZT3120_DECODER_T515B      1
#define ZT3120_DECODER_ADV7180    2

///// Supporting List of Video Sensors
#define ZT3120_SENSOR_NONE        0
#define ZT3120_SENSOR_OV7725      1
#define ZT3120_SENSOR_NT99140     2
#define ZT3120_SENSOR_NT99141     3

///// Supporting List of Video solutions
#define ZT3120_SOLUTION_NONE      0
#define ZT3120_SOLUTION_DECODER   1
#define ZT3120_SOLUTION_VGA       2
#define ZT3120_SOLUTION_HD        3
#define ZT3120_SOLUTION_sHD       4


#ifdef ZT3120_T515B_T515B
	#define CLK_SDRAM 120 //MHz
	#define YUVSENSOR
	#define SENSOR1_EQ_SENSOR0
	#define ZT3120_DECODER  ZT3120_DECODER_T515B
	#define ZT3120_SENSOR   ZT3120_SENSOR_NONE
	#define ZT3120_SOLUTION ZT3120_SOLUTION_DECODER
#endif // ZT3120_T515B_T515B

#ifdef ZT3120_ADV7180_ADV7180
	#define CLK_SDRAM 120 //MHz
	#define YUVSENSOR
	#define SENSOR1_EQ_SENSOR0
	#define ZT3120_DECODER  ZT3120_DECODER_ADV7180
	#define ZT3120_SENSOR   ZT3120_SENSOR_NONE
	#define ZT3120_SOLUTION ZT3120_SOLUTION_DECODER
#endif // ZT3120_ADV7180_ADV7180

#ifdef ZT3120_T515B_OV7725
	#define CLK_SDRAM 120 //MHz
	#define YUVSENSOR
	#define ZT3120_DECODER ZT3120_DECODER_T515B
	#define ZT3120_SENSOR  ZT3120_SENSOR_OV7725
	#define ZT3120_SOLUTION ZT3120_SOLUTION_VGA
#endif // ZT3120_T515B_OV7725

#ifdef ZT3120_ADV7180_OV7725
	#define CLK_SDRAM 120 //MHz
	#define YUVSENSOR
	#define ZT3120_DECODER  ZT3120_DECODER_ADV7180
	#define ZT3120_SENSOR   ZT3120_SENSOR_OV7725
	#define ZT3120_SOLUTION ZT3120_SOLUTION_VGA
#endif // ZT3120_ADV7180_OV7725

#ifdef ZT3120_T515B_NT99140
	#define CLK_SDRAM 120 //MHz
	#define YUVSENSOR
	#define ZT3120_DECODER  ZT3120_DECODER_T515B
	#define ZT3120_SENSOR   ZT3120_SENSOR_NT99140
	#define ZT3120_SOLUTION ZT3120_SOLUTION_HD
#endif // ZT3120_T515B_NT99140

#ifdef ZT3120_T515B_NT99141
	#define CLK_SDRAM 160 //MHz
	#define YUVSENSOR
	#define ZT3120_DECODER  ZT3120_DECODER_T515B
	#define ZT3120_SENSOR   ZT3120_SENSOR_NT99141
	#define ZT3120_SOLUTION ZT3120_SOLUTION_HD
	//#define ZT3120_SOLUTION ZT3120_SOLUTION_sHD
#endif // ZT3120_T515B_NT99141

#ifdef ZT3120_ADV7180_NT99141
	#define CLK_SDRAM 160 //MHz
	#define YUVSENSOR
	#define ZT3120_DECODER  ZT3120_DECODER_ADV7180
	#define ZT3120_SENSOR   ZT3120_SENSOR_NT99141
	#define ZT3120_SOLUTION ZT3120_SOLUTION_HD
	//#define ZT3120_SOLUTION ZT3120_SOLUTION_sHD
#endif // ZT3120_ADV7180_OV7725

#if ZT3120_SOLUTION == ZT3120_SOLUTION_sHD
	#define SUPPORT_SENSOR_SCALING
	#define D2_RESOLUTION_SCALED_HD_OFFSET 8 //Must be larger than 4
#endif // ZT3120_SOLUTION == ZT3120_SOLUTION_sHD


// SENSOR0 of ZT3120 solutions must be one of the supported video decoders.
//
#if ZT3120_DECODER == ZT3120_DECODER_T515B
	#define SENSOR0_I2C_CONFIG (I2C_REGISTER_ADDRESS_BYTE+I2C_REGISTER_VALUE_BYTE+I2C_RESTART_READ)
	#define SENSOR0_I2C_SLAVE     0xB0     // Sensor 0 slave address
	#define SENSOR0_REGISTER_ADDRESS 1     // Sensor 0 one-byte register address
	#define SENSOR0_REGISTER_VALUE   1     // Sensor 0 one-byte register value
	//#define SENSOR0_NACK_ON_I2C_READ
#elif ZT3120_DECODER == ZT3120_DECODER_ADV7180
	#define SENSOR0_I2C_CONFIG (I2C_REGISTER_ADDRESS_BYTE+I2C_REGISTER_VALUE_BYTE+I2C_RESTART_READ)
	#define SENSOR0_I2C_SLAVE     0x40     // Sensor 0 slave address
	#define SENSOR0_REGISTER_ADDRESS 1     // Sensor 0 one-byte register address
	#define SENSOR0_REGISTER_VALUE   1     // Sensor 0 one-byte register value
	//#define SENSOR0_NACK_ON_I2C_READ
#endif // ZT3120_DECODER


// SENSOR1 of ZT3120 solutions can be one of the supported video decoders.
// SENSOR1 of ZT3120 solutions can be one of the supported video sensors.
//
#if ZT3120_SENSOR == ZT3120_SENSOR_NONE
	#if ZT3120_DECODER == ZT3120_DECODER_T515B
		#define SENSOR1_I2C_CONFIG (I2C_REGISTER_ADDRESS_BYTE+I2C_REGISTER_VALUE_BYTE+I2C_RESTART_READ)
		#define SENSOR1_I2C_SLAVE     0xB0     // Sensor 1 slave address
		#define SENSOR1_REGISTER_ADDRESS 1     // Sensor 1 one-byte register address
		#define SENSOR1_REGISTER_VALUE   1     // Sensor 1 one-byte register value
		//#define SENSOR1_NACK_ON_I2C_READ
	#elif ZT3120_DECODER == ZT3120_DECODER_ADV7180
		#define SENSOR1_I2C_CONFIG (I2C_REGISTER_ADDRESS_BYTE+I2C_REGISTER_VALUE_BYTE+I2C_RESTART_READ)
		#define SENSOR1_I2C_SLAVE     0x40     // Sensor 1 slave address
		#define SENSOR1_REGISTER_ADDRESS 1     // Sensor 1 one-byte register address
		#define SENSOR1_REGISTER_VALUE   1     // Sensor 1 one-byte register value
		//#define SENSOR1_NACK_ON_I2C_READ
	#endif // ZT3120_DECODER
#elif ZT3120_SENSOR == ZT3120_SENSOR_OV7725
	#define SENSOR1_I2C_CONFIG (I2C_REGISTER_ADDRESS_BYTE+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
	#define SENSOR1_I2C_SLAVE     0x42     // Sensor 1 slave address
	#define SENSOR1_REGISTER_ADDRESS 1     // Sensor 1 one-byte register address
	#define SENSOR1_REGISTER_VALUE   1     // Sensor 1 one-byte register value
	#define SENSOR1_NACK_ON_I2C_READ
#elif ZT3120_SENSOR == ZT3120_SENSOR_NT99140
	#define SENSOR1_I2C_CONFIG (I2C_REGISTER_ADDRESS_WORD+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
	#define SENSOR1_I2C_SLAVE     0x54     // Sensor 1 slave address
	#define SENSOR1_REGISTER_ADDRESS 2     // Sensor 1 two-byte register address
	#define SENSOR1_REGISTER_VALUE   1     // Sensor 1 one-byte register value
	#define SENSOR1_NACK_ON_I2C_READ
#elif ZT3120_SENSOR == ZT3120_SENSOR_NT99141
	#define SENSOR1_I2C_CONFIG (I2C_REGISTER_ADDRESS_WORD+I2C_REGISTER_VALUE_BYTE+I2C_STOP_START_READ)
	#define SENSOR1_I2C_SLAVE     0x54     // Sensor 1 slave address
	#define SENSOR1_REGISTER_ADDRESS 2     // Sensor 1 two-byte register address
	#define SENSOR1_REGISTER_VALUE   1     // Sensor 1 one-byte register value
	#define SENSOR1_NACK_ON_I2C_READ
#endif // ZT3120_SENSOR

#endif // __ZT3120_H__
