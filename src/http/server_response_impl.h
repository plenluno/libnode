// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef SRC_HTTP_SERVER_RESPONSE_IMPL_H_
#define SRC_HTTP_SERVER_RESPONSE_IMPL_H_

#include <libj/string_buffer.h>
#include <string.h>
#include <uv.h>
#include <list>
#include <string>

#include "libnode/http/server_response.h"
#include "libnode/http/status.h"

namespace libj {
namespace node {
namespace http {

class ServerContext;

class ServerResponseImpl : public ServerResponse {
 private:
    static const String::CPtr HEADERS;
    static const String::CPtr STATUS_CODE;

 public:
    typedef LIBJ_PTR(ServerResponseImpl) Ptr;

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
        return flush();
    }

    Boolean write(String::CPtr str, String::Encoding enc) {
        Buffer::Ptr buf = Buffer::create(str, enc);
        return write(buf);
    }

    Boolean end();

    Boolean destroy();

    Boolean destroySoon();

    Boolean writable() const;

 private:
    Boolean flush();

    void makeHeader() {
        static const String::CPtr http11 = String::create("HTTP/1.1 ");
        static const String::CPtr ws = String::create(" ");
        static const String::CPtr nl = String::create("\r\n");
        static const String::CPtr colon = String::create(": ");

        if (hasFlag(HEADER_MADE)) return;

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
        output_->add(0, Buffer::create(header->toString(), String::ASCII));
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
    ServerContext* context_;

    http::Status::CPtr status_;
    JsArray::Ptr output_;

    EventEmitter::Ptr ee_;

 public:
    ServerResponseImpl(ServerContext* context);

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // SRC_HTTP_SERVER_RESPONSE_IMPL_H_
