//
// Zealtek D2 project
// ZT3150 solutions collection
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 23, 2012.

#include "system.h"

#if ZT3150_SOLUTION == ZT3150_SOLUTION_VGA
	#include "zt3150_vga.c"
#elif ZT3150_SOLUTION == ZT3150_SOLUTION_CCTV
	#include "zt3150_cctv.c"
#elif ZT3150_SOLUTION == ZT3150_SOLUTION_HD
	#include "zt3150_hd.c"
#elif ZT3150_SOLUTION == ZT3150_SOLUTION_D2VGA
	#include "zt3150_d2vga.c"
#elif ZT3150_SOLUTION == ZT3150_SOLUTION_D2HD
	#include "zt3150_d2hd.c"
#endif // ZT3150_SOLUTION
