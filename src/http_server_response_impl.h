// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_RESPONSE_IMPL_H_
#define LIBNODE_HTTP_SERVER_RESPONSE_IMPL_H_

#include "libj/json.h"
#include "libnode/http_server_response.h"
#include <uv.h>

namespace libj {
namespace node {
namespace http {

class ServerResponseImpl : public ServerResponse {
 private:
    static Type<String>::Cptr HEADERS;
    static Type<String>::Cptr STATUS_CODE;

 public:
    static LIBJ_PTR(ServerResponseImpl) create() {
        LIBJ_PTR(ServerResponseImpl) p(new ServerResponseImpl());
        return p;
    }
    
    static LIBJ_PTR(ServerResponseImpl)
    create(uv_write_t* uw, uv_stream_t* us) {
        LIBJ_PTR(ServerResponseImpl) p(new ServerResponseImpl(uw, us));
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
    
    Type<JsObject>::Ptr getHeaders() {
        Type<JsObject>::Ptr headers = getPtr<JsObject>(HEADERS);
        if (!headers) {
            headers = JsObject::create();
            put(HEADERS, headers);
        }
        return headers;
    }
   
    void setHeader(Type<String>::Cptr name, Type<String>::Cptr value) {
        getHeaders()->put(name, value);
    }
    
    Type<String>::Cptr getHeader(Type<String>::Cptr name) const {
        Type<JsObject>::Ptr headers = getPtr<JsObject>(HEADERS);
        if (headers) {
            Value v = headers->get(name);
            return toCptr<String>(v);
        } else {
            LIBJ_NULL_CPTR(String, nullp);
            return nullp;
        }
    }
    
    void removeHeader(Type<String>::Cptr name) {
        getHeaders()->remove(name);
    }
    
    void write(Type<Object>::Cptr chunk) {
        if (chunk->instanceOf(Type<String>::id())) {
            Type<String>::Cptr s = LIBJ_STATIC_CPTR_CAST(String)(chunk);
            body_ = body_->concat(s);
        } else {
            // TODO: else
        }
    }
    
    void makeResponse() {
        res_ = String::create("HTTP/1.1 200 OK\r\n");
        Int len = body_->length();
        setHeader(
            String::create("Content-Length"),
            json::stringify(len));
        Type<String>::Cptr colon = String::create(": ");
        Type<String>::Cptr nl = String::create("\r\n");
        Type<JsObject>::Ptr headers = getHeaders();
        Type<Set>::Cptr ks = headers->keySet();
        Type<Iterator>::Ptr itr = ks->iterator();
        while (itr->hasNext()) {
            Type<String>::Cptr name = toCptr<String>(itr->next());
            Type<String>::Cptr value = toCptr<String>(headers->get(name));
            res_ = res_->concat(name)->concat(colon)
                       ->concat(value)->concat(nl);
        }
        res_ = res_->concat(nl)->concat(body_);
    }
    
    void end() {
        makeResponse();
        Size len = res_->length();
        const char* r = static_cast<const char*>(res_->data(0));
        resBuf_ = static_cast<uv_buf_t*>(malloc(sizeof(uv_buf_t)));
        resBuf_->base = static_cast<char*>(malloc(len));
        resBuf_->len = len;
        strncpy(resBuf_->base, r, len);
        if (write_ && stream_) {
            uv_write(
                write_,
                stream_,
                resBuf_,
                1,
                ServerResponseImpl::afterWrite);
        }
    }
    
 private:
    static void onClose(uv_handle_t* handle) {
        free(handle);
    }
    
    static void afterWrite(uv_write_t* write, int status) {
        uv_close(
            reinterpret_cast<uv_handle_t*>(write->handle),
            ServerResponseImpl::onClose);
    }
   
 private:
    uv_write_t* write_;
    uv_stream_t* stream_;
    uv_buf_t* resBuf_;
    
    // TODO: introduce Buffer
    Type<String>::Cptr res_;
    Type<String>::Cptr body_;
    Type<EventEmitter>::Ptr ee_;

    ServerResponseImpl()
        : write_(0)
        , stream_(0)
        , resBuf_(0)
        , res_(String::create())
        , body_(String::create())
        , ee_(EventEmitter::create()) {
    }
    
    ServerResponseImpl(
        uv_write_t* uw,
        uv_stream_t* us)
        : write_(uw)
        , stream_(us)
        , resBuf_(0)
        , res_(String::create())
        , body_(String::create())
        , ee_(EventEmitter::create()) {
    }

 public:
    ~ServerResponseImpl() {
        free(resBuf_);
    }

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_RESPONSE_IMPL_H_
