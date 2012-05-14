// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_JS_FUNCTION_H_
#define LIBNODE_JS_FUNCTION_H_

#include "libj/function.h"

namespace libj {
namespace node {

typedef Function JsFunction;

#define LIBNODE_JS_FUNCTION(T) public libj::node::JsFunction { \
    LIBJ_MUTABLE_DECLS(T, libj::node::JsFunction)

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_JS_FUNCTION_H_