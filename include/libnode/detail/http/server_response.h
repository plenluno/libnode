// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_HTTP_SERVER_RESPONSE_H_
#define LIBNODE_DETAIL_HTTP_SERVER_RESPONSE_H_

#include <libnode/http/server_response.h>
#include <libnode/detail/http/outgoing_message_list.h>
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
        : ServerResponseBase(msg)
        , msg_(msg) {}

    OutgoingMessage::Ptr msg_;

 public:
    ~ServerResponse() {
        if (msg_->hasFlag(OutgoingMessage::UNUSED)) {
            outgoingMessageList()->free(msg_);
        } else {
            msg_->setFlag(OutgoingMessage::UNUSED);
        }
    }
};

}  // namespace http
}  // namespace detail
}  // namespace node
}  // namespace libj

#include <libnode/detail/http/impl/server_response.h>

#endif  // LIBNODE_DETAIL_HTTP_SERVER_RESPONSE_H_
