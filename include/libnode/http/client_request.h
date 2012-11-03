// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_CLIENT_REQUEST_H_
#define LIBNODE_HTTP_CLIENT_REQUEST_H_

#include "libnode/stream/writable_stream.h"

namespace libj {
namespace node {
namespace http {

class ClientRequest : LIBNODE_WRITABLE_STREAM(ClientRequest)
 public:
    static const String::CPtr EVENT_RESPONSE;
    static const String::CPtr EVENT_SOCKET;
    static const String::CPtr EVENT_CONNECT;
    static const String::CPtr EVENT_UPGRADE;
    static const String::CPtr EVENT_CONTINUE;

    virtual void abort() = 0;
    virtual Boolean setNoDelay(Boolean noDelay = true) = 0;
    virtual Boolean setSocketKeepAlive(
        Boolean enable = false, Int initialDelay = 0) = 0;
    virtual Boolean setTimeout(
        Int timeout,
        JsFunction::Ptr callback = JsFunction::null()) = 0;
};

#define LIBNODE_HTTP_CLIENT_REQUEST(T) \
    public libj::node::http::ClientRequest { \
    LIBJ_MUTABLE_DEFS(T, libj::node::http::ClientRequest)

#define LIBNODE_HTTP_CLIENT_REQUEST_IMPL(CR) \
    LIBNODE_WRITABLE_STREAM_IMPL(CR); \
public: \
    virtual Boolean setNoDelay(Boolean noDelay = true) { \
        return CR->setNoDelay(noDelay); \
    } \
    virtual Boolean setSocketKeepAlive( \
        Boolean enable = false, Int initialDelay = 0) { \
        return CR->setSocketKeepAlive(enable, initialDelay); \
    } \
    virtual Boolean setTimeout( \
        Int timeout, \
        JsFunction::Ptr callback = JsFunction::null()) { \
        return CR->setTimeout(timeout, callback); \
    }

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_CLIENT_REQUEST_H_
