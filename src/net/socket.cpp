// Copyright (c) 2012 Plenluno All rights reserved.

#include "./socket_impl.h"

namespace libj {
namespace node {
namespace net {

LIBJ_SYMBOL_DEF(Socket::EVENT_CONNECT, "connect");
LIBJ_SYMBOL_DEF(Socket::EVENT_TIMEOUT, "timeout");

Socket::Ptr Socket::create(JsObject::CPtr options) {
    return SocketImpl::create(options);
}

}  // namespace net
}  // namespace node
}  // namespace libj
