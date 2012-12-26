// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_BRIDGE_HTTP_ABSTRACT_CLIENT_REQUEST_H_
#define LIBNODE_BRIDGE_HTTP_ABSTRACT_CLIENT_REQUEST_H_

#include <libnode/http/client_request.h>
#include <libnode/bridge/stream/abstract_writable_stream.h>

namespace libj {
namespace node {
namespace bridge {
namespace http {

template<typename I, typename B>
class AbstractClientRequest : public stream::AbstractWritableStream<I> {
 public:
    AbstractClientRequest(typename B::Ptr request)
        : stream::AbstractWritableStream<I>(request)
        , request_(request) {}

    virtual void abort() {
        request_->abort();
    }

    virtual void setTimeout(
        UInt timeout,
        JsFunction::Ptr callback = JsFunction::null()) {
        request_->setTimeout(timeout, callback);
    }

    virtual void setNoDelay(Boolean noDelay = true) {
        request_->setNoDelay(noDelay);
    }

    virtual void setSocketKeepAlive(
        Boolean enable = false, UInt initialDelay = 0) {
        request_->setSocketKeepAlive(enable, initialDelay);
    }

 private:
    typename B::Ptr request_;
};

}  // namespace http
}  // namespace bridge
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_BRIDGE_HTTP_ABSTRACT_CLIENT_REQUEST_H_
