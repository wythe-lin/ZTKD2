//
// Zealtek D2 project
// Header file for ZT3150 Calibration Tables
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// June. 08, 2012.

#ifdef SUPPORT_FUZZY_STITCHING
	#if (ZT3150_CALITYPE == ZT3150_CALIBRATION_DUAL)
extern code unsigned char D2CalibrationTable1[]; // Table for 640x240
extern code unsigned char D2CalibrationTable2[]; // Table for 640x240
extern code unsigned char D2CalibrationTable3[]; // Table for 640x480
extern code unsigned char D2CalibrationTable4[]; // Table for 640x480
	#else  // (ZT3150_CALITYPE == ZT3150_CALIBRATION_DUAL)
extern code unsigned char D2CalibrationTable1[];
extern code unsigned char D2CalibrationTable2[];
	#endif // (ZT3150_CALITYPE == ZT3150_CALIBRATION_DUAL)
#endif //SUPPORT_FUZZY_STITCHING
