// Copyright (c) 2012-2013 Plenluno All rights reserved.

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
    Status::CPtr s = Status::create(Status::NOT_FOUND);
    ASSERT_TRUE(s->message()->equals(str("Not Found")));

    s = Status::create(libj::Status::OK);
    ASSERT_TRUE(s->message()->equals(str("Unknown")));
}

TEST(GTestHttpStatus, TestMessage2) {
    Status::CPtr s = Status::create(Status::NOT_FOUND, str("404"));
    ASSERT_TRUE(s->message()->equals(str("404")));
}

TEST(GTestHttpStatus, TestToString) {
    Status::CPtr s = Status::create(Status::NOT_FOUND);
    ASSERT_TRUE(s->toString()->equals(str("Not Found")));
}

TEST(GTestHttpStatus, TestInstanceOf) {
    Status::CPtr s = Status::create(Status::NOT_FOUND);
    ASSERT_TRUE(s->is<Status>());
    ASSERT_TRUE(s->is<libj::Status>());
    ASSERT_TRUE(s->is<Immutable>());
    ASSERT_TRUE(s->is<Object>());
}

}  // namespace http
}  // namespace node
}  // namespace libj
