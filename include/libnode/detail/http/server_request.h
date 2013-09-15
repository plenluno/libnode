// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_HTTP_SERVER_REQUEST_H_
#define LIBNODE_DETAIL_HTTP_SERVER_REQUEST_H_

#include <libnode/http/server_request.h>
#include <libnode/detail/http/incoming_message_list.h>
#include <libnode/bridge/http/abstract_server_request.h>

namespace libj {
namespace node {
namespace detail {
namespace http {

typedef bridge::http::AbstractServerRequest<
    node::http::ServerRequest,
    IncomingMessage> ServerRequestBase;

class ServerRequest : public ServerRequestBase {
 public:
    static Ptr create(IncomingMessage::Ptr msg) {
        if (msg) {
            return Ptr(new ServerRequest(msg));
        } else {
            return null();
        }
    }

 private:
    ServerRequest(IncomingMessage::Ptr msg)
        : ServerRequestBase(msg)
        , msg_(msg) {}

    IncomingMessage::Ptr msg_;

 public:
    virtual ~ServerRequest() {
        if (msg_->hasFlag(IncomingMessage::UNUSED)) {
            incomingMessageList()->free(msg_);
        } else {
            msg_->setFlag(IncomingMessage::UNUSED);
        }
    }
};

}  // namespace http
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_HTTP_SERVER_REQUEST_H_
