// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_CLIENT_REQUEST_IMPL_H_
#define LIBNODE_SRC_HTTP_CLIENT_REQUEST_IMPL_H_

#include "libnode/http/client_request.h"

#include "./outgoing_message.h"

namespace libj {
namespace node {
namespace http {

class ClientRequestImpl : LIBNODE_HTTP_CLIENT_REQUEST(ClientRequestImpl)
 public:
    static Ptr create(OutgoingMessage::Ptr msg) {
        if (msg) {
            return Ptr(new ClientRequestImpl(msg));
        } else {
            return null();
        }
    }

 private:
    OutgoingMessage::Ptr msg_;

    ClientRequestImpl(OutgoingMessage::Ptr msg) : msg_(msg) {}

    LIBNODE_HTTP_CLIENT_REQUEST_IMPL(msg_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_CLIENT_REQUEST_IMPL_H_
