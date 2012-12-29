// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/url.h>

#include "./gtest_http_common.h"

namespace libj {
namespace node {

static const Size NUM_REQS = 7;

class GTestHttpServerOnEnd : LIBJ_JS_FUNCTION(GTestHttpServerOnEnd)
 public:
    GTestHttpServerOnEnd(
        http::Server::Ptr srv,
        http::ServerRequest::Ptr req,
        http::ServerResponse::Ptr res,
        GTestOnData::Ptr onData)
        : srv_(srv)
        , req_(req)
        , res_(res)
        , onData_(onData) {}

    static void clear() {
        count_ = 0;
    }

    virtual Value operator()(JsArray::Ptr args) {
        String::CPtr body = onData_->string();

        res_->setHeader(
            http::HEADER_CONTENT_TYPE,
            String::create("text/plain"));
        res_->setHeader(
            http::HEADER_CONTENT_LENGTH,
            String::valueOf(Buffer::byteLength(body)));
        res_->write(body);
        res_->end();

        count_++;
        if (count_ >= NUM_REQS) srv_->close();
        return Status::OK;
    }

 private:
    static UInt count_;

    http::Server::Ptr srv_;
    http::ServerRequest::Ptr req_;
    http::ServerResponse::Ptr res_;
    GTestOnData::Ptr onData_;
};

UInt GTestHttpServerOnEnd::count_ = 0;

class GTestHttpServerOnRequest : LIBJ_JS_FUNCTION(GTestHttpServerOnRequest)
 public:
    GTestHttpServerOnRequest(
        http::Server::Ptr srv) : srv_(srv) {}

    virtual Value operator()(JsArray::Ptr args) {
        http::ServerRequest::Ptr req =
            toPtr<http::ServerRequest>(args->get(0));
        http::ServerResponse::Ptr res =
            toPtr<http::ServerResponse>(args->get(1));

        GTestOnData::Ptr onData(new GTestOnData());
        GTestOnClose::Ptr onClose(new GTestOnClose());
        GTestHttpServerOnEnd::Ptr onEnd(
            new GTestHttpServerOnEnd(srv_, req, res, onData));
        req->setEncoding(Buffer::UTF8);
        req->on(http::ServerRequest::EVENT_DATA, onData);
        req->on(http::ServerRequest::EVENT_END, onEnd);
        req->on(http::ServerRequest::EVENT_CLOSE, onClose);
        res->on(http::ServerResponse::EVENT_CLOSE, onClose);
        return Status::OK;
    }

 private:
    http::Server::Ptr srv_;
};

TEST(GTestHttpEcho, TestConnectionKeepAlive) {
    GTestOnEnd::clear();
    GTestOnClose::clear();
    GTestHttpServerOnEnd::clear();

    String::CPtr msg = String::create("abc");

    http::Server::Ptr srv = http::Server::create();
    GTestHttpServerOnRequest::Ptr onRequest(new GTestHttpServerOnRequest(srv));
    srv->on(http::Server::EVENT_REQUEST, onRequest);
    srv->listen(10000);

    String::CPtr url = String::create("http://127.0.0.1:10000/xyz");
    JsObject::Ptr options = url::parse(url);
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
    Size numMsgs = messages->length();
    ASSERT_EQ(NUM_REQS, numMsgs);
    for (Size i = 0; i < numMsgs; i++) {
        console::printf(console::INFO, ".");
        ASSERT_TRUE(messages->get(i).equals(msg));
    }
    console::printf(console::INFO, "\n");
}

TEST(GTestHttpEcho, TestConnectionClose) {
    GTestOnEnd::clear();
    GTestOnClose::clear();
    GTestHttpServerOnEnd::clear();

    String::CPtr msg = String::create("xyz");

    http::Server::Ptr srv = http::Server::create();
    GTestHttpServerOnRequest::Ptr onRequest(new GTestHttpServerOnRequest(srv));
    srv->on(http::Server::EVENT_REQUEST, onRequest);
    srv->listen(10000);

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
    for (Size i = 0; i < NUM_REQS; i++) {
        http::ClientRequest::Ptr req = http::request(options, onResponse);
        req->write(msg);
        req->end();
    }

    node::run();

    ASSERT_EQ(0, GTestOnClose::count());

    JsArray::CPtr messages = GTestOnEnd::messages();
    Size numMsgs = messages->length();
    ASSERT_EQ(NUM_REQS, numMsgs);
    for (Size i = 0; i < numMsgs; i++) {
        console::printf(console::INFO, ".");
        ASSERT_TRUE(messages->get(i).equals(msg));
    }
    console::printf(console::INFO, "\n");
}

}  // namespace node
}  // namespace libj
