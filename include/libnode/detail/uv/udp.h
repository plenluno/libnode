// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_UV_UDP_H_
#define LIBNODE_DETAIL_UV_UDP_H_

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
        std::string addr = address->toStdString();

        Int r;
        switch (family) {
        case AF_INET:
            r = uv_udp_bind(&udp_, uv_ip4_addr(addr.c_str(), port), flags);
            break;
        case AF_INET6:
            r = uv_udp_bind6(&udp_, uv_ip6_addr(addr.c_str(), port), flags);
            break;
        default:
            assert(false);
            r = -1;
        }
        if (r) setLastError();
        return r;
    }

    Int bind4(String::CPtr address, Int port, Int flags) {
        return bind(address, port, flags, AF_INET);
    }

    Int bind6(String::CPtr address, Int port, Int flags) {
        return bind(address, port, flags, AF_INET6);
    }

    Int setTTL(Int flag) {
        int r = uv_udp_set_ttl(&udp_, flag);
        if (r) setLastError();
        return r;
    }

    Int setBroadcast(Int flag) {
        int r = uv_udp_set_broadcast(&udp_, flag);
        if (r) setLastError();
        return r;
    }

    Int setMulticastTTL(Int flag) {
        int r = uv_udp_set_multicast_ttl(&udp_, flag);
        if (r) setLastError();
        return r;
    }

    Int setMulticastLoopback(Int flag) {
        int r = uv_udp_set_multicast_loop(&udp_, flag);
        if (r) setLastError();
        return r;
    }

    Int setMembership(
        String::CPtr address,
        String::CPtr iface,
        uv_membership membership) {
        assert(address);
        std::string addr = address->toStdString();
        std::string ifc = iface ? iface->toStdString() : std::string();
        int r = uv_udp_set_membership(
            &udp_,
            addr.c_str(),
            iface ? ifc.c_str() : NULL,
            membership);
        if (r) setLastError();
        return r;
    }

    Int addMembership(String::CPtr address, String::CPtr iface) {
        return setMembership(address, iface, UV_JOIN_GROUP);
    }

    Int dropMembership(String::CPtr address, String::CPtr iface) {
        return setMembership(address, iface, UV_LEAVE_GROUP);
    }

    UdpSend* send(
        Buffer::CPtr buffer,
        Size offset,
        Size length,
        Int port,
        String::CPtr address,
        Int family) {
        assert(buffer && address);
        assert(offset <  buffer->length());
        assert(length <= buffer->length() - offset);

        std::string addr = address->toStdString();

        UdpSend* udpSend = new UdpSend();
        udpSend->buffer = buffer;
        uv_buf_t buf = uv_buf_init(
            static_cast<char*>(const_cast<void*>(buffer->data())) + offset,
            length);

        Int r;
        switch (family) {
        case AF_INET:
            r = uv_udp_send(
                &udpSend->req,
                &udp_,
                &buf,
                1,
                uv_ip4_addr(addr.c_str(), port),
                onSend);
            break;
        case AF_INET6:
            r = uv_udp_send6(
                &udpSend->req,
                &udp_,
                &buf,
                1,
                uv_ip6_addr(addr.c_str(), port),
                onSend);
            break;
        default:
            assert(false);
            r = -1;
        }

        udpSend->dispatched();

        if (r) {
            setLastError();
            delete udpSend;
            return NULL;
        } else {
            return udpSend;
        }
    }

    UdpSend* send4(
        Buffer::CPtr buf,
        Size offset,
        Size length,
        Int port,
        String::CPtr address) {
        return send(buf, offset, length, port, address, AF_INET);
    }

    UdpSend* send6(
        Buffer::CPtr buf,
        Size offset,
        Size length,
        Int port,
        String::CPtr address) {
        return send(buf, offset, length, port, address, AF_INET6);
    }

    Boolean recvStart() {
        int r = uv_udp_recv_start(&udp_, onAlloc, onRecv);
        if (r && uv_last_error(uv_default_loop()).code != UV_EALREADY) {
            setLastError();
            return false;
        } else {
            return true;
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
            setLastError();
            return JsObject::null();
        } else {
            return addressToJs(reinterpret_cast<const sockaddr*>(&addr));
        }
    }

 private:
    static void onSend(uv_udp_send_t* req, int status) {
        UdpSend* udpSend = reinterpret_cast<UdpSend*>(req->data);
        if (status) setLastError();
        udpSend->onComplete->call(status, udpSend);
        delete udpSend;
    }

    static uv_buf_t onAlloc(uv_handle_t* handle, size_t suggestedSize) {
        Udp* udp = static_cast<Udp*>(handle->data);
        udp->buffer_ = Buffer::create(suggestedSize);
        return uv_buf_init(
            static_cast<char*>(const_cast<void*>(udp->buffer_->data())),
            suggestedSize);
    }

    static void onRecv(
        uv_udp_t* handle,
        ssize_t nread,
        uv_buf_t buf,
        struct sockaddr* addr,
        unsigned flags) {
        LIBJ_STATIC_SYMBOL_DEF(symSize, "size");

        Udp* udp = static_cast<Udp*>(handle->data);
        JsFunction::Ptr onMessage = udp->onMessage_;

        if (nread == 0) return;

        JsObject::Ptr rinfo;
        if (nread < 0)  {
            setLastError();
            udp->buffer_ = Buffer::null();
            rinfo = JsObject::null();
        } else if (nread > 0) {
            udp->buffer_ = udp->buffer_->slice(0, nread);
            rinfo = addressToJs(addr);
            rinfo->put(symSize, static_cast<Size>(nread));
        }
        if (onMessage) onMessage->call(udp->buffer_, rinfo);
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
