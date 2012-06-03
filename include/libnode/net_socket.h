// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_NET_SOCKET_H_
#define LIBNODE_NET_SOCKET_H_

#include "libnode/event_emitter.h"

namespace libj {
namespace node {
namespace net {

class Socket : LIBNODE_EVENT_EMITTER(Socket)
 public:
    virtual String::CPtr remoteAddress() = 0;
};

}  // namespace net
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_NET_SOCKET_H_
