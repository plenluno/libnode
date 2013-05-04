// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_GTEST_GTEST_HTTP_COMMON_H_
#define LIBNODE_GTEST_GTEST_HTTP_COMMON_H_

#include <libnode/http.h>

#include "./gtest_common.h"

namespace libj {
namespace node {

class GTestHttpServerOnEnd : LIBJ_JS_FUNCTION(GTestHttpServerOnEnd)
 public:
    GTestHttpServerOnEnd(
        http::Server::Ptr srv,
        http::ServerResponse::Ptr res,
        GTestOnData::Ptr onData,
        UInt numReqs,
        Boolean chunked = false)
        : srv_(srv)
        , res_(res)
        , onData_(onData)
        , numReqs_(numReqs)
        , chunked_(chunked) {}

    static void clear() {
        count_ = 0;
    }

    virtual Value operator()(JsArray::Ptr args) {
        String::CPtr body = onData_->string();

        res_->setHeader(
            http::HEADER_CONTENT_TYPE,
            String::create("text/plain"));
        if (!chunked_) {
            res_->setHeader(
                http::HEADER_CONTENT_LENGTH,
                String::valueOf(Buffer::byteLength(body)));
        }
        res_->write(body);
        res_->end();

        count_++;
        if (count_ >= numReqs_) srv_->close();
        return Status::OK;
    }

 private:
    static UInt count_;

    http::Server::Ptr srv_;
    http::ServerResponse::Ptr res_;
    GTestOnData::Ptr onData_;
    UInt numReqs_;
    Boolean chunked_;
};

class GTestHttpServerOnRequest : LIBJ_JS_FUNCTION(GTestHttpServerOnRequest)
 public:
    GTestHttpServerOnRequest(
        http::Server::Ptr srv,
        UInt numReqs,
        Boolean chunked = false)
        : srv_(srv)
        , numReqs_(numReqs)
        , chunked_(chunked) {}

    virtual Value operator()(JsArray::Ptr args) {
        http::ServerRequest::Ptr req =
            toPtr<http::ServerRequest>(args->get(0));
        http::ServerResponse::Ptr res =
            toPtr<http::ServerResponse>(args->get(1));

        req->setEncoding(Buffer::UTF8);
        res->setSendDate(false);

        GTestOnData::Ptr onData(new GTestOnData());
        GTestOnClose::Ptr onClose(new GTestOnClose());
        GTestHttpServerOnEnd::Ptr onEnd(
            new GTestHttpServerOnEnd(
                srv_, res, onData, numReqs_, chunked_));
        req->on(http::ServerRequest::EVENT_DATA, onData);
        req->on(http::ServerRequest::EVENT_END, onEnd);
        req->on(http::ServerRequest::EVENT_CLOSE, onClose);
        res->on(http::ServerResponse::EVENT_CLOSE, onClose);
        return Status::OK;
    }

 private:
    http::Server::Ptr srv_;
    UInt numReqs_;
    Boolean chunked_;
};

class GTestHttpClientOnResponse : LIBJ_JS_FUNCTION(GTestHttpClientOnResponse)
 public:
    static JsArray::Ptr statusCodes() {
        if (!statusCodes_) {
            statusCodes_ = JsArray::create();
            LIBJ_DEBUG_PRINT(
                "static: JsArray %p",
                LIBJ_DEBUG_OBJECT_PTR(statusCodes_));
        }
        return statusCodes_;
    }

    static void clear() {
        statusCodes()->clear();
    }

    virtual Value operator()(JsArray::Ptr args) {
        http::ClientResponse::Ptr res =
            toPtr<http::ClientResponse>(args->get(0));

        statusCodes_->add(res->statusCode());

        GTestOnData::Ptr onData(new GTestOnData());
        GTestOnClose::Ptr onClose(new GTestOnClose());
        GTestOnEnd::Ptr onEnd(new GTestOnEnd(onData));
        res->setEncoding(Buffer::UTF8);
        res->on(http::ClientResponse::EVENT_DATA, onData);
        res->on(http::ClientResponse::EVENT_END, onEnd);
        res->on(http::ClientResponse::EVENT_CLOSE, onClose);
        return Status::OK;
    }

 private:
    static JsArray::Ptr statusCodes_;
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_GTEST_GTEST_HTTP_COMMON_H_
