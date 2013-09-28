// Copyright (c) 2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/url.h>

#include "./gtest_http_common.h"

// #define USE_PROXY

namespace libj {
namespace node {

TEST(GTestDns, TestHttpGet) {
    JsObject::Ptr headers = JsObject::create();
#ifdef USE_PROXY
    JsObject::Ptr options = JsObject::create();
    options->put(http::OPTION_HEADERS, headers);
    options->put(http::OPTION_PATH, str("http://nodejs.org"));
    options->put(http::OPTION_HOST, str("proxy"));  // proxy host
    options->put(http::OPTION_PORT, str("8080"));   // proxy port
    headers->put(http::HEADER_HOST, str("nodejs.org"));
#else
    JsObject::Ptr options = url::parse(str("http://nodejs.org"));
#endif
    headers->put(http::HEADER_CONNECTION, str("close"));

    GTestHttpClientOnResponse::Ptr onResponse(new GTestHttpClientOnResponse());
    http::get(options, onResponse);

    node::run();

    JsArray::CPtr msgs = GTestOnEnd::messages();
    JsArray::CPtr codes = GTestHttpClientOnResponse::statusCodes();
    ASSERT_EQ(1, msgs->length());
    ASSERT_EQ(1, codes->length());
    ASSERT_TRUE(!!msgs->getCPtr<String>(0));
    ASSERT_TRUE(codes->get(0).equals(200));
    // console::log(msgs->get(0));

    clearGTestHttpCommon();
}

}  // namespace node
}  // namespace libj
