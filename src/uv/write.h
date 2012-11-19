// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_UV_WRITE_H_
#define LIBNODE_SRC_UV_WRITE_H_

#include "libnode/buffer.h"

#include "./req.h"

namespace libj {
namespace node {
namespace uv {

class Write : public Req<uv_write_t> {
 public:
    Write()
        : bytes(0)
        , buffer(Buffer::null())
        , cb(JsFunction::null()) {}

    Size bytes;
    Buffer::CPtr buffer;
    JsFunction::Ptr cb;
};

}  // namespace uv
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_UV_WRITE_H_
