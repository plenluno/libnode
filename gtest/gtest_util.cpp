// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/util.h>

namespace libj {
namespace node {

TEST(GTestUtil, TestExtend) {
    JsObject::Ptr addtion = JsObject::create();
    addtion->put(str("a"), 3);
    addtion->put(str("b"), str("c"));

    JsObject::Ptr target = JsObject::create();
    target->put(str("d"), 5);

    ASSERT_EQ(target, util::extend(target, addtion));
    ASSERT_TRUE(target->get(str("a")).equals(3));
    ASSERT_TRUE(target->get(str("b")).equals(str("c")));
    ASSERT_TRUE(target->get(str("d")).equals(5));
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
    Buffer::Ptr decoded = util::hexDecode(str());
    ASSERT_EQ(0, decoded->length());

    decoded = util::hexDecode(str("a9"));
    ASSERT_EQ(1, decoded->length());
    UByte b = 0;
    ASSERT_TRUE(decoded->readUInt8(0, &b));
    ASSERT_EQ(0xa9, b);

    decoded = util::hexDecode(str("uv"));
    ASSERT_FALSE(decoded);

    decoded = util::hexDecode(str("012"));
    ASSERT_FALSE(decoded);
}

TEST(GTestUtil, TestBase64Encode) {
    String::CPtr encoded = util::base64Encode(Buffer::create());
    ASSERT_TRUE(encoded && encoded->isEmpty());

    Buffer::Ptr buf = Buffer::create(str("ABCDEFG"));
    encoded = util::base64Encode(buf);
    ASSERT_TRUE(encoded->equals(str("QUJDREVGRw==")));
}

TEST(GTestUtil, TestBase64Decode) {
    Buffer::Ptr decoded = util::base64Decode(str());
    ASSERT_EQ(0, decoded->length());

    decoded = util::base64Decode(str("QUJDREVGRw=="));
    ASSERT_TRUE(!!decoded);
    ASSERT_TRUE(decoded->toString()->equals(str("ABCDEFG")));

    decoded = util::base64Decode(str("QUJDREVGRw==@[]@"));
    ASSERT_TRUE(!decoded);
}

TEST(GTestUtil, TestPercentEncode) {
    String::CPtr encoded = util::percentEncode(str());
    ASSERT_TRUE(encoded && encoded->isEmpty());

    encoded = util::percentEncode(str(" "));
    ASSERT_TRUE(encoded->equals(str("%20")));

    encoded = util::percentEncode(str("[\"123\"]"));
    ASSERT_TRUE(encoded->equals(str("%5B%22123%22%5D")));
}

TEST(GTestUtil, TestPercentDecode) {
    String::CPtr decoded = util::percentDecode(str());
    ASSERT_TRUE(decoded->equals(str()));

    decoded = util::percentDecode(str("%20"));
    ASSERT_TRUE(decoded->equals(str(" ")));

    decoded = util::percentDecode(str("%5B%22123%22%5D"));
    ASSERT_TRUE(decoded->equals(str("[\"123\"]")));
}

}  // namespace node
}  // namespace libj
