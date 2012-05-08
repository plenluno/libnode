// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_RESPONSE_IMPL_H_
#define LIBNODE_HTTP_SERVER_RESPONSE_IMPL_H_

#include "libj/string_buffer.h"
#include "libnode/http_server_response.h"
#include <uv.h>
#include <iostream>

namespace libj {
namespace node {
namespace http {

class ServerContext;

class ServerResponseImpl : public ServerResponse {
 private:
    static Type<String>::Cptr HEADERS;
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
        body_->append(chunk);
    }
    
    void makeResponse() {
        res_->append(String::create("HTTP/1.1 200 OK\r\n"));
        Int len = body_->length();
        setHeader(
            String::create("Content-Length"),
            String::valueOf(len));
        Type<String>::Cptr colon = String::create(": ");
        Type<String>::Cptr nl = String::create("\r\n");
        Type<JsObject>::Ptr headers = getHeaders();
        Type<Set>::Cptr ks = headers->keySet();
        Type<Iterator>::Ptr itr = ks->iterator();
        while (itr->hasNext()) {
            Type<String>::Cptr name = toCptr<String>(itr->next());
            Type<String>::Cptr value = toCptr<String>(headers->get(name));
            res_->append(name);
            res_->append(colon);
            res_->append(value);
            res_->append(nl);
        }
        res_->append(nl);
        res_->append(body_);
    }
    
    // TODO: introduce Buffer
    void makeResBuf() {
        Size len = res_->length();
        std::string r;
        for (Size i = 0; i < len; i++) {
            r += static_cast<char>(res_->charAt(i));
        }
        resBuf_.base = static_cast<char*>(malloc(len));
        resBuf_.len = len;
        strncpy(resBuf_.base, r.c_str(), len);
    }
    
    void end();
    
 private:
    static void onClose(uv_handle_t* handle);
    
    static void afterWrite(uv_write_t* write, int status) {
        write->handle->data = write->data;
        uv_close(
            reinterpret_cast<uv_handle_t*>(write->handle),
            ServerResponseImpl::onClose);
    }
   
 private:
    ServerContext* context_;
    uv_buf_t resBuf_;
    
    // TODO: introduce Buffer
    Type<StringBuffer>::Ptr res_;
    Type<StringBuffer>::Ptr body_;
    Type<EventEmitter>::Ptr ee_;

 public:
    ServerResponseImpl()
        : context_(0)
        , res_(StringBuffer::create())
        , body_(StringBuffer::create())
        , ee_(EventEmitter::create()) {
    }
    
    ServerResponseImpl(ServerContext* context);

    ~ServerResponseImpl() {
        free(resBuf_.base);
    }

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_RESPONSE_IMPL_H_
