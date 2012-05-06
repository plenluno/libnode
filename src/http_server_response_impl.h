// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_HTTP_SERVER_RESPONSE_IMPL_H_
#define LIBNODE_HTTP_SERVER_RESPONSE_IMPL_H_

#include "libnode/http_server_response.h"
#include <uv.h>

namespace libj {
namespace node {
namespace http {

class ServerResponseImpl : public ServerResponse {
 private:
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
        if (!responseBuf) {
            const char response[] =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 6\r\n"
                "\r\n"
                "hello\n";
            responseBuf = static_cast<uv_buf_t*>(malloc(sizeof(uv_buf_t)));
            responseBuf->base = static_cast<char*>(malloc(sizeof(response)));
            responseBuf->len = sizeof(response);
            strncpy(responseBuf->base, response, sizeof(response));
        }
        if (write_ && stream_) {
            uv_write(
                write_,
                stream_,
                responseBuf,
                1,
                ServerResponseImpl::afterWrite);
        }
    }
    
 private:
    static uv_buf_t* responseBuf;
    
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
    
    Type<EventEmitter>::Ptr ee_;

    ServerResponseImpl()
        : write_(0)
        , stream_(0)
        , ee_(EventEmitter::create()) {
    }
    
    ServerResponseImpl(
        uv_write_t* uw,
        uv_stream_t* us)
        : write_(uw)
        , stream_(us)
        , ee_(EventEmitter::create()) {
    }

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_HTTP_SERVER_RESPONSE_IMPL_H_
