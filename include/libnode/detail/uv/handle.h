// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_UV_HANDLE_H_
#define LIBNODE_DETAIL_UV_HANDLE_H_

#include <libnode/uv/error.h>

#include <libj/symbol.h>
#include <libj/js_object.h>

#include <assert.h>

namespace libj {
namespace node {
namespace detail {
namespace uv {

class Handle {
 protected:
    Handle(uv_handle_t* handle)
        : handle_(handle)
        , unref_(false) {
        assert(handle_);
        handle_->data = this;
    }

 public:
    virtual ~Handle() {}

    virtual void setHandle(uv_handle_t* handle) {
        handle_ = handle;
        handle_->data = this;
    }

    uv_handle_type type() const { return handle_->type; }

    void ref() {
        uv_ref(handle_);
        unref_ = false;
    }

    void unref() {
        uv_unref(handle_);
        unref_ = true;
    }

    void close() {
        if (handle_) {
            uv_close(handle_, onClose);
            handle_ = NULL;
        }
    }

 public:
    static uv_handle_type guessHandleType(int fd) {
        assert(fd >= 0);
        return uv_guess_handle(fd);
    }

 protected:
    static void setLastError() {
        node::uv::Error::setLast(uv_last_error(uv_default_loop()).code);
    }

    static JsObject::Ptr addressToJs(const sockaddr* addr) {
        LIBJ_STATIC_SYMBOL_DEF(symIpV4,    "IPv4");
        LIBJ_STATIC_SYMBOL_DEF(symIpV6,    "IPv6");
        LIBJ_STATIC_SYMBOL_DEF(symPort,    "port");
        LIBJ_STATIC_SYMBOL_DEF(symFamily,  "family");
        LIBJ_STATIC_SYMBOL_DEF(symAddress, "address");

        char ip[INET6_ADDRSTRLEN];
        Int port;
        const sockaddr_in *a4;
        const sockaddr_in6 *a6;

        JsObject::Ptr res = JsObject::create();
        switch (addr->sa_family) {
        case AF_INET:
            a4 = reinterpret_cast<const sockaddr_in*>(addr);
            uv_inet_ntop(AF_INET, &a4->sin_addr, ip, sizeof ip);
            port = ntohs(a4->sin_port);
            res->put(symFamily, symIpV4);
            break;
        case AF_INET6:
            a6 = reinterpret_cast<const sockaddr_in6*>(addr);
            uv_inet_ntop(AF_INET6, &a6->sin6_addr, ip, sizeof ip);
            port = ntohs(a6->sin6_port);
            res->put(symFamily, symIpV6);
            break;
        default:
            assert(false);
        }
        res->put(symPort, port);
        res->put(symAddress, String::create(ip));
        return res;
    }

 private:
    static void onClose(uv_handle_t* handle) {
        Handle* self = static_cast<Handle*>(handle->data);
        assert(self->handle_ == NULL);
        delete self;
    }

 protected:
    uv_handle_t* handle_;
    bool unref_;
};

}  // namespace uv
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_UV_HANDLE_H_
