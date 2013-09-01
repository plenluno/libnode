// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <libnode/trace.h>
#include <libnode/detail/http/agent.h>

#include <libj/detail/gc_collect.h>

#include "./gtest_common.h"

int main(int argc, char** argv) {
    LIBNODE_DEBUG_TRACE_ON;

    testing::InitGoogleTest(&argc, argv);
    int r = RUN_ALL_TESTS();

#ifdef LIBNODE_DEBUG
    libj::node::GTestOnEnd::clear();
    libj::node::detail::http::freeGlobalAgent();
#endif

    LIBJ_GC_COLLECT;

    LIBJ_DEBUG_PRINT(
        "remaining objects: %d",
        LIBJ_DEBUG_OBJECT_COUNT);

    LIBNODE_DEBUG_TRACE_OFF;
    return r;
}
