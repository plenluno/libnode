// Copyright (c) 2012 Plenluno All rights reserved.

#include "./http_server_response_impl.h"
#include "./http_server_context.h"

namespace libj {
namespace node {
namespace http {

Type<String>::Cptr ServerResponseImpl::HEADERS = String::create("headers");
Type<String>::Cptr ServerResponseImpl::STATUS_CODE = String::create("statusCode");

ServerResponseImpl::ServerResponseImpl(ServerContext* context)
    : context_(context)
    , res_(StringBuffer::create())
    , body_(StringBuffer::create())
    , ee_(EventEmitter::create()) {
}

void ServerResponseImpl::end() {
    makeResponse();
    makeResBuf();
    uv_write_t* write = &context_->write;
    uv_stream_t* stream = reinterpret_cast<uv_stream_t*>(&context_->tcp);
    if (write && stream) {
        write->data = context_;
        uv_write(
            write,
            stream,
            &resBuf_,
            1,
            ServerResponseImpl::afterWrite);
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
