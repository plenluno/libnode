// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/net.h>

#include <uv.h>

#include "./net/server_impl.h"
#include "./net/socket_impl.h"

namespace libj {
namespace node {
namespace net {

Int isIP(String::CPtr ip) {
    std::string s = ip->toStdString();
    char address_buffer[sizeof(struct in6_addr)];
    if (uv_inet_pton(
            AF_INET,
            s.c_str(),
            &address_buffer).code == UV_OK) {
        return 4;
    } else if (uv_inet_pton(
            AF_INET6,
            s.c_str(),
            &address_buffer).code == UV_OK) {
        return 6;
    } else {
        return 0;
    }
}

Boolean isIPv4(String::CPtr ip) {
    return isIP(ip) == 4;
}

Boolean isIPv6(String::CPtr ip) {
    return isIP(ip) == 6;
}

Server::Ptr createServer(
    JsObject::CPtr options,
    JsFunction::Ptr onConnection) {
    return ServerImpl::create(options, onConnection);
}

Socket::Ptr createConnection(
    JsObject::CPtr options,
    JsFunction::Ptr onConnect) {
    SocketImpl::Ptr sock = SocketImpl::create(options);
    sock->connect(options, onConnect);
    return sock;
}

Socket::Ptr createConnection(
    Int port,
    String::CPtr host,
    JsFunction::Ptr onConnect) {
    SocketImpl::Ptr sock = SocketImpl::create();
    sock->connect(port, host, onConnect);
    return sock;
}

Socket::Ptr createConnection(
    String::CPtr path,
    JsFunction::Ptr onConnect) {
    SocketImpl::Ptr sock = SocketImpl::create();
    sock->connect(path, onConnect);
    return sock;
}

Socket::Ptr connect(
    JsObject::CPtr options,
    JsFunction::Ptr onConnect) {
    return createConnection(options, onConnect);
}

Socket::Ptr connect(
    Int port,
    String::CPtr host,
    JsFunction::Ptr onConnect) {
    return createConnection(port, host, onConnect);
}

Socket::Ptr connect(
    String::CPtr path,
    JsFunction::Ptr onConnect) {
    return createConnection(path, onConnect);
}

}  // namespace net
}  // namespace node
}  // namespace libj
