// Copyright (c) 2012 Plenluno All rights reserved.

#include "./socket_impl.h"

namespace libj {
namespace node {
namespace net {

const String::CPtr Socket::EVENT_CONNECT = String::intern("connect");
const String::CPtr Socket::EVENT_TIMEOUT = String::intern("timeout");

Socket::Ptr Socket::create() {
    return SocketImpl::create();
}

}  // namespace net
}  // namespace node
}  // namespace libj
