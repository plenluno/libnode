// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_BRIDGE_HTTP_ABSTRACT_SERVER_REQUEST_H_
#define LIBNODE_BRIDGE_HTTP_ABSTRACT_SERVER_REQUEST_H_

#include <libnode/http/server_request.h>
#include <libnode/bridge/stream/abstract_readable_stream.h>

namespace libj {
namespace node {
namespace bridge {
namespace http {

template<typename I, typename B>
class AbstractServerRequest : public stream::AbstractReadableStream<I> {
 public:
    AbstractServerRequest(typename B::Ptr request)
        : stream::AbstractReadableStream<I>(request)
        , request_(request) {}

    String::CPtr method() const {
        return request_->method();
    }

    String::CPtr url() const {
        return request_->url();
    }

    JsObject::CPtr headers() const {
        return request_->headers();
    }

    JsObject::CPtr trailers() const {
        return request_->trailers();
    }

    String::CPtr httpVersion() const {
        return request_->httpVersion();
    }

    net::Socket::Ptr connection() const {
        return request_->connection();
    }

 private:
    typename B::Ptr request_;
};

}  // namespace http
}  // namespace bridge
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_BRIDGE_HTTP_ABSTRACT_SERVER_REQUEST_H_
