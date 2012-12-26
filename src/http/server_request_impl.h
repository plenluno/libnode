// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_SERVER_REQUEST_IMPL_H_
#define LIBNODE_SRC_HTTP_SERVER_REQUEST_IMPL_H_

#include <libnode/http/server_request.h>
#include <libnode/bridge/http/abstract_server_request.h>

#include "./incoming_message.h"

namespace libj {
namespace node {
namespace http {

typedef bridge::http::AbstractServerRequest<
    ServerRequest,
    IncomingMessage> ServerRequestBase;

class ServerRequestImpl : public ServerRequestBase {
 public:
    static Ptr create(IncomingMessage::Ptr msg) {
        if (msg) {
            return Ptr(new ServerRequestImpl(msg));
        } else {
            return null();
        }
    }

 private:
    ServerRequestImpl(IncomingMessage::Ptr msg)
        : ServerRequestBase(msg) {}
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_SERVER_REQUEST_IMPL_H_
