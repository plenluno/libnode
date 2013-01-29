// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_UV_FS_READ_H_
#define LIBNODE_DETAIL_UV_FS_READ_H_

#include <libnode/buffer.h>
#include <libnode/detail/uv/fs_req.h>

namespace libj {
namespace node {
namespace detail {
namespace uv {

class FsRead : public FsReq {
 public:
    FsRead(Size len, JsFunction::Ptr cb)
        : FsReq(cb)
        , length(len)
        , offset(0)
        , buffer(new char[len])
        , res(Buffer::null()) {}

    virtual ~FsRead() {
        delete[] buffer;
    }

    Size length;
    Size offset;
    char* buffer;
    Buffer::Ptr res;
};

}  // namespace uv
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_UV_FS_READ_H_
