// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/http_server_response.h"

namespace libj {
namespace node {
namespace http {

class ServerResponseImpl : public ServerResponse {
 private:
    static Type<String>::Cptr STATUS_CODE;

 public:
    static Ptr create() {
        Ptr p(new ServerResponseImpl());
        return p;
    }
   
    void writeHead(Int statusCode) {
        put(STATUS_CODE, statusCode);
    }
   
    Int statusCode() const {
        Value v = get(STATUS_CODE);
        if (v.type() == Type<Int>::id()) {
            Int sc;
            to<Int>(v, &sc);
            return sc;
        } else {
            return 0;
        }
    }
   
    void setHeader(Type<String>::Cptr name, Type<String>::Cptr value) {
        if (name)
            put(name->toLowerCase(), value);
    }
    
    Type<String>::Cptr getHeader(Type<String>::Cptr name) const {
        return getCptr<String>(name);
    }
    
    void removeHeader(Type<String>::Cptr name) {
        remove(name);
    }
    
    void write(Type<Object>::Cptr chunk) {
        // TODO: implement
    }
    
    void end() {
        // TODO: implement
    }
   
 private:
    Type<EventEmitter>::Ptr ee_;

    ServerResponseImpl()
        : ee_(EventEmitter::create()) {
    }

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

Type<String>::Cptr ServerResponseImpl::STATUS_CODE = String::create("statusCode");

Type<String>::Cptr ServerResponse::EVENT_CLOSE = String::create("close");

Type<ServerResponse>::Ptr ServerResponse::create() {
    return ServerResponseImpl::create();
}


}  // namespace http
}  // namespace node
}  // namespace libj
