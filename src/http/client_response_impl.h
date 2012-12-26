// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_CLIENT_RESPONSE_IMPL_H_
#define LIBNODE_SRC_HTTP_CLIENT_RESPONSE_IMPL_H_

#include <libnode/http/client_response.h>
#include <libnode/bridge/http/abstract_client_response.h>

#include "./incoming_message.h"

namespace libj {
namespace node {
namespace http {

typedef bridge::http::AbstractClientResponse<
    ClientResponse,
    IncomingMessage> ClientResponseBase;

class ClientResponseImpl : public ClientResponseBase {
 public:
    static Ptr create(IncomingMessage::Ptr msg) {
        if (msg) {
            return Ptr(new ClientResponseImpl(msg));
        } else {
            return null();
        }
    }

 private:
    ClientResponseImpl(IncomingMessage::Ptr msg)
        : ClientResponseBase(msg) {}
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_CLIENT_RESPONSE_IMPL_H_
