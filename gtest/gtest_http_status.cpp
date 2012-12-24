// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/http/status.h>

namespace libj {
namespace node {
namespace http {

TEST(GTestHttpStatus, TestCreate) {
    Status::CPtr s = Status::create(Status::OK);
    ASSERT_TRUE(!!s);
    s = Status::create(libj::Status::OK);
    ASSERT_TRUE(!!s);
}

TEST(GTestHttpStatus, TestMessage) {
    String::CPtr strNotFound = String::create("Not Found");
    String::CPtr strUnknown = String::create("Unknown");

    Status::CPtr s = Status::create(Status::NOT_FOUND);
    ASSERT_TRUE(s->message()->equals(strNotFound));

    s = Status::create(libj::Status::OK);
    ASSERT_TRUE(s->message()->equals(strUnknown));
}

TEST(GTestHttpStatus, TestMessage2) {
    String::CPtr str404 = String::create("404");
    Status::CPtr s = Status::create(Status::NOT_FOUND, str404);
    ASSERT_TRUE(s->message()->equals(str404));
}

TEST(GTestHttpStatus, TestToString) {
    String::CPtr strNotFound = String::create("Not Found");
    Status::CPtr s = Status::create(Status::NOT_FOUND);
    ASSERT_TRUE(s->toString()->equals(strNotFound));
}

TEST(GTestHttpStatus, TestInstanceOf) {
    Status::CPtr s = Status::create(Status::NOT_FOUND);
    ASSERT_TRUE(s->instanceof(Type<Status>::id()));
    ASSERT_TRUE(s->instanceof(Type<libj::Status>::id()));
    ASSERT_TRUE(s->instanceof(Type<Immutable>::id()));
    ASSERT_TRUE(s->instanceof(Type<Object>::id()));
}

}  // namespace http
}  // namespace node
}  // namespace libj
