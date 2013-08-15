// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/dgram.h>
#include <libnode/detail/dgram/socket.h>

namespace libj {
namespace node {
namespace dgram {

Socket::Ptr createSocket(Socket::Type type, JsFunction::Ptr callback) {
    return detail::dgram::Socket::create(type, callback);
}

}  // namespace dgram
}  // namespace node
}  // namespace libj
