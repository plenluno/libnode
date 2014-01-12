// Copyright (c) 2012-2014 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_UV_STREAM_H_
#define LIBNODE_DETAIL_UV_STREAM_H_

#include <libnode/invoke.h>
#include <libnode/detail/uv/handle.h>
#include <libnode/detail/uv/write.h>

namespace libj {
namespace node {
namespace detail {
namespace uv {

typedef class Req<uv_connect_t> Connect;
typedef class Req<uv_shutdown_t> Shutdown;

class Stream : public Handle {
 protected:
    Stream(uv_stream_t* stream)
        : Handle(reinterpret_cast<uv_handle_t*>(stream))
        , stream_(stream)
        , buffer_(Buffer::null())
        , owner_(NULL)
        , onRead_(JsFunction::null())
        , onConnection_(JsFunction::null()) {
        assert(stream_);
        stream_->data = this;
    }

 public:
    virtual Int listen(Int backlog) = 0;

    virtual void setHandle(uv_handle_t* handle) {
        Handle::setHandle(handle);
        stream_ = reinterpret_cast<uv_stream_t*>(handle);
        stream_->data = this;
    }

    void setOwner(void* owner) {
        owner_ = owner;
    }

    void setOnRead(JsFunction::Ptr callback) {
        onRead_ = callback;
    }

    void setOnConnection(JsFunction::Ptr callback) {
        onConnection_ = callback;
    }

    Int readStart() {
        Boolean ipcPipe =
            stream_->type == UV_NAMED_PIPE &&
            reinterpret_cast<uv_pipe_t*>(stream_)->ipc;

        Int r;
        if (ipcPipe) {
            r = uv_read2_start(stream_, onAlloc, onRead2);
        } else {
            r = uv_read_start(stream_, onAlloc, onRead);
        }
        if (r) setLastError();
        return r;
    }

    Int readStop() {
        Int r = uv_read_stop(stream_);
        if (r) setLastError();
        return r;
    }

    Write* writeBuffer(Buffer::CPtr buf) {
        Write* req = new Write();
        req->buffer = buf;

        uv_buf_t uvBuf;
        Size length = buf->length();
        uvBuf.base = static_cast<char*>(const_cast<void*>(buf->data()));
        uvBuf.len = length;
        Int r = uv_write(
                    &req->req,
                    stream_,
                    &uvBuf,
                    1,
                    afterWrite);

        req->dispatched();
        req->bytes = length;

        if (r) {
            setLastError();
            delete req;
            return NULL;
        } else {
            return req;
        }
    }

    Write* writeString(
        String::CPtr str,
        Buffer::Encoding enc,
        uv_stream_t* sendStream = NULL) {
        Buffer::Ptr buf = Buffer::create(str, enc);
        Write* req = new Write();
        req->buffer = buf;

        uv_buf_t uvBuf;
        Size length = buf->length();
        uvBuf.base = static_cast<char*>(const_cast<void*>(buf->data()));
        uvBuf.len = length;

        Boolean ipcPipe =
            stream_->type == UV_NAMED_PIPE &&
            reinterpret_cast<uv_pipe_t*>(stream_)->ipc;

        Int r;
        if (!ipcPipe) {
            r = uv_write(
                    &req->req,
                    stream_,
                    &uvBuf,
                    1,
                    afterWrite);
        } else {
            r = uv_write2(
                    &req->req,
                    stream_,
                    &uvBuf,
                    1,
                    sendStream,
                    afterWrite);
        }

        req->dispatched();
        req->bytes = length;

        if (r) {
            setLastError();
            delete req;
            return NULL;
        } else {
            return req;
        }
    }

    Shutdown* shutdown() {
        Shutdown* req = new Shutdown();
        Int r = uv_shutdown(&req->req, stream_, afterShutdown);
        req->dispatched();

        if (r) {
            setLastError();
            delete req;
            return NULL;
        } else {
            return req;
        }
    }

 private:
    static uv_buf_t onAlloc(uv_handle_t* handle, size_t suggestedSize) {
        Stream* stream = static_cast<Stream*>(handle->data);
        assert(stream->stream_ == reinterpret_cast<uv_stream_t*>(handle));

        stream->buffer_ = Buffer::create(suggestedSize);
        uv_buf_t uvBuf;
        uvBuf.base = static_cast<char*>(
            const_cast<void*>(stream->buffer_->data()));
        uvBuf.len = suggestedSize;
        return uvBuf;
    }

    static void onReadCommon(
        uv_stream_t* handle,
        ssize_t nread,
        uv_buf_t buf,
        uv_handle_type pending);

    static void onRead(
        uv_stream_t* handle,
        ssize_t nread,
        uv_buf_t buf) {
        onReadCommon(handle, nread, buf, UV_UNKNOWN_HANDLE);
    }

    static void onRead2(
        uv_pipe_t* handle,
        ssize_t nread,
        uv_buf_t buf,
        uv_handle_type pending) {
        onReadCommon(
            reinterpret_cast<uv_stream_t*>(handle), nread, buf, pending);
    }

    static void afterWrite(uv_write_t* req, int status) {
        Write* wreq = static_cast<Write*>(req->data);
        Stream* stream = static_cast<Stream*>(req->handle->data);
        if (status) setLastError();
        invoke(wreq->onComplete, status, stream, wreq);
        delete wreq;
    }

    static void afterShutdown(uv_shutdown_t* req, int status) {
        Shutdown* sreq = static_cast<Shutdown*>(req->data);
        Stream* stream = static_cast<Stream*>(req->handle->data);
        if (status) setLastError();
        invoke(sreq->onComplete, status, stream, sreq);
        delete sreq;
    }

 protected:
    // for Pipe and Tcp
    static void afterConnect(uv_connect_t* req, int status) {
        Connect* creq = static_cast<Connect*>(req->data);
        Stream* stream = static_cast<Stream*>(req->handle->data);

        Boolean readable, writable;
        if (status) {
            setLastError();
            readable = writable = false;
        } else {
            readable = uv_is_readable(req->handle) != 0;
            writable = uv_is_writable(req->handle) != 0;
        }

        invoke(creq->onComplete, status, stream, creq, readable, writable);
        delete creq;
    }

 protected:
    uv_stream_t* stream_;
    Buffer::Ptr buffer_;
    void* owner_;
    JsFunction::Ptr onRead_;
    JsFunction::Ptr onConnection_;
};

}  // namespace uv
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_UV_STREAM_H_
