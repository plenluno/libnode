// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_NET_SOCKET_H_
#define LIBNODE_NET_SOCKET_H_

#include "libnode/events/event_emitter.h"

namespace libj {
namespace node {
namespace net {

class Socket : LIBNODE_EVENT_EMITTER(Socket)
 public:
    static const String::CPtr EVENT_CONNECT;
    static const String::CPtr EVENT_DATA;
    static const String::CPtr EVENT_END;
    static const String::CPtr EVENT_TIMEOUT;
    static const String::CPtr EVENT_DRAIN;
    static const String::CPtr EVENT_ERROR;
    static const String::CPtr EVENT_CLOSE;

    static Ptr create();

#if 0
    virtual void connect(Int port) = 0;
    virtual void connect(String::CPtr path) = 0;
    virtual String::CPtr address() const = 0;
#endif
    virtual String::CPtr remoteAddress() = 0;
#if 0
    virtual Int remotePort() const = 0;
    virtual Int bytesRead() const = 0;
    virtual Int bytesWritten() const = 0;
    virtual void write(Object::CPtr chunk) = 0;
    virtual void end() = 0;
#endif
};

}  // namespace net
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_NET_SOCKET_H_
