// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_QUERYSTRING_H_
#define LIBNODE_QUERYSTRING_H_

#include <libj/js_object.h>
#include <libj/string.h>

namespace libj {
namespace node {
namespace querystring {

JsObject::Ptr parse(String::CPtr str, Char sep = '&', Char eq = '=');

String::CPtr stringify(JsObject::CPtr obj, Char sep = '&', Char eq = '=');

}  // namespace querystring
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_QUERYSTRING_H_
