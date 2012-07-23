// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef SRC_NET_SOCKET_IMPL_H_
#define SRC_NET_SOCKET_IMPL_H_

#include <assert.h>
#include <uv.h>
#include <vector>

#include "libnode/net/socket.h"

namespace libj {
namespace node {
namespace net {

class SocketWriteContext {
 public:
    uv_write_t uvWrite;
    uv_buf_t uvBuf;
    int status;
    Buffer::CPtr buffer;
    JsFunction::Ptr callback;

    SocketWriteContext()
        : status(0)
        , buffer(LIBJ_NULL(Buffer))
        , callback(LIBJ_NULL(JsFunction)) {
        uvWrite.data = this;
        uvBuf.base = NULL;
    }

    ~SocketWriteContext() {
        free(uvBuf.base);
    }
};

class SocketImpl : public Socket {
 public:
    typedef LIBJ_PTR(SocketImpl) Ptr;

    static Ptr create() {
        Ptr p(new SocketImpl());
        return p;
    }

    uv_tcp_t* getUvTcp() { return &tcp_; }

    JsObject::Ptr address() {
        static const String::CPtr strPort = String::create("port");
        static const String::CPtr strFamily = String::create("family");
        static const String::CPtr strAddress = String::create("address");
        static const String::CPtr strIpV4 = String::create("IPv4");
        static const String::CPtr strIpV6 = String::create("IPv6");

        struct sockaddr_storage addr;
        int len = sizeof(addr);
        if (!uv_tcp_getsockname(
                &tcp_,
                reinterpret_cast<struct sockaddr*>(&addr),
                &len)) {
            JsObject::Ptr res = JsObject::create();
            char ip[INET6_ADDRSTRLEN];
            switch (addr.ss_family) {
            case AF_INET:
                res->put(strFamily, strIpV4);
                res->put(strPort, static_cast<int>(ntohs(
                    reinterpret_cast<struct sockaddr_in*>(&addr)->sin_port)));
                if (!uv_ip4_name(
                        reinterpret_cast<struct sockaddr_in*>(&addr),
                        ip,
                        INET6_ADDRSTRLEN)) {
                    res->put(strAddress, String::create(ip));
                }
                break;
            case AF_INET6:
                res->put(strFamily, strIpV6);
                res->put(strPort, static_cast<int>(ntohs(
                    reinterpret_cast<struct sockaddr_in6*>(&addr)->sin6_port)));
                if (!uv_ip6_name(
                        reinterpret_cast<struct sockaddr_in6*>(&addr),
                        ip,
                        INET6_ADDRSTRLEN)) {
                    res->put(strAddress, String::create(ip));
                }
                break;
            }
            return res;
        }
        LIBJ_NULL_PTR(JsObject, nullp);
        return nullp;
    }

    String::CPtr remoteAddress() {
        struct sockaddr_storage addr;
        int len = sizeof(addr);
        if (!uv_tcp_getpeername(
                &tcp_,
                reinterpret_cast<struct sockaddr*>(&addr),
                &len)) {
            char ip[INET6_ADDRSTRLEN];
            switch (addr.ss_family) {
            case AF_INET:
                if (!uv_ip4_name(
                        reinterpret_cast<struct sockaddr_in*>(&addr),
                        ip,
                        INET6_ADDRSTRLEN)) {
                    return String::create(ip);
                }
                break;
            case AF_INET6:
                if (!uv_ip6_name(
                        reinterpret_cast<struct sockaddr_in6*>(&addr),
                        ip,
                        INET6_ADDRSTRLEN)) {
                    return String::create(ip);
                }
                break;
            }
        }
        LIBJ_NULL_CPTR(String, nullp);
        return nullp;
    }

    Int remotePort() {
        struct sockaddr_storage addr;
        int len = sizeof(addr);
        if (!uv_tcp_getpeername(
                &tcp_,
                reinterpret_cast<struct sockaddr*>(&addr),
                &len)) {
            switch (addr.ss_family) {
            case AF_INET:
                return static_cast<int>(ntohs(
                    reinterpret_cast<struct sockaddr_in*>(&addr)->sin_port));
            case AF_INET6:
                return static_cast<int>(ntohs(
                    reinterpret_cast<struct sockaddr_in6*>(&addr)->sin6_port));
            }
        }
        return -1;
    }

    Boolean setNoDelay(Boolean noDelay) {
        return !uv_tcp_nodelay(&tcp_, noDelay ? 1 : 0);
    }

    Boolean setKeepAlive(Boolean enable, Int initialDelay) {
        if (initialDelay < 0) {
            return false;
        } else {
            return !uv_tcp_keepalive(
                &tcp_, enable ? 1 : 0, initialDelay / 1000);
        }
    }

    void write(Buffer::CPtr buf) {
        LIBJ_NULL_PTR(JsFunction, nullp);
        write(buf, nullp);
    }

    void write(String::CPtr str, String::Encoding enc) {
        Buffer::Ptr buf = Buffer::create(str, enc);
        LIBJ_NULL_PTR(JsFunction, nullp);
        write(buf, nullp);
    }

    void write(String::CPtr str, String::Encoding enc, JsFunction::Ptr cb) {
        Buffer::Ptr buf = Buffer::create(str, enc);
        write(buf, cb);
    }

    void write(Buffer::CPtr buf, JsFunction::Ptr cb) {
        SocketWriteContext* context = new SocketWriteContext();
        context->buffer = buf;
        context->callback = cb;
        uv_stream_t* stream = reinterpret_cast<uv_stream_t*>(&tcp_);
        Int len = buf->length();
        context->uvBuf.base = static_cast<char*>(malloc(len));
        context->uvBuf.len = len;
        strncpy(
            context->uvBuf.base,
            reinterpret_cast<const char*>(buf->data()),
            len);
        int err = uv_write(
            &(context->uvWrite),
            stream,
            &(context->uvBuf),
            1,
            SocketImpl::afterWrite);
        if (err) {
            delete context;
        }
    }

    Boolean end() {
        if (!hasFlag(WRITABLE)) {
            return false;
        } else {
            unsetFlag(WRITABLE);
        }
        if (!hasFlag(READABLE)) {
            return destroySoon();
        } else {
            setFlag(SHUTDOWN);
            // shutdown
            return true;
        }
    }

    Boolean destroy(Boolean hadError) {
        if (hasFlag(DESTROYED))
            return false;
        unsetFlag(READABLE);
        unsetFlag(WRITABLE);
        // connectQueueCleanUp();
        // handle.close();
        // fireErrorCallbacks();
        JsArray::Ptr args = JsArray::create();
        args->add(hadError);
        emit(EVENT_CLOSE, args);
        setFlag(DESTROYED);
        return true;
    }

    Boolean destroy() {
        return destroy(false);
    }

    Boolean destroySoon() {
        if (hasFlag(DESTROYED))
            return false;
        unsetFlag(WRITABLE);
        setFlag(DESTROY_SOON);
        if (pendingWriteReqs_) {
            return true;
        } else {
            return destroy();
        }
    }

    Boolean readable() const {
        return hasFlag(READABLE);
    }

    Boolean writable() const {
        return hasFlag(WRITABLE);
    }

 private:
    static void afterWrite(uv_write_t* write, int status) {
        SocketWriteContext* context =
            static_cast<SocketWriteContext*>(write->data);
        context->status = status;
        write->handle->data = context;
        uv_close(reinterpret_cast<uv_handle_t*>(write->handle), onWriteClose);
    }

    static void onWriteClose(uv_handle_t* handle) {
        SocketWriteContext* context = static_cast<SocketWriteContext*>(handle->data);
        assert(context);
        if (context->callback) {
            JsArray::Ptr args = JsArray::create();
            args->add(static_cast<Int>(context->status));
            (*context->callback)(args);
        }
        delete context;
    }

 private:
    enum Flag {
        READABLE        = 1 << 0,
        WRITABLE        = 1 << 1,
        DESTROYED       = 1 << 2,
        DESTROY_SOON    = 1 << 3,
        SHUTDOWN        = 1 << 4,
    };

    void setFlag(Flag flag) {
        flags_ |= flag;
    }

    void unsetFlag(Flag flag) {
        flags_ &= ~flag;
    }

    Boolean hasFlag(Flag flag) const {
        return flags_ & flag;
    }

 private:
    UInt flags_;
    uv_tcp_t tcp_;
    UInt pendingWriteReqs_;
    EventEmitter::Ptr ee_;

    SocketImpl()
        : flags_(0)
        , pendingWriteReqs_(0)
        , ee_(events::EventEmitter::create()) {
        uv_tcp_init(uv_default_loop(), &tcp_);
    }

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace net
}  // namespace node
}  // namespace libj

#endif  // SRC_NET_SOCKET_IMPL_H_
