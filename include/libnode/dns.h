// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DNS_H_
#define LIBNODE_DNS_H_

#include <libj/js_function.h>

namespace libj {
namespace node {
namespace dns {

Boolean lookup(String::CPtr domain, JsFunction::Ptr callback);

Boolean lookup(String::CPtr domain, Int family, JsFunction::Ptr callback);

}  // namespace dns
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DNS_H_
