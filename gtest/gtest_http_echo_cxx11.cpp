// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/url.h>

#include <assert.h>
#include <libj/json.h>
#include <libj/js_closure.h>

#include "./gtest_http_common.h"

namespace libj {
namespace node {

TEST(GTestHttpEchoCxx11, TestEchoCxx11) {
    GTestOnEnd::clear();
    GTestHttpClientOnResponse::clear();

    auto srv = http::Server::create();
    srv->on(
        http::Server::EVENT_REQUEST,
        JsClosure::create([srv] (JsArray::Ptr args) {
            auto req = toPtr<http::ServerRequest>(args->get(0));
            auto res = toPtr<http::ServerResponse>(args->get(1));

            req->setEncoding(Buffer::UTF8);

            GTestOnData::Ptr onData(new GTestOnData());
            req->on(http::ServerRequest::EVENT_DATA, onData);

            req->on(
                http::ServerRequest::EVENT_END,
                JsClosure::create(
                    [srv, req, res, onData]
                    (JsArray::Ptr args) {
                    auto body = onData->string();
                    res->setHeader(
                        http::HEADER_CONTENT_TYPE,
                        String::create("text/plain"));
                    res->setHeader(
                        http::HEADER_CONTENT_LENGTH,
                        String::valueOf(Buffer::byteLength(body)));
                    res->write(body);
                    res->end();
                    srv->close();
                    return UNDEFINED;
                }));

            return UNDEFINED;
        }));
    srv->listen(10000);

    auto msg = String::create("cxx11");
    auto url = String::create("http://127.0.0.1:10000/abc");
    auto options = url::parse(url);
    auto headers = JsObject::create();
    headers->put(
        http::HEADER_CONNECTION,
        String::create("close"));
    headers->put(
        http::HEADER_CONTENT_LENGTH,
        String::valueOf(Buffer::byteLength(msg)));
    options->put(http::OPTION_HEADERS, headers);

    JsFunction::Ptr onResponse(new GTestHttpClientOnResponse());
    auto req = http::request(options, onResponse);
    req->write(msg);
    req->end();

    node::run();

    auto msgs = GTestOnEnd::messages();
    auto codes = GTestHttpClientOnResponse::statusCodes();
    ASSERT_EQ(1, msgs->length());
    ASSERT_EQ(1, codes->length());
    ASSERT_TRUE(msgs->get(0).equals(msg));
    ASSERT_TRUE(codes->get(0).equals(200));
}

}  // namespace node
}  // namespace libj
