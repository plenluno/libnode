// Copyright (c) 2013-2015 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_UV_UDP_H_
#define LIBNODE_DETAIL_UV_UDP_H_

#include <libnode/invoke.h>
#include <libnode/detail/uv/handle.h>
#include <libnode/detail/uv/udp_send.h>

namespace libj {
namespace node {
namespace detail {
namespace uv {

class Udp : public Handle {
 public:
    Udp()
        : Handle(reinterpret_cast<uv_handle_t*>(&udp_))
        , buffer_(Buffer::null())
        , onMessage_(JsFunction::null()) {
        int r = uv_udp_init(uv_default_loop(), &udp_);
        assert(r == 0);
    }

    void setOnMessage(JsFunction::Ptr callback) {
        onMessage_ = callback;
    }

    Int bind(String::CPtr address, Int port, Int flags, Int family) {
        assert(address);

        char saddr[sizeof(sockaddr_in6)];
        int err = uvAddr(address, port, family, saddr);
        if (err) return err;

        return uv_udp_bind(
            &udp_,
            reinterpret_cast<const sockaddr*>(saddr),
            flags);
    }

    Int bind4(String::CPtr address, Int port, Int flags) {
        return bind(address, port, flags, AF_INET);
    }

    Int bind6(String::CPtr address, Int port, Int flags) {
        return bind(address, port, flags, AF_INET6);
    }

    Int setTTL(Int flag) {
        return uv_udp_set_ttl(&udp_, flag);
    }

    Int setBroadcast(Int flag) {
        return uv_udp_set_broadcast(&udp_, flag);
    }

    Int setMulticastTTL(Int flag) {
        return uv_udp_set_multicast_ttl(&udp_, flag);
    }

    Int setMulticastLoopback(Int flag) {
        return uv_udp_set_multicast_loop(&udp_, flag);
    }

    Int setMembership(
        String::CPtr address,
        String::CPtr iface,
        uv_membership membership) {
        assert(address);
        std::string addr = address->toStdString();
        std::string ifc = iface ? iface->toStdString() : std::string();
        return uv_udp_set_membership(
            &udp_,
            addr.c_str(),
            iface ? ifc.c_str() : NULL,
            membership);
    }

    Int addMembership(String::CPtr address, String::CPtr iface) {
        return setMembership(address, iface, UV_JOIN_GROUP);
    }

    Int dropMembership(String::CPtr address, String::CPtr iface) {
        return setMembership(address, iface, UV_LEAVE_GROUP);
    }

    Int send(
        Buffer::CPtr buffer,
        Size offset,
        Size length,
        Int port,
        String::CPtr address,
        Int family,
        JsFunction::Ptr onComplete,
        JsFunction::Ptr cb) {
        assert(buffer && address);
        assert(offset <  buffer->length());
        assert(length <= buffer->length() - offset);

        char saddr[sizeof(sockaddr_in6)];
        int err = uvAddr(address, port, family, saddr);
        if (err) return err;

        uv::UdpSend* udpSend = new uv::UdpSend();
        udpSend->buffer = buffer;
        udpSend->onComplete = onComplete;
        udpSend->cb = cb;
        udpSend->dispatched();

        uv_buf_t buf = uv_buf_init(
            static_cast<char*>(const_cast<void*>(buffer->data())) + offset,
            length);
        err = uv_udp_send(
            &udpSend->req,
            &udp_,
            &buf,
            1,
            reinterpret_cast<const sockaddr*>(saddr),
            onSend);
        if (err) {
            delete udpSend;
        }
        return err;
    }

    Int send4(
        Buffer::CPtr buf,
        Size offset,
        Size length,
        Int port,
        String::CPtr address,
        JsFunction::Ptr onComplete,
        JsFunction::Ptr cb) {
        return send(
            buf, offset, length, port, address, AF_INET, onComplete, cb);
    }

    Int send6(
        Buffer::CPtr buf,
        Size offset,
        Size length,
        Int port,
        String::CPtr address,
        JsFunction::Ptr onComplete,
        JsFunction::Ptr cb) {
        return send(
            buf, offset, length, port, address, AF_INET6, onComplete, cb);
    }

    Int recvStart() {
        int err = uv_udp_recv_start(&udp_, onAlloc, onRecv);
        if (err == UV_EALREADY) {
            return 0;
        } else {
            return err;
        }
    }

    Int recvStop() {
        return uv_udp_recv_stop(&udp_);
    }

    JsObject::Ptr getSockName() {
        struct sockaddr_storage addr;
        int len = sizeof(addr);
        int r = uv_udp_getsockname(
            &udp_,
            reinterpret_cast<sockaddr*>(&addr),
            &len);
        if (r) {
            return JsObject::null();
        } else {
            return addressToJs(reinterpret_cast<const sockaddr*>(&addr));
        }
    }

 private:
    static void onSend(uv_udp_send_t* req, int status) {
        UdpSend* udpSend = reinterpret_cast<UdpSend*>(req->data);
        invoke(udpSend->onComplete, status, udpSend);
        delete udpSend;
    }

    static void onAlloc(
        uv_handle_t* handle,
        size_t suggestedSize,
        uv_buf_t* buf) {
        Udp* udp = static_cast<Udp*>(handle->data);
        udp->buffer_ = Buffer::create(suggestedSize);
        buf->base = static_cast<char*>(const_cast<void*>(udp->buffer_->data()));
        buf->len = suggestedSize;
    }

    static void onRecv(
        uv_udp_t* handle,
        ssize_t nread,
        const uv_buf_t* buf,
        const struct sockaddr* addr,
        unsigned flags) {
        LIBJ_STATIC_SYMBOL_DEF(symSize, "size");

        Udp* udp = static_cast<Udp*>(handle->data);
        JsFunction::Ptr onMessage = udp->onMessage_;

        if (nread == 0) return;

        JsObject::Ptr rinfo;
        if (nread < 0)  {
            udp->buffer_ = Buffer::null();
            rinfo = JsObject::null();
        } else if (nread > 0) {
            udp->buffer_ = udp->buffer_->slice(0, nread);
            rinfo = addressToJs(addr);
            rinfo->put(symSize, static_cast<Size>(nread));
        }
        if (onMessage) invoke(onMessage, udp->buffer_, rinfo);
    }

    int uvAddr(String::CPtr address, Int port, Int family, char* saddr) {
        std::string addr = address->toStdString();
        switch (family) {
        case AF_INET:
            return uv_ip4_addr(
                addr.c_str(),
                port,
                reinterpret_cast<sockaddr_in*>(saddr));
        case AF_INET6:
            return uv_ip6_addr(
                addr.c_str(),
                port,
                reinterpret_cast<sockaddr_in6*>(saddr));
        default:
            assert(false);
            return -1;
        }
    }

 private:
    uv_udp_t udp_;
    Buffer::Ptr buffer_;
    JsFunction::Ptr onMessage_;
};

}  // namespace uv
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_UV_UDP_H_
