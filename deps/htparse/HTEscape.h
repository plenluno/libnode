/*

  					W3C Sample Code Library libwww URIs


!
  Escape and Unescape Illegal Characters in URIs
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

URLs are written only with the graphic printable characters of the US-ASCII
coded character set. All other characters must be escaped before they can
be used in URLs. This module defines the methods required for escaping and
unescaping the URLs. Internally in the Library, all URLs are escaped so that
we never see illegal characters in the URL handled by the Library. We only
unescape the URLs when we have to, for example while taking to the local
file system etc.

This module is implemented by HTEscape.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTESCAPE_H
#define HTESCAPE_H


/*
.
  Encode Unacceptable Characters using %xy
.

This function takes a string containing any sequence of ASCII characters,
and returns a malloced string containing the same infromation but with all
"unacceptable" characters represented in the form %xy where
x and y are two hex digits.
*/

typedef enum _HTURIEncoding {
    URL_XALPHAS		= 0x1,     /* Escape all unsafe characters */
    URL_XPALPHAS	= 0x2,     /* As URL_XALPHAS but allows '+' */
    URL_PATH		= 0x4,     /* As URL_XPALPHAS but allows '/' */
    URL_DOSFILE         = 0x8      /* As URL_URLPATH but allows ':' */
} HTURIEncoding;

extern char * HTEscape (const char * str, HTURIEncoding mask);

/*

.
  Convert a single Character from Ascii Hex to Char
.

*/

extern char HTAsciiHexToChar (char c);

/*

.
  Decode %xy Escaped Characters
.

This function takes a pointer to a string in which character smay have been
encoded in %xy form, where xy is the acsii hex
code for character 16x+y. The string is converted in place, as it will never
grow.
*/

extern char * HTUnEscape (char * str);

/*
*/

#endif	/* HTESCAPE_H */

/*

  

  @(#) $Id: HTEscape.html,v 2.17 1998/05/24 23:03:13 frystyk Exp $

*/
