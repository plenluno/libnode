// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_ARGUMENTS_H_
#define LIBNODE_DETAIL_ARGUMENTS_H_

#include <libnode/config.h>

#include <libj/js_array.h>
#include <libj/debug_print.h>

#ifdef LIBNODE_USE_SP
# ifdef LIBNODE_DEBUG
#  define LIBNODE_ARGUMENTS_CREATE(A) \
    static libj::JsArray::Ptr A = libj::JsArray::null(); \
    if (!A) { \
        A = libj::JsArray::create(); \
        LIBJ_DEBUG_PRINT( \
            "static: args@EventEmitter::emit %p", \
            LIBJ_DEBUG_OBJECT_PTR(A)); \
    }
#  define LIBNODE_ARGUMENTS_CLEAR(A) \
    if (A.use_count() == 1) { \
        A->clear(); \
    } else { \
        A = libj::JsArray::create(); \
        LIBJ_DEBUG_PRINT( \
            "static: args@EventEmitter::emit %p", \
            LIBJ_DEBUG_OBJECT_PTR(A)); \
    }
# else
#  define LIBNODE_ARGUMENTS_CREATE(A) \
    static libj::JsArray::Ptr A = libj::JsArray::create();
#  define LIBNODE_ARGUMENTS_CLEAR(A) \
    if (A.use_count() == 1) { \
        A->clear(); \
    } else { \
        A = libj::JsArray::create(); \
    }
# endif
#else
# define LIBNODE_ARGUMENTS_CREATE(A) \
    libj::JsArray::Ptr A = libj::JsArray::create();
# define LIBNODE_ARGUMENTS_CLEAR(A)
#endif

#endif  // LIBNODE_DETAIL_ARGUMENTS_H_
