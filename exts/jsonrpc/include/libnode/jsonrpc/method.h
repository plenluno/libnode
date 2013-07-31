// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_JSONRPC_METHOD_H_
#define LIBNODE_JSONRPC_METHOD_H_

#include <libj/js_array.h>
#include <libj/js_object.h>
#include <libj/js_function.h>

namespace libj {
namespace node {
namespace jsonrpc {

class Method : LIBJ_JS_OBJECT(Method)
 public:
    static Ptr create(
        String::CPtr name,
        JsArray::CPtr params,
        JsFunction::Ptr func);

    virtual String::CPtr name() const = 0;

    virtual JsArray::CPtr parameters() const = 0;

    virtual JsFunction::Ptr function() const = 0;
};

}  // namespace jsonrpc
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_JSONRPC_METHOD_H_
