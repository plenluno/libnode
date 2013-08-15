// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_UV_UDP_SEND_H_
#define LIBNODE_DETAIL_UV_UDP_SEND_H_

#include <libnode/buffer.h>
#include <libnode/detail/uv/req.h>

namespace libj {
namespace node {
namespace detail {
namespace uv {

class UdpSend : public Req<uv_udp_send_t> {
 public:
    UdpSend()
        : buffer(Buffer::null())
        , cb(JsFunction::null()) {}

    Buffer::CPtr buffer;
    JsFunction::Ptr cb;
};

}  // namespace uv
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_UV_UDP_SEND_H_
