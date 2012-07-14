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

#if 0
    virtual void connect(Int port) = 0;
    virtual void connect(String::CPtr path) = 0;
#endif
    virtual JsObject::Ptr address() = 0;
    virtual String::CPtr remoteAddress() = 0;
    virtual Int remotePort() = 0;
    virtual Boolean setNoDelay(Boolean noDelay = true) = 0;
    virtual Boolean setKeepAlive(
        Boolean enable = false, Int initialDelay = 0) = 0;
};

}  // namespace net
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_NET_SOCKET_H_
