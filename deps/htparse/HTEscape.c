/*								     HTEscape.c
**	ESCAPE AND UNESACPE ILLEGAL CHARACTERS IN A URI
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id: HTEscape.c,v 2.21 1999/02/22 22:10:11 frystyk Exp $
**
** history:
**	Nov 13 94	Spawned from HTParse, as it then can be used in utility
**			programs without loading the whole library
*/

/* Library include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTEscape.h"					 /* Implemented here */

#define HEX_ESCAPE '%'
#define ACCEPTABLE(a)	( a>=32 && a<128 && ((isAcceptable[a-32]) & mask))

/*
**  Not BOTH static AND const at the same time in gcc :-(, Henrik 18/03-94 
**  code gen error in gcc when making random access to static const table(!!)
*/

/*
**	Bit 0		xalpha		-- see HTFile.h
**	Bit 1		xpalpha		-- as xalpha but with plus.
**	Bit 2 ...	path		-- as xpalpha but with /
*/
PRIVATE unsigned char isAcceptable[96] =
{/* 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xA 0xB 0xC 0xD 0xE 0xF */
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xF,0xE,0x0,0xF,0xF,0xC, /* 2x  !"#$%&'()*+,-./   */
    0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0x8,0x0,0x0,0x0,0x0,0x0, /* 3x 0123456789:;<=>?   */
    0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF, /* 4x @ABCDEFGHIJKLMNO   */
    0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0x0,0x0,0x0,0x0,0xF, /* 5X PQRSTUVWXYZ[\]^_   */
    0x0,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF, /* 6x `abcdefghijklmno   */
    0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0x0,0x0,0x0,0x0,0x0  /* 7X pqrstuvwxyz{\}~DEL */
};
PRIVATE char *hex = "0123456789ABCDEF";

/* ------------------------------------------------------------------------- */

/*		Escape undesirable characters using %		HTEscape()
**		-------------------------------------
**
**	This function takes a pointer to a string in which
**	some characters may be unacceptable unescaped.
**	It returns a string which has these characters
**	represented by a '%' character followed by two hex digits.
**
**	In the tradition of being conservative in what you do and liberal
**	in what you accept, we encode some characters which in fact are
**	allowed in URLs unencoded -- so DON'T use the table below for
**	parsing! 
**
**	Unlike HTUnEscape(), this routine returns a HT_MALLOCed string.
**
*/
PUBLIC char * HTEscape (const char * str, HTURIEncoding mask)
{
    const char * p;
    char * q;
    char * result;
    int unacceptable = 0;
    if (!str) return NULL;
    for(p=str; *p; p++)
        if (!ACCEPTABLE((unsigned char)TOASCII(*p)))
		unacceptable++;
    if ((result = (char  *) HT_MALLOC(p-str + unacceptable+ unacceptable + 1)) == NULL)
        HT_OUTOFMEM("HTEscape");
    for(q=result, p=str; *p; p++) {
    	unsigned char a = TOASCII(*p);
	if (!ACCEPTABLE(a)) {
	    *q++ = HEX_ESCAPE;	/* Means hex commming */
	    *q++ = hex[a >> 4];
	    *q++ = hex[a & 15];
	}
	else *q++ = *p;
    }
    *q++ = 0;			/* Terminate */
    return result;
}


PUBLIC char HTAsciiHexToChar (char c)
{
    return  c >= '0' && c <= '9' ?  c - '0' 
    	    : c >= 'A' && c <= 'F'? c - 'A' + 10
    	    : c - 'a' + 10;	/* accept small letters just in case */
}

/*		Decode %xx escaped characters			HTUnEscape()
**		-----------------------------
**
**	This function takes a pointer to a string in which some
**	characters may have been encoded in %xy form, where xy is
**	the acsii hex code for character 16x+y.
**	The string is converted in place, as it will never grow.
*/
PUBLIC char * HTUnEscape (char * str)
{
    char * p = str;
    char * q = str;

    if (!str) {					      /* Just for safety ;-) */
	HTTRACE(URI_TRACE, "HTUnEscape.. Called with NULL argument.\n");
	return NULL;
    }
    while(*p) {
        if (*p == HEX_ESCAPE) {
	    p++;
	    if (*p) *q = HTAsciiHexToChar(*p++) * 16;
#if 1
	    /* Suggestion from Markku Savela */
	    if (*p) *q = FROMASCII(*q + HTAsciiHexToChar(*p)), ++p;
	    q++;
#else 
	    if (*p) *q = FROMASCII(*q + HTAsciiHexToChar(*p));
	    p++, q++;
#endif
	} else {
	    *q++ = *p++; 
	}
    }
    
    *q++ = 0;
    return str;
    
} /* HTUnEscape */

