/*

  
  					W3C Sample Code Library libwww System Dependencies


!
  System Dependencies
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This file makes up for the differencies in the systems and platforms supported
by libwww. On Unix, it is a question of using
autoconf to figure out what environment we are in. This is done by running
the configure script which creates a wwwconf.h file. This
configuration include file contains a large set of macro definitions telling
what features we have and don't have. On platforms not supported by autoconf
(Windows, Mac, VMS etc.) you will find the information normally contained
in the wwwconf.h file directly included below. The
second part of this file uses all the information
that we either have from the wwwconf.h file or directly coded and actually
does the includes etc.

	 
	   o 
	     Unix
  o 
	     Microsoft Windows Win32 API
  o 
	     Macintosh
  o 
	     Vax VMS

	 
This module is a part of the  W3C Sample
Code Library.
(
  Authors
)

  
    TBL
  
    Tim Berners-Lee, W3 project, CERN, <timbl@w3.org>
  
    EvA
  
    Eelco van Asperen <evas@cs.few.eur.nl>
  
    MA
  
    Marc Andreesen NCSA
  
    MD
  
    Mark Donszelmann <duns@vxcern.cern.ch>
  
    AT
  
    Aleksandar Totic <atotic@ncsa.uiuc.edu>
  
    SCW
  
    Susan C. Weber <sweber@kyle.eitech.com>
  
    HF
  
    Henrik Frystyk, <frystyk@w3.org>
  
    CLB
  
    Charlie Brooks, <cbrooks@osf.org>

(
  History:
)

  
    22 Feb 91
  
    Written (TBL) as part of the WWW library.
  
    16 Jan 92
  
    PC code from (EvA)
  
    22 Apr 93
  
    Merged diffs bits from xmosaic release
  
    29 Apr 93
  
    Windows/NT code from (SCW)
  
    29 Sep 93
  
    Mar 96 CLB - changed SLEEP() macro for Windows/NT MSC compiler added
    BOOLEAN_DEFINED macro to avoid duplicate definitions in HUtils.h changed
    netread() macros to support reading from stdin,etc. as well as sockets. (Required
    for linemode browser to work).
  
    Henrik
  
    Changed to support autoconf for Unix

*/

#ifndef SYSDEP_H
#define SYSDEP_H

/*
!
  Platform Specific Stuff
!
.
  Unix
.

We rely on autoconf to do the dirty job. If you have any changhes
then please add them to the configure
script
*/

#ifdef HAVE_CONFIG_H
#include <wwwconf.h>
#endif

/*
.
  Microsoft Windows Win32 API
.

Help provided by Eric Prud'hommeaux, Susan C. Weber
<sweber@kyle.eitech.com>, Paul Hounslow
<P.M.Hounslow@reading.ac.uk>, and a lot of other PC people.
*/

#if defined(_WINDOWS) || defined(_CONSOLE)
#define WWW_MSWINDOWS
#endif

#if defined(_WINDOWS) && !defined (_CONSOLE)
#define WWW_WIN_WINDOW
#endif

#if defined(_CONSOLE)
#define WWW_WIN_CONSOLE
#endif

#ifdef WWW_MSWINDOWS

#include <windows.h>
#include <io.h>
#include <process.h>
#include <winsock.h>

#include "windows/config.h"

#define NETREAD(s,b,l)  recv((s),(b),(l),0)
#define NETWRITE(s,b,l) send((s),(b),(l),0)
#define NETCLOSE(s)     closesocket(s)
#define IOCTL(s,c,a)	ioctlsocket(s,c, (long *) a)

#define MKDIR(a,b)	mkdir((a))
#define REMOVE(a)	remove((a))
#define DEFAULT_SUFFIXES	"."

#ifndef _CONSOLE
#define NO_STDIO
#endif

#define SOCKET SOCKET			/* WinSocks socket descriptor */
#define INVSOC INVALID_SOCKET		/* WinSocks invalid socket */

#define DESIRED_WINSOCK_VERSION	0x0101  /* we'd like winsock ver 1.1... */
#define MINIMUM_WINSOCK_VERSION	0x0101  /* ...but we'll take ver 1.1 :) */

/*
(
  File and Directory Access
)

These next defintions are because the UNIX stuff is not supplied with BC4
(Paul Hounslow <P.M.Hounslow@reading.ac.uk>)
*/

#define NO_UNIX_IO

#define	_IFMT		0170000	/* type of file */
#define	_IFDIR		0040000	/* directory */
#define	_IFCHR		0020000	/* character special */
#define	_IFBLK		0060000	/* block special */
#define	_IFREG		0100000	/* regular */
#define	_IFLNK		0120000	/* symbolic link */
#define	_IFSOCK		0140000	/* socket */
#define	_IFIFO		0010000	/* fifo */

#define	S_ISUID		0004000	/* set user id on execution */
#define	S_ISGID		0002000	/* set group id on execution */
#define	S_ISVTX		0001000	/* save swapped text even after use */

#ifdef S_IREAD
#undef S_IREAD
#define	S_IREAD		0000400	/* read permission, owner */
#endif

#ifdef S_IWRITE
#undef S_IWRITE
#define	S_IWRITE 	0000200	/* write permission, owner */
#endif

#ifdef S_IEXEC
#undef S_IEXEC
#define	S_IEXEC		0000100	/* execute/search permission, owner */
#endif

#define	S_ENFMT 	0002000	/* enforcement-mode locking */

#ifdef S_IFMT
#undef S_IFMT
#define	S_IFMT		_IFMT
#endif

#ifdef S_IDIR
#undef S_IDIR
#define	S_IFDIR		_IFDIR
#endif

#ifdef S_IFCHR
#undef S_IFCHR
#define	S_IFCHR		_IFCHR
#endif

#ifdef S_IBLK
#undef S_IBLK
#define	S_IFBLK		_IFBLK
#endif

#ifdef S_IREG
#undef S_IREG
#define	S_IFREG		_IFREG
#endif

#define	S_IFLNK		_IFLNK

#ifdef S_IFIFO
#undef S_IFIFO
#define	S_IFIFO		_IFIFO
#endif

#define	S_IRWXU 	0000700	/* rwx, owner */
#define		S_IRUSR	0000400	/* read permission, owner */
#define		S_IWUSR	0000200	/* write permission, owner */
#define		S_IXUSR	0000100	/* execute/search permission, owner */
#define	S_IRWXG		0000070	/* rwx, group */
#define		S_IRGRP	0000040	/* read permission, group */
#define		S_IWGRP	0000020	/* write permission, grougroup */
#define		S_IXGRP	0000010	/* execute/search permission, group */
#define	S_IRWXO		0000007	/* rwx, other */
#define		S_IROTH	0000004	/* read permission, other */
#define		S_IWOTH	0000002	/* write permission, other */
#define		S_IXOTH	0000001	/* execute/search permission, other */

#define	S_ISREG(m)	(((m)&_IFMT) == _IFREG)

#define DIR_SEPARATOR
#define DIR_SEPARATOR_CHAR	'\\'
#define DIR_SEPARATOR_STR	"\\"

/*
(
  Errno and Return Codes
)

Winsock has its own errno codes and it returns them through WSAGetLastError().
However, it does also support BSD error codes, so we make a compromise. WSA
definitions moved from _WIN32 ifdef by EGP
*/

#define socerrno WSAGetLastError()
#define ERRNO_DONE

/*

Return code for socket functions. We can't use -1 as return value

*/

#define EWOULDBLOCK     WSAEWOULDBLOCK
#define EINPROGRESS     WSAEINPROGRESS
#define ECONNREFUSED    WSAECONNREFUSED
#define ETIMEDOUT       WSAETIMEDOUT
#define ENETUNREACH     WSAENETUNREACH
#define EHOSTUNREACH    WSAEHOSTUNREACH
#define EHOSTDOWN       WSAEHOSTDOWN
#define EISCONN         WSAEISCONN
#define EINVAL          WSAEINVAL
#define ECONNRESET      WSAECONNRESET
#define ECONNABORTED    WSAECONNABORTED
#define ESHUTDOWN       WSAESHUTDOWN

/* Some compilers do only define WIN32 and NOT _WINDOWS */
#define NO_GROUPS

#ifdef _WIN32
#define MKDIR(a,b)	mkdir((a))     /* CLB NT has mkdir, but only one arg */
#define SLEEP(n)        Sleep((n)*1000)
#else
#define MKDIR(a,b)	_mkdir((a))    /* CLB NT has mkdir, but only one arg */
#endif /* WIN32 */

#endif /* WWW_MSWINDOWS */

/*
.
  Macintosh
.
(
  mingw32 - Minimalist GNU for Windows
)

A bit like Cygwin, except it uses the native Windows API, which means
the programs do not need the huge Cygwin DLL to run.

Patch by Richard Atterer <richard@atterer.net>, October 2001
*/

#ifdef __MINGW32__

#include <winsock2.h>

#define WWW_MSWINDOWS
/* #define WWW_WIN_CONSOLE */
#define WWW_WIN_WINDOW
/* #define WWW_WIN_ASYNC */
/* #define WWW_WIN_DLL */

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#define NETREAD(s,b,l)  recv((s),(b),(l),0)
#define NETWRITE(s,b,l) send((s),(b),(l),0)
#define NETCLOSE(s)     closesocket(s)
#define IOCTL(s,c,a)	ioctlsocket(s,c, (long *) a)

#define MKDIR(a,b)      mkdir(a)
#define REMOVE(a)	remove((a))
#define DEFAULT_SUFFIXES	"."

#define SOCKET SOCKET			/* WinSocks socket descriptor */
#define INVSOC INVALID_SOCKET		/* WinSocks invalid socket */

#define DESIRED_WINSOCK_VERSION 0x0101  /* we'd like winsock ver 1.1... */
#define MINIMUM_WINSOCK_VERSION 0x0101  /* ...but we'll take ver 1.1 :) */

#define DIR_SEPARATOR
#define DIR_SEPARATOR_CHAR     '\\'
#define DIR_SEPARATOR_STR      "\\"

#define socerrno WSAGetLastError()
#define ERRNO_DONE

/* Taken from the WIN32 stuff above. */
#define EWOULDBLOCK     WSAEWOULDBLOCK
#define EINPROGRESS     WSAEINPROGRESS
#define ECONNREFUSED    WSAECONNREFUSED
#define ETIMEDOUT       WSAETIMEDOUT
#define ENETUNREACH     WSAENETUNREACH
#define EHOSTUNREACH    WSAEHOSTUNREACH
#define EHOSTDOWN       WSAEHOSTDOWN
#define EISCONN         WSAEISCONN
/*#define EINVAL          WSAEINVAL*/
#define ECONNRESET      WSAECONNRESET
#define ECONNABORTED    WSAECONNABORTED
#define ESHUTDOWN       WSAESHUTDOWN

/* The configure.in script is wrong to default to #define GETGROUPS_T int */
#ifdef GETGROUPS_T
#undef GETGROUPS_T
#endif

#define HT_LSTAT stat

#endif /* __MINGW32__ */

/*
.
  Macintosh
.

We have two environments on Macintosh: Codeworrior and MPV.
(
  Metrowerks Codewarrior 6
)

Metrowerks Codewarrior is one development environment on the Mac. We are
using GUSI (1.5.9) by Matthias Neeracher <neeri@iis.ee.ethz.ch> for
our socket lib. You can find more information about the
GUSI Library from
Switzerland.

Compiles on PPC. Should compile on 68K.

August 31, 1995 by Steven T. Roussey <sroussey@eng.uci.edu> (STR).
and jeff@macalot.com (Jeff Dripps). Thanks a bunch!
*/

#ifdef __MWERKS__
#include <gusi.h>
#include <dirent.h>
#include <errno.h>
#include <sys/errno.h>
#include <sioux.h>

#define INCLUDES_DONE
#define TCP_INCLUDES_DONE

#define GUSI                    /* Identifies changes made for GUSI */

#undef  HAVE_GETDOMAINNAME      /* STR */
#undef  HAVE_GETPASS
#undef  HAVE_GETWD

#define HAVE_GETCWD
#define USE_DIRENT
#define NO_GROUPS
#define GOT_READ_DIR

#undef  HAVE_TIMEZONE           /* STR */
#define NO_GMTOFF
#define HAVE_STRERROR
#define HAVE_GETHOSTNAME

#define d_ino           d_fileno        /* backward compatibility */

#define SLEEP(n)        GUSIDefaultSpin( SP_SLEEP, n/60)

#define MKDIR(a,b)      mkdir(a)

#define HAVE_STRFTIME           // added JTD:5/1/96
#define HAVE_MKTIME             // added JTD:5/1/96
#define HAVE_STRCHR             // added JTD:5/1/96
#define STDC_HEADERS            // added JTD:5/1/96
#define HAVE_MEMCPY             // added JTD:5/1/96
#define TTY_IS_SELECTABLE       // added JTD:5/1/96
#define HAVE_READDIR            // added JTD:5/1/96
#define HAVE_DIRENT_INO         // added JTD:5/1/96
#define HAVE_DIRENT_H           // added JTD:5/1/96

#endif

/*
(
  MPW
)

MPW is one development environment on the Mac.

This entry was created by Aleksandar Totic (atotic@ncsa.uiuc.edu) this file
is compatible with sockets package released by NCSA. One major conflict is
that this library redefines write/read/etc as macros. In some of HTML code
these macros get executed when they should not be. Such files should define
NO_SOCKET_DEFS on top. This is a temporary hack.
*/

#ifdef applec			/* MPW  */
#undef HAVE_SYSTEM
#define DEBUG			/* Can't put it on the CC command line */

#define NO_UNIX_IO		/* getuid() missing */
#undef  HAVE_GETPID		/* getpid() does not exist */
#define NO_GETWD		/* getwd() does not exist */

#define NETCLOSE s_close    /* Routine to close a TCP-IP socket */
#define NETREAD  s_read	    /* Routine to read from a TCP-IP socket */
#define NETWRITE s_write    /* Routine to write to a TCP-IP socket */

#define _ANSI_SOURCE
#define GUI
#define LINEFEED 10
#define ANON_FTP_HOSTNAME
#ifndef NO_SOCKET_DEFS
#include <MacSockDefs.h>
#endif /* NO_SOCKET_DEFS */

#include <socket.ext.h>
#include <string.h>

#endif /* applec MPW */

/*
.
  VAX/VMS
.

Under VMS, there are many versions of TCP-IP. Define one if you do not use
Digital's UCX product:

  
    UCX
  
    DEC's "Ultrix connection" (default)
  
    WIN_TCP
  
    From Wollongong, now GEC software.
  
    MULTINET
  
    From SRI, now from TGV Inv.
  
    DECNET
  
    Cern's TCP socket emulation over DECnet


The last three do not interfere with the unix i/o library, and so they need
special calls to read, write and close sockets. In these cases the socket
number is a VMS channel number, so we make the @@@ HORRIBLE @@@ assumption
that a channel number will be greater than 10 but a unix file descriptor
less than 10. It works.
*/

#ifdef VMS
#include "HTVMSUtils.h"
#define CACHE_FILE_PREFIX	"SYS$LOGIN:Z_"
#define DEFAULT_SUFFIXES	"._"

#define HAVE_CUSERID

#ifdef WIN_TCP
#define NETREAD(s,b,l)	((s)>10 ? netread((s),(b),(l)) : read((s),(b),(l)))
#define NETWRITE(s,b,l)	((s)>10 ? netwrite((s),(b),(l)) : write((s),(b),(l)))
#define NETCLOSE(s) 	((s)>10 ? netclose(s) : close(s))
#endif /* WIN_TCP */

#ifdef MULTINET
#undef NETCLOSE
#undef NETREAD
#undef NETWRITE
#define NETREAD(s,b,l)	((s)>10 ? socket_read((s),(b),(l)) : read((s),(b),(l)))
#define NETWRITE(s,b,l)	((s)>10 ? socket_write((s),(b),(l)) : \
				write((s),(b),(l)))
#define NETCLOSE(s) 	((s)>10 ? socket_close(s) : close(s))
#define IOCTL(s,c,a)	socket_ioctl(s,c,a);
#endif /* MULTINET */

#ifdef DECNET
#define DNP_OBJ 80	/* This one doesn't look busy, but we must check */
			/* That one was for decnet */
#undef SELECT		/* not supported */
#define NETREAD(s,b,l)	((s)>10 ? recv((s),(b),(l),0) : read((s),(b),(l)))
#define NETWRITE(s,b,l)	((s)>10 ? send((s),(b),(l),0) : write((s),(b),(l)))
#define NETCLOSE(s) 	((s)>10 ? socket_close(s) : close(s))

#undef HAVE_GETHOSTNAME			/* Decnet doesn't have a name server */
#endif /* Decnet */

#undef HAVE_GETDOMAINNAME
         
/*	Certainly this works for UCX and Multinet; not tried for Wollongong
*/
#ifdef MULTINET
#include <time.h>
#ifdef __TIME_T
#define __TYPES
#define __TYPES_LOADED
#endif /* __TIME_T */
#include <multinet_root:[multinet.include.sys]types.h>
#include <multinet_root:[multinet.include]errno.h>
#ifdef __TYPES
#define __TIME_T
#endif /* __TYPE */
#ifdef __TIME_LOADED
#define __TIME
#endif /* __TIME_LOADED */
#include <multinet_root:[multinet.include.sys]time.h>
#else /* not MULTINET */
#include <types.h>
#include <errno.h>
#include <time.h>
#endif /* not MULTINET */

#include string

#ifndef STDIO_H
#include <stdio>
#define STDIO_H
#endif

#include file

#ifndef DECNET  /* Why is it used at all ? Types conflict with "types.h> */
#include unixio
#endif

#define INCLUDES_DONE

#ifdef MULTINET  /* Include from standard Multinet directories */
#include <multinet_root:[multinet.include.sys]socket.h>
#ifdef __TIME_LOADED  /* defined by sys$library:time.h */
#define __TIME  /* to avoid double definitions in next file */
#endif
#include <multinet_root:[multinet.include.netinet]in.h>
#include <multinet_root:[multinet.include.arpa]inet.h>
#include <multinet_root:[multinet.include]netdb.h>
#include <multinet_root:[multinet.include.sys]ioctl.h>

#else  /* not multinet */
#ifdef DECNET
#include <types.h>  /* for socket.h */
#include <socket.h>
#include <dn>
#include <dnetdb>

#else /* UCX or WIN */
#ifdef CADDR_T
#define __CADDR_T
#endif /* problem with xlib.h inclusion */
#include <socket.h>
#include <in.h>
#include <inet.h>
#include <netdb.h>
#include <ucx$inetdef.h>

#endif  /* not DECNET */
#endif  /* of Multinet or other TCP includes */

#define TCP_INCLUDES_DONE

#ifdef UCX
#define SIMPLE_TELNET
#endif

/*

On VMS directory browsing is available through a separate copy of dirent.c.
The definition of R_OK seem to be missing from the system include files...
*/

#define USE_DIRENT
#define GOT_READ_DIR
#include <dirent.h>
#define DIR struct dirent
#define R_OK 4

/*

On VMS machines, the linker needs to be told to put global data sections
into a data segment using these storage classes. (MarkDonszelmann)
*/

#ifdef VAXC
#define GLOBALDEF globaldef
#define GLOBALREF globalref
#endif /*  VAXC */
#endif	/* vms */

/*

On non-VMS machines, the GLOBALDEF and GLOBALREF storage types default to
normal C storage types.
*/

#ifndef GLOBALREF
#define GLOBALDEF
#define GLOBALREF extern
#endif

/*

On non-VMS machines STAT should be stat, unless that was overridden
somewhere above. On VMS machines STAT is a function that converts
directories and devices so that you can stat them.
*/

#ifdef VMS
typedef unsigned long mode_t;
#define HT_STAT		HTStat
#define HT_LSTAT	HTStat
#else
#ifndef HT_STAT
#define HT_STAT		stat
#endif
#ifndef HT_LSTAT
#define HT_LSTAT	lstat
#endif
#endif /* non VMS */

/*
(
  Dynamic Memory
)

Replace memory allocation and free C RTL functions with VAXC$xxx_OPT alternatives
for VAXC (but not DECC) on VMS. This makes a big performance difference.
(Foteos Macrides). Also have a look at the Dynamic
Memory Module for how to handle malloc and
calloc.
*/

#ifdef VMS
#include <stdio.h>
#include <stdlib.h>
#include <unixlib.h>
#include <ctype.h>
#if defined(VAXC) && !defined(__DECC)
#define malloc	VAXC$MALLOC_OPT
#define calloc	VAXC$CALLOC_OPT
#define free	VAXC$FREE_OPT
#define cfree	VAXC$CFREE_OPT
#define realloc	VAXC$REALLOC_OPT
#endif /* VAXC but not DECC */
#define unlink remove
#define gmtime localtime
#include <stat.h>
#define S_ISDIR(m)      (((m)&S_IFMT) == S_IFDIR)
#define S_ISREG(m)      (((m)&S_IFMT) == S_IFREG)
#define putenv HTVMS_putenv
#endif /* VMS */

/*
(
  Strftime and other time stuff
)
*/

#ifdef VMS
#ifndef DECC
#undef  HAVE_STRFTIME
#endif
#undef  HAVE_MKTIME
#undef  HAVE_TIMEGM
#define NO_GMTOFF
#undef  HAVE_TIMEZONE
#endif


/*
(
  Definition of Errno
)
*/

#ifdef VMS
#ifndef __DECC
extern int uerrno;	/* Deposit of error info (as per errno.h) */
extern volatile noshare int socket_errno; /* socket VMS error info 
                                          (used for translation of vmserrno) */
extern volatile noshare int vmserrno;	/* Deposit of VMS error info */
extern volatile noshare int errno;  /* noshare to avoid PSECT conflict */
#define ERRNO_DONE
#endif /* not DECC */
#endif /* VMS */

/*

  
!
  Platform Independent Stuff
!

Here we use all the knowledge we got above...
.
  Include Files
.

This file includes all system header files that are needed, iff they exist.
Their existance is discovered by configure.
*/

/* stdio.h */
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

/* types.h */
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#else
#ifdef HAVE_TYPES_H
#include <types.h>
#endif
#endif

/* unistd.h */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#ifdef HAVE_SYS_UNISTD_H
#include <sys/unistd.h>
#endif
#endif

/* fcntl.h */
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#ifdef HAVE_SYS_FCNTL_H
#include <sys/fcntl.h>
#endif
#endif

/* sys/machine.h */
#ifdef HAVE_SYS_MACHINE_H
#include <sys/machine.h>
#endif

/* limits.h */
#ifdef HAVE_SYS_LIMITS_H
#include <sys/limits.h>
#else
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#endif

/* stat.h */
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#else
#ifdef HAVE_STAT_H
#include <stat.h>
#endif
#endif

/* Patch for problems in glibc6 */
#if defined(__GLIBC__)
#undef S_IFMT
#undef S_IFDIR
#define S_IFMT __S_IFMT
#define S_IFDIR __S_IFDIR
#endif

/* in.h */
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#else
#ifdef HAVE_IN_H
#include <in.h>
#endif
#endif

/* tcp.h */
#ifdef HAVE_NETINET_TCP_H
#include <netinet/tcp.h>
#else
#ifdef HAVE_TCP_H
#include <tcp.h>
#endif
#endif

/* file.h */
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

/* systeminfo.h */
#ifdef HAVE_SYS_SYSTEMINFO_H
#include <sys/systeminfo.h>
#endif

/* ioctl.h */
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

/* termios.h */
#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif

/* time.h */
#ifdef TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#endif
#endif

/* string{,s}.h */
#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif

/* syslog.h */
#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#else
#ifdef HAVE_SYS_SYSLOG_H
#include <sys/socket.h>
#endif
#endif

/* socket.h */
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#else
#ifdef HAVE_SOCKET_H
#include <socket.h>
#endif
#endif

/* socket.ext.h */
#ifdef HAVE_SOCKET_EXT_H
#include <socket.ext.h>
#endif

/* appkit.h */
#ifdef HAVE_APPKIT_APPKIT_H
#include <appkit/appkit.h>
#else
#ifdef HAVE_APPKIT_H
#include <appkit.h>
#endif
#endif

/* dn.h */
#ifdef HAVE_DN_H
#include <dn.h>
#endif

/* ipc.h */
#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif

/* errno.h */
#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
#ifdef HAVE_SYS_ERRNO_H
#include <sys/errno.h>
#else
#ifdef HAVE_NET_ERRNO_H
#include <net/errno.h>
#endif
#endif
#endif

/* pwd.h */
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

/* grp.h */
#ifdef HAVE_GRP_H
#include <grp.h>
#endif

/* inet.h */
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#else
#ifdef HAVE_INET_H
#include <inet.h>
#endif
#endif

/* netdb.h */
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

/* manifest.h */
#ifdef HAVE_MANIFEST_H
#include <manifest.h>
#endif

/* bsdtypes.h */
#ifdef HAVE_BSDTYPES_H
#include <bsdtypes.h>
#endif

/* stdefs.h */
#ifdef HAVE_STDEFS_H
#include <stdefs.h>
#endif

/* bsdtime.h */
#ifdef HAVE_BSDTIME_H
#include <bsdtime.h>
#endif

/* select.h */
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#else
#ifdef HAVE_SELECT_H
#include <select.h>
#endif
#endif

/* dnetdb.h */
#ifdef HAVE_DNETDB_H
#include <dnetdb.h>
#endif

/* ucx$inetdef.h */
#ifdef HAVE_UCX_INETDEF_H
#include <ucx$inetdef.h>
#endif

/* libc.h */
#ifdef HAVE_LIBC_H
#include <libc.h>
#endif

/* stdlib.h */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

/* malloc.h */
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

/* memory.h */
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif

/* unixlib.h */
#ifdef HAVE_UNIXLIB_H
#include <unixlib.h>
#endif

/* direct.h */
#ifdef HAVE_DIRECT_H
#include <direct.h>
#endif

/* ctype.h */
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

/* curses.h */
#ifdef CURSES
#ifdef HAVE_CURSESX_H
#include <cursesX.h>
#else
#ifdef HAVE_CURSES_H
#include <curses.h>
#endif
#endif
#endif

/* resource.h (some wait.h's require it) */
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#else
#ifdef HAVE_RESOURCE_H
#include <resource.h>
#endif
#endif

/* dirent.h / ndir.h / dir.h */
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#define dirent direct
#define NAMLEN(dirent) (dirent)->d_namlen
#ifdef HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#ifdef HAVE_DIR_H
#include <dir.h>
#endif
#ifdef HAVE_NDIR_H
#include <ndir.h>
#endif
#endif

#if !defined(HAVE_STRCHR)
#define strchr index
#define strrchr rindex
#endif
#if !defined(HAVE_MEMCPY)
#define memcpy(d, s, n) bcopy((s), (d), (n))
#define memmove(d, s, n) bcopy((s), (d), (n))
#endif

/* Definition of var args */
#if defined(STDC_HEADERS) || defined(__STDC__)
#include <stdarg.h>
#else
#ifndef WWW_MSWINDOWS
#include <varargs.h>
#endif
#endif

/* wait.h */
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifndef WEXITSTATUS
#define WEXITSTATUS(stat_val) (((unsigned)(stat_val) >> 8) & 255)
#endif
#ifndef WIFEXITED
#define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif

#ifdef HT_POSIX_REGEX
#ifdef HAVE_RXPOSIX_H
#include <rxposix.h>
#else
#ifdef HAVE_REGEX_H
#include <regex.h>
#endif
#endif
#define W3C_DEFAULT_REGEX_FLAGS		(REG_EXTENDED | REG_NEWLINE)
#endif

/*
.
  Booleans
.
*/

#ifndef __MINGW32__ /* has a typedef for BOOLEAN */
#ifndef BOOLEAN
typedef char	BOOLEAN;				    /* Logical value */
#endif
#endif

#ifndef CURSES
#ifndef TRUE
#define TRUE	(BOOLEAN)1
#define	FALSE	(BOOLEAN)0
#endif
#endif	 /*  CURSES  */

#if !defined(BOOL) && !defined(WWW_MSWINDOWS)
#define BOOL BOOLEAN
#endif

#ifndef YES
#define YES             (BOOL)1
#define NO              (BOOL)0
#endif

/*
.
  Integers
.

If we don't have these (for some mysterious reason) then define them. This
should (is?) be handled by the configure script already.
*/

/* Richard Atterer  
   Disabled - caused problems because mingw32 has typedefs for these. 
   They seem useless now. */

/* 
#ifndef u_short
#define u_short unsigned short
#endif

#ifndef u_long
#define u_long unsigned long
#endif

*/

/*
.
  NULL Definition
.
*/

#ifndef NULL
#define NULL ((void *)0)
#endif

/*
.
  Sleep System Call
.

Some systems use milliseconds instead of seconds
*/

#ifndef SLEEP
#define SLEEP(n)	sleep(n)
#endif

/*
.
  SOCKS
.

SOCKS is a package for allowing socket connections to tunnel through firewalls
in carefully controlled situations. This package can be optionally compiled
with SOCKS support; these definitions replace the normal socket calls with
the SOCKS ones. Initial modification of the library is credited to Ian Dunkin
<imd1707@ggr.co.uk>.
*/

#ifdef SOCKS
#ifdef SOCKS4
#define connect	        Rconnect
#define getsockname	Rgetsockname
#define getpeername	Rgetpeername
#define bind		Rbind
#define accept		Raccept
#define listen		Rlisten
#define select		Rselect
#define recvfrom	Rrecvfrom
#define sendto		Rsendto
#define recv		Rrecv
#define send		Rsend
#define read		Rread
#define write		Rwrite
#define rresvport	Rrresvport
#define shutdown	Rshutdown
#define listen		Rlisten
#define close		Rclose
#define dup		Rdup
#define dup2		Rdup2
#define fclose		Rfclose
#define gethostbyname	Rgethostbyname
#else
#ifdef SOCKS5
#define connect		SOCKSconnect
#define getsockname	SOCKSgetsockname
#define getpeername	SOCKSgetpeername
#define bind		SOCKSbind
#define accept		SOCKSaccept
#define listen		SOCKSlisten
#define select		SOCKSselect
#define recvfrom	SOCKSrecvfrom
#define sendto		SOCKSsendto
#define recv		SOCKSrecv
#define send		SOCKSsend
#define read		SOCKSread
#define write		SOCKSwrite
#define rresvport	SOCKSrresvport
#define shutdown	SOCKSshutdown
#define listen		SOCKSlisten
#define close		SOCKSclose
#define dup		SOCKSdup
#define dup2		SOCKSdup2
#define fclose		SOCKSfclose
#define gethostbyname	SOCKSgethostbyname
#endif /* SOCKS5 */
#endif /* SOCKS4 */
#endif /* SOCKS */

/*
.
  Network Family
.
*/

#ifdef DECNET
typedef struct sockaddr_dn SockA;  /* See netdnet/dn.h or custom vms.h */
#else /* Internet */
typedef struct sockaddr_in SockA;  /* See netinet/in.h */
#endif

/*
.
  Default Values of Network Access
.
*/

#ifndef NETCLOSE
#define NETCLOSE close		/* Routine to close a TCP-IP socket */
#endif

#ifndef NETREAD
#define NETREAD  read		/* Routine to read from a TCP-IP socket	*/
#endif

#ifndef NETWRITE
#define NETWRITE write		/* Routine to write to a TCP-IP socket */
#endif

#ifndef NETWRITEV
#define NETWRITEV writev
#endif

/*
.
  Definition of errno and Return Code
.

This is the definition of error codes and the corresponding string constants.
If we do not have the strerror function then try the error list
table.
*/

#ifndef ERRNO_DONE
extern int errno;
#define socerrno errno
#endif

#ifndef HAVE_STRERROR				  /* Otherwise use the table */
extern char *sys_errlist[];
extern int sys_nerr;
#endif

/*
.
  Definition of Socket Desrciptores
.

This is necessary in order to support Windows NT...
*/

#ifndef SOCKET
#define SOCKET int		/* Unix like socket descriptor */
#define INVSOC (-1)		/* Unix invalid socket */
#endif

#ifdef __svr4__
#define HT_BACKLOG 32		 /* Number of pending connect requests (TCP) */
#else
#define HT_BACKLOG 5		 /* Number of pending connect requests (TCP) */
#endif /* __svr4__ */

/*
.
  Additional Network variables for SOCKET, file access bits
.
*/

#if 0
#ifndef _WINSOCKAPI_
#define FD_READ         0x01
#define FD_WRITE        0x02
#define FD_OOB          0x04
#define FD_ACCEPT       0x08
#define FD_CONNECT      0x10
#define FD_CLOSE        0x20
#endif /* _WINSOCKAPI_ */
#endif

/*
.
  Rough estimate of max path length
.
*/

#ifndef HT_MAX_PATH
#ifdef MAXPATHLEN
#define HT_MAX_PATH MAXPATHLEN
#else
#ifdef PATH_MAX
#define HT_MAX_PATH PATH_MAX
#else
#define HT_MAX_PATH 1024	 	      	/* Any better ideas? */
#endif
#endif
#endif /* HT_MAX_PATH */

#ifndef HT_MAX_TMPNAM
#ifdef L_tmpnam
#define HT_MAX_TMPNAM	L_tmpnam
#else
#define HT_MAX_TMPNAM	128
#endif
#endif

/*
.
  Hash Table Sizes
.

The size of the hash tables used by the various libwww classes like the
HTNet class, the HTHost
class etc. is a speed-size compromize. Here you can set the sizes depending
on whether you have a lot of memory or not. The values must be prime numbers,
of course.
*/

#ifdef LIBWWW_SMALL
#define HT_XL_HASH_SIZE		101
#define HT_L_HASH_SIZE		67
#define HT_M_HASH_SIZE		31
#define HT_S_HASH_SIZE		5
#else
#ifdef LIBWWW_LARGE
#define HT_XL_HASH_SIZE		1499
#define HT_L_HASH_SIZE		599
#define HT_M_HASH_SIZE		101
#define HT_S_HASH_SIZE		67
#else
#ifdef LIBWWW_EXTRA_LARGE
#define HT_XL_HASH_SIZE		9973
#define HT_L_HASH_SIZE		1499
#define HT_M_HASH_SIZE		599
#define HT_S_HASH_SIZE		101
#else
#define HT_XL_HASH_SIZE		599
#define HT_L_HASH_SIZE		101
#define HT_M_HASH_SIZE		67
#define HT_S_HASH_SIZE		11
#endif
#endif
#endif

/*
.
  Definition of NGROUPS
.
*/

#ifdef GETGROUPS_T
#ifndef NGROUPS
#ifdef NGROUPS_MAX
#define NGROUPS		NGROUPS_MAX
#else
#define NGROUPS		20			/* Any better ideas? */
#endif
#endif
#endif

/*
.
  Definition of Max Domain Name Length
.
*/

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64			/* Any better ideas? */
#endif

/*
.
  File/Directory Management
.
*/

#ifndef MKDIR
#define MKDIR(a,b)	mkdir((a), (b))
#endif

#ifndef RMDIR
#define RMDIR(a)	rmdir((a))
#endif

#ifndef REMOVE
#define REMOVE(a)	unlink((a))
#endif

#ifndef DEFAULT_SUFFIXES
#define DEFAULT_SUFFIXES	".,_"
#endif

#ifndef DIR_SEPARATOR
#define DIR_SEPARATOR_CHAR	'/'
#define DIR_SEPARATOR_STR	"/"
#endif

#ifndef F_OK
#define R_OK    4
#define W_OK    2
#define X_OK    1
#define F_OK    0
#endif

/*
.
  Macros for manipulating masks for select()
.
*/

#ifndef FD_SET
#define FD_SET(fd, m) (*(unsigned*)(m) |=  (1 << (fd)))
#endif

#ifndef FD_CLR
#define FD_CLR(fd, m) (*(unsigned*)(m) &= ~(1 << (fd)))
#endif

#ifndef FD_ZERO
#define FD_ZERO(m)    (*(unsigned*)(m)) = 0
#endif

#ifndef FD_ISSET
#define FD_ISSET(fd, m) (*(unsigned*)(m) & (1 << (fd)))
#endif

/*
.
  Macros for converting characters
.
*/

#ifndef TOASCII
#define TOASCII(c) (c)
#define FROMASCII(c) (c)                                   
#endif

/*
.
  Cache file prefix
.

This is something onto which we tag something meaningful to make a cache
file name. used in HTWSRC.c at least. If it is not defined at all, caching
is turned off.
*/

#ifndef CACHE_FILE_PREFIX
#ifdef unix
#define CACHE_FILE_PREFIX  "/usr/wsrc/"
#endif
#endif

/*
.
  Thread Safe Setup
.

These are some constants setting the size of buffers used by thread safe
versions of some system calls.
*/

#ifdef HT_REENTRANT

#ifdef _POSIX_LOGIN_NAME_MAX
#define HT_LOGNAME_MAX  _POSIX_LOGIN_NAME_MAX
#else
#ifdef LOGNAME_MAX
#define HT_LOGNAME_MAX  (LOGNAME_MAX+1)
#else
#define HT_LOGNAME_MAX  64
#endif
#endif

#define HOSTENT_MAX	1024
#define CTIME_MAX	26
#endif /* HT_REENTRANT */

/*
.
  Types
.

We define some types here so we son't have to worry about it later
*/

typedef unsigned long ms_t;

/*
*/

#endif

/*

  

  @(#) $Id: wwwsys.html,v 2.127 2002/06/06 10:46:04 kahan Exp $

*/
