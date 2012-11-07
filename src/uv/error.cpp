
// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/uv/error.h"

namespace libj {
namespace node {
namespace uv {

#define LIBNODE_UV_ERR_CASE_GEN(VAL, NAME, S) \
    case _##NAME:

#define LIBNODE_UV_ERR_MSG_DECL_GEN(VAL, NAME, S) \
    static const String::CPtr MSG_UV_##NAME;

#define LIBNODE_UV_ERR_MSG_CASE_GEN(VAL, NAME, S) \
    case _##NAME: \
        msg = MSG_UV_##NAME; \
        break;

class ErrorImpl : public Error {
 private:
    UV_ERRNO_MAP(LIBNODE_UV_ERR_MSG_DECL_GEN)

 private:
    libj::Status::CPtr status_;

    ErrorImpl(Int code, String::CPtr msg)
        : status_(libj::Status::create(code, msg)) {}

 public:
    static CPtr create(Int code) {
        String::CPtr msg;
        switch (code) {
            UV_ERRNO_MAP(LIBNODE_UV_ERR_MSG_CASE_GEN)
        default:
            assert(false);
            return null();
        }
        return CPtr(new ErrorImpl(code, msg));
    }

    static CPtr create(Int code, String::CPtr msg) {
        switch (code) {
            UV_ERRNO_MAP(LIBNODE_UV_ERR_CASE_GEN)
            break;
        default:
            assert(false);
            return null();
        }
        return CPtr(new ErrorImpl(code, msg));
    }

    LIBJ_STATUS_IMPL(status_);
};

#define LIBNODE_UV_ERR_MSG_DEF_GEN(VAL, NAME, S) \
    const String::CPtr ErrorImpl::MSG_UV_##NAME = String::create(S);

UV_ERRNO_MAP(LIBNODE_UV_ERR_MSG_DEF_GEN)

Error::CPtr Error::create(Error::Code code) {
    return ErrorImpl::create(code);
}

Error::CPtr Error::create(Error::Code code, String::CPtr msg) {
    return ErrorImpl::create(code, msg);
}

Error::CPtr Error::valueOf(uv_err_code code) {
    return ErrorImpl::create(_OK + code);
}

static Error::CPtr lastErr = Error::null();

Error::CPtr Error::last() {
    return lastErr;
}

void Error::setLast(uv_err_code code) {
    lastErr = valueOf(code);
}

}  // namespace uv
}  // namespace node
}  // namespace libj
