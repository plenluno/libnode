// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_NET_SOCKET_H_
#define LIBNODE_NET_SOCKET_H_

#include "libnode/stream/duplex_stream.h"

namespace libj {
namespace node {
namespace net {

class Socket : LIBNODE_DUPLEX_STREAM(Socket)
 public:
    static Symbol::CPtr EVENT_CONNECT;
    static Symbol::CPtr EVENT_TIMEOUT;

    static Ptr create(JsObject::CPtr options = JsObject::null());

    virtual JsObject::Ptr address() = 0;

    virtual String::CPtr remoteAddress() = 0;

    virtual Int remotePort() = 0;

    virtual Size bytesRead() const = 0;

    virtual Size bytesWritten() const = 0;

    virtual Boolean connect(
        Int port,
        String::CPtr host = String::null(),
        JsFunction::Ptr callback = JsFunction::null()) = 0;

    virtual Boolean connect(
        String::CPtr path,
        JsFunction::Ptr callback = JsFunction::null()) = 0;

    virtual Boolean setNoDelay(Boolean noDelay = true) = 0;

    virtual Boolean setKeepAlive(
        Boolean enable = false, UInt initialDelay = 0) = 0;

    virtual void setTimeout(
        UInt timeout,
        JsFunction::Ptr callback = JsFunction::null()) = 0;
};

#define LIBNODE_NET_SOCKET(T) \
    public libj::node::net::Socket { \
    LIBJ_MUTABLE_DEFS(T, libj::node::net::Socket)

#define LIBNODE_NET_SOCKET_IMPL(S) \
    LIBNODE_DUPLEX_STREAM_IMPL(S); \
public: \
    virtual JsObject::Ptr address() { \
        return S->address(); \
    } \
    virtual String::CPtr remoteAddress() { \
        return S->remoteAddress(); \
    } \
    virtual Int remotePort() { \
        return S->remotePort(); \
    } \
    virtual Boolean setNoDelay(Boolean noDelay = true) { \
        return S->setNoDelay(noDelay); \
    } \
    virtual Boolean setKeepAlive( \
        Boolean enable = false, UInt initialDelay = 0) { \
        return S->setKeepAlive(enable, initialDelay); \
    } \
    virtual void setTimeout( \
        UInt timeout, \
        JsFunction::Ptr callback = JsFunction::null()) { \
        S->setTimeout(timeout, callback); \
    }

}  // namespace net
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_NET_SOCKET_H_
