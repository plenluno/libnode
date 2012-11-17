// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_SERVER_RESPONSE_IMPL_H_
#define LIBNODE_SRC_HTTP_SERVER_RESPONSE_IMPL_H_

#include "libnode/http/server_response.h"

#include "./outgoing_message.h"

namespace libj {
namespace node {
namespace http {

class ServerResponseImpl : LIBNODE_HTTP_SERVER_RESPONSE(ServerResponseImpl)
 public:
    static Ptr create(OutgoingMessage::Ptr msg) {
        if (msg) {
            return Ptr(new ServerResponseImpl(msg));
        } else {
            return ServerResponseImpl::null();
        }
    }

 private:
    OutgoingMessage::Ptr msg_;

    ServerResponseImpl(OutgoingMessage::Ptr msg) : msg_(msg) {}

    LIBNODE_HTTP_SERVER_RESPONSE_IMPL(msg_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_SERVER_RESPONSE_IMPL_H_
