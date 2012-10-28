// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_SERVER_RESPONSE_IMPL_H_
#define LIBNODE_SRC_HTTP_SERVER_RESPONSE_IMPL_H_

#include <libj/string_buffer.h>
#include <string.h>
#include <uv.h>
#include <list>
#include <string>

#include "libnode/http.h"

#include "./outcoming_message.h"
#include "../net/socket_impl.h"

namespace libj {
namespace node {
namespace http {

class ServerResponseImpl : LIBNODE_HTTP_SERVER_RESPONSE(ServerResponseImpl)
 private:
    static const String::CPtr HEADERS;
    static const String::CPtr STATUS_CODE;

    class SocketEnd : LIBJ_JS_FUNCTION(SocketEnd)
     public:
        static Ptr create(
            ServerResponseImpl* res,
            net::SocketImpl::Ptr sock) {
            return Ptr(new SocketEnd(res, sock));
        }

        Value operator()(JsArray::Ptr args) {
            response_->removeAllListeners(EVENT_CLOSE);
            socket_->end();
            return libj::Status::OK;
        }

     private:
        ServerResponseImpl* response_;
        net::SocketImpl::Ptr socket_;

        SocketEnd(
            ServerResponseImpl* res,
            net::SocketImpl::Ptr sock)
            : response_(res)
            , socket_(sock) {}
    };

 public:
    static Ptr create(net::SocketImpl::Ptr sock) {
        return Ptr(new ServerResponseImpl(sock));
    }

 private:
    OutcomingMessage::Ptr msg_;

 public:
    // ServerResponseImpl(ServerReqestImpl::CPtr req)
    ServerResponseImpl(net::SocketImpl::Ptr sock)
        : msg_(OutcomingMessage::create()) {
    }

    LIBNODE_HTTP_OUTCOMING_MESSAGE_IMPL(msg_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_SERVER_RESPONSE_IMPL_H_
