// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/error.h>
#include <uv.h>

namespace libj {
namespace node {

TEST(GTestError, TestCreate) {
    Error::CPtr e = Error::create(Error::UV_ERR_EOF);
    ASSERT_TRUE(e);
    e = Error::create(Status::OK);
    ASSERT_FALSE(e);
}

TEST(GTestError, TestMessage) {
    String::CPtr strEOF = String::create("end of file");
    Error::CPtr e = Error::create(Error::UV_ERR_EOF);
    ASSERT_TRUE(e->message()->equals(strEOF));
}

TEST(GTestError, TestToString) {
    String::CPtr strEOF = String::create("end of file");
    Error::CPtr e = Error::create(Error::UV_ERR_EOF);
    ASSERT_TRUE(e->toString()->equals(strEOF));
}

TEST(GTestError, TestValueOf) {
    String::CPtr strEOF = String::create("end of file");
    Error::CPtr e = Error::valueOf(UV_EOF);
    ASSERT_TRUE(e->message()->equals(strEOF));
}

TEST(GTestError, TestInstanceOf) {
    Error::CPtr e = Error::create(Error::UV_ERR_EOF);
    ASSERT_TRUE(e->instanceof(Type<libj::node::Error>::id()));
    ASSERT_TRUE(e->instanceof(Type<libj::Error>::id()));
    ASSERT_TRUE(e->instanceof(Type<libj::Status>::id()));
    ASSERT_TRUE(e->instanceof(Type<Immutable>::id()));
    ASSERT_TRUE(e->instanceof(Type<Object>::id()));
}

}  // namespace node
}  // namespace libj
