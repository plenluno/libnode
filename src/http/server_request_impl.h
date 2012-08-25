// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef SRC_HTTP_SERVER_REQUEST_IMPL_H_
#define SRC_HTTP_SERVER_REQUEST_IMPL_H_

#include "libnode/http/server_request.h"
#include "../net/socket_impl.h"

namespace libj {
namespace node {
namespace http {

class ServerRequestImpl : public ServerRequest {
 private:
    static const String::CPtr METHOD;
    static const String::CPtr URL;
    static const String::CPtr HEADERS;
    static const String::CPtr HTTP_VERSION;

 public:
    typedef LIBJ_PTR(ServerRequestImpl) Ptr;
    typedef LIBJ_CPTR(ServerRequestImpl) CPtr;

    static Ptr create(net::SocketImpl::Ptr sock) {
        Ptr p(new ServerRequestImpl(sock));
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

    net::Socket::Ptr connection() const {
        return socket_;
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

    String::CPtr getHeader(String::CPtr name) const {
        JsObject::Ptr headers = getPtr<JsObject>(HEADERS);
        if (headers) {
            return headers->getCPtr<String>(name);
        } else {
            return String::null();
        }
    }

    void setHttpVersion(String::CPtr httpVersion) {
        put(HTTP_VERSION, httpVersion);
    }

    Boolean destroy() {
        if (socket_) {
            return socket_->destroy();
        } else {
            return false;
        }
    }

    Boolean readable() const {
        if (socket_) {
            return socket_->readable();
        } else {
            return false;
        }
    }

 private:
    net::SocketImpl::Ptr socket_;

    EventEmitter::Ptr ee_;

 public:
    ServerRequestImpl(net::SocketImpl::Ptr sock)
        : socket_(sock)
        , ee_(EventEmitter::create()) {
    }

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // SRC_HTTP_SERVER_REQUEST_IMPL_H_
