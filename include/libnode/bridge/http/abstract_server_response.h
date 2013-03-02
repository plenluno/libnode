// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_BRIDGE_HTTP_ABSTRACT_SERVER_RESPONSE_H_
#define LIBNODE_BRIDGE_HTTP_ABSTRACT_SERVER_RESPONSE_H_

#include <libnode/http/server_response.h>
#include <libnode/bridge/stream/abstract_writable_stream.h>

namespace libj {
namespace node {
namespace bridge {
namespace http {

template<typename I, typename B>
class AbstractServerResponse : public stream::AbstractWritableStream<I> {
 public:
    AbstractServerResponse(typename B::Ptr response)
        : stream::AbstractWritableStream<I>(response)
        , response_(response) {}

    virtual void writeContinue() {
        response_->writeContinue();
    }

    virtual void writeHead(
        Int statusCode,
        String::CPtr reasonPhrase = String::null(),
        JsObject::CPtr headers = JsObject::null()) {
        response_->writeHead(statusCode, reasonPhrase, headers);
    }

    virtual Int statusCode() const {
        return response_->statusCode();
    }

    virtual String::CPtr getHeader(String::CPtr name) const {
        return response_->getHeader(name);
    }

    virtual void setHeader(String::CPtr name, String::CPtr value) {
        response_->setHeader(name, value);
    }

    virtual void removeHeader(String::CPtr name) {
        response_->removeHeader(name);
    }

    virtual void addTrailers(JsObject::CPtr headers) {
        response_->addTrailers(headers);
    }

 private:
    typename B::Ptr response_;
};

}  // namespace http
}  // namespace bridge
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_BRIDGE_HTTP_ABSTRACT_SERVER_RESPONSE_H_
