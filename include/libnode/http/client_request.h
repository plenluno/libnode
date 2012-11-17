// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_CLIENT_REQUEST_H_
#define LIBNODE_HTTP_CLIENT_REQUEST_H_

#include "libnode/stream/writable_stream.h"

namespace libj {
namespace node {
namespace http {

class ClientRequest : LIBNODE_WRITABLE_STREAM(ClientRequest)
 public:
    static Symbol::CPtr EVENT_RESPONSE;
    static Symbol::CPtr EVENT_SOCKET;
    static Symbol::CPtr EVENT_CONNECT;
    static Symbol::CPtr EVENT_UPGRADE;
    static Symbol::CPtr EVENT_CONTINUE;

    virtual void abort() = 0;

    virtual void setNoDelay(Boolean noDelay = true) = 0;

    virtual void setSocketKeepAlive(
        Boolean enable = false, UInt initialDelay = 0) = 0;

    virtual void setTimeout(
        UInt timeout,
        JsFunction::Ptr callback = JsFunction::null()) = 0;
};

#define LIBNODE_HTTP_CLIENT_REQUEST(T) \
    public libj::node::http::ClientRequest { \
    LIBJ_MUTABLE_DEFS(T, libj::node::http::ClientRequest)

#define LIBNODE_HTTP_CLIENT_REQUEST_IMPL(CR) \
    LIBNODE_WRITABLE_STREAM_IMPL(CR); \
public: \
    virtual void abort() { \
        CR->abort(); \
    } \
    virtual void setNoDelay(Boolean noDelay = true) { \
        CR->setNoDelay(noDelay); \
    } \
    virtual void setSocketKeepAlive( \
        Boolean enable = false, UInt initialDelay = 0) { \
        CR->setSocketKeepAlive(enable, initialDelay); \
    } \
    virtual void setTimeout( \
        UInt timeout, \
        JsFunction::Ptr callback = JsFunction::null()) { \
        CR->setTimeout(timeout, callback); \
    }

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_CLIENT_REQUEST_H_
