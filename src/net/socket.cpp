// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/net/socket.h"
#include "./socket_impl.h"

namespace libj {
namespace node {
namespace net {

const String::CPtr Socket::EVENT_CONNECT = String::create("connect");
const String::CPtr Socket::EVENT_DATA = String::create("data");
const String::CPtr Socket::EVENT_END = String::create("end");
const String::CPtr Socket::EVENT_TIMEOUT = String::create("timeout");
const String::CPtr Socket::EVENT_DRAIN = String::create("drain");
const String::CPtr Socket::EVENT_ERROR = String::create("error");
const String::CPtr Socket::EVENT_CLOSE = String::create("close");

Socket::Ptr Socket::create() {
    return SocketImpl::create();
}

}  // namespace net
}  // namespace node
}  // namespace libj
