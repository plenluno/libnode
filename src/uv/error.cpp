// Copyright (c) 2012-2014 Plenluno All rights reserved.

#include <libnode/uv/error.h>

#include <libj/debug_print.h>
#include <libj/detail/status.h>

namespace libj {
namespace node {
namespace uv {

#define LIBNODE_UV_ERR_MSG_DEF_GEN(NAME, S) \
    LIBJ_STATIC_CONST_STRING_DEF(MSG_UV_##NAME, S);

#define LIBNODE_UV_ERR_CASE_GEN(NAME, S) \
    case _##NAME:

#define LIBNODE_UV_ERR_MSG_CASE_GEN(NAME, S) \
    case _##NAME: \
        msg = MSG_UV_##NAME; \
        break;

UV_ERRNO_MAP(LIBNODE_UV_ERR_MSG_DEF_GEN);

Error::CPtr Error::create(Error::Code code) {
    String::CPtr msg;
    switch (code) {
        UV_ERRNO_MAP(LIBNODE_UV_ERR_MSG_CASE_GEN)
    default:
        assert(false);
        return null();
    }
    return CPtr(new libj::detail::Status<Error>(code, msg));
}

Error::CPtr Error::create(Error::Code code, String::CPtr msg) {
    switch (code) {
        UV_ERRNO_MAP(LIBNODE_UV_ERR_CASE_GEN)
        break;
    default:
        assert(false);
        return null();
    }
    return CPtr(new libj::detail::Status<Error>(code, msg));
}

Error::CPtr Error::valueOf(uv_errno_t code) {
    return create(static_cast<Code>(code));
}

}  // namespace uv
}  // namespace node
}  // namespace libj
