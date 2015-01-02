// Copyright (c) 2012-2015 Plenluno All rights reserved.

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
        return uv_listen(
            reinterpret_cast<uv_stream_t*>(&pipe_),
            backlog,
            onConnection);
    }

    void open(Int fd) {
        uv_pipe_open(&pipe_, fd);
    }

    Int bind(String::CPtr name) {
        assert(name);
        return uv_pipe_bind(&pipe_, name->toStdString().c_str());
    }

    void connect(String::CPtr name, JsFunction::Ptr onComplete) {
        assert(name);
        Connect* creq = new Connect();
        creq->onComplete = onComplete;
        creq->dispatched();

        uv_pipe_connect(
            &creq->req,
            &pipe_,
            name->toStdString().c_str(),
            afterConnect);
    }

 private:
    static void onConnection(uv_stream_t* handle, int status) {
        Pipe* self = static_cast<Pipe*>(handle->data);
        assert(self && self->stream_ == handle);

        if (status) {
            invoke(self->onConnection_);
            return;
        } else {
            Pipe* pipe = new Pipe(false);
            if (uv_accept(handle, pipe->stream_)) return;
            invoke(self->onConnection_, pipe);
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
