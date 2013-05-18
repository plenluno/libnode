// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_UV_FS_REQ_H_
#define LIBNODE_DETAIL_UV_FS_REQ_H_

#include <libnode/buffer.h>
#include <libnode/detail/uv/req.h>

namespace libj {
namespace node {
namespace detail {
namespace uv {

class FsReq : public Req<uv_fs_t> {
 public:
    FsReq(JsFunction::Ptr cb)
        : Req(cb)
        , file(-1)
        , buffer(Buffer::null()) {
        req.data = this;
        req.errorno = UV_UNKNOWN;
    }

    virtual ~FsReq() {
        uv_fs_req_cleanup(&req);
    }

    uv_file file;
    std::string path;
    Buffer::Ptr buffer;
};

}  // namespace uv
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_UV_FS_REQ_H_
