// Copyright (c) 2012 Plenluno All rights reserved.

#include "./http_server_context.h"
#include "./http_server_request_impl.h"

namespace libj {
namespace node {
namespace http {

const String::CPtr ServerRequestImpl::METHOD =
    String::create("method");
const String::CPtr ServerRequestImpl::URL =
    String::create("url");
const String::CPtr ServerRequestImpl::HEADERS =
    String::create("headers");
const String::CPtr ServerRequestImpl::HTTP_VERSION =
    String::create("httpVerion");

ServerRequestImpl::ServerRequestImpl(ServerContext* context)
    : context_(context)
    , ee_(EventEmitter::create()) {
}

net::Socket::Ptr ServerRequestImpl::connection() const {
    return context_->socket;
}

}  // namespace http
}  // namespace node
}  // namespace libj
