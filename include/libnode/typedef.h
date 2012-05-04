// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_TYPEDEF_H_
#define LIBNODE_TYPEDEF_H_

#include "libj/array_list.h"
#include "libj/function.h"
#include "libj/map.h"
#include "libj/singleton.h"

namespace libj {
namespace node {

typedef ArrayList JsArray;
typedef Function JsFunction;
typedef Map JsObject;

#define LIBNODE_MODULE(T) \
    LIBJ_SINGLETON(T)

#define LIBNODE_JS_ARRAY(T) public libj::node::JsArray { \
    LIBJ_MUTABLE_DECLS(T, libj::node::JsArray)

#define LIBNODE_JS_FUNCTION(T) public libj::node::JsFunction { \
    LIBJ_MUTABLE_DECLS(T, libj::node::JsFunction)

#define LIBNODE_JS_OBJECT(T) public libj::node::JsObject { \
    LIBJ_MUTABLE_DECLS(T, libj::node::JsObject)

#define LIBNODE_JS_OBJECT_IMPL(JO) \
    LIBJ_MAP_IMPL(JO)

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_TYPEDEF_H_