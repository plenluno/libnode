// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/buffer.h>

namespace libj {
namespace node {

TEST(GTestBuffer, TestCreate) {
    Buffer::Ptr buf = Buffer::create();
    ASSERT_TRUE(buf);
}

TEST(GTestBuffer, TestCreate2) {
    Buffer::Ptr buf = Buffer::create("abc", 3);
    ASSERT_TRUE(buf);
    ASSERT_EQ(buf->length(), 3);
    Byte b = 0;
    ASSERT_TRUE(buf->readInt8(&b, 2));
    ASSERT_EQ(b, 'c');
}

TEST(GTestBuffer, TestCreate3) {
    JsTypedArray<UByte>::Ptr ary = JsTypedArray<UByte>::create();
    ary->add(static_cast<UByte>('0'));
    ary->add(static_cast<UByte>('1'));
    Buffer::Ptr buf = Buffer::create(ary);
    ASSERT_TRUE(buf);
    ASSERT_EQ(buf->length(), 2);
    UByte b = 0;
    ASSERT_TRUE(buf->readUInt8(&b, 1));
    ASSERT_EQ(b, '1');
}

TEST(GTestBuffer, TestCreate4) {
    const UByte d[] = {
        0xe3, 0x81, 0x82,
        0xe3, 0x81, 0x84,
        0xe3, 0x81, 0x86,
        0x00
    };
    String::CPtr str = String::create(d, String::UTF8);
    ASSERT_EQ(str->length(), 3);
    Buffer::Ptr buf = Buffer::create(str, String::UTF8);
    ASSERT_TRUE(buf);
    ASSERT_EQ(buf->length(), 9);
    UByte b = 0;
    ASSERT_TRUE(buf->readUInt8(&b, 8));
    ASSERT_EQ(b, 0x86);
}

TEST(GTestBuffer, TestByteLength) {
    const UByte d[] = {
        0xe3, 0x81, 0x82,
        0xe3, 0x81, 0x84,
        0xe3, 0x81, 0x86,
        0x00
    };
    String::CPtr str = String::create(d, String::UTF8);
    ASSERT_EQ(Buffer::byteLength(str), 9);
}

TEST(GTestBuffer, TestWrite) {
    Buffer::Ptr buf = Buffer::create(2);
    Byte wb = 15;
    ASSERT_TRUE(buf->writeInt8(wb, 1));
    Byte rb = 0;
    ASSERT_TRUE(buf->readInt8(&rb, 1));
    ASSERT_EQ(wb, rb);
}

TEST(GTestBuffer, TestLength) {
    Size len = 35472;
    Buffer::Ptr buf = Buffer::create(len);
    ASSERT_EQ(buf->length(), len);
}

}  // namespace node
}  // namespace libj
