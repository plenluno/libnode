// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include "./gtest_http_common.h"

namespace libj {
namespace node {

static const UInt NUM_REQS = 7;

TEST(GTestHttpEcho, TestConnectionKeepAlive) {
    GTestOnEnd::clear();
    GTestOnClose::clear();
    GTestHttpServerOnEnd::clear();
    GTestHttpClientOnResponse::clear();

    String::CPtr msg = str("abc");

    http::Server::Ptr srv = http::Server::create();
    GTestHttpServerOnRequest::Ptr onRequest(
        new GTestHttpServerOnRequest(srv, NUM_REQS));
    srv->on(http::Server::EVENT_REQUEST, onRequest);
    srv->listen(10000);

    JsObject::Ptr options = url::parse(str("http://127.0.0.1:10000/xyz"));
    JsObject::Ptr headers = JsObject::create();
    headers->put(
        http::HEADER_CONTENT_LENGTH,
        String::valueOf(Buffer::byteLength(msg)));
    options->put(http::OPTION_HEADERS, headers);

    GTestHttpClientOnResponse::Ptr onResponse(new GTestHttpClientOnResponse());
    for (Size i = 0; i < NUM_REQS; i++) {
        http::ClientRequest::Ptr req = http::request(options, onResponse);
        req->write(msg);
        req->end();
    }

    node::run();

    ASSERT_EQ(0, GTestOnClose::count());

    JsArray::CPtr messages = GTestOnEnd::messages();
    JsArray::CPtr statusCodes = GTestHttpClientOnResponse::statusCodes();
    Size numMsgs = messages->length();
    Size numCodes = statusCodes->length();
    ASSERT_EQ(NUM_REQS, numMsgs);
    ASSERT_EQ(NUM_REQS, numCodes);
    for (Size i = 0; i < numMsgs; i++) {
        ASSERT_TRUE(messages->get(i).equals(msg));
        ASSERT_TRUE(statusCodes->get(i).equals(200));
        console::printf(console::LEVEL_INFO, ".");
    }
    console::printf(console::LEVEL_INFO, "\n");
}

TEST(GTestHttpEcho, TestConnectionClose) {
    GTestOnEnd::clear();
    GTestOnClose::clear();
    GTestHttpServerOnEnd::clear();
    GTestHttpClientOnResponse::clear();

    String::CPtr msg = str("xyz");

    http::Server::Ptr srv = http::Server::create();
    GTestHttpServerOnRequest::Ptr onRequest(
        new GTestHttpServerOnRequest(srv, NUM_REQS));
    srv->on(http::Server::EVENT_REQUEST, onRequest);
    srv->listen(10000);

    JsObject::Ptr options = url::parse(str("http://127.0.0.1:10000/abc"));
    JsObject::Ptr headers = JsObject::create();
    headers->put(http::HEADER_CONNECTION, str("close"));
    headers->put(
        http::HEADER_CONTENT_LENGTH,
        String::valueOf(Buffer::byteLength(msg)));
    options->put(http::OPTION_HEADERS, headers);

    GTestHttpClientOnResponse::Ptr onResponse(new GTestHttpClientOnResponse());
    for (Size i = 0; i < NUM_REQS; i++) {
        http::ClientRequest::Ptr req = http::request(options, onResponse);
        req->write(msg);
        req->end();
    }

    node::run();

    ASSERT_EQ(0, GTestOnClose::count());

    JsArray::CPtr messages = GTestOnEnd::messages();
    JsArray::CPtr statusCodes = GTestHttpClientOnResponse::statusCodes();
    Size numMsgs = messages->length();
    Size numCodes = statusCodes->length();
    ASSERT_EQ(NUM_REQS, numMsgs);
    ASSERT_EQ(NUM_REQS, numCodes);
    for (Size i = 0; i < numMsgs; i++) {
        ASSERT_TRUE(messages->get(i).equals(msg));
        ASSERT_TRUE(statusCodes->get(i).equals(200));
        console::printf(console::LEVEL_INFO, ".");
    }
    console::printf(console::LEVEL_INFO, "\n");
}

}  // namespace node
}  // namespace libj
