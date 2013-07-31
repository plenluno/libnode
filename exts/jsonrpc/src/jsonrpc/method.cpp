// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/jsonrpc/method.h>
#include <libnode/jsonrpc/detail/method.h>

namespace libj {
namespace node {
namespace jsonrpc {

static Boolean checkParams(JsArray::CPtr params) {
    if (!params) return false;

    for (Size i = 0; i < params->length(); i++) {
        if (!params->getCPtr<String>(i)) return false;
    }
    return true;
}

Method::Ptr Method::create(
    String::CPtr name,
    JsArray::CPtr params,
    JsFunction::Ptr func) {
    if (name && checkParams(params) && func) {
        return Method::Ptr(new jsonrpc::detail::Method(name, params, func));
    } else {
        return Method::null();
    }
}

}  // namespace jsonrpc
}  // namespace node
}  // namespace libj
