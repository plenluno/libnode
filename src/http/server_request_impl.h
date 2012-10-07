// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_SERVER_REQUEST_IMPL_H_
#define LIBNODE_SRC_HTTP_SERVER_REQUEST_IMPL_H_

#include "libnode/http/server_request.h"

#include "./incoming_message.h"
#include "../net/socket_impl.h"

namespace libj {
namespace node {
namespace http {

class ServerRequestImpl : public ServerRequest {
 public:
    typedef LIBJ_PTR(ServerRequestImpl) Ptr;
    typedef LIBJ_CPTR(ServerRequestImpl) CPtr;

    static Ptr create(net::SocketImpl::Ptr sock) {
        Ptr p(new ServerRequestImpl(sock));
        return p;
    }

 private:
    IncomingMessage::Ptr im_;

 public:
    ServerRequestImpl(net::SocketImpl::Ptr sock)
        : im_(IncomingMessage::create(sock)) {
    }

    LIBNODE_HTTP_INCOMING_MESSAGE_IMPL(im_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_SERVER_REQUEST_IMPL_H_
