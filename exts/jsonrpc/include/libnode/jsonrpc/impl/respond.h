// Copyright (c) 2013-2014 Plenluno All rights reserved.

#ifndef LIBNODE_JSONRPC_IMPL_RESPOND_H_
#define LIBNODE_JSONRPC_IMPL_RESPOND_H_

namespace libj {
namespace node {
namespace jsonrpc {

inline Value Respond::operator()(JsArray::Ptr args) {
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

#define LIBNODE_JSONRPC_RESPOND_INSTANCEOF(ID) \
    (ID == libj::Type<libj::node::jsonrpc::Respond>::id() \
        || LIBJ_JS_FUNCTION_INSTANCEOF(ID))

#define LIBNODE_JSONRPC_RESPOND_DEFS(T) \
    LIBJ_MUTABLE_DEFS(T, LIBNODE_JSONRPC_RESPOND) \
    LIBJ_JS_FUNCTION_TO_STRING(T)

#endif  // LIBNODE_JSONRPC_IMPL_RESPOND_H_
