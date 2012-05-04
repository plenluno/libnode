/*

  
  					W3C Sample Code Library libwww Dynamic Memory Handlers


!
  Dynamic Memory Handlers
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module defines any memory handler to be used by libwww for allocating
and de-allocating dynamic memory. As dynamic memory may be a scarce resource,
it is required that an application can handle memory exhaustion gracefully.
This module provides an interface that covers the following situations:

	 
	   o 
	     Handling of allocation, reallocation and de-allocation
    of dynamic memory
  o 
	     Recovering from temporary lack of available memory
  o 
	     Panic handling in case a new allocation fails

	 
Note: The Library core provides a default set of memory handlers
for allocating and de-allocating dynamic memory. In order to maintain a
reasonable performance, they are not registered dynamically but assigned
using C style macros. Hence, it is not possible to swap memory handler
at run time but this was considered to be a reasonable trade-off.

This module is implemented by HTMemory.c, and it
is a part of the W3C Sample Code
Library.
*/

#ifndef HTMEMORY_H
#define HTMEMORY_H

#include "HTUtils.h"

/*
.
  Allocation, Reallocation and De-allocation
.

The Library provides a default set of methods for handling dynamic memory.
They are very basic and essentially identical to the C style
malloc, calloc, realloc, and
free:
*/
extern void* HTMemory_malloc(size_t size);
extern void* HTMemory_calloc(size_t count, size_t size);
extern void* HTMemory_realloc(void * ptr, size_t size);
extern void HTMemory_free(void* ptr);

/*
(
  Memory Macros
)

The methods above are not referred directly in the Library. Instead we use
a set of C style macros. If you don't wany any memory management beyond normal
malloc and alloc then you can just use that instead of the HTMemory_* function.
You can of course also provide your own methods as well.
*/

#define HT_MALLOC(size)		HTMemory_malloc((size))
#define HT_CALLOC(count, size)	HTMemory_calloc((count), (size))
#define HT_REALLOC(ptr, size)	HTMemory_realloc((ptr), (size))
#define HT_FREE(pointer)	{HTMemory_free((pointer));((pointer))=NULL;}

/*
.
  Memory Freer Functions
.

The dynamic memory freer functions are typically functions that are capable
of freeing large chunks of memory. In case a new allocation fails, the allocation
method looks for any registered freer functions to call. There can be multiple
freer functions and after each call, the allocation method tries again to
allocate the desired amount of dynamic memory. The freer functions are called
in reverse order meaning that the last one registered gets
called first. That way, it is easy to add temporary freer functions
which then are guaranteed to be called first if a methods fails.
(
  Add a Freer Function
)

You can add a freer function by using the following method. The Library may
itself register a set of free functions during initialization. If the application
does not register any freer functions then the Library looks how it can free
internal memory. The freer function is passed the total number of
bytes requested by the allocation.
*/
typedef void HTMemoryCallback(size_t size);

extern BOOL HTMemoryCall_add (HTMemoryCallback * cbf);

/*
(
  Delete a Freer Function
)

Freer functions can be deleted at any time in which case they are not called
anymore.
*/

extern BOOL HTMemoryCall_delete (HTMemoryCallback * cbf);
extern BOOL HTMemoryCall_deleteAll (void);

/*
.
  Panic Handling
.

If the freer functions are not capable of de-allocation enough memory then
the application must have an organized way of closing down. This is done
using the panic handler. In the libwww, each allocation is tested and
HT_OUTOFMEM is called if a NULL was returned.
HT_OUTOFMEM is a macro which by default calls
HTMemory_outofmem() but of course can point to any method. The
default handler calls an exit function defined by the application in a call
to HTMemory_setExit(). If the application has not defined
an exit function, HTMemory_outofmem() prints an error message
and calls exit(1).
*/

typedef void HTMemory_exitCallback(char *name, char *file, unsigned long line);

extern void HTMemory_setExit(HTMemory_exitCallback * pExit);
extern HTMemory_exitCallback * HTMemory_exit(void);

/*
(
  Call the Exit Handler
)

If an allocation fails then this function is called. If the application has
registered its own panic handler then this is called directly from this function.
Otherwise, the default behavior is to write a small message to stderr and
then exit.
*/

#define outofmem(file, name)	HT_OUTOFMEM(name)
#define HT_OUTOFMEM(name)	HTMemory_outofmem((name), __FILE__, __LINE__)

extern void HTMemory_outofmem(char * name, char * file, unsigned long line);

/*
*/

#endif /* HTMEMORY_H */

/*

  

  @(#) $Id: HTMemory.html,v 2.12 1998/05/14 02:10:45 frystyk Exp $

*/
