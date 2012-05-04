// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/http_server.h"
#include <uv.h>

namespace libj {
namespace node {

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
    
    bool listen(Int port) {
        return !(uv_tcp_bind(&server_, uv_ip4_addr("0.0.0.0", port)));
    }
    
    void close() {
        // TODO: implement
    }

 private:
    uv_tcp_t server_;
    Type<EventEmitter>::Ptr ee_;
    
    HttpServerImpl()
        : ee_(EventEmitter::create()) {
        uv_tcp_init(uv_default_loop(), &server_);
    }

    LIBNODE_JS_OBJECT_IMPL(ee_);
    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

Type<String>::Cptr HttpServer::EVENT_REQUEST = String::create("request");
Type<String>::Cptr HttpServer::EVENT_CONNECTION = String::create("connection");
Type<String>::Cptr HttpServer::EVENT_CLOSE = String::create("close");

Type<HttpServer>::Ptr HttpServer::create() {
    return HttpServerImpl::create();
}

Type<HttpServer>::Ptr HttpServer::create(Type<JsFunction>::Ptr requestListener) {
    return HttpServerImpl::create(requestListener);
}

}  // namespace node
}  // namespace libj
