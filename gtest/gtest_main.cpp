// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/debug.h>
#include <libnode/detail/http/agent.h>

#include <libj/detail/gc_base.h>

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    int r = RUN_ALL_TESTS();

#ifdef LIBNODE_DEBUG
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

    LIBJ_DEBUG_PRINT(
        "remaining objects: %d",
        LIBJ_DEBUG_OBJECT_COUNT);
    return r;
}
