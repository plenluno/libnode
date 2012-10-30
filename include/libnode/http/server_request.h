// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_REQUEST_H_
#define LIBNODE_HTTP_SERVER_REQUEST_H_

#include "libnode/net/socket.h"
#include "libnode/stream/readable_stream.h"

namespace libj {
namespace node {
namespace http {

class ServerRequest : LIBNODE_READABLE_STREAM(ServerRequest)
 public:
    virtual String::CPtr method() const = 0;
    virtual String::CPtr url() const = 0;
    virtual JsObject::CPtr headers() const = 0;
    virtual String::CPtr httpVersion() const = 0;
    virtual net::Socket::Ptr connection() const = 0;
};

#define LIBNODE_HTTP_SERVER_REQUEST(T) \
    public libj::node::http::ServerRequest { \
    LIBJ_MUTABLE_DEFS(T, libj::node::http::ServerRequest)

#define LIBNODE_HTTP_SERVER_REQUEST_IMPL(SR) \
    LIBNODE_READABLE_STREAM_IMPL(SR); \
public: \
    String::CPtr method() const { \
        return SR->method(); \
    } \
    String::CPtr url() const { \
        return SR->url(); \
    } \
    JsObject::CPtr headers() const { \
        return SR->headers(); \
    } \
    String::CPtr httpVersion() const { \
        return SR->httpVersion(); \
    } \
    net::Socket::Ptr connection() const { \
        return SR->connection(); \
    }

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_REQUEST_H_
