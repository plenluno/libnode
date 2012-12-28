// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_UV_TCP_H_
#define LIBNODE_DETAIL_UV_TCP_H_

#include <libnode/detail/uv/stream.h>

#include <libj/symbol.h>

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
        Int r = uv_listen(
            reinterpret_cast<uv_stream_t*>(&tcp_),
            backlog,
            onConnection);
        if (r) setLastError();
        return r;
    }

    JsObject::Ptr getSockName() {
        struct sockaddr_storage addr;
        int len = sizeof(addr);
        Int r = uv_tcp_getsockname(
                    &tcp_,
                    reinterpret_cast<struct sockaddr*>(&addr),
                    &len);
        if (r) {
            setLastError();
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
            setLastError();
            return JsObject::null();
        } else {
            return addressToJs(reinterpret_cast<const sockaddr*>(&addr));
        }
    }

    Int setNoDelay(Boolean enable) {
        Int r = uv_tcp_nodelay(&tcp_, enable ? 1 : 0);
        if (r) setLastError();
        return r;
    }

    Int setKeepAlive(Boolean enable, UInt delay) {
        Int r = uv_tcp_keepalive(
            &tcp_, enable ? 1 : 0, delay);
        if (r) setLastError();
        return r;
    }

    Int bind(String::CPtr ip, Int port = 0) {
        struct sockaddr_in address =
            uv_ip4_addr(ip->toStdString().c_str(), port);
        Int r = uv_tcp_bind(&tcp_, address);
        if (r) setLastError();
        return r;
    }

    Int bind6(String::CPtr ip6, Int port = 0) {
        struct sockaddr_in6 address =
            uv_ip6_addr(ip6->toStdString().c_str(), port);
        Int r = uv_tcp_bind6(&tcp_, address);
        if (r) setLastError();
        return r;
    }

    Connect* connect(String::CPtr ip, Int port) {
        assert(ip);
        struct sockaddr_in address =
            uv_ip4_addr(ip->toStdString().c_str(), port);
        Connect* creq = new Connect();
        Int r = uv_tcp_connect(&creq->req, &tcp_, address, afterConnect);
        creq->dispatched();

        if (r) {
            setLastError();
            delete creq;
            return NULL;
        } else {
            return creq;
        }
    }

    Connect* connect6(String::CPtr ip6, Int port) {
        assert(ip6);
        struct sockaddr_in6 address =
            uv_ip6_addr(ip6->toStdString().c_str(), port);
        Connect* creq = new Connect();
        Int r = uv_tcp_connect6(&creq->req, &tcp_, address, afterConnect);
        creq->dispatched();

        if (r) {
            setLastError();
            delete creq;
            return NULL;
        } else {
            return creq;
        }
    }

 private:
    static JsObject::Ptr addressToJs(const sockaddr* addr) {
        LIBJ_STATIC_SYMBOL_DEF(symIpV4,    "IPv4");
        LIBJ_STATIC_SYMBOL_DEF(symIpV6,    "IPv6");
        LIBJ_STATIC_SYMBOL_DEF(symPort,    "port");
        LIBJ_STATIC_SYMBOL_DEF(symFamily,  "family");
        LIBJ_STATIC_SYMBOL_DEF(symAddress, "address");

        char ip[INET6_ADDRSTRLEN];
        Int port;
        const sockaddr_in *a4;
        const sockaddr_in6 *a6;

        JsObject::Ptr res = JsObject::create();
        switch (addr->sa_family) {
        case AF_INET:
            a4 = reinterpret_cast<const sockaddr_in*>(addr);
            uv_inet_ntop(AF_INET, &a4->sin_addr, ip, sizeof ip);
            port = ntohs(a4->sin_port);
            res->put(symFamily, symIpV4);
            break;
        case AF_INET6:
            a6 = reinterpret_cast<const sockaddr_in6*>(addr);
            uv_inet_ntop(AF_INET6, &a6->sin6_addr, ip, sizeof ip);
            port = ntohs(a6->sin6_port);
            res->put(symFamily, symIpV6);
            break;
        default:
            assert(false);
        }
        res->put(symPort, port);
        res->put(symAddress, String::create(ip));
        return res;
    }

    static void onConnection(uv_stream_t* handle, int status) {
        Tcp* self = static_cast<Tcp*>(handle->data);
        assert(self && self->stream_ == handle);

        if (status) {
            setLastError();
            self->onConnection_->call();
            return;
        } else {
            Tcp* tcp = new Tcp();
            if (uv_accept(handle, tcp->stream_)) return;
            self->onConnection_->call(tcp);
        }
    }

 private:
    uv_tcp_t tcp_;
};

}  // namespace uv
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_UV_TCP_H_
