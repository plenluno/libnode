// Copyright (c) 2012-2013 Plenluno All rights reserved.

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
    uv_buf_t buf,
    uv_handle_type pending) {
    Stream* stream = static_cast<Stream*>(handle->data);
    JsFunction::Ptr onRead = stream->onRead_;

    if (nread < 0)  {
        if (buf.base != NULL) {
            stream->buffer_ = Buffer::null();
        }
        setLastError();

        if (onRead) onRead->call(Buffer::null(), 0);
        return;
    }

    assert(buf.base != NULL);
    stream->buffer_ = stream->buffer_->slice(0, nread);

    if (nread == 0) return;
    assert(static_cast<size_t>(nread) <= buf.len);

    Stream* pendingObj = NULL;
    if (pending == UV_TCP) {
        pendingObj = new Tcp();
    } else if (pending == UV_NAMED_PIPE) {
        pendingObj = new Pipe();
    } else {
        assert(pending == UV_UNKNOWN_HANDLE);
    }
    if (onRead) onRead->call(stream->buffer_, pendingObj);
}

}  // namespace uv
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_UV_STREAM_COMMON_H_
