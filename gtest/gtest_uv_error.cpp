// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/uv/error.h>

namespace libj {
namespace node {
namespace uv {

TEST(GTestError, TestCreate) {
    Error::CPtr e;
    e = Error::create(Error::_UNKNOWN);
    ASSERT_TRUE(!!e);
    e = Error::create(Error::_ENODEV);
    ASSERT_TRUE(!!e);
}

TEST(GTestError, TestMessage) {
    String::CPtr strEOF = String::create("end of file");
    Error::CPtr e = Error::create(Error::_EOF);
    ASSERT_TRUE(e->message()->equals(strEOF));
}

TEST(GTestError, TestToString) {
    String::CPtr strEOF = String::create("end of file");
    Error::CPtr e = Error::create(Error::_EOF);
    ASSERT_TRUE(e->toString()->equals(strEOF));
}

TEST(GTestError, TestValueOf) {
    String::CPtr strEOF = String::create("end of file");
    Error::CPtr e = Error::valueOf(UV_EOF);
    ASSERT_TRUE(e->message()->equals(strEOF));
}

TEST(GTestError, TestInstanceOf) {
    Error::CPtr e = Error::create(uv::Error::_EOF);
    ASSERT_TRUE(e->is<uv::Error>());
    ASSERT_TRUE(e->is<libj::Error>());
    ASSERT_TRUE(e->is<libj::Status>());
    ASSERT_TRUE(e->is<Immutable>());
    ASSERT_TRUE(e->is<Object>());
}

}  // namespace uv
}  // namespace node
}  // namespace libj
