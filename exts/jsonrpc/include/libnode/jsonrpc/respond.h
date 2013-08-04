// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_JSONRPC_RESPOND_H_
#define LIBNODE_JSONRPC_RESPOND_H_

#include <libj/error.h>
#include <libj/js_function.h>

namespace libj {
namespace node {
namespace jsonrpc {

class Respond : LIBJ_JS_FUNCTION(Respond)
 public:
    virtual Value operator()(JsArray::Ptr args);

    virtual Boolean result(const Value& res) = 0;

    virtual Boolean error(libj::Error::CPtr err) = 0;
};

}  // namespace jsonrpc
}  // namespace node
}  // namespace libj

#include <libnode/jsonrpc/impl/respond.h>

#define LIBNODE_JSONRPC_RESPOND(T) \
    public libj::node::jsonrpc::Respond { \
    LIBNODE_JSONRPC_RESPOND_DEFS(T)

#endif  // LIBNODE_JSONRPC_RESPOND_H_
