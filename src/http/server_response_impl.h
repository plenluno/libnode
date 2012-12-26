// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_SERVER_RESPONSE_IMPL_H_
#define LIBNODE_SRC_HTTP_SERVER_RESPONSE_IMPL_H_

#include <libnode/http/server_response.h>
#include <libnode/bridge/http/abstract_server_response.h>

#include "./outgoing_message.h"

namespace libj {
namespace node {
namespace http {

typedef bridge::http::AbstractServerResponse<
    ServerResponse,
    OutgoingMessage> ServerResponseBase;

class ServerResponseImpl : public ServerResponseBase {
 public:
    static Ptr create(OutgoingMessage::Ptr msg) {
        if (msg) {
            return Ptr(new ServerResponseImpl(msg));
        } else {
            return null();
        }
    }

 private:
    ServerResponseImpl(OutgoingMessage::Ptr msg)
        : ServerResponseBase(msg) {}
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_SERVER_RESPONSE_IMPL_H_
