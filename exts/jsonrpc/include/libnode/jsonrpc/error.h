// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_JSONRPC_ERROR_H_
#define LIBNODE_JSONRPC_ERROR_H_

#include <libj/error.h>

namespace libj {
namespace node {
namespace jsonrpc {

#define LIBNODE_JSONRPC_ERR_MAP(GEN) \
    GEN(PARSE_ERROR,      -32700, "Parse error") \
    GEN(INVALID_REQUEST,  -32600, "Invalid Request") \
    GEN(METHOD_NOT_FOUND, -32601, "Method not found") \
    GEN(INVALID_PARAMS,   -32602, "Invalid params") \
    GEN(INTERNAL_ERROR,   -32603, "Internal error")

#define LIBNODE_JSONRPC_ERR_CODE_GEN(NAME, CODE, MSG) NAME = CODE,

class Error : LIBJ_ERROR(Error)
 public:
    enum Code {
        LIBNODE_JSONRPC_ERR_MAP(LIBNODE_JSONRPC_ERR_CODE_GEN)
    };

    static CPtr create(Code code);
};

}  // namespace jsonrpc
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_JSONRPC_ERROR_H_
