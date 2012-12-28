// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_HTTP_CLIENT_REQUEST_H_
#define LIBNODE_DETAIL_HTTP_CLIENT_REQUEST_H_

#include <libnode/http/client_request.h>
#include <libnode/detail/http/outgoing_message.h>
#include <libnode/bridge/http/abstract_client_request.h>

namespace libj {
namespace node {
namespace detail {
namespace http {

typedef bridge::http::AbstractClientRequest<
    node::http::ClientRequest,
    OutgoingMessage> ClientRequestBase;

class ClientRequest : public ClientRequestBase {
 public:
    static Ptr create(OutgoingMessage::Ptr msg);

    static Ptr create(
        libj::JsObject::CPtr options,
        libj::JsFunction::Ptr callback);

 private:
    ClientRequest(OutgoingMessage::Ptr msg)
        : ClientRequestBase(msg) {}
};

}  // namespace http
}  // namespace detail
}  // namespace node
}  // namespace libj

#include <libnode/detail/http/impl/client_request.h>

#endif  // LIBNODE_DETAIL_HTTP_CLIENT_REQUEST_H_
