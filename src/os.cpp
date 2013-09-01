// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/os.h>
#include <libnode/detail/os.h>

namespace libj {
namespace node {
namespace os {

JsObject::Ptr networkInterface() {
    return node::detail::os::networkInterface();
}

}  // namespace os
}  // namespace node
}  // namespace libj
