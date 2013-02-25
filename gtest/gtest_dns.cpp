// Copyright (c) 2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/url.h>

#include "./gtest_http_common.h"

// #define USE_PROXY

namespace libj {
namespace node {

TEST(GTestDns, TestHttpGet) {
    GTestOnEnd::clear();
    GTestHttpClientOnResponse::clear();

    String::CPtr url = String::create("http://nodejs.org");
    JsObject::Ptr headers = JsObject::create();
#ifdef USE_PROXY
    String::CPtr host = String::create("nodejs.org");
    String::CPtr proxyHost = String::create("proxy");
    String::CPtr proxyPort = String::create("8080");
    JsObject::Ptr options = JsObject::create();
    options->put(http::OPTION_HEADERS, headers);
    options->put(http::OPTION_PATH, url);
    options->put(http::OPTION_HOST, proxyHost);
    options->put(http::OPTION_PORT, proxyPort);
    headers->put(http::HEADER_HOST, host);
#else
    JsObject::Ptr options = url::parse(url);
#endif
    headers->put(http::HEADER_CONNECTION, String::create("close"));

    GTestHttpClientOnResponse::Ptr onResponse(new GTestHttpClientOnResponse());
    http::get(options, onResponse);

    node::run();

    JsArray::CPtr msgs = GTestOnEnd::messages();
    JsArray::CPtr codes = GTestHttpClientOnResponse::statusCodes();
    ASSERT_EQ(1, msgs->length());
    ASSERT_EQ(1, codes->length());
    ASSERT_TRUE(msgs->getCPtr<String>(0));
    ASSERT_TRUE(codes->get(0).equals(200));
    // console::log(msgs->get(0));
}

}  // namespace node
}  // namespace libj
