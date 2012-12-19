// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_INCOMING_MESSAGE_H_
#define LIBNODE_SRC_HTTP_INCOMING_MESSAGE_H_

#include <assert.h>
#include <libj/linked_list.h>

#include "libnode/http/header.h"
#include "libnode/process.h"
#include "libnode/stream/readable_stream.h"
#include "libnode/string_decoder.h"

#include "../flag.h"
#include "../net/socket_impl.h"

namespace libj {
namespace node {
namespace http {

class OutgoingMessage;

class IncomingMessage
    : public FlagMixin
    , LIBNODE_READABLE_STREAM(IncomingMessage)
 public:
    static Ptr create(net::SocketImpl::Ptr sock) {
        if (sock) {
            return Ptr(new IncomingMessage(sock));
        } else {
            return null();
        }
    }

    net::Socket::Ptr socket() const {
        return socket_;
    }

    net::Socket::Ptr connection() const {
        return socket_;
    }

    Int statusCode() const {
        return statusCode_;
    }

    String::CPtr httpVersion() const {
        return httpVersion_;
    }

    JsObject::CPtr headers() const {
        return headers_;
    }

    JsObject::CPtr trailers() const {
        return trailers_;
    }

    String::CPtr url() const {
        return url_;
    }

    String::CPtr method() const {
        return method_;
    }

    Boolean readable() const {
        return hasFlag(READABLE);
    }

    Boolean pause() {
        setFlag(PAUSED);
        return socket_ && socket_->pause();
    }

    Boolean resume() {
        unsetFlag(PAUSED);
        Boolean res = socket_ && socket_->resume();
        emitPending();
        return res;
    }

    Boolean destroy() {
        return socket_ && socket_->destroy();
    }

    Boolean setEncoding(Buffer::Encoding enc) {
        decoder_ = StringDecoder::create(enc);
        return !!decoder_;
    }

 public:
    Int httpVersionMajor() const {
        return httpVersionMajor_;
    }

    Int httpVersionMinor() const {
        return httpVersionMinor_;
    }

    void setHttpVersion(String::CPtr version) {
        httpVersion_ = version;
    }

    void setHttpVersionMajor(Int major) {
        httpVersionMajor_ = major;
    }

    void setHttpVersionMinor(Int minor) {
        httpVersionMinor_ = minor;
    }

    void setStatusCode(Int statusCode) {
        statusCode_ = statusCode;
    }

    String::CPtr getHeader(String::CPtr name) const {
        return headers_->getCPtr<String>(name);
    }

    void setHeader(String::CPtr name, String::CPtr value) {
        headers_->put(name->toLowerCase(), value);
    }

    void addHeaderLine(String::CPtr name, String::CPtr value) {
        LIBJ_STATIC_SYMBOL_DEF(symExtPrefix, "x-");

        static Set::Ptr commaSeparated = Set::null();
        if (!commaSeparated) {
            commaSeparated = Set::create();
            commaSeparated->add(LHEADER_ACCEPT);
            commaSeparated->add(LHEADER_ACCEPT_CHARSET);
            commaSeparated->add(LHEADER_ACCEPT_ENCODING);
            commaSeparated->add(LHEADER_ACCEPT_LANGUAGE);
            commaSeparated->add(LHEADER_CONNECTION);
            commaSeparated->add(LHEADER_COOKIE);
            commaSeparated->add(LHEADER_PRAGMA);
            commaSeparated->add(LHEADER_LINK);
            commaSeparated->add(LHEADER_WWW_AUTHENTICATE);
            commaSeparated->add(LHEADER_PROXY_AUTHENTICATE);
            commaSeparated->add(LHEADER_SEC_WEBSOCKET_EXTENSIONS);
            commaSeparated->add(LHEADER_SEC_WEBSOCKET_PROTOCOL);
        }

        assert(name && value);

        JsObject::Ptr dest = hasFlag(COMPLETE) ? trailers_ : headers_;
        String::CPtr field = name->toLowerCase();
        if (field->equals(LHEADER_SET_COOKIE)) {
            JsArray::Ptr vals = headers_->getPtr<JsArray>(field);
            if (vals) {
                vals->add(value);
            } else {
                JsArray::Ptr vals = JsArray::create();
                vals->add(value);
                dest->put(field, vals);
            }
        } else if (commaSeparated->contains(field) ||
                   field->startsWith(symExtPrefix)) {
            String::CPtr vals = headers_->getCPtr<String>(field);
            if (vals) {
                StringBuffer::Ptr sb = StringBuffer::create();
                sb->append(vals);
                sb->appendCStr(", ");
                sb->append(value);
                dest->put(field, sb->toString());
            } else {
                dest->put(field, value);
            }
        } else {
            dest->put(field, value);
        }
    }

    void setUrl(String::CPtr url) {
        url_ = url;
    }

    void setMethod(String::CPtr method) {
        method_ = method;
    }

    Buffer::Encoding encoding() const {
        return decoder_ ? decoder_->encoding() : Buffer::NONE;
    }

    OutgoingMessage* req() {
        return req_;
    }

    void setReq(OutgoingMessage* req) {
        req_ = req;
    }

    LinkedList::Ptr pendings() const {
        return pendings_;
    }

    void emitPending(JsFunction::Ptr callback = JsFunction::null()) {
        if (pendings_->isEmpty()) {
            if (callback) (*callback)();
        } else {
            JsFunction::Ptr emit(new EmitPending(this, callback));
            process::nextTick(emit);
        }
    }

    void emitData(Buffer::CPtr buf) {
        if (decoder_) {
            String::CPtr str = decoder_->write(buf);
            if (!str->isEmpty()) {
                emit(EVENT_DATA, str);
            }
        } else {
            emit(EVENT_DATA, buf);
        }
    }

    void emitEnd() {
        if (!hasFlag(END_EMITTED)) {
            emit(EVENT_END);
            setFlag(END_EMITTED);
            // 'end' only once
            removeAllListeners(EVENT_END);
            // no 'data' after 'end'
            removeAllListeners(EVENT_DATA);
        }
    }

 public:
    typedef enum {
        COMPLETE    = 1 << 0,
        READABLE    = 1 << 1,
        PAUSED      = 1 << 2,
        END_EMITTED = 1 << 3,
        UPGRADE     = 1 << 4,
    } Flag;

 private:
    class EmitPending : LIBJ_JS_FUNCTION(EmitPending)
     public:
        EmitPending(
            IncomingMessage* incoming,
            JsFunction::Ptr callback)
            : self_(incoming)
            , callback_(callback) {}

        virtual Value operator()(JsArray::Ptr args) {
            LinkedList::Ptr pendings = self_->pendings_;
            assert(pendings);
            while (!self_->hasFlag(PAUSED) && pendings->length()) {
                Buffer::Ptr chunk = toPtr<Buffer>(pendings->shift());
                if (chunk) {
                    assert(Buffer::isBuffer(chunk));
                    self_->emitData(chunk);
                } else {
                    assert(pendings->isEmpty());
                    self_->unsetFlag(READABLE);
                    self_->emitEnd();
                }
            }
            if (callback_) (*callback_)();
            return libj::Status::OK;
        }

     private:
        IncomingMessage* self_;
        JsFunction::Ptr callback_;
    };

 private:
    net::SocketImpl::Ptr socket_;
    String::CPtr method_;
    String::CPtr url_;
    String::CPtr httpVersion_;
    Int httpVersionMajor_;
    Int httpVersionMinor_;
    Int statusCode_;
    JsObject::Ptr headers_;
    JsObject::Ptr trailers_;
    LinkedList::Ptr pendings_;
    StringDecoder::Ptr decoder_;
    OutgoingMessage* req_;
    EventEmitter::Ptr ee_;

    IncomingMessage(net::SocketImpl::Ptr sock)
        : socket_(sock)
        , method_(String::null())
        , url_(String::create())
        , httpVersion_(String::null())
        , httpVersionMajor_(0)
        , httpVersionMinor_(0)
        , statusCode_(0)
        , headers_(JsObject::create())
        , trailers_(JsObject::create())
        , pendings_(LinkedList::create())
        , decoder_(StringDecoder::null())
        , req_(NULL)
        , ee_(EventEmitter::create()) {
        setFlag(READABLE);
    }

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_INCOMING_MESSAGE_H_
