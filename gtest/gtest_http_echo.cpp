// Copyright (c) 2012 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <libnode/http.h>
#include <libnode/node.h>
#include <libnode/url.h>

#include <assert.h>
#include <libj/console.h>
#include <libj/json.h>
#include <libj/string_buffer.h>

namespace libj {
namespace node {

class GTestHttpOnData : LIBJ_JS_FUNCTION(GTestHttpOnData)
 public:
    GTestHttpOnData() : body_(StringBuffer::create()) {}

    String::CPtr getBody() const { return body_->toString(); }

    Value operator()(JsArray::Ptr args) {
        String::CPtr chunk = toCPtr<String>(args->get(0));
        if (chunk) {
            body_->append(chunk);
        }
        return Status::OK;
    }

 private:
    StringBuffer::Ptr body_;
};

class GTestHttpOnClose :  LIBJ_JS_FUNCTION(GTestHttpOnClose)
 public:
    Value operator()(JsArray::Ptr args) {
        assert(false);
        return Status::OK;
    }
};

class GTestHttpServerOnEnd : LIBJ_JS_FUNCTION(GTestHttpServerOnEnd)
 public:
    GTestHttpServerOnEnd(
        http::Server::Ptr srv,
        http::ServerRequest::Ptr req,
        http::ServerResponse::Ptr res,
        GTestHttpOnData::Ptr onData)
        : srv_(srv)
        , req_(req)
        , res_(res)
        , onData_(onData) {}

    Value operator()(JsArray::Ptr args) {
        String::CPtr body = onData_->getBody();

        res_->setHeader(
            http::HEADER_CONTENT_TYPE,
            String::create("text/plain"));
        res_->setHeader(
            http::HEADER_CONTENT_LENGTH,
            String::valueOf(Buffer::byteLength(body)));
        res_->write(body);
        res_->end();

        req_->removeAllListeners();
        srv_->close();
        return Status::OK;
    }

 private:
    http::Server::Ptr srv_;
    http::ServerRequest::Ptr req_;
    http::ServerResponse::Ptr res_;
    GTestHttpOnData::Ptr onData_;
};

class GTestHttpServerOnRequest : LIBJ_JS_FUNCTION(GTestHttpServerOnRequest)
 public:
    GTestHttpServerOnRequest(
        http::Server::Ptr srv) : srv_(srv) {}

    Value operator()(JsArray::Ptr args) {
        http::ServerRequest::Ptr req =
            toPtr<http::ServerRequest>(args->get(0));
        http::ServerResponse::Ptr res =
            toPtr<http::ServerResponse>(args->get(1));

        GTestHttpOnData::Ptr onData(new GTestHttpOnData());
        GTestHttpOnClose::Ptr onClose(new GTestHttpOnClose());
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

class GTestHttpClientOnEnd : LIBJ_JS_FUNCTION(GTestHttpClientOnEnd)
 public:
    GTestHttpClientOnEnd(GTestHttpOnData::Ptr onData)
        : onData_(onData) {}

    Value operator()(JsArray::Ptr args) {
        msg_ = onData_->getBody();
        return Status::OK;
    }

    static String::CPtr getMessage() { return msg_; }

 private:
    static String::CPtr msg_;

    GTestHttpOnData::Ptr onData_;
};

String::CPtr GTestHttpClientOnEnd::msg_ = String::null();

class GTestHttpClientOnResponse : LIBJ_JS_FUNCTION(GTestHttpClientOnResponse)
 public:
    Value operator()(JsArray::Ptr args) {
        http::ClientResponse::Ptr res =
            toPtr<http::ClientResponse>(args->get(0));

        GTestHttpOnData::Ptr onData(new GTestHttpOnData());
        GTestHttpOnClose::Ptr onClose(new GTestHttpOnClose());
        GTestHttpClientOnEnd::Ptr onEnd(new GTestHttpClientOnEnd(onData));
        res->setEncoding(Buffer::UTF8);
        res->on(http::ClientResponse::EVENT_DATA, onData);
        res->on(http::ClientResponse::EVENT_END, onEnd);
        res->on(http::ClientResponse::EVENT_CLOSE, onClose);
        return Status::OK;
    }
};

TEST(GTestHttpEcho, TestEcho) {
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
    options->put(String::create("headers"), headers);

    GTestHttpClientOnResponse::Ptr onResponse(new GTestHttpClientOnResponse());
    http::ClientRequest::Ptr req = http::request(options, onResponse);
    req->write(msg);
    req->end();

    node::run();
    ASSERT_TRUE(GTestHttpClientOnEnd::getMessage()->equals(msg));
}

}  // namespace node
}  // namespace libj
