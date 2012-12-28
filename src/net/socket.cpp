// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/detail/net/socket.h>

namespace libj {
namespace node {
namespace net {

LIBJ_SYMBOL_DEF(Socket::EVENT_CONNECT,          "connect");
LIBJ_SYMBOL_DEF(Socket::EVENT_TIMEOUT,          "timeout");
LIBJ_SYMBOL_DEF(Socket::OPTION_FD,              "fd");
LIBJ_SYMBOL_DEF(Socket::OPTION_TYPE,            "type");
LIBJ_SYMBOL_DEF(Socket::OPTION_ALLOW_HALF_OPEN, "allowHalfOpen");

Socket::Ptr Socket::create(JsObject::CPtr options) {
    return detail::net::Socket::create(options);
}

}  // namespace net
}  // namespace node
}  // namespace libj
