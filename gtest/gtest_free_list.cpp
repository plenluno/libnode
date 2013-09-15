// Copyright (c) 2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/detail/free_list.h>

#include <libj/string.h>

namespace libj {
namespace node {
namespace detail {

TEST(GTestFreeList, TestFreeList1) {
    FreeList<String::CPtr> sl(2);
    ASSERT_TRUE(!sl.alloc());

    sl.free(String::create("a"));
    sl.free(String::create("b"));
    sl.free(String::create("c"));

    ASSERT_EQ(2, sl.length());

    String::CPtr a = sl.alloc();
    String::CPtr b = sl.alloc();
    ASSERT_TRUE(a->equals(String::create("a")));
    ASSERT_TRUE(b->equals(String::create("b")));
    ASSERT_TRUE(!sl.alloc());

    sl.free(a);
}

TEST(GTestFreeList, TestFreeList12) {
    FreeList<std::string*> sl(2);
    ASSERT_TRUE(!sl.alloc());

    sl.free(new std::string("a"));
    sl.free(new std::string("b"));
    sl.free(new std::string("c"));

    ASSERT_EQ(2, sl.length());

    std::string* a = sl.alloc();
    std::string* b = sl.alloc();
    ASSERT_EQ("a", *a);
    ASSERT_EQ("b", *b);
    ASSERT_TRUE(!sl.alloc());

    sl.free(a);
    delete b;
}

}  // namespace detail
}  // namespace node
}  // namespace libj
