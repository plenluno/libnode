// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_RESPONSE_H_
#define LIBNODE_HTTP_SERVER_RESPONSE_H_

#include <libnode/stream/writable.h>

namespace libj {
namespace node {
namespace http {

class ServerResponse : LIBNODE_STREAM_WRITABLE(ServerResponse)
 public:
    virtual void writeContinue() = 0;

    virtual void writeHead(
        Int statusCode,
        String::CPtr reasonPhrase = String::null(),
        JsObject::CPtr headers = JsObject::null()) = 0;

    virtual Int statusCode() const = 0;

    virtual String::CPtr getHeader(String::CPtr name) const = 0;

    virtual void setHeader(String::CPtr name, String::CPtr value) = 0;

    virtual void removeHeader(String::CPtr name) = 0;

    virtual void addTrailers(JsObject::CPtr headers) = 0;

    virtual Boolean headersSent() const = 0;

    virtual Boolean sendDate() const = 0;

    virtual void setSendDate(Boolean send) = 0;
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_RESPONSE_H_
