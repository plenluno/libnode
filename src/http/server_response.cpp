// Copyright (c) 2012 Plenluno All rights reserved.

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
    : context_(context)
    , status_(LIBJ_NULL(http::Status))
    , res_(StringBuffer::create())
    , body_(StringBuffer::create())
    , ee_(EventEmitter::create()) {}

class AfterSocketWrite : LIBJ_JS_FUNCTION(AfterSocketWrite)
 public:
    static Ptr create(ServerContext* context) {
        Ptr p(new AfterSocketWrite(context));
        return p;
    }

    Value operator()(JsArray::Ptr args) {
        context_->socket->end();
        if (context_)
            delete context_;
        return Status::OK;
    }

 private:
    ServerContext* context_;

    AfterSocketWrite(ServerContext* context) : context_(context) {}
};

Boolean ServerResponseImpl::end() {
    makeResponse();
    AfterSocketWrite::Ptr cb = AfterSocketWrite::create(context_);
    context_->socket->write(res_->toString(), String::UTF8, cb);
    return true;
}

}  // namespace http
}  // namespace node
}  // namespace libj
