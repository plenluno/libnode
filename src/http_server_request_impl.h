// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_REQUEST_IMPL_H_
#define LIBNODE_HTTP_SERVER_REQUEST_IMPL_H_

#include "libnode/http_server_request.h"
#include <iostream>

namespace libj {
namespace node {
namespace http {

class ServerContext;

class ServerRequestImpl : public ServerRequest {
 private:
    static String::CPtr METHOD;
    static String::CPtr URL;
    static String::CPtr HEADERS;
    static String::CPtr HTTP_VERSION;
 
 public:
    static Ptr create() {
        Ptr p(new ServerRequestImpl());
        return p;
    }
    
    String::CPtr method() const {
        return getCPtr<String>(METHOD);
    }
    
    String::CPtr url() const {
        return getCPtr<String>(URL);
    }
    
    JsObject::CPtr headers() const {
        return getCPtr<JsObject>(HEADERS);
    }
    
    String::CPtr httpVersion() const {
        return getCPtr<String>(HTTP_VERSION);
    }
    
    void setMethod(String::CPtr method) {
        put(METHOD, method);
    }
    
    void setUrl(String::CPtr url) {
        put(URL, url);
    }
    
    void setHeader(String::CPtr name, String::CPtr value) {
        JsObject::Ptr headers = getPtr<JsObject>(HEADERS);
        if (!headers) {
            headers = JsObject::create();
            put(HEADERS, headers);
        }
        headers->put(name->toLowerCase(), value);
    }
    
    void setHttpVersion(String::CPtr httpVersion) {
        put(HTTP_VERSION, httpVersion);
    }
    
 private:
     ServerContext* context_;
     
     EventEmitter::Ptr ee_;

 public:
     ServerRequestImpl()
         : context_(0)
         , ee_(EventEmitter::create()) {
     }
     
     ServerRequestImpl(ServerContext* context);

     ~ServerRequestImpl();

     LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_REQUEST_IMPL_H_
