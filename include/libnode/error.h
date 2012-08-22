// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_UV_ERROR_H_
#define LIBNODE_UV_ERROR_H_

#include <libj/error.h>
#include <uv.h>

namespace libj {
namespace node {

#define LIBNODE_UV_ERR_GEN(VAL, NAME, S) UV_ERR_##NAME = 1025 + VAL,

class Error : LIBJ_ERROR(Error)
 public:
    enum Code {
        UV_ERRNO_MAP(LIBNODE_UV_ERR_GEN)

        END_OF_CODE
    };

    static CPtr create(Int code);
    static CPtr create(Int code, String::CPtr msg);
    static CPtr valueOf(uv_err_code code);
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_UV_ERROR_H_
