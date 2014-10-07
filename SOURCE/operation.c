//
// Zealtek D2 project
// System Operation Sample Code
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// April 24, 2012.

#include "system.h"
#include "d2.h"
#include "sensor0.h"
#include "sensor1.h"

#ifdef SUPPORT_D2_OPERATION

unsigned char fnSetIdle()
{
	return SUCCESS;
}

unsigned char fnSetBypassSensor0()
{
	return SUCCESS;
}

unsigned char fnSetBypassSensor1()
{
	return SUCCESS;
}

#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
unsigned char fnSetHorizontalSideBySide()
{
	return SUCCESS;
}

unsigned char fnSetHalfSideBySide()
{
	return SUCCESS;
}
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE

#ifdef SUPPORT_ANAGLYPH_STITCHING
unsigned char fnSetAnaglyph()
{
	return SUCCESS;
}
#endif // SUPPORT_ANAGLYPH_STITCHING

#ifdef SUPPORT_FUZZY_STITCHING
unsigned char fnSetFuzzy()
{
	return SUCCESS;
}

unsigned char fnSetAccurate()
{
	return SUCCESS;
}
#endif // SUPPORT_FUZZY_STITCHING

#ifdef SUPPORT_WDR_STITCHING
unsigned char fnSetWDR()
{
	return SUCCESS;
}

unsigned char fnSetWDR_LDC()
{
	return SUCCESS;
}
#endif // SUPPORT_WDR_STITCHING

#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
unsigned char fnSetVerticalSideBySide()
{
	return SUCCESS;
}
#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE

// for AWB calibration, by XYZ, 2014.01.22 - begin
unsigned char opmode_calibration(void)
{
	return SUCCESS;
}
// for AWB calibration, by XYZ, 2014.01.22 - end

unsigned char fnSetOperatingMode(
	unsigned char ucMode,
	unsigned char ucResolution)
{
	switch (ucMode)
	{
	case D2_OPERATION_IDLE:
		return fnSetIdle();
	case D2_OPERATION_BYPASS_SENSOR0:
		return fnSetBypassSensor0();
	case D2_OPERATION_BYPASS_SENSOR1:
		return fnSetBypassSensor1();
#ifdef SUPPORT_HORIZONTAL_SIDE_BY_SIDE
	case D2_OPERATION_HORIZONTAL_SIDE_BY_SIDE:
		return fnSetHorizontalSideBySide();
	case D2_OPERATION_HALF_SIDE_BY_SIDE:
		return fnSetHalfSideBySide();
#endif // SUPPORT_HORIZONTAL_SIDE_BY_SIDE
#ifdef SUPPORT_VERTICAL_SIDE_BY_SIDE
	case D2_OPERATION_VERTICAL_SIDE_BY_SIDE:
		return fnSetVerticalSideBySide();
#endif // SUPPORT_VERTICAL_SIDE_BY_SIDE
#ifdef SUPPORT_ANAGLYPH_STITCHING
	case D2_OPERATION_ANAGLYPH_STITCHING:
		return fnSetAnaglyph();
#endif // SUPPORT_ANAGLYPH_STITCHING
#ifdef SUPPORT_FUZZY_STITCHING
	case D2_OPERATION_FUZZY_STITCHING:
		return fnSetFuzzy();
	case D2_OPERATION_ACCURATE_STITCHING:
		return fnSetAccurate();
#endif // SUPPORT_FUZZY_STITCHING
#ifdef SUPPORT_WDR_STITCHING
	case D2_OPERATION_LDC_WDR_STITCHING:
		return fnSetWDR_LDC();
	case D2_OPERATION_WDR_STITCHING:
		return fnSetWDR();
#endif // SUPPORT_WDR_STITCHING

	// for AWB calibration, by XYZ, 2014.01.22 - begin
	case D2_OPERATION_3A_CALIBRATION:
		return opmode_calibration();
	// for AWB calibration, by XYZ, 2014.01.22 - end
	}
	return FAILURE;
}

void fnDoHouseKeeping()
{
}

void fnDoInitialization()
{
}

#endif // SUPPORT_D2_OPERATION
