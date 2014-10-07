//
// Zealtek D2 project
// ZT3120 solutions collection
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// Sep 11, 2012.

#include "system.h"

#if ZT3120_SENSOR == ZT3120_SENSOR_OV7725
	#include "zt3120_ov7725.c"
#elif ZT3120_SENSOR == ZT3120_SENSOR_NT99140
	#include "zt3120_nt99140.c"
#elif ZT3120_SENSOR == ZT3120_SENSOR_NT99141
	#include "zt3120_nt99141.c"
#elif ZT3120_SENSOR == ZT3120_SENSOR_NONE
	#include "zt3120_cvbs.c"
#endif // ZT3120_SENSOR

#ifdef SUPPORT_D2_OPERATION
extern void fnMain();

void main()
{
	fnMain();
}
#endif // SUPPORT_D2_OPERATION
