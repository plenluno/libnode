// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_NET_SERVER_H_
#define LIBNODE_NET_SERVER_H_

#include "libnode/net/socket.h"

namespace libj {
namespace node {
namespace net {

class Server : LIBNODE_NET_SOCKET(Server)
 public:
    static const String::CPtr IN_ADDR_ANY;
    static const String::CPtr EVENT_LISTENING;
    static const String::CPtr EVENT_CONNECTION;

    static Ptr create();

    virtual Boolean listen(
        Int port,
        String::CPtr hostName = IN_ADDR_ANY,
        Int backlog = 511) = 0;
    virtual void close() = 0;
};

#define LIBNODE_NET_SERVER(T) \
    public libj::node::net::Server { \
    LIBJ_MUTABLE_DEFS(T, libj::node::net::Server)

#define LIBNODE_NET_SERVER_IMPL(S) \
    LIBNODE_NET_SOCKET_IMPL(S) \
    virtual Boolean listen( \
        Int port, \
        String::CPtr hostName = IN_ADDR_ANY, \
        Int backlog = 511) { \
        return S->listen(port, hostName, backlog); \
    } \
    virtual void close() { \
        S->close(); \
    }

}  // namespace net
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_NET_SERVER_H_
