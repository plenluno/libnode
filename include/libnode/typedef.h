// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_TYPEDEF_H_
#define LIBNODE_TYPEDEF_H_

#include "libj/array_list.h"
#include "libj/function.h"

namespace libj {
namespace node {

typedef ArrayList JsArray;
typedef Function JsFunction;

#define LIBNODE_JS_ARRAY(T) public libj::node::JsArray { \
    LIBJ_MUTABLE_DECLS(T, libj::node::JsArray)

#define LIBNODE_JS_FUNCTION(T) public libj::node::JsFunction { \
    LIBJ_MUTABLE_DECLS(T, libj::node::JsFunction)

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_TYPEDEF_H_