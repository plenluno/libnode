// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_NET_SOCKET_IMPL_H_
#define LIBNODE_SRC_NET_SOCKET_IMPL_H_

#include <assert.h>
#include <uv.h>
#include <cstring>
#include <vector>

#include "libnode/error.h"
#include "libnode/net/socket.h"

namespace libj {
namespace node {
namespace net {

class SocketImpl;

class SocketWriteContext {
 public:
    uv_write_t uvWrite;
    uv_buf_t uvBuf;
    SocketImpl* socket;
    Buffer::CPtr buffer;
    JsFunction::Ptr callback;

    SocketWriteContext(SocketImpl* sock)
        : socket(sock)
        , buffer(Buffer::null())
        , callback(JsFunction::null()) {
        uvWrite.data = this;
        uvBuf.base = NULL;
    }

    ~SocketWriteContext() {
        free(uvBuf.base);
    }
};

class SocketShutdownContext {
 public:
    uv_shutdown_t uvShutdown;
    SocketImpl* socket;

    SocketShutdownContext(SocketImpl* sock)
        : socket(sock) {
        uvShutdown.data = this;
    }
};

class SocketImpl : public Socket {
 public:
    typedef LIBJ_PTR(SocketImpl) Ptr;

    Boolean instanceof(libj::TypeId id) const {
        return id == libj::Type<SocketImpl>::id()
            || Socket::instanceof(id);
    }

    static Ptr create() {
        Ptr p(new SocketImpl());
        return p;
    }

    uv_tcp_t* getUvTcp() { return tcp_; }

    JsObject::Ptr address() {
        static const String::CPtr strPort = String::create("port");
        static const String::CPtr strFamily = String::create("family");
        static const String::CPtr strAddress = String::create("address");
        static const String::CPtr strIpV4 = String::create("IPv4");
        static const String::CPtr strIpV6 = String::create("IPv6");

        struct sockaddr_storage addr;
        int len = sizeof(addr);
        if (!uv_tcp_getsockname(
                tcp_,
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
        return JsObject::null();
    }

    String::CPtr remoteAddress() {
        struct sockaddr_storage addr;
        int len = sizeof(addr);
        if (!uv_tcp_getpeername(
                tcp_,
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
        return String::null();
    }

    Int remotePort() {
        struct sockaddr_storage addr;
        int len = sizeof(addr);
        if (!uv_tcp_getpeername(
                tcp_,
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
        return !uv_tcp_nodelay(tcp_, noDelay ? 1 : 0);
    }

    Boolean setKeepAlive(Boolean enable, Int initialDelay) {
        if (initialDelay < 0) {
            return false;
        } else {
            return !uv_tcp_keepalive(
                tcp_, enable ? 1 : 0, initialDelay / 1000);
        }
    }

    Boolean write(Buffer::CPtr buf) {
        return write(buf, JsFunction::null());
    }

    Boolean write(String::CPtr str, Buffer::Encoding enc) {
        Buffer::Ptr buf = Buffer::create(str, enc);
        return write(buf, JsFunction::null());
    }

    Boolean write(String::CPtr str, Buffer::Encoding enc, JsFunction::Ptr cb) {
        Buffer::Ptr buf = Buffer::create(str, enc);
        return write(buf, cb);
    }

    Boolean write(Buffer::CPtr buf, JsFunction::Ptr cb) {
        if (!tcp_) return false;

        SocketWriteContext* context = new SocketWriteContext(this);
        context->buffer = buf;
        context->callback = cb;
        uv_stream_t* stream = reinterpret_cast<uv_stream_t*>(tcp_);
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
            afterWrite);
        assert(!err);
        return true;
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
            SocketShutdownContext* context = new SocketShutdownContext(this);
            uv_stream_t* stream = reinterpret_cast<uv_stream_t*>(tcp_);
            int err = uv_shutdown(
                &(context->uvShutdown),
                stream,
                afterShutdown);
            assert(!err);
            return true;
        }
    }

    Boolean destroy() {
        return destroy(Error::null());
    }

    Boolean destroy(libj::Error::CPtr err) {
        return destroy(err, JsFunction::null());
    }

    Boolean destroySoon() {
        if (hasFlag(DESTROYED))
            return false;

        unsetFlag(WRITABLE);
        setFlag(DESTROY_SOON);
        return destroy();
    }

    Boolean readable() const {
        return hasFlag(READABLE);
    }

    Boolean writable() const {
        return hasFlag(WRITABLE);
    }

    void stopReading() {
        unsetFlag(READABLE);
    }

    void stopWriting() {
        unsetFlag(WRITABLE);
    }

    Boolean hasEncoding() const {
        return enc_ != Buffer::NONE;
    }

    Buffer::Encoding getEncoding() const {
        return enc_;
    }

    Boolean setEncoding(Buffer::Encoding enc) {
        switch (enc) {
        case Buffer::UTF8:
        case Buffer::UTF16BE:
        case Buffer::UTF16LE:
        case Buffer::UTF32BE:
        case Buffer::UTF32LE:
        case Buffer::HEX:
        case Buffer::NONE:
            enc_ = enc;
            return true;
        default:
            return false;
        }
    }

 private:
    Boolean destroy(libj::Error::CPtr err, JsFunction::Ptr cb) {
        if (hasFlag(DESTROYED))
            return false;

        unsetFlag(READABLE);
        unsetFlag(WRITABLE);

        if (err)
            setFlag(HAD_ERROR);
        if (tcp_) {
            uv_handle_t* handle = reinterpret_cast<uv_handle_t*>(tcp_);
            assert(handle->data == this);
            uv_close(handle, afterClose);
        }

        setFlag(DESTROYED);
        return true;
    }

    static void afterClose(uv_handle_t* handle) {
        SocketImpl* socket = static_cast<SocketImpl*>(handle->data);
        assert(socket && socket->tcp_);
        delete socket->tcp_;
        socket->tcp_ = NULL;

        JsArray::Ptr args = JsArray::create();
        args->add(socket->hasFlag(HAD_ERROR));
        socket->emit(Socket::EVENT_CLOSE, args);
    }

    static void afterWrite(uv_write_t* write, int status) {
        SocketWriteContext* context =
            static_cast<SocketWriteContext*>(write->data);
        assert(context && context->socket);
        SocketImpl* socket = context->socket;
        JsFunction::Ptr callback = context->callback;
        delete context;

        if (socket->hasFlag(DESTROYED)) return;

        if (status) {
            uv_err_t err = uv_last_error(uv_default_loop());
            socket->destroy(Error::valueOf(err.code));
            return;
        }

        if (callback) {
            (*callback)(JsArray::create());
        }
    }

    static void afterShutdown(uv_shutdown_t* req, int status) {
        SocketShutdownContext* context =
            static_cast<SocketShutdownContext*>(req->data);
        assert(context && context->socket);
        SocketImpl* socket = context->socket;
        delete context;

        if (!socket->hasFlag(DESTROYED) && !socket->hasFlag(READABLE)) {
            socket->destroy();
        }
    }

 private:
    enum Flag {
        READABLE        = 1 << 0,
        WRITABLE        = 1 << 1,
        DESTROYED       = 1 << 2,
        DESTROY_SOON    = 1 << 3,
        SHUTDOWN        = 1 << 4,
        HAD_ERROR       = 1 << 5,
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
    uv_tcp_t* tcp_;
    Buffer::Encoding enc_;
    EventEmitter::Ptr ee_;

    SocketImpl()
        : flags_(0)
        , tcp_(new uv_tcp_t)
        , enc_(Buffer::NONE)
        , ee_(events::EventEmitter::create()) {
        uv_tcp_init(uv_default_loop(), tcp_);
        tcp_->data = this;
        setFlag(READABLE);
        setFlag(WRITABLE);
    }

 public:
    ~SocketImpl() {
        delete tcp_;
    }

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace net
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_NET_SOCKET_IMPL_H_
