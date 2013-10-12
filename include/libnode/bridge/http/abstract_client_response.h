// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_BRIDGE_HTTP_ABSTRACT_CLIENT_RESPONSE_H_
#define LIBNODE_BRIDGE_HTTP_ABSTRACT_CLIENT_RESPONSE_H_

#include <libnode/http/client_response.h>
#include <libnode/bridge/stream/abstract_readable.h>

namespace libj {
namespace node {
namespace bridge {
namespace http {

template<typename I, typename B>
class AbstractClientResponse : public stream::AbstractReadable<I> {
 public:
    AbstractClientResponse(typename B::Ptr response)
        : stream::AbstractReadable<I>(response)
        , response_(response) {}

    virtual Int statusCode() const {
        return response_->statusCode();
    }

    virtual String::CPtr httpVersion() const {
        return response_->httpVersion();
    }

    virtual JsObject::CPtr headers() const {
        return response_->headers();
    }

    virtual JsObject::CPtr trailers() const {
        return response_->trailers();
    }

 private:
    typename B::Ptr response_;
};

}  // namespace http
}  // namespace bridge
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_BRIDGE_HTTP_ABSTRACT_CLIENT_RESPONSE_H_
