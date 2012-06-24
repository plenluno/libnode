// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/buffer.h>

namespace libj {
namespace node {

TEST(GTestBuffer, TestCreate) {
    Buffer::Ptr buf = Buffer::create();
    ASSERT_TRUE(buf);
}

TEST(GTestBuffer, TestSetInt8) {
    Buffer::Ptr buf = Buffer::create(1);
    Byte wb = 15;
    ASSERT_TRUE(buf->writeInt8(wb, 0));
    Byte rb = 0;
    ASSERT_TRUE(buf->readInt8(&rb, 0));
    ASSERT_EQ(wb, rb);
}

TEST(GTestBuffer, TestLength) {
    Size len = 35472;
    Buffer::Ptr buf = Buffer::create(len);
    ASSERT_EQ(buf->length(), len);
}

}  // namespace node
}  // namespace libj
