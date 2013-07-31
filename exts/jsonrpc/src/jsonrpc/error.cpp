// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/jsonrpc/error.h>

#include <libj/detail/status.h>

namespace libj {
namespace node {
namespace jsonrpc {

#define LIBNODE_JSONRPC_ERR_MSG_DEF_GEN(NAME, CODE, MSG) \
    LIBJ_STATIC_CONST_STRING_DEF(MSG_JSONRPC_##NAME, MSG);

#define LIBNODE_JSONRPC_ERR_DEF_GEN(NAME, CODE, MSG) \
    static const Error::CPtr ERR_JSONRPC_##NAME( \
        new libj::detail::Status<Error>( \
            CODE, \
            MSG_JSONRPC_##NAME));

#define LIBNODE_JSONRPC_ERR_CASE_GEN(NAME, CODE, MSG) \
    case NAME: \
        return ERR_JSONRPC_##NAME;

LIBNODE_JSONRPC_ERR_MAP(LIBNODE_JSONRPC_ERR_MSG_DEF_GEN);

Error::CPtr Error::create(Error::Code code) {
    LIBNODE_JSONRPC_ERR_MAP(LIBNODE_JSONRPC_ERR_DEF_GEN);
    switch (code) {
        LIBNODE_JSONRPC_ERR_MAP(LIBNODE_JSONRPC_ERR_CASE_GEN);
    default:
        assert(false);
        return null();
    }
}

}  // namespace jsonrpc
}  // namespace node
}  // namespace libj
