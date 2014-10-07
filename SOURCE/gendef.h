//
// Zealtek D2 project
// General Definitions
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 11, 2012.

#ifndef _GENDEF_
#define _GENDEF_

// Macros to access big endian word and double word.
#define GETWORDLSB(w)		*(((unsigned char *)&(w)) + 1)
#define GETWORDMSB(w)		*(((unsigned char *)&(w)))
#define GETDWORDLSB(w)		*(((unsigned char *)&(w)) + 3)
#define GETDWORD2NDLSB(w)	*(((unsigned char *)&(w)) + 2)
#define GETDWORD2NDMSB(w)	*(((unsigned char *)&(w)) + 1)
#define GETDWORDMSB(w)		*(((unsigned char *)&(w)))
#define GETDWORDLSW(w)		*(((unsigned short *)&(w)) + 1)
#define GETDWORDMSW(w)		*((unsigned short *)&(w))
#define SETWORDLSB(w,b)		{*(((unsigned char *)&(w)) + 1) = (b);}
#define SETWORDMSB(w,b)		{*(((unsigned char *)&(w))) = (b);}
#define SETDWORDLSB(w,b)	{*(((unsigned char *)&(w)) + 3) = (b);}
#define SETDWORD2NDLSB(w,b)	{*(((unsigned char *)&(w)) + 2) = (b);}
#define SETDWORD2NDMSB(w,b)	{*(((unsigned char *)&(w)) + 1) = (b);}
#define SETDWORDMSB(w,b)	{*(((unsigned char *)&(w))) = (b);}
#define SETDWORDLSW(w,b)	{*(((unsigned short *)&(w)) + 1) = (b);}
#define SETDWORDMSW(w,b)	{*((unsigned short *)&(w)) = (b);}

#endif // _GENDEF_
