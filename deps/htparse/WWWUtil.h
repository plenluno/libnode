/*

  
  
  					W3C Sample Code Library libwww Basic Utilities


!
  Declaration of W3C Sample Code Library Basic Utilities
!
*/
/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Basic Utility modules are mostly container modules, dynamic strings and
other stuff that you can't live without when building applications. The modules
are used by the core part of the W3C Sample Code Library but can also be used
by the application. See also the main Library include file called
WWWLib.h which contains all converters and protocol
modules known to the Library. You can include this one if the application
is to use all the functionality of the Library.
*/

#ifndef WWWUTIL_H
#define WWWUTIL_H

/*
*/

#ifdef __cplusplus
extern "C" { 
#endif

/*
.
  System dependencies
.

The wwwsys.h file includes system-specific include
files and flags for I/O to network and disk. The only reason for this file
is that the Internet world is more complicated than Posix and ANSI.
*/

#include "wwwsys.h"

/*
.
  Library Includes
.
(
  Basic Macros etc
)

This module includes file contains things we need everywhere, generally macros
for declarations, booleans, etc.
*/

#include "HTUtils.h"

/*
(
  Dynamic Arrays
)

This module implements a flexible array of pointers. It is a general utility
module. An array is a structure which may be extended. These routines create
and append data to arrays, automatically reallocating them as necessary.
It is garanteed that the last entry in an array is NULL
*/

#include "HTArray.h"

/*
(
  Association Lists
)

This is a small module build on top of the HTList
module that provides a way to store Name-Value pairs in an easy
way.
*/

#include "HTAssoc.h"

/*
(
  Atoms
)

Atoms are strings which are given representative pointer values so that they
can be stored more efficiently, and comparisons for equality done more
efficiently. The pointer values are in fact entries into a hash table.
*/

#include "HTAtom.h"

/*
(
  Dynamic Strings
)

A Chunk is a block wise expandable array of type (char *) and is a sort of
apology for real strings in C. Chunks make it easier to handle dynamic strings
of unknown size. It is often faster than using the String Copy Routines.
*/

#include "HTChunk.h"

/*
(
  Linked Lists
)

This module provides the functionality for managing a generic list of data
objects. The module is implemented as a single linked list using the scheme
first in - last out (FILO).
*/

#include "HTList.h"

/*
(
  Dymamic Memory Management
)

This module defines any memory handler to be used by libwww for allocating
and de-allocating dynamic memory. As dynamic memory may be a scarce resource,
it is required that an application can handle memory exhaustion gracefully.
*/

#include "HTMemory.h"

/*
(
  String Utilities
)

Routines for dynamic arrays of characters include string copy, case insensitive
comparison etc.
*/

#include "HTString.h"

/*
(
  UU encode and decode
)

File module provides functions functions for uuencode and decode strings
which convert a buffer of bytes to/from
RFC
1113 printable encoding format. This technique is similar to the familiar
Unix uuencode format in that it maps 6 binary bits to one ASCII character
(or more aptly, 3 binary bytes to 4 ASCII characters). However, RFC 1113
does not use the same mapping to printable characters as uuencode.
*/

#include "HTUU.h"

/*

End of utility modules
*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif

/*

  

  @(#) $Id: WWWUtil.html,v 2.12 1998/05/04 19:37:57 frystyk Exp $

*/
