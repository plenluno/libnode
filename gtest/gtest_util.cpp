// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/util.h>

namespace libj {
namespace node {

TEST(GTestUtil, TestPercentEncode) {
    String::CPtr str = String::create();
    String::CPtr encoded = util::percentEncode(str);
    ASSERT_EQ(encoded->compareTo(String::create()), 0);

    str = String::create(" ");
    encoded = util::percentEncode(str);
    ASSERT_EQ(encoded->compareTo(String::create("%20")), 0);

    str = String::create("[\"123\"]");
    encoded = util::percentEncode(str);
    ASSERT_EQ(encoded->compareTo(String::create("%5B%22123%22%5D")), 0);
}

TEST(GTestUtil, TestPercentDecode) {
    String::CPtr str = String::create();
    String::CPtr decoded = util::percentDecode(str);
    ASSERT_EQ(decoded->compareTo(String::create()), 0);

    str = String::create("%20");
    decoded = util::percentDecode(str);
    ASSERT_EQ(decoded->compareTo(String::create(" ")), 0);

    str = String::create("%5B%22123%22%5D");
    decoded = util::percentDecode(str);
    ASSERT_EQ(decoded->compareTo(String::create("[\"123\"]")), 0);
}


}  // namespace node
}  // namespace libj
