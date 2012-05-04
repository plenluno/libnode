/*

  
  					W3C Sample Code Library libwww URI Management


!
  URI Management
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module contains code to parse URIs and various related things such as:
	 
	   o 
	     Parse a URI for tokens
  o 
	     Canonicalization of URIs
  o 
	     Search a URI for illegal characters in order to prevent
    security holes

	 
This module is implemented by HTParse.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTPARSE_H
#define HTPARSE_H

#include "HTEscape.h"

/*
.
  Parsing URIs
.

These functions can be used to get information in a URI.
(
  Parse a URI relative to another URI
)

This returns those parts of a name which are given (and requested) substituting
bits from the related name where necessary. The aName argument
is the (possibly relative) URI to be parsed, the relatedName
is the URI which the aName is to be parsed relative to. Passing
an empty string means that the aName is an absolute URI. The
following are flag bits which may be OR'ed together to form a number to give
the 'wanted' argument to HTParse. As an example we have the URL:
"/TheProject.html#news"
*/

#define PARSE_ACCESS		16	/* Access scheme, e.g. "HTTP" */
#define PARSE_HOST		 8	/* Host name, e.g. "www.w3.org" */
#define PARSE_PATH		 4	/* URL Path, e.g. "pub/WWW/TheProject.html" */

#define PARSE_VIEW               2      /* Fragment identifier, e.g. "news" */
#define PARSE_FRAGMENT           PARSE_VIEW
#define PARSE_ANCHOR		 PARSE_VIEW

#define PARSE_PUNCTUATION	 1	/* Include delimiters, e.g, "/" and ":" */
#define PARSE_ALL		31

/*

where the format of a URI is as follows: "ACCESS :// HOST / PATH #
ANCHOR"

PUNCTUATION means any delimiter like '/', ':', '#' between the
tokens above. The string returned by the function must be freed by the caller.
*/

extern char * HTParse  (const char * aName, const char * relatedName,
			int wanted);

/*
(
  Create a Relative (Partial) URI
)

This function creates and returns a string which gives an expression of one
address as related to another. Where there is no relation, an absolute address
is retured.

  
    On entry,
  
    Both names must be absolute, fully qualified names of nodes (no anchor bits)
  
    On exit,
  
    The return result points to a newly allocated name which, if parsed by HTParse
    relative to relatedName, will yield aName. The caller is responsible for
    freeing the resulting name later.

*/

extern char * HTRelative (const char * aName, const char *relatedName);

/*
.
  Is a URL Relative or Absolute?
.

Search the URL and determine whether it is a relative or absolute URL. We
check to see if there is a ":" before any "/", "?", and "#". If this is the
case then we say it is absolute. Otherwise we say it is relative.
*/

extern BOOL HTURL_isAbsolute (const char * url);

/*
.
  URL Canonicalization
.

Canonicalization of URIs is a difficult job, but it saves a lot of down loads
and double entries in the cache if we do a good job. A URI is allowed to
contain the seqeunce xxx/../ which may be replaced by "" , and the seqeunce
"/./" which may be replaced by "/". Simplification helps us recognize duplicate
URIs. Thus, the following transformations are done:
	 
	   o 
	     /etc/junk/../fred becomes /etc/fred
	   o 
	     /etc/junk/./fred becomes /etc/junk/fred
	 
	 
but we should NOT change
	 
	   o 
	     http://fred.xxx.edu/../.. or
	   o 
	     ../../albert.html
	 
	 
In the same manner, the following prefixed are preserved:
	 
	   o 
	     ./<etc>
	   o 
	     //<etc>
	 
	 
In order to avoid empty URIs the following URIs become:
	 
	   o 
	     /fred/.. becomes /fred/..
	   o 
	     /fred/././.. becomes /fred/..
	   o 
	     /fred/.././junk/.././ becomes /fred/..
	 
	 
If more than one set of `://' is found (several proxies in cascade) then
only the part after the last `://' is simplified.
*/

extern char *HTSimplify (char **filename);

/*
.
  Prevent Security Holes
.

In many telnet like protocols, it can be very dangerous to allow a full ASCII
character set to be in a URI. Therefore we have to strip them out.
HTCleanTelnetString() makes sure that the given string doesn't
contain characters that could cause security holes, such as newlines in ftp,
gopher, news or telnet URLs; more specifically: allows everything between
hexadesimal ASCII 20-7E, and also A0-FE, inclusive.

  
    str
  
    the string that is *modified* if necessary. The string will be truncated
    at the first illegal character that is encountered.
  
    returns
  
    YES, if the string was modified. NO, otherwise.

*/

extern BOOL HTCleanTelnetString (char * str);

/*
*/

#endif	/* HTPARSE_H */

/*

  

  @(#) $Id: HTParse.html,v 2.39 2000/08/04 08:10:53 kahan Exp $

*/
