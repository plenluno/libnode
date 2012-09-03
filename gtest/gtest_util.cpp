// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/util.h>

namespace libj {
namespace node {

TEST(GTestUtil, TestHexEncode) {
    Buffer::Ptr buf = Buffer::create();
    Buffer::Ptr encoded = util::hexEncode(buf);
    ASSERT_EQ(0, encoded->length());

    buf = Buffer::create(1);
    buf->writeUInt8(0xa9, 0);
    encoded = util::hexEncode(buf);
    ASSERT_EQ(2, encoded->length());
    ASSERT_EQ('a', encoded->get(0));
    ASSERT_EQ('9', encoded->get(1));
}

TEST(GTestUtil, TestHexDecode) {
    Buffer::Ptr buf = Buffer::create();
    Buffer::Ptr decoded = util::hexDecode(buf);
    ASSERT_EQ(0, decoded->length());

    buf = Buffer::create(2);
    buf->writeUInt8('a', 0);
    buf->writeUInt8('9', 1);
    decoded = util::hexDecode(buf);
    ASSERT_EQ(1, decoded->length());
    ASSERT_EQ(0xa9, decoded->get(0));
}

TEST(GTestUtil, TestBase64Encode) {
    Buffer::Ptr buf = Buffer::create();
    ASSERT_TRUE(buf);
    Buffer::Ptr encoded = util::base64Encode(buf);
    ASSERT_TRUE(encoded);
    ASSERT_EQ(0, encoded->length());

    String::CPtr str = String::create("ABCDEFG");
    buf = Buffer::create(str, Buffer::ASCII);
    ASSERT_EQ(7, buf->length());
    encoded = util::base64Encode(buf);
    ASSERT_TRUE(encoded->toString()->equals(String::create("QUJDREVGRw==")));
}

TEST(GTestUtil, TestBase64Decode) {
    Buffer::Ptr buf = Buffer::create();
    Buffer::Ptr decoded = util::base64Decode(buf);
    ASSERT_EQ(0, decoded->length());

    String::CPtr str = String::create("QUJDREVGRw==");
    buf = Buffer::create(str, Buffer::ASCII);
    ASSERT_EQ(12, buf->length());
    decoded = util::base64Decode(buf);
    ASSERT_EQ(7, decoded->length());
    ASSERT_TRUE(decoded->toString()->equals(String::create("ABCDEFG")));
}

TEST(GTestUtil, TestPercentEncode) {
    String::CPtr str = String::create();
    String::CPtr encoded = util::percentEncode(str);
    ASSERT_TRUE(encoded->equals(String::create()));

    str = String::create(" ");
    encoded = util::percentEncode(str);
    ASSERT_TRUE(encoded->equals(String::create("%20")));

    str = String::create("[\"123\"]");
    encoded = util::percentEncode(str);
    ASSERT_TRUE(encoded->equals(String::create("%5B%22123%22%5D")));
}

TEST(GTestUtil, TestPercentDecode) {
    String::CPtr str = String::create();
    String::CPtr decoded = util::percentDecode(str);
    ASSERT_TRUE(decoded->equals(String::create()));

    str = String::create("%20");
    decoded = util::percentDecode(str);
    ASSERT_TRUE(decoded->equals(String::create(" ")));

    str = String::create("%5B%22123%22%5D");
    decoded = util::percentDecode(str);
    ASSERT_TRUE(decoded->equals(String::create("[\"123\"]")));
}


}  // namespace node
}  // namespace libj
