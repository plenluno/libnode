// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_SERVER_RESPONSE_IMPL_H_
#define LIBNODE_SRC_HTTP_SERVER_RESPONSE_IMPL_H_

#include <libj/string_buffer.h>
#include <string.h>
#include <uv.h>
#include <list>
#include <string>

#include "libnode/http/server_response.h"
#include "libnode/http/status.h"
#include "../net/socket_impl.h"

namespace libj {
namespace node {
namespace http {

class ServerResponseImpl : public ServerResponse {
 private:
    static const String::CPtr HEADERS;
    static const String::CPtr STATUS_CODE;

    class SocketEnd : LIBJ_JS_FUNCTION(SocketEnd)
     public:
        static Ptr create(
            ServerResponseImpl* res,
            net::SocketImpl::Ptr sock) {
            Ptr p(new SocketEnd(res, sock));
            return p;
        }

        Value operator()(JsArray::Ptr args) {
            response_->removeAllListeners(EVENT_CLOSE);
            socket_->end();
            return libj::Status::OK;
        }

     private:
        ServerResponseImpl* response_;
        net::SocketImpl::Ptr socket_;

        SocketEnd(
            ServerResponseImpl* res,
            net::SocketImpl::Ptr sock)
            : response_(res)
            , socket_(sock) {}
    };

 public:
    typedef LIBJ_PTR(ServerResponseImpl) Ptr;
    typedef LIBJ_CPTR(ServerResponseImpl) CPtr;

    static Ptr create(net::SocketImpl::Ptr sock) {
        Ptr p(new ServerResponseImpl(sock));
        return p;
    }

    Boolean writeHead(Int statusCode) {
        status_ = http::Status::create(statusCode);
        if (status_) {
            put(STATUS_CODE, statusCode);
            return true;
        } else {
            return false;
        }
    }

    Boolean writeHead(Int statusCode, String::CPtr reasonPhrase) {
        status_ = http::Status::create(statusCode, reasonPhrase);
        if (status_) {
            put(STATUS_CODE, statusCode);
            return true;
        } else {
            return false;
        }
    }

    Int statusCode() const {
        if (status_) {
            return status_->code();
        } else {
            return 0;
        }
    }

    JsObject::Ptr getHeaders() {
        JsObject::Ptr headers = getPtr<JsObject>(HEADERS);
        if (!headers) {
            headers = JsObject::create();
            put(HEADERS, headers);
        }
        return headers;
    }

    void setHeader(String::CPtr name, String::CPtr value) {
        getHeaders()->put(name, value);
    }

    String::CPtr getHeader(String::CPtr name) const {
        JsObject::Ptr headers = getPtr<JsObject>(HEADERS);
        if (headers) {
            Value v = headers->get(name);
            return toCPtr<String>(v);
        } else {
            return String::null();
        }
    }

    void removeHeader(String::CPtr name) {
        getHeaders()->remove(name);
    }

    Boolean write(Buffer::CPtr buf) {
        if (hasFlag(FINISHED))
            return false;
        if (!hasFlag(HEADER_SENT))
            makeHeader();
        output_->add(buf);
        return flush(JsFunction::null());
    }

    Boolean write(String::CPtr str, Buffer::Encoding enc) {
        Buffer::Ptr buf = Buffer::create(str, enc);
        return write(buf);
    }

    Boolean end() {
        if (hasFlag(FINISHED)) return false;

        if (!socket_) {
            setFlag(FINISHED);
            return false;
        }

        if (!hasFlag(HEADER_SENT)) makeHeader();

        // force a flush even if there is no data to be sent
        // cause event 'close' if the socket has been already closed
        flush(SocketEnd::create(this, socket_));

        setFlag(FINISHED);
        return true;
    }

    Boolean destroy() {
        if (socket_) {
            return socket_->destroy();
        } else {
            return false;
        }
    }

    Boolean destroySoon() {
        if (socket_) {
            return socket_->destroySoon();
        } else {
            return false;
        }
    }

    Boolean writable() const {
        if (socket_) {
            return socket_->writable();
        } else {
            return false;
        }
    }

 private:
    Boolean flush(JsFunction::Ptr cb) {
        if (!socket_) return false;

        Buffer::Ptr buf = Buffer::concat(output_);
        socket_->write(buf, cb);
        output_->clear();

        assert(hasFlag(HEADER_MADE));
        setFlag(HEADER_SENT);
        return true;
    }

    void makeHeader() {
        static const String::CPtr http11 = String::create("HTTP/1.1 ");
        static const String::CPtr ws = String::create(" ");
        static const String::CPtr nl = String::create("\r\n");
        static const String::CPtr colon = String::create(": ");
        static const String::CPtr close = String::create("close");

        if (hasFlag(HEADER_MADE)) return;

        setHeader(HEADER_CONNECTION, close);

        StringBuffer::Ptr header = StringBuffer::create();
        header->append(http11);
        if (!status_)
            status_ = http::Status::create(200);
        header->append(String::valueOf(status_->code()));
        header->append(ws);
        header->append(status_->toString());
        header->append(nl);

        JsObject::Ptr headers = getHeaders();
        Set::CPtr ks = headers->keySet();
        Iterator::Ptr itr = ks->iterator();
        while (itr->hasNext()) {
            String::CPtr name = toCPtr<String>(itr->next());
            String::CPtr value = toCPtr<String>(headers->get(name));
            header->append(name);
            header->append(colon);
            header->append(value);
            header->append(nl);
        }
        header->append(nl);
        output_->add(0, Buffer::create(header->toString()));
        setFlag(HEADER_MADE);
    }

 private:
    enum Flag {
        HEADER_MADE = 1 << 0,
        HEADER_SENT = 1 << 1,
        FINISHED    = 1 << 2,
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
    net::SocketImpl::Ptr socket_;

    http::Status::CPtr status_;
    JsArray::Ptr output_;

    EventEmitter::Ptr ee_;

 public:
    ServerResponseImpl(net::SocketImpl::Ptr sock)
        : flags_(0)
        , socket_(sock)
        , status_(http::Status::null())
        , output_(JsArray::create())
        , ee_(EventEmitter::create()) {}

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_SERVER_RESPONSE_IMPL_H_
