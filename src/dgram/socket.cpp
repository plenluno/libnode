// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/detail/dgram/socket.h>

namespace libj {
namespace node {
namespace dgram {

LIBJ_SYMBOL_DEF(Socket::EVENT_MESSAGE,   "message");
LIBJ_SYMBOL_DEF(Socket::EVENT_LISTENING, "listenning");
LIBJ_SYMBOL_DEF(Socket::EVENT_CLOSE,     "close");
LIBJ_SYMBOL_DEF(Socket::EVENT_ERROR,     "error");

Socket::Ptr Socket::create(Socket::Type type, JsFunction::Ptr callback) {
    return detail::dgram::Socket::create(type, callback);
}

}  // namespace dgram
}  // namespace node
}  // namespace libj
