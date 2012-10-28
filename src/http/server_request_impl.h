// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_SERVER_REQUEST_IMPL_H_
#define LIBNODE_SRC_HTTP_SERVER_REQUEST_IMPL_H_

#include "libnode/http/server_request.h"

#include "./incoming_message.h"
#include "../net/socket_impl.h"

namespace libj {
namespace node {
namespace http {

class ServerRequestImpl : LIBNODE_HTTP_SERVER_REQUEST(ServerRequestImpl)
 public:
    static Ptr create(net::SocketImpl::Ptr sock) {
        return Ptr(new ServerRequestImpl(sock));
    }

 private:
    IncomingMessage::Ptr msg_;

 public:
    ServerRequestImpl(net::SocketImpl::Ptr sock)
        : msg_(IncomingMessage::create(sock)) {
    }

    LIBNODE_HTTP_INCOMING_MESSAGE_IMPL(msg_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_SERVER_REQUEST_IMPL_H_
