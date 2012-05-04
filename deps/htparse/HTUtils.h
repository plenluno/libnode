/*

  					W3C Sample Code Library libwww Debug Information and General Purpose
  Macros


!
  Debug Information and General Purpose Macros
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module is a part of the  W3C Sample
Code Library. See also the system dependent file
sysdep module for system specific information.
*/

#ifndef HTUTILS_H
#define HTUTILS_H

/*
.
  Destination for User Print Messages
.

You can send print messages to the user to various destinations
depending on the type of your application. By default, on Unix the
messages are sent to stdout using
fprintf. If we are on MSWindows and have a windows
applications then register a HTPrintCallback
function. This is done with HTPrint_setCallback. It tells
HTPrint to call a HTPrintCallback. If
HTDEBUG
is not defined then don't do any of the above.
*/

typedef int HTPrintCallback(const char * fmt, va_list pArgs);
extern void HTPrint_setCallback(HTPrintCallback * pCall);
extern HTPrintCallback * HTPrint_getCallback(void);

extern int HTPrint(const char * fmt, ...);

/*
.
  Debug Message Control
.

This is the global flag for setting the WWWTRACE options.
The verbose mode is no longer a simple boolean but a bit field so that it
is possible to see parts of the output messages.
*/

#if defined(NODEBUG) || defined(NDEBUG) || defined(_NDEBUG)
#undef HTDEBUG
#else
#ifndef HTDEBUG
#define HTDEBUG		1
#endif /* HTDEBUG */
#endif

/*
(
  C Preprocessor defines
)

Make sure that the following macros are defined
*/

#ifndef __FILE__
#define __FILE__	""
#endif

#ifndef __LINE__
#define __LINE__	0L
#endif

/*
(
  Definition of the Global Trace Flag
)

The global trace flag variable is available everywhere.
*/

#ifdef HTDEBUG
#ifdef WWW_WIN_DLL
extern int *		WWW_TraceFlag;	 /* In DLLs, we need the indirection */
#define WWWTRACE	(*WWW_TraceFlag) 
#else
extern unsigned int	WWW_TraceFlag;	     /* Global flag for all W3 trace */
#define WWWTRACE	(WWW_TraceFlag)
#endif /* WWW_WIN_DLL */
#else
#define WWWTRACE	0
#endif /* HTDEBUG */

/*
(
  Select which Trace Messages to show
)

Libwww has a huge set of trace messages and it is therefor a good idea to
be able to select which ones to see for any particular trace. An easy way
to set this is using the funtion
HTSetTraceMessageMask. The WWWTRACE
define outputs messages if verbose mode is active according to the following
rules:
*/

typedef enum _HTTraceFlags {
    SHOW_UTIL_TRACE	= 0x1,
    SHOW_APP_TRACE	= 0x2,
    SHOW_CACHE_TRACE	= 0x4,
    SHOW_SGML_TRACE	= 0x8,
    SHOW_BIND_TRACE	= 0x10,
    SHOW_THREAD_TRACE	= 0x20,
    SHOW_STREAM_TRACE	= 0x40,
    SHOW_PROTOCOL_TRACE = 0x80,
    SHOW_MEM_TRACE	= 0x100,
    SHOW_URI_TRACE	= 0x200,
    SHOW_AUTH_TRACE	= 0x400,
    SHOW_ANCHOR_TRACE	= 0x800,
    SHOW_PICS_TRACE	= 0x1000,
    SHOW_CORE_TRACE	= 0x2000,
    SHOW_MUX_TRACE      = 0x4000,
    SHOW_SQL_TRACE      = 0x8000,
    SHOW_XML_TRACE      = 0x10000,
    SHOW_ALL_TRACE	= (int) 0xFFFFFFFF
} HTTraceFlags;

/*

The flags are made so that they can serve as a group flag for correlated
trace messages, e.g. showing messages for SGML and HTML at the same time.
*/

#define UTIL_TRACE	(WWWTRACE & SHOW_UTIL_TRACE)
#define APP_TRACE	(WWWTRACE & SHOW_APP_TRACE)
#define CACHE_TRACE	(WWWTRACE & SHOW_CACHE_TRACE)
#define SGML_TRACE	(WWWTRACE & SHOW_SGML_TRACE)
#define BIND_TRACE	(WWWTRACE & SHOW_BIND_TRACE)
#define THD_TRACE	(WWWTRACE & SHOW_THREAD_TRACE)
#define STREAM_TRACE	(WWWTRACE & SHOW_STREAM_TRACE)
#define PROT_TRACE	(WWWTRACE & SHOW_PROTOCOL_TRACE)
#define MEM_TRACE	(WWWTRACE & SHOW_MEM_TRACE)
#define URI_TRACE	(WWWTRACE & SHOW_URI_TRACE)
#define AUTH_TRACE	(WWWTRACE & SHOW_AUTH_TRACE)
#define ANCH_TRACE	(WWWTRACE & SHOW_ANCHOR_TRACE)
#define PICS_TRACE	(WWWTRACE & SHOW_PICS_TRACE)
#define CORE_TRACE	(WWWTRACE & SHOW_CORE_TRACE)
#define MUX_TRACE	(WWWTRACE & SHOW_MUX_TRACE)
#define SQL_TRACE	(WWWTRACE & SHOW_SQL_TRACE)
#define XML_TRACE	(WWWTRACE & SHOW_XML_TRACE)
#define ALL_TRACE	(WWWTRACE & SHOW_ALL_TRACE)

/*
(
  Destination for Trace Messages
)

You can send trace messages to various destinations depending on the type
of your application. By default, on Unix the messages are sent to
stderr using fprintf. If we are on MSWindows and
have a windows applications then register a HTTraceCallback
function. This is done with HTTrace_setCallback. It tells
HTTrace to call a HTTraceCallback. If 
HTDEBUG is not defined then don't do any of the above.
*/

typedef int HTTraceCallback(const char * fmt, va_list pArgs);
extern void HTTrace_setCallback(HTTraceCallback * pCall);
extern HTTraceCallback * HTTrace_getCallback(void);

/*

The HTTRACE macro uses "_" as parameter separater
instead of ",". This enables us to use a single macro instead
of a macro for each number of arguments which we consider a more elegant
and flexible solution. The implication is, however, that we can't have variables
that start or end with an "_" if they are to be used in a trace
message.
*/

#ifdef HTDEBUG
#undef _
#define _ ,
#define HTTRACE(TYPE, FMT) \
	do { if (TYPE) HTTrace(FMT); } while (0);
extern int HTTrace(const char * fmt, ...);
#else
#define HTTRACE(TYPE, FMT)		/* empty */
#endif /* HTDEBUG */

/*
(
  Data Trace Logging
)

A similar mechanism exists for logging data, except that is adds a data and
length argument to the trace call. Again, you can register your own callbacks
if need be.
*/

typedef int HTTraceDataCallback(char * data, size_t len, char * fmt, va_list pArgs);
extern void HTTraceData_setCallback(HTTraceDataCallback * pCall);
extern HTTraceDataCallback * HTTraceData_getCallback(void);

/*

Again we use the same macro expansion mechanism as for HTTrace
*/

#ifdef HTDEBUG
#define HTTRACEDATA(DATA, LEN, FMT) HTTraceData((DATA), (LEN), FMT)
extern int HTTraceData(char * data, size_t len, char * fmt, ...);
#else
#define HTTRACEDATA(DATA, LEN, FMT)	/* empty */
#endif /* HTDEBUG */

/*
(
  Debug Breaks
)

Call this function and the program halts. We use the same macro expansion
mechanism as for HTTrace
*/

extern void HTDebugBreak(char * file, unsigned long line, const char * fmt, ...);

#ifdef HTDEBUG
#define HTDEBUGBREAK(FMT) HTDebugBreak(__FILE__, __LINE__, FMT)
#else
#define HTDEBUGBREAK(FMT)		/* empty */
#endif /* HTDEBUG */

/*
.
  Macros for Function Declarations
.

These function prefixes are used by scripts and other tools and helps figuring
out which functions are exported and which are not. See also the
libwww style guide.
*/

#define PUBLIC			/* Accessible outside this module     */
#define PRIVATE static		/* Accessible only within this module */

/*
.
  Often used Interger Macros
.
(
  Min and Max functions
)
*/

#ifndef HTMIN 
#define HTMIN(a,b) ((a) <= (b) ? (a) : (b))
#define HTMAX(a,b) ((a) >= (b) ? (a) : (b))
#endif

/*
(
  Double abs function
)
*/

#ifndef HTDABS
#define HTDABS(a) ((a) < 0.0 ? (-(a)) : (a))
#endif

/*


.
  Return Codes for Protocol Modules and Streams
.

Theese are the codes returned from the protocol modules, and the stream modules.
Success are (>=0) and failure are (<0)
*/

#define HT_OK			0	/* Generic success */
#define HT_ALL			1	/* Used by Net Manager */

#define HT_CONTINUE             100     /* Continue an operation */
#define HT_UPGRADE              101     /* Switching protocols */

#define HT_LOADED		200  	/* Everything's OK */
#define HT_CREATED  	        201     /* New object is created */
#define HT_ACCEPTED  	        202     /* Accepted */
#define HT_NO_DATA		204  	/* OK but no data was loaded */
#define HT_RESET_CONTENT        205     /* Reset content */
#define HT_PARTIAL_CONTENT	206  	/* Partial Content */

#define HT_MULTIPLE_CHOICES     300     /* Multiple choices */
#define HT_PERM_REDIRECT	301  	/* Permanent redirection */
#define HT_FOUND        	302  	/* Found */
#define HT_SEE_OTHER            303     /* See other */
#define HT_NOT_MODIFIED         304     /* Not Modified */
#define HT_USE_PROXY            305     /* Use Proxy */
#define HT_PROXY_REDIRECT       306     /* Proxy Redirect */
#define HT_TEMP_REDIRECT        307     /* Temporary redirect */

#define HT_IGNORE		900  	/* Ignore this in the Net manager */
#define HT_CLOSED		901  	/* The socket was closed */
#define HT_PENDING		902  	/* Wait for connection */
#define HT_RELOAD		903  	/* If we must reload the document */

#define HT_ERROR		-1	/* Generic failure */

#define HT_NO_ACCESS		-401	/* Unauthorized */
#define HT_FORBIDDEN		-403	/* Access forbidden */
#define HT_NOT_FOUND		-404	/* Not found */
#define HT_NOT_ACCEPTABLE	-406	/* Not Acceptable */
#define HT_NO_PROXY_ACCESS      -407    /* Proxy Authentication Failed */
#define HT_CONFLICT             -409    /* Conflict */
#define HT_LENGTH_REQUIRED      -411    /* Length required */
#define HT_PRECONDITION_FAILED  -412    /* Precondition failed */
#define HT_TOO_BIG              -413    /* Request entity too large */
#define HT_URI_TOO_BIG          -414    /* Request-URI too long */
#define HT_UNSUPPORTED          -415    /* Unsupported */
#define HT_BAD_RANGE            -416    /* Request Range not satisfiable */
#define HT_EXPECTATION_FAILED   -417    /* Expectation Failed */
#define HT_REAUTH               -418    /* Reauthentication required */
#define HT_PROXY_REAUTH         -419    /* Proxy Reauthentication required */

#define HT_RETRY		-503	/* If service isn't available */
#define HT_BAD_VERSION		-505	/* Bad protocol version */

#ifdef HT_DAV                           /* WebDAV Status codes */
#define HT_PROCESSING            102    /* Processing  */
#define HT_MULTI_STATUS          207    /* Multi-Status */
#define HT_UNPROCESSABLE        -422    /* Unprocessable Entity */  
#define HT_LOCKED               -423    /* Locked */
#define HT_FAILED_DEPENDENCY    -424    /* Failed Dependency */
#define HT_INSUFFICIENT_STORAGE -507    /* Insufficient Storage */
#endif

#define HT_INTERNAL		-900    /* Weird -- should never happen. */
#define HT_WOULD_BLOCK		-901    /* If we are in a select */
#define HT_INTERRUPTED 		-902    /* Note the negative value! */
#define HT_PAUSE                -903    /* If we want to pause a stream */
#define HT_RECOVER_PIPE         -904    /* Recover pipe line */
#define HT_TIMEOUT              -905    /* Connection timeout */
#define HT_NO_HOST              -906    /* Can't locate host */

/*
.
  Upper- and Lowercase macros
.

The problem here is that toupper(x) is not defined officially unless isupper(x)
is. These macros are CERTAINLY needed on #if defined(pyr) || define(mips)
or BDSI platforms. For safefy, we make them mandatory.
*/

#ifndef TOLOWER
#define TOLOWER(c) tolower((int) (c))
#define TOUPPER(c) toupper((int) (c))
#endif

/*
.
  Max and Min values for Integers and Floating Point
.
*/

#ifdef FLT_EPSILON				    /* The ANSI C way define */
#define HT_EPSILON FLT_EPSILON
#else
#define HT_EPSILON 0.00000001
#endif

/*
.
  The local equivalents of CR and LF
.

We can check for these after net ascii text has been converted to the local
representation. Similarly, we include them in strings to be sent as net ascii
after translation.
*/

#define LF   FROMASCII('\012')  /* ASCII line feed LOCAL EQUIVALENT */
#define CR   FROMASCII('\015')  /* Will be converted to ^M for transmission */

/*
.
  Library Dynamic Memory Magement
.

The Library has it's own dynamic memory API which is declared in
memory management module.
*/

#include "HTMemory.h"

/*
*/

#endif /* HT_UTILS.h */

/*

  

  @(#) $Id: HTUtils.html,v 2.94 2002/06/04 15:14:11 kirschpi Exp $

*/
