// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_NET_SERVER_IMPL_H_
#define LIBNODE_SRC_NET_SERVER_IMPL_H_

#include "libnode/net/server.h"
#include "./socket_impl.h"

namespace libj {
namespace node {
namespace net {

class ServerImpl : public Server {
 public:
    static Ptr create() {
        Ptr p(new ServerImpl());
        return p;
    }

    static void onConnection(uv_stream_t* stream, int status) {
        ServerImpl* srv = static_cast<ServerImpl*>(stream->data);
        SocketImpl::Ptr socket = SocketImpl::create();
        uv_stream_t* tcp = reinterpret_cast<uv_stream_t*>(socket->getUvTcp());
        if (uv_accept(stream, tcp))
            return;
        uv_read_start(tcp, onAlloc, onRead);
        JsArray::Ptr args = JsArray::create();
        args->add(socket);
        srv->emit(EVENT_CONNECTION, args);
    }

    static uv_buf_t onAlloc(uv_handle_t* handle, size_t suggestedSize) {
        uv_buf_t buf;
        buf.base = static_cast<char*>(malloc(suggestedSize));
        buf.len = suggestedSize;
        return buf;
    }

    static void onRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
        SocketImpl* sock = static_cast<SocketImpl*>(stream->data);
        sock->active();
        if (nread > 0) {
            Buffer::Ptr buffer = Buffer::create(buf.base, nread);
            JsArray::Ptr args = JsArray::create();
            if (sock->hasEncoding()) {
                args->add(buffer->toString(sock->getEncoding()));
            } else {
                args->add(buffer);
            }
            sock->emit(EVENT_DATA, args);
        } else if (nread < 0) {
            uv_err_t err = uv_last_error(uv_default_loop());
            if (err.code == UV_EOF) {
                sock->stopReading();
                sock->emit(EVENT_END, JsArray::create());
                if (!sock->writable()) sock->destroy();
            } else {
                sock->destroy(Error::valueOf(err.code));
            }
        }
        free(buf.base);
    }

    static void onClose(uv_handle_t* handle) {
        ServerImpl* srv = static_cast<ServerImpl*>(handle->data);
        srv->emit(EVENT_CLOSE, JsArray::create());
    }

    uv_tcp_t* getUvTcp() {
        return sock_->getUvTcp();
    }

    Boolean listen(Int port, String::CPtr hostName, Int backlog) {
        if (isOpen_ || !hostName || backlog <= 0)
            return false;
        uv_tcp_t* tcp = getUvTcp();
        isOpen_ = !(uv_tcp_bind(
                        tcp,
                        uv_ip4_addr(hostName->toStdString().c_str(), port))) &&
                  !uv_listen(
                        reinterpret_cast<uv_stream_t*>(tcp),
                        backlog,
                        onConnection);
        if (isOpen_) {
            JsArray::Ptr args = JsArray::create();
            emit(EVENT_LISTENING, args);
        }
        return isOpen_;
    }

    void close() {
        if (isOpen_) {
            uv_tcp_t* tcp =  getUvTcp();
            assert(tcp->data == this);
            uv_close(reinterpret_cast<uv_handle_t*>(tcp), onClose);
            isOpen_ = false;
        }
    }

 private:
    Boolean isOpen_;
    SocketImpl::Ptr sock_;

    ServerImpl()
        : isOpen_(false)
        , sock_(SocketImpl::create()) {
        uv_tcp_t* tcp = sock_->getUvTcp();
        tcp->data = this;
    }

    LIBNODE_SOCKET_IMPL(sock_);
};

}  // namespace net
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_NET_SERVER_IMPL_H_
