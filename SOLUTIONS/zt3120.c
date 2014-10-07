//
// Zealtek D2 project
// ZT3120 solutions collection
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 23, 2012.

#include "system.h"

#if ZT3120_SOLUTION == ZT3120_SOLUTION_DECODER
	#include "zt3120_decoders.c"
#elif ZT3120_SOLUTION == ZT3120_SOLUTION_VGA
	#include "zt3120_vga.c"
#elif ZT3120_SOLUTION == ZT3120_SOLUTION_HD
	#include "zt3120_hd.c"
#elif ZT3120_SOLUTION == ZT3120_SOLUTION_sHD
	#include "zt3120_shd.c"
#endif // ZT3120_SOLUTION
