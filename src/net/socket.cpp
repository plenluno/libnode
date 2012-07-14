// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/net/socket.h"
#include "./socket_impl.h"

namespace libj {
namespace node {
namespace net {

const String::CPtr Socket::EVENT_CONNECT = String::create("connect");
const String::CPtr Socket::EVENT_END = String::create("end");

Socket::Ptr Socket::create() {
    return SocketImpl::create();
}

}  // namespace net
}  // namespace node
}  // namespace libj
