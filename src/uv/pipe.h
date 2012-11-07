// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_UV_PIPE_H_
#define LIBNODE_SRC_UV_PIPE_H_

#include "./stream.h"

namespace libj {
namespace node {
namespace uv {

class Pipe : public Stream {
 public:
    uv_pipe_t* uvPipe() { return &pipe_; }

    Pipe(Boolean ipc = false)
        : Stream(reinterpret_cast<uv_stream_t*>(&pipe_)) {
        Int r = uv_pipe_init(uv_default_loop(), &pipe_, ipc);
        assert(r == 0);
        pipe_.data = this;
    }

    Int bind(String::CPtr name) {
        Int r = uv_pipe_bind(&pipe_, name->toStdString().c_str());
        if (r) setLastError();
        return r;
    }

    Int listen(Int backlog) {
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

    void connect(String::CPtr name) {
        Connect* creq = new Connect();
        uv_pipe_connect(
            &creq->req,
            &pipe_,
            name->toStdString().c_str(),
            afterConnect);
        creq->dispatched();
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
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_UV_PIPE_H_
