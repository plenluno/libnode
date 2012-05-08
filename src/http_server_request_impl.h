// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_REQUEST_IMPL_H_
#define LIBNODE_HTTP_SERVER_REQUEST_IMPL_H_

#include "libnode/http_server_request.h"
#include <iostream>

namespace libj {
namespace node {
namespace http {

class ServerRequestImpl : public ServerRequest {
 private:
    static Type<String>::Cptr METHOD;
    static Type<String>::Cptr URL;
    static Type<String>::Cptr HEADERS;
    static Type<String>::Cptr HTTP_VERSION;
 
 public:
    static Ptr create() {
        Ptr p(new ServerRequestImpl());
        return p;
    }
    
    Type<String>::Cptr method() const {
        return getCptr<String>(METHOD);
    }
    
    Type<String>::Cptr url() const {
        return getCptr<String>(URL);
    }
    
    Type<JsObject>::Cptr headers() const {
        return getCptr<JsObject>(HEADERS);
    }
    
    Type<String>::Cptr httpVersion() const {
        return getCptr<String>(HTTP_VERSION);
    }
    
    void setMethod(Type<String>::Cptr method) {
        put(METHOD, method);
    }
    
    void setUrl(Type<String>::Cptr url) {
        put(URL, url);
    }
    
    void setHeader(Type<String>::Cptr name, Type<String>::Cptr value) {
        Type<JsObject>::Ptr headers = getPtr<JsObject>(HEADERS);
        if (!headers) {
            headers = JsObject::create();
            put(HEADERS, headers);
        }
        headers->put(name->toLowerCase(), value);
    }
    
    void setHttpVersion(Type<String>::Cptr httpVersion) {
        put(HTTP_VERSION, httpVersion);
    }
    
 private:
     Type<EventEmitter>::Ptr ee_;

 public:
     ServerRequestImpl()
         : ee_(EventEmitter::create()) {
     }

     LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_REQUEST_IMPL_H_
