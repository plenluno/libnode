// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libj/console.h>
#include <libj/detail/gc_base.h>

#ifdef LIBJ_USE_BDWGC
    #include <gc.h>
#endif

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    int r = RUN_ALL_TESTS();

#ifdef LIBJ_DEBUG
# ifdef LIBJ_USE_BDWGC
    libj::Long before;
    libj::Long after;
    do {
        before = LIBJ_DEBUG_OBJECT_COUNT;
        GC_gcollect();
        after = LIBJ_DEBUG_OBJECT_COUNT;
    } while (before > after);
# endif
    libj::console::debug(
        "[LIBJ DEBUG] remaining objects: %d",
        LIBJ_DEBUG_OBJECT_COUNT);
#endif

    return r;
}
