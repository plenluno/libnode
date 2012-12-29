// Copyright (c) 2012 Plenluno All rights reserved.

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
    ASSERT_TRUE(GTestOnEnd::messages()->isEmpty());

    http::Server::Ptr srv = http::Server::create();
    srv->on(
        http::Server::EVENT_REQUEST,
        JsClosure::create([srv] (JsArray::Ptr args) -> Value {
            http::ServerRequest::Ptr req =
                toPtr<http::ServerRequest>(args->get(0));
            http::ServerResponse::Ptr res =
                toPtr<http::ServerResponse>(args->get(1));

            req->setEncoding(Buffer::UTF8);

            GTestOnData::Ptr onData(new GTestOnData());
            req->on(http::ServerRequest::EVENT_DATA, onData);

            req->on(
                http::ServerRequest::EVENT_END,
                JsClosure::create(
                    [srv, req, res, onData]
                    (JsArray::Ptr args) -> Value {
                    String::CPtr body = onData->string();
                    res->setHeader(
                        http::HEADER_CONTENT_TYPE,
                        String::create("text/plain"));
                    res->setHeader(
                        http::HEADER_CONTENT_LENGTH,
                        String::valueOf(Buffer::byteLength(body)));
                    res->write(body);
                    res->end();
                    srv->close();
                    return Status::OK;
                }));

            return Status::OK;
        }));
    srv->listen(10000);

    String::CPtr msg = String::create("cxx11");
    String::CPtr url = String::create("http://127.0.0.1:10000/abc");
    JsObject::Ptr options = url::parse(url);
    JsObject::Ptr headers = JsObject::create();
    headers->put(
        http::HEADER_CONNECTION,
        String::create("close"));
    headers->put(
        http::HEADER_CONTENT_LENGTH,
        String::valueOf(Buffer::byteLength(msg)));
    options->put(http::OPTION_HEADERS, headers);

    GTestHttpClientOnResponse::Ptr onResponse(new GTestHttpClientOnResponse());
    http::ClientRequest::Ptr req = http::request(options, onResponse);
    req->write(msg);
    req->end();

    node::run();

    JsArray::CPtr msgs = GTestOnEnd::messages();
    ASSERT_EQ(1, msgs->length());
    ASSERT_TRUE(msgs->get(0).equals(msg));
}

}  // namespace node
}  // namespace libj
