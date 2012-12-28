// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_HTTP_SERVER_RESPONSE_H_
#define LIBNODE_DETAIL_HTTP_SERVER_RESPONSE_H_

#include <libnode/http/server_response.h>
#include <libnode/detail/http/outgoing_message.h>
#include <libnode/bridge/http/abstract_server_response.h>

namespace libj {
namespace node {
namespace detail {
namespace http {

typedef bridge::http::AbstractServerResponse<
    node::http::ServerResponse,
    OutgoingMessage> ServerResponseBase;

class ServerResponse : public ServerResponseBase {
 public:
    static Ptr create(OutgoingMessage::Ptr msg);

 private:
    ServerResponse(OutgoingMessage::Ptr msg)
        : ServerResponseBase(msg) {}
};

}  // namespace http
}  // namespace detail
}  // namespace node
}  // namespace libj

#include <libnode/detail/http/impl/server_response.h>

#endif  // LIBNODE_DETAIL_HTTP_SERVER_RESPONSE_H_
