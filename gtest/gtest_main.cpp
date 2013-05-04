// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/debug_print.h>
#include <libnode/detail/http/agent.h>

#include <libj/trace.h>
#include <libj/detail/gc_base.h>

#include "./gtest_common.h"

int main(int argc, char** argv) {
    LIBJ_DEBUG_TRACE_ON;
    LIBJ_DEBUG_TRACE_INCLUDE("^libj::node");
    LIBJ_DEBUG_TRACE_INCLUDE("^libj::(.*)::create");

    testing::InitGoogleTest(&argc, argv);
    int r = RUN_ALL_TESTS();

#ifdef LIBNODE_DEBUG
    libj::node::GTestOnEnd::clear();
    libj::node::detail::http::freeGlobalAgent();
#endif

#ifdef LIBJ_USE_BDWGC
    libj::Long before;
    libj::Long after;
    do {
        before = LIBJ_DEBUG_OBJECT_COUNT;
        GC_gcollect();
        after = LIBJ_DEBUG_OBJECT_COUNT;
    } while (before > after);
#endif

    if (libj::node::uv::Error::last()) {
        LIBJ_DEBUG_PRINT(
            "static: uv::Error::last %p",
            LIBJ_DEBUG_OBJECT_PTR(libj::node::uv::Error::last()));
    }

    LIBJ_DEBUG_PRINT(
        "remaining objects: %d",
        LIBJ_DEBUG_OBJECT_COUNT);

    LIBJ_DEBUG_TRACE_OFF;
    return r;
}
