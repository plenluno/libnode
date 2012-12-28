// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_UV_PIPE_H_
#define LIBNODE_DETAIL_UV_PIPE_H_

#include <libnode/detail/uv/stream.h>

namespace libj {
namespace node {
namespace detail {
namespace uv {

class Pipe : public Stream {
 public:
    Pipe(Boolean ipc = false)
        : Stream(reinterpret_cast<uv_stream_t*>(&pipe_)) {
        Int r = uv_pipe_init(uv_default_loop(), &pipe_, ipc);
        assert(r == 0);
        pipe_.data = this;
    }

    virtual Int listen(Int backlog) {
        Int r = uv_listen(
            reinterpret_cast<uv_stream_t*>(&pipe_),
            backlog,
            onConnection);
        if (r) setLastError();
        return r;
    }

    void open(int fd) {
        uv_pipe_open(&pipe_, fd);
    }

    Int bind(String::CPtr name) {
        Int r = uv_pipe_bind(&pipe_, name->toStdString().c_str());
        if (r) setLastError();
        return r;
    }

    Connect* connect(String::CPtr name) {
        assert(name);
        Connect* creq = new Connect();
        uv_pipe_connect(
            &creq->req,
            &pipe_,
            name->toStdString().c_str(),
            afterConnect);
        creq->dispatched();
        return creq;
    }

 private:
    static void onConnection(uv_stream_t* handle, int status) {
        Pipe* self = static_cast<Pipe*>(handle->data);
        assert(self && self->stream_ == handle);

        if (status) {
            setLastError();
            self->onConnection_->call();
            return;
        } else {
            Pipe* pipe = new Pipe(false);
            if (uv_accept(handle, pipe->stream_)) return;
            self->onConnection_->call(pipe);
        }
    }

 private:
    uv_pipe_t pipe_;
};

}  // namespace uv
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_UV_PIPE_H_
