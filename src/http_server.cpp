// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/http_server.h"
#include <uv.h>

namespace libj {
namespace node {
namespace http {

class ServerImpl
    : public Server {
 public:
    Type<String>::Cptr toString() const {
        return ee_->toString();
    }

    static Ptr create() {
        Ptr p(new ServerImpl());
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
    
    ServerImpl()
        : ee_(EventEmitter::create()) {
        uv_tcp_init(uv_default_loop(), &server_);
    }

    LIBNODE_JS_OBJECT_IMPL(ee_);
    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

Type<String>::Cptr Server::EVENT_REQUEST = String::create("request");
Type<String>::Cptr Server::EVENT_CONNECTION = String::create("connection");
Type<String>::Cptr Server::EVENT_CLOSE = String::create("close");

Type<Server>::Ptr Server::create() {
    return ServerImpl::create();
}

Type<Server>::Ptr Server::create(Type<JsFunction>::Ptr requestListener) {
    return ServerImpl::create(requestListener);
}

}  // namespace http
}  // namespace node
}  // namespace libj
