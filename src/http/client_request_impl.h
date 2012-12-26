// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_CLIENT_REQUEST_IMPL_H_
#define LIBNODE_SRC_HTTP_CLIENT_REQUEST_IMPL_H_

#include <libnode/http/client_request.h>
#include <libnode/bridge/http/abstract_client_request.h>

#include "./outgoing_message.h"

namespace libj {
namespace node {
namespace http {

typedef bridge::http::AbstractClientRequest<
    ClientRequest,
    OutgoingMessage> ClientRequestBase;

class ClientRequestImpl : public ClientRequestBase {
 public:
    static Ptr create(OutgoingMessage::Ptr msg) {
        if (msg) {
            return Ptr(new ClientRequestImpl(msg));
        } else {
            return null();
        }
    }

 private:
    ClientRequestImpl(OutgoingMessage::Ptr msg)
        : ClientRequestBase(msg) {}
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_CLIENT_REQUEST_IMPL_H_
