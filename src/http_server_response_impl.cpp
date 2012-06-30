// Copyright (c) 2012 Plenluno All rights reserved.

#include <assert.h>

#include "./http_server_context.h"
#include "./http_server_response_impl.h"

namespace libj {
namespace node {
namespace http {

const String::CPtr ServerResponseImpl::HEADERS =
    String::create("headers");
const String::CPtr ServerResponseImpl::STATUS_CODE =
    String::create("statusCode");

ServerResponseImpl::ServerResponseImpl(ServerContext* context)
    : context_(context)
    , status_(LIBJ_NULL(http::Status))
    , res_(StringBuffer::create())
    , body_(StringBuffer::create())
    , ee_(EventEmitter::create()) {
    resBuf_.base = 0;
}

ServerResponseImpl::~ServerResponseImpl() {
    free(resBuf_.base);
}

void ServerResponseImpl::end() {
    makeResponse();
    makeResBuf();
    uv_write_t* write = &context_->write;
    uv_tcp_t* tcp = context_->socket->getTcp();
    uv_stream_t* stream = reinterpret_cast<uv_stream_t*>(tcp);
    assert(write && stream);

    write->data = context_;
    int err = uv_write(
        write,
        stream,
        &resBuf_,
        1,
        ServerResponseImpl::afterWrite);
    if (err) {
        delete context_;
    }
}

void ServerResponseImpl::onClose(uv_handle_t* handle) {
    ServerContext* context = static_cast<ServerContext*>(handle->data);
    if (context)
        delete context;
}

}  // namespace http
}  // namespace node
}  // namespace libj
