// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_JSONRPC_IMPL_RESPONSE_H_
#define LIBNODE_JSONRPC_IMPL_RESPONSE_H_

namespace libj {
namespace node {
namespace jsonrpc {

inline Value Response::operator()(JsArray::Ptr args) {
    libj::Error::CPtr err = args->getCPtr<libj::Error>(0);
    if (err) {
        return error(err);
    } else {
        return result(args->get(1));
    }
}

}  // namespace jsonrpc
}  // namespace node
}  // namespace libj

#define LIBNODE_JSONRPC_RESPONSE_DEFS(T) \
    LIBJ_MUTABLE_DEFS(T, libj::node::jsonrpc::Response) \
    LIBJ_JS_FUNCTION_TO_STRING(T)

#endif  // LIBNODE_JSONRPC_IMPL_RESPONSE_H_
