// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_HTTP_CLIENT_RESPONSE_H_
#define LIBNODE_DETAIL_HTTP_CLIENT_RESPONSE_H_

#include <libnode/http/client_response.h>
#include <libnode/detail/http/incoming_message.h>
#include <libnode/bridge/http/abstract_client_response.h>

namespace libj {
namespace node {
namespace detail {
namespace http {

typedef bridge::http::AbstractClientResponse<
    node::http::ClientResponse,
    IncomingMessage> ClientResponseBase;

class ClientResponse : public ClientResponseBase {
 public:
    static Ptr create(IncomingMessage::Ptr msg) {
        if (msg) {
            return Ptr(new ClientResponse(msg));
        } else {
            return null();
        }
    }

 private:
    ClientResponse(IncomingMessage::Ptr msg)
        : ClientResponseBase(msg) {}
};

}  // namespace http
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_HTTP_CLIENT_RESPONSE_H_
