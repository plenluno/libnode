// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_JSONRPC_DETAIL_METHOD_H_
#define LIBNODE_JSONRPC_DETAIL_METHOD_H_

#include <libnode/jsonrpc/method.h>

#include <libj/detail/js_object.h>

namespace libj {
namespace node {
namespace jsonrpc {
namespace detail {

class Method : public libj::detail::JsObject<jsonrpc::Method> {
 public:
    Method(
        String::CPtr name,
        JsArray::CPtr params,
        JsFunction::Ptr func)
    : name_(name)
    , params_(params)
    , func_(func) {}

    virtual String::CPtr name() const {
        return name_;
    }

    virtual JsArray::CPtr parameters() const {
        return params_;
    }

    virtual JsFunction::Ptr function() const {
        return func_;
    }

 private:
    String::CPtr name_;
    JsArray::CPtr params_;
    JsFunction::Ptr func_;
};

}  // namespace detail
}  // namespace jsonrpc
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_JSONRPC_DETAIL_METHOD_H_
