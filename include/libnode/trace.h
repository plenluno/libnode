// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_TRACE_H_
#define LIBNODE_TRACE_H_

#include <libj/trace.h>

#define LIBNODE_DEBUG_TRACE_ON \
    LIBJ_DEBUG_TRACE_ON; \
    LIBJ_DEBUG_TRACE_INCLUDE("^libj::node"); \
    LIBJ_DEBUG_TRACE_INCLUDE("^libj::.*::create");

#define LIBNODE_DEBUG_TRACE_OFF \
    LIBJ_DEBUG_TRACE_OFF;

#endif  // LIBNODE_TRACE_H_
