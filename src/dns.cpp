// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/detail/dns.h>

namespace libj {
namespace node {
namespace dns {

Boolean lookup(String::CPtr domain, JsFunction::Ptr callback) {
    return node::detail::dns::lookup(domain, callback);
}

Boolean lookup(String::CPtr domain, Int family, JsFunction::Ptr callback) {
    return node::detail::dns::lookup(domain, family, callback);
}

}  // namespace dns
}  // namespace node
}  // namespace libj
