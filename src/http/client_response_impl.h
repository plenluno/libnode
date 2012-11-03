// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_CLIENT_RESPONSE_IMPL_H_
#define LIBNODE_SRC_HTTP_CLIENT_RESPONSE_IMPL_H_

#include "libnode/http/client_response.h"

#include "./incoming_message.h"

namespace libj {
namespace node {
namespace http {

class ClientResponseImpl : LIBNODE_HTTP_CLIENT_RESPONSE(ClientResponseImpl)
 public:
    static Ptr create(IncomingMessage::Ptr msg) {
        return Ptr(new ClientResponseImpl(msg));
    }

 private:
    IncomingMessage::Ptr msg_;

    ClientResponseImpl(IncomingMessage::Ptr msg) : msg_(msg) {}

    LIBNODE_HTTP_CLIENT_RESPONSE_IMPL(msg_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_CLIENT_RESPONSE_IMPL_H_
