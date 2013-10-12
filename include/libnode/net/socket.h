// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_NET_SOCKET_H_
#define LIBNODE_NET_SOCKET_H_

#include <libnode/stream/duplex.h>

namespace libj {
namespace node {
namespace net {

class Socket : LIBNODE_STREAM_DUPLEX(Socket)
 public:
    static Symbol::CPtr EVENT_CONNECT;
    static Symbol::CPtr EVENT_TIMEOUT;

    static Symbol::CPtr OPTION_FD;
    static Symbol::CPtr OPTION_TYPE;
    static Symbol::CPtr OPTION_ALLOW_HALF_OPEN;

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

    virtual void setTimeout(
        UInt timeout,
        JsFunction::Ptr callback = JsFunction::null()) = 0;

    virtual Boolean setNoDelay(Boolean noDelay = true) = 0;

    virtual Boolean setKeepAlive(
        Boolean enable = false, UInt initialDelay = 0) = 0;
};

}  // namespace net
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_NET_SOCKET_H_
