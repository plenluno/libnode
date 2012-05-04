// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/http_server.h"

namespace libj {
namespace node {

Type<String>::Cptr EVENT_REQUEST = String::create("request");

class HttpServerImpl
    : public HttpServer {
 public:
    Type<String>::Cptr toString() const {
        return ee_->toString();
    }

    static Ptr create() {
        Ptr p(new HttpServerImpl());
        return p;
    }

    static Ptr create(Type<JsFunction>::Ptr requestListener) {
        Ptr p = create();
        p->on(EVENT_REQUEST, requestListener);
        return p;
    }

 private:
    Type<EventEmitter>::Ptr ee_;
    
    HttpServerImpl()
        : ee_(EventEmitter::create()) {}

    LIBNODE_JS_OBJECT_IMPL(ee_);
    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

Type<HttpServer>::Ptr HttpServer::create() {
    return HttpServerImpl::create();
}

Type<HttpServer>::Ptr HttpServer::create(Type<JsFunction>::Ptr requestListener) {
    return HttpServerImpl::create(requestListener);
}

}  // namespace node
}  // namespace libj
