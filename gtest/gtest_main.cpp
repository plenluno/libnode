// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/debug_print.h>
#include <libnode/detail/http/agent.h>

#include <libj/trace.h>
#include <libj/detail/debug_gc.h>

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

    LIBJ_DEBUG_GC;

    LIBJ_DEBUG_PRINT(
        "remaining objects: %d",
        LIBJ_DEBUG_OBJECT_COUNT);

    LIBJ_DEBUG_TRACE_OFF;
    return r;
}
