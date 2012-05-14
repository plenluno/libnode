// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_JS_ARRAY_H_
#define LIBNODE_JS_ARRAY_H_

#include "libj/array_list.h"

namespace libj {
namespace node {

typedef ArrayList JsArray;

#define LIBNODE_JS_ARRAY(T) public libj::node::JsArray { \
    LIBJ_MUTABLE_DECLS(T, libj::node::JsArray)

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_JS_ARRAY_H_