// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_CONTEXT_H_
#define LIBNODE_HTTP_SERVER_CONTEXT_H_

#include <uv.h>
#include <http_parser.h>
#include "./net_socket_impl.h"
#include "./http_server_request_impl.h"
#include "./http_server_response_impl.h"

namespace libj {
namespace node {
namespace http {

class ServerContext {
 public:
    ServerContext(void* srv)
        : server(srv)
        , socket(net::SocketImpl::create())
        , request(static_cast<ServerRequestImpl*>(0))
        , response(static_cast<ServerResponseImpl*>(0)) {
    }
    
    http_parser parser;
    uv_write_t write;
    void* server;
    net::SocketImpl::Ptr socket;
    ServerRequestImpl* request;
    ServerResponseImpl* response;
};
    
}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_CONTEXT_H_
