// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_JSONRPC_RESPONSE_H_
#define LIBNODE_JSONRPC_RESPONSE_H_

#include <libj/error.h>
#include <libj/js_function.h>

namespace libj {
namespace node {
namespace jsonrpc {

class Response : LIBJ_JS_FUNCTION(Response)
 public:
    virtual Value operator()(JsArray::Ptr args);

    virtual Boolean result(const Value& res) = 0;

    virtual Boolean error(libj::Error::CPtr err) = 0;
};

}  // namespace jsonrpc
}  // namespace node
}  // namespace libj

#include <libnode/jsonrpc/impl/response.h>

#define LIBNODE_JSONRPC_RESPONSE(T) \
    public libj::node::jsonrpc::Response { \
    LIBNODE_JSONRPC_RESPONSE_DEFS(T)

#endif  // LIBNODE_JSONRPC_RESPONSE_H_
