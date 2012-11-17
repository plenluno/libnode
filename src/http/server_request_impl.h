// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_SERVER_REQUEST_IMPL_H_
#define LIBNODE_SRC_HTTP_SERVER_REQUEST_IMPL_H_

#include "libnode/http/server_request.h"

#include "./incoming_message.h"

namespace libj {
namespace node {
namespace http {

class ServerRequestImpl : LIBNODE_HTTP_SERVER_REQUEST(ServerRequestImpl)
 public:
    static Ptr create(IncomingMessage::Ptr msg) {
        if (msg) {
            return Ptr(new ServerRequestImpl(msg));
        } else {
            return ServerRequestImpl::null();
        }
    }

 private:
    IncomingMessage::Ptr msg_;

    ServerRequestImpl(IncomingMessage::Ptr msg) : msg_(msg) {}

    LIBNODE_HTTP_SERVER_REQUEST_IMPL(msg_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_SERVER_REQUEST_IMPL_H_
