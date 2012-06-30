// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef SRC_NET_SOCKET_IMPL_H_
#define SRC_NET_SOCKET_IMPL_H_

#include <uv.h>

#include "libnode/net/socket.h"

namespace libj {
namespace node {
namespace net {

class SocketImpl : public Socket {
 private:
    uv_tcp_t tcp_;

    EventEmitter::Ptr ee_;

 public:
    typedef LIBJ_PTR(SocketImpl) Ptr;

    static Ptr create() {
        Ptr p(new SocketImpl());
        return p;
    }

    SocketImpl() {
        uv_tcp_init(uv_default_loop(), &tcp_);
    }

    uv_tcp_t* getTcp() { return &tcp_; }

    String::CPtr remoteAddress() {
        struct sockaddr_storage addr;
        int len = sizeof(addr);
        if (!uv_tcp_getpeername(
                &tcp_,
                reinterpret_cast<struct sockaddr*>(&addr),
                &len)) {
            char addrStr[16];
            if (!uv_ip4_name(
                    reinterpret_cast<struct sockaddr_in*>(&addr),
                    addrStr,
                    16)) {
                return String::create(addrStr);
            }
        }
        return String::create();
    }

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace net
}  // namespace node
}  // namespace libj

#endif  // SRC_NET_SOCKET_IMPL_H_
