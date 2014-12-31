// Copyright (c) 2012-2014 Plenluno All rights reserved.

#ifndef LIBNODE_UV_ERROR_H_
#define LIBNODE_UV_ERROR_H_

#include <libj/error.h>

#include <uv.h>

namespace libj {
namespace node {
namespace uv {

#define LIBNODE_UV_ERR_GEN(NAME, S) _##NAME = UV_##NAME,

class Error : LIBJ_ERROR(Error)
 public:
    enum Code {
        UV_ERRNO_MAP(LIBNODE_UV_ERR_GEN)
    };

    static CPtr create(Code code);

    static CPtr create(Code code, String::CPtr msg);

    static CPtr valueOf(uv_errno_t code);
};

}  // namespace uv
}  // namespace node
}  // namespace libj

#define LIBNODE_UV_ERROR(E) \
    libj::node::uv::Error::valueOf(static_cast<uv_errno_t>(E))

#endif  // LIBNODE_UV_ERROR_H_
