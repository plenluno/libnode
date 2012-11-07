// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_UV_ERROR_H_
#define LIBNODE_UV_ERROR_H_

#include <uv.h>
#include <libj/error.h>

namespace libj {
namespace node {
namespace uv {

#define LIBNODE_UV_ERR_GEN(VAL, NAME, S) _##NAME = 1025 + VAL,

class Error : LIBJ_ERROR(Error)
 public:
    enum Code {
        UV_ERRNO_MAP(LIBNODE_UV_ERR_GEN)
    };

    static CPtr create(Code code);
    static CPtr create(Code code, String::CPtr msg);
    static CPtr valueOf(uv_err_code code);

    static CPtr last();
    static void setLast(uv_err_code);
};

}  // namespace uv
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_UV_ERROR_H_
