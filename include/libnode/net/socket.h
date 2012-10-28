// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_NET_SOCKET_H_
#define LIBNODE_NET_SOCKET_H_

#include "libnode/stream/duplex_stream.h"

namespace libj {
namespace node {
namespace net {

class Socket : LIBNODE_DUPLEX_STREAM(Socket)
 public:
    static const String::CPtr EVENT_CONNECT;
    static const String::CPtr EVENT_TIMEOUT;

    static Ptr create();

    // TODO(plenluno): implement
    // virtual void connect(Int port) = 0;
    // virtual void connect(String::CPtr path) = 0;

    virtual JsObject::Ptr address() = 0;
    virtual String::CPtr remoteAddress() = 0;
    virtual Int remotePort() = 0;
    virtual Boolean setNoDelay(Boolean noDelay = true) = 0;
    virtual Boolean setKeepAlive(
        Boolean enable = false, Int initialDelay = 0) = 0;
    virtual Boolean setTimeout(
        Int timeout,
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
        Boolean enable = false, Int initialDelay = 0) { \
        return S->setKeepAlive(enable, initialDelay); \
    } \
    virtual Boolean setTimeout( \
        Int timeout, \
        JsFunction::Ptr callback = JsFunction::null()) { \
        return S->setTimeout(timeout, callback); \
    }

}  // namespace net
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_NET_SOCKET_H_
