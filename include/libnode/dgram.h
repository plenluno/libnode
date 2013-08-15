// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DGRAM_H_
#define LIBNODE_DGRAM_H_

#include <libnode/dgram/socket.h>

namespace libj {
namespace node {
namespace dgram {

Socket::Ptr createSocket(
    Socket::Type type,
    JsFunction::Ptr callback = JsFunction::null());

}  // namespace dgram
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DGRAM_H_
