// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/util.h>

namespace libj {
namespace node {

TEST(GTestUtil, TestExtend) {
    JsObject::Ptr super = JsObject::create();
    super->put(String::create("a"), 3);
    super->put(String::create("b"), String::create("c"));
    JsObject::Ptr derived = JsObject::create();
    ASSERT_TRUE(util::extend(derived, super));
    ASSERT_TRUE(derived->get(String::create("a")).equals(3));
    ASSERT_TRUE(derived->get(String::create("b")).equals(String::create("c")));
}

TEST(GTestUtil, TestHexEncode) {
    String::CPtr encoded = util::hexEncode(Buffer::create());
    ASSERT_TRUE(encoded && encoded->isEmpty());

    Buffer::Ptr buf = Buffer::create(1);
    buf->writeUInt8(0xa9, 0);
    encoded = util::hexEncode(buf);
    ASSERT_EQ(2, encoded->length());
    ASSERT_EQ('a', encoded->charAt(0));
    ASSERT_EQ('9', encoded->charAt(1));
}

TEST(GTestUtil, TestHexDecode) {
    Buffer::Ptr decoded = util::hexDecode(String::create());
    ASSERT_EQ(0, decoded->length());

    decoded = util::hexDecode(String::create("a9"));
    ASSERT_EQ(1, decoded->length());
    ASSERT_EQ(0xa9, decoded->at(0));

    decoded = util::hexDecode(String::create("uv"));
    ASSERT_FALSE(decoded);

    decoded = util::hexDecode(String::create("012"));
    ASSERT_FALSE(decoded);
}

TEST(GTestUtil, TestBase64Encode) {
    String::CPtr encoded = util::base64Encode(Buffer::create());
    ASSERT_TRUE(encoded && encoded->isEmpty());

    String::CPtr str = String::create("ABCDEFG");
    Buffer::Ptr buf = Buffer::create(str);
    encoded = util::base64Encode(buf);
    ASSERT_TRUE(encoded->equals(String::create("QUJDREVGRw==")));
}

TEST(GTestUtil, TestBase64Decode) {
    Buffer::Ptr decoded = util::base64Decode(String::create());
    ASSERT_EQ(0, decoded->length());

    String::CPtr str = String::create("QUJDREVGRw==");
    decoded = util::base64Decode(str);
    ASSERT_TRUE(!!decoded);
    ASSERT_TRUE(decoded->toString()->equals(String::create("ABCDEFG")));

    str = String::create("QUJDREVGRw==@[]@");
    decoded = util::base64Decode(str);
    ASSERT_TRUE(!decoded);
}

TEST(GTestUtil, TestPercentEncode) {
    String::CPtr encoded = util::percentEncode(String::create());
    ASSERT_TRUE(encoded && encoded->isEmpty());

    encoded = util::percentEncode(String::create(" "));
    ASSERT_TRUE(encoded->equals(String::create("%20")));

    encoded = util::percentEncode(String::create("[\"123\"]"));
    ASSERT_TRUE(encoded->equals(String::create("%5B%22123%22%5D")));
}

TEST(GTestUtil, TestPercentDecode) {
    String::CPtr decoded = util::percentDecode(String::create());
    ASSERT_TRUE(decoded->equals(String::create()));

    decoded = util::percentDecode(String::create("%20"));
    ASSERT_TRUE(decoded->equals(String::create(" ")));

    decoded = util::percentDecode(String::create("%5B%22123%22%5D"));
    ASSERT_TRUE(decoded->equals(String::create("[\"123\"]")));
}

}  // namespace node
}  // namespace libj
