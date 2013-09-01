// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/os.h>
#include <libnode/detail/os.h>

#include <libj/endian.h>

namespace libj {
namespace node {
namespace os {

String::CPtr endianness() {
    LIBJ_STATIC_SYMBOL_DEF(symBE, "BE");
    LIBJ_STATIC_SYMBOL_DEF(symLE, "LE");

    return endian() == BIG ? symBE : symLE;
}

JsObject::Ptr networkInterface() {
    return node::detail::os::networkInterface();
}

}  // namespace os
}  // namespace node
}  // namespace libj
