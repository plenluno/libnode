// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/http_server.h>

namespace libj {
namespace node {

TEST(GTestHttpServer, TestCreate) {
    Type<HttpServer>::Ptr srv = HttpServer::create();
    ASSERT_TRUE(srv ? true : false);
}

}  // namespace node
}  // namespace libj
