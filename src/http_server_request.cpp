// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/http_server_request.h"
#include "libj/null.h"

namespace libj {
namespace node {
namespace http {

class ServerRequestImpl : public ServerRequest {
 private:
    static Type<String>::Cptr METHOD;
    static Type<String>::Cptr URL;
    static Type<String>::Cptr HEADERS;
    static Type<String>::Cptr HTTP_VERSION;
 
 private:
    template<typename T>
    typename Type<T>::Cptr get(Type<String>::Cptr name) const {
        Value v = get(name);
        if (v.instanceOf(Type<Null>::id())) {
            typename Type<T>::Cptr nullp(static_cast<T*>(0));
            return nullp;
        }
        typename Type<T>::Cptr p;
        if (!toCptr<T>(v, &p)) {
            typename Type<T>::Cptr nullp(static_cast<T*>(0));
            return nullp;
        }
        return p;
    }
 
 public:
    Type<String>::Cptr toString() const {
        return ee_->toString();
    }

    static Ptr create() {
        Ptr p(new ServerRequestImpl());
        return p;
    }
    
    Type<String>::Cptr method() const {
        return get<String>(METHOD);
    }
    
    Type<String>::Cptr url() const {
        return get<String>(URL);
    }
    
    Type<JsObject>::Cptr headers() const {
        return get<JsObject>(HEADERS);
    }
    
    Type<String>::Cptr httpVersion() const {
        return get<String>(HTTP_VERSION);
    }
    
 private:
     Type<EventEmitter>::Ptr ee_;

     ServerRequestImpl()
         : ee_(EventEmitter::create()) {
     }

     LIBNODE_JS_OBJECT_IMPL(ee_);
     LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

Type<String>::Cptr ServerRequestImpl::METHOD = String::create("method");
Type<String>::Cptr ServerRequestImpl::URL = String::create("url");
Type<String>::Cptr ServerRequestImpl::HEADERS = String::create("headers");
Type<String>::Cptr ServerRequestImpl::HTTP_VERSION = String::create("httpVerion");

Type<String>::Cptr ServerRequest::EVENT_DATA = String::create("data");
Type<String>::Cptr ServerRequest::EVENT_END = String::create("end");
Type<String>::Cptr ServerRequest::EVENT_CLOSE = String::create("close");

Type<ServerRequest>::Ptr ServerRequest::create() {
    return ServerRequestImpl::create();
}

}  // namespace http
}  // namespace node
}  // namespace libj
