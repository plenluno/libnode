// Copyright (c) 2012-2015 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_UV_TCP_H_
#define LIBNODE_DETAIL_UV_TCP_H_

#include <libnode/detail/uv/stream.h>

namespace libj {
namespace node {
namespace detail {
namespace uv {

class Tcp : public Stream {
 public:
    Tcp() : Stream(reinterpret_cast<uv_stream_t*>(&tcp_)) {
        int r = uv_tcp_init(uv_default_loop(), &tcp_);
        assert(r == 0);
    }

    virtual Int listen(Int backlog) {
        return uv_listen(
            reinterpret_cast<uv_stream_t*>(&tcp_),
            backlog,
            onConnection);
    }

    JsObject::Ptr getSockName() {
        struct sockaddr_storage addr;
        int len = sizeof(addr);
        Int r = uv_tcp_getsockname(
                    &tcp_,
                    reinterpret_cast<struct sockaddr*>(&addr),
                    &len);
        if (r) {
            return JsObject::null();
        } else {
            return addressToJs(reinterpret_cast<const sockaddr*>(&addr));
        }
    }

    JsObject::Ptr getPeerName() {
        struct sockaddr_storage addr;
        int len = sizeof(addr);
        Int r = uv_tcp_getpeername(
                    &tcp_,
                    reinterpret_cast<struct sockaddr*>(&addr),
                    &len);
        if (r) {
            return JsObject::null();
        } else {
            return addressToJs(reinterpret_cast<const sockaddr*>(&addr));
        }
    }

    Int setNoDelay(Boolean enable) {
        return uv_tcp_nodelay(&tcp_, enable ? 1 : 0);
    }

    Int setKeepAlive(Boolean enable, UInt delay) {
        return uv_tcp_keepalive(&tcp_, enable ? 1 : 0, delay);
    }

    Int bind(String::CPtr ip, Int port = 0) {
        assert(ip);
        struct sockaddr_in addr;
        int err = uv_ip4_addr(ip->toStdString().c_str(), port, &addr);
        if (err) return err;

        return uv_tcp_bind(
            &tcp_,
            reinterpret_cast<const sockaddr*>(&addr),
            0);
    }

    Int bind6(String::CPtr ip6, Int port = 0) {
        assert(ip6);
        struct sockaddr_in6 addr;
        int err = uv_ip6_addr(ip6->toStdString().c_str(), port, &addr);
        if (err) return err;

        return uv_tcp_bind(
            &tcp_,
            reinterpret_cast<const sockaddr*>(&addr),
            0);
    }

    Int connect(String::CPtr ip, Int port, JsFunction::Ptr onComplete) {
        assert(ip);
        struct sockaddr_in addr;
        int err = uv_ip4_addr(ip->toStdString().c_str(), port, &addr);
        if (err) return err;

        return conn(reinterpret_cast<const sockaddr*>(&addr), onComplete);
    }

    Int connect6(String::CPtr ip6, Int port, JsFunction::Ptr onComplete) {
        assert(ip6);
        struct sockaddr_in6 addr;
        int err = uv_ip6_addr(ip6->toStdString().c_str(), port, &addr);
        if (err) return err;

        return conn(reinterpret_cast<const sockaddr*>(&addr), onComplete);
    }

 private:
    static void onConnection(uv_stream_t* handle, int status) {
        Tcp* self = static_cast<Tcp*>(handle->data);
        assert(self && self->stream_ == handle);

        if (status) {
            invoke(self->onConnection_);
            return;
        } else {
            Tcp* tcp = new Tcp();
            if (uv_accept(handle, tcp->stream_)) return;
            invoke(self->onConnection_, tcp);
        }
    }

    Int conn(const sockaddr* addr, JsFunction::Ptr onComplete) {
        Connect* creq = new Connect();
        creq->onComplete = onComplete;
        creq->dispatched();

        int err = uv_tcp_connect(
            &creq->req,
            &tcp_,
            addr,
            afterConnect);
        if (err) {
            delete creq;
        }
        return err;
    }

 private:
    uv_tcp_t tcp_;
};

}  // namespace uv
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_UV_TCP_H_
