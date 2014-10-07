//
// Zealtek D2 project
// ZT3150 solutions collection
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// Sep 10, 2012.

#include "system.h"

#ifndef SENSOR1_EQ_SENSOR0
	#error ZT3150 expects to see Sensor 0 and Sensor 1 be the same model.
#endif // SENSOR1_EQ_SENSOR0

#if ZT3150_SENSOR == ZT3150_SENSOR_OV7725
	#include "zt3150_ov7725.c"
#elif ZT3150_SENSOR == ZT3150_SENSOR_NT99050
	#include "zt3150_nt99050.c"
#elif ZT3150_SENSOR == ZT3150_SENSOR_NT99140
	#include "zt3150_nt99140.c"
#elif ZT3150_SENSOR == ZT3150_SENSOR_NT99141
	#include "zt3150_nt99141.c"
#elif ZT3150_SENSOR == ZT3150_SENSOR_ZT3150
	// No customer files are prepared for this solution.
#endif // ZT3150_SENSOR

#ifdef SUPPORT_D2_OPERATION
extern void fnMain();

void main()
{
	fnMain();
}
#endif // SUPPORT_D2_OPERATION
