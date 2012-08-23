
// Copyright (c) 2012 Plenluno All rights reserved.

#include "libj/error.h"
#include "libnode/error.h"

namespace libj {
namespace node {

#define LIBNODE_UV_ERR_CASE_GEN(VAL, NAME, S) \
    case UV_ERR_##NAME:

#define LIBNODE_UV_ERR_MSG_DECL_GEN(VAL, NAME, S) \
    static const String::CPtr MSG_UV_ERR_##NAME;

#define LIBNODE_UV_ERR_MSG_CASE_GEN(VAL, NAME, S) \
    case UV_ERR_##NAME: \
        msg = MSG_UV_ERR_##NAME; \
        break;

class ErrorImpl : public Error {
 private:
    UV_ERRNO_MAP(LIBNODE_UV_ERR_MSG_DECL_GEN)

 private:
    libj::Status::CPtr status_;

    ErrorImpl(Int code, String::CPtr msg)
        : status_(libj::Status::create(code, msg)) {}

    ErrorImpl(libj::Error::CPtr err) : status_(err) {}

 public:
    static CPtr create(Int code) {
        String::CPtr msg;
        switch (code) {
            UV_ERRNO_MAP(LIBNODE_UV_ERR_MSG_CASE_GEN)
        default:
            return null();
        }
        CPtr p(new ErrorImpl(code, msg));
        return p;
    }

    static CPtr create(Int code, String::CPtr msg) {
        switch (code) {
            UV_ERRNO_MAP(LIBNODE_UV_ERR_CASE_GEN)
            break;
        default:
            return null();
        }
        CPtr p(new ErrorImpl(code, msg));
        return p;
    }

    LIBJ_STATUS_IMPL(status_);
};

#define LIBNODE_UV_ERR_MSG_DEF_GEN(VAL, NAME, S) \
    const String::CPtr ErrorImpl::MSG_UV_ERR_##NAME = String::create(S);

UV_ERRNO_MAP(LIBNODE_UV_ERR_MSG_DEF_GEN)

Error::CPtr Error::create(Int code) {
    return ErrorImpl::create(code);
}

Error::CPtr Error::create(Int code, String::CPtr msg) {
    return ErrorImpl::create(code, msg);
}

Error::CPtr Error::valueOf(uv_err_code code) {
    return create(UV_ERR_OK + code);
}

}  // namespace node
}  // namespace libj
