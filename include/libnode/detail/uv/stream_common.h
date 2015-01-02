// Copyright (c) 2012-2015 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_UV_STREAM_COMMON_H_
#define LIBNODE_DETAIL_UV_STREAM_COMMON_H_

#include <libnode/detail/uv/pipe.h>
#include <libnode/detail/uv/tcp.h>

namespace libj {
namespace node {
namespace detail {
namespace uv {

inline void Stream::onReadCommon(
    uv_stream_t* handle,
    ssize_t nread,
    const uv_buf_t* buf,
    uv_handle_type pending) {
    Stream* stream = static_cast<Stream*>(handle->data);
    JsFunction::Ptr onRead = stream->onRead_;

    if (nread < 0)  {
        if (buf->base) {
            stream->buffer_ = Buffer::null();
        }

        if (onRead) invoke(onRead, nread, Buffer::null());
        return;
    }

    if (nread == 0) {
        if (buf->base) {
            stream->buffer_ = Buffer::null();
        }
        return;
    }

    assert(buf->base);
    assert(static_cast<size_t>(nread) <= buf->len);
    stream->buffer_ = stream->buffer_->slice(0, nread);

    // TODO(plenluno): create pendingObj
    // Stream* pendingObj = NULL;
    // if (type == UV_TCP) {
    //     pendingObj = new Tcp();
    // } else if (type == UV_NAMED_PIPE) {
    //     pendingObj = new Pipe();
    // } else {
    //     assert(pending == UV_UNKNOWN_HANDLE);
    // }
    if (onRead) invoke(onRead, nread, stream->buffer_);
}

}  // namespace uv
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_UV_STREAM_COMMON_H_
