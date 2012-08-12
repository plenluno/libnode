// Copyright (c) 2012 Plenluno All rights reserved.

#include <assert.h>
#include <list>

#include "./server_context.h"
#include "./server_response_impl.h"

namespace libj {
namespace node {
namespace http {

const String::CPtr ServerResponseImpl::HEADERS =
    String::create("headers");
const String::CPtr ServerResponseImpl::STATUS_CODE =
    String::create("statusCode");

ServerResponseImpl::ServerResponseImpl(ServerContext* context)
    : flags_(0)
    , context_(context)
    , status_(LIBJ_NULL(http::Status))
    , output_(JsArray::create())
    , ee_(EventEmitter::create()) {}

Boolean ServerResponseImpl::destroy() {
    assert(context_);
    return context_->socket->destroy();
}

Boolean ServerResponseImpl::destroySoon() {
    assert(context_);
    return context_->socket->destroySoon();
}

Boolean ServerResponseImpl::writable() const {
    assert(context_);
    return context_->socket->writable();
}

Boolean ServerResponseImpl::flush() {
    Buffer::Ptr buf = Buffer::concat(output_);
    context_->socket->write(buf);
    output_->clear();

    assert(hasFlag(HEADER_MADE));
    setFlag(HEADER_SENT);
    return true;
}

Boolean ServerResponseImpl::end() {
    if (hasFlag(FINISHED))
        return false;
    if (!hasFlag(HEADER_SENT)) {
        makeHeader();
        flush();
    }
    context_->socket->end();
    setFlag(FINISHED);
    return true;
}

}  // namespace http
}  // namespace node
}  // namespace libj
