// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef SRC_HTTP_SERVER_CONTEXT_H_
#define SRC_HTTP_SERVER_CONTEXT_H_

#include <http_parser.h>
#include <uv.h>

#include "./server_request_impl.h"
#include "./server_response_impl.h"
#include "../net/server_impl.h"
#include "../net/socket_impl.h"

namespace libj {
namespace node {
namespace http {

class ServerImpl;

class ServerContext {
 public:
    ServerContext(
        ServerImpl* srv,
        net::SocketImpl::Ptr sock)
        : server(srv)
        , socket(sock)
        , request(LIBJ_NULL(ServerRequestImpl))
        , response(LIBJ_NULL(ServerResponseImpl)) {
        ServerRequestImpl::Ptr req(new ServerRequestImpl(this));
        ServerResponseImpl::Ptr res(new ServerResponseImpl(this));
        request = req;
        response = res;
        parser.data = this;
    }

    ServerImpl* server;
    net::SocketImpl::Ptr socket;
    ServerRequestImpl::Ptr request;
    ServerResponseImpl::Ptr response;
    http_parser parser;
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // SRC_HTTP_SERVER_CONTEXT_H_
