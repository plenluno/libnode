// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_OUTGOING_MESSAGE_H_
#define LIBNODE_SRC_HTTP_OUTGOING_MESSAGE_H_

#include <assert.h>
#include <stdio.h>
#include <libj/typed_linked_list.h>

#include "libnode/http.h"
#include "libnode/stream/writable_stream.h"

#include "../flag.h"
#include "../net/socket_impl.h"

namespace libj {
namespace node {
namespace http {

class OutgoingMessage
    : public FlagMixin
    , LIBNODE_WRITABLE_STREAM(OutgoingMessage)
 public:
    static Ptr create() {
        return Ptr(new OutgoingMessage());
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

    Boolean setHeader(String::CPtr name, String::CPtr value) {
        if (header_ && !header_->isEmpty()) return false;

        if (name && value) {
            String::CPtr key = name->toLowerCase();
            headers_->put(key, value);
            headerNames_->put(key, name);
            return true;
        } else {
            return false;
        }
    }

    String::CPtr getHeader(String::CPtr name) const {
        if (name) {
            return headers_->getCPtr<String>(name->toLowerCase());
        } else {
            return String::null();
        }
    }

    void removeHeader(String::CPtr name) {
        if (!name) return;

        String::CPtr key = name->toLowerCase();
        headers_->remove(key);
        headerNames_->remove(key);
    }

    Boolean write(const Value& chunk, Buffer::Encoding enc) {
        LIBJ_STATIC_SYMBOL_DEF(symCRLF, "\r\n");

        if (!header_ || header_->isEmpty()) {
            implicitHeader();
        }

        if (!hasFlag(HAS_BODY)) {
            return true;
        }

        String::CPtr str = toCPtr<String>(chunk);
        Buffer::CPtr buf = toCPtr<Buffer>(chunk);
        if (str) {
            if (str->isEmpty()) return false;
        } else if (buf) {
            if (buf->isEmpty()) return false;
        } else {
            return false;
        }

        if (hasFlag(CHUNKED_ENCODING)) {
            if (str) {
                Size len = Buffer::byteLength(str, enc);
                StringBuffer::Ptr data = StringBuffer::create();
                data->append(toHex(len));
                data->appendCStr("\r\n");
                data->append(str);
                data->appendCStr("\r\n");
                return send(data->toString(), enc);
            } else {
                Size len = buf->length();
                send(toHex(len)->concat(symCRLF));
                send(buf);
                return send(symCRLF);
            }
        } else {
            return send(chunk, enc);
        }
    }

    Boolean end(const Value& data, Buffer::Encoding enc) {
        if (hasFlag(FINISHED)) return false;

        if (!header_ || header_->isEmpty()) {
            implicitHeader();
        }

        Value d;
        if (hasFlag(HAS_BODY)) {
            d = UNDEFINED;
        } else {
            d = data;
        }

        String::CPtr str = toCPtr<String>(d);
        Boolean hot =
            !hasFlag(HEADER_SENT) &&
            str && !str->isEmpty() &&
            output_->isEmpty() &&
            socket_ && socket_->writable() &&
            socket_->httpMessage() == this;

        Boolean ret;
        if (hot) {
            if (hasFlag(CHUNKED_ENCODING)) {
                Size len = Buffer::byteLength(str, enc);
                StringBuffer::Ptr chunk = StringBuffer::create();
                chunk->append(header_);
                chunk->append(toHex(len));
                chunk->appendCStr("\r\n");
                chunk->append(str);
                chunk->appendCStr("\r\n0\r\n");
                chunk->append(trailer_);
                chunk->appendCStr("\r\n");
                ret = socket_->write(chunk->toString(), enc);
            } else {
                ret = socket_->write(header_->concat(str), enc);
            }
            setFlag(HEADER_SENT);
        } else if (!d.isUndefined()) {
            ret = write(d, enc);
        }

        if (!hot) {
            if (hasFlag(CHUNKED_ENCODING)) {
                StringBuffer::Ptr chunk = StringBuffer::create();
                chunk->appendCStr("0\r\n");
                chunk->append(trailer_);
                chunk->appendCStr("\r\n");
                ret = send(chunk->toString());
            } else {
                ret = send(String::create());
            }
        }

        setFlag(FINISHED);
        if (output_->length() == 0 && socket_->httpMessage() == this) {
            finish();
        }
        return ret;
    }

    virtual void writeHead(
        Int statusCode,
        String::CPtr reasonPhrase = String::null(),
        JsObject::CPtr obj = JsObject::null()) {
        statusCode_ = statusCode;
        Status::CPtr status = Status::create(statusCode, reasonPhrase);

        JsObject::CPtr headers;
        if (obj && headers_) {
            JsObject::Ptr hs = renderHeaders();
            Set::CPtr keys = obj->keySet();
            Iterator::Ptr itr = keys->iterator();
            while (itr->hasNext()) {
                Value key = itr->next();
                hs->put(key, obj->get(key));
            }
            headers = hs;
        } else if (headers_) {
            headers = renderHeaders();
        } else {
            headers = obj;
        }

        StringBuffer::Ptr statusLine = StringBuffer::create();
        statusLine->appendCStr("HTTP/1.1 ");
        statusLine->append(status->code());
        statusLine->appendChar(' ');
        statusLine->append(status->message());
        statusLine->appendCStr("\r\n");

        if (statusCode == 204 || statusCode == 304 ||
            (statusCode >= 100 && statusCode < 200)) {
            unsetFlag(HAS_BODY);
        }

        if (hasFlag(EXPECT_CONTINUE) && !hasFlag(SENT_100)) {
            unsetFlag(SHOULD_KEEP_ALIVE);
        }

        storeHeader(statusLine->toString(), headers);
    }

    Int statusCode() const {
        return statusCode_;
    }

    void assignSocket(Ptr self, net::SocketImpl::Ptr socket) {
        LIBJ_STATIC_SYMBOL_DEF(symHttpMessage, "httpMessage");

        assert(!socket->httpMessage());
        socket->setHttpMessage(this);
        socket->put(symHttpMessage, self);
        socketOnClose_->setSocket(&(*socket));
        socket->on(net::Socket::EVENT_CLOSE, socketOnClose_);
        socket_ = socket;
        flush();
    }

    void detachSocket(net::SocketImpl::Ptr socket) {
        LIBJ_STATIC_SYMBOL_DEF(symHttpMessage, "httpMessage");

        assert(socket->httpMessage() == this);
        socketOnClose_->setSocket(NULL);
        socket->removeListener(net::Socket::EVENT_CLOSE, socketOnClose_);
        socket->setHttpMessage(NULL);
        socket->remove(symHttpMessage);
        socket_ = net::SocketImpl::null();
    }

    void writeContinue() {
        static const String::CPtr header =
            String::create("HTTP/1.1 100 Continue\r\n\r\n");
        writeRaw(header, Buffer::UTF8);
        setFlag(SENT_100);
    }

 private:
    class SocketOnClose : LIBJ_JS_FUNCTION(SocketOnClose)
     public:
        static Ptr create() {
            return Ptr(new SocketOnClose());
        }

        Value operator()(JsArray::Ptr args) {
            assert(socket_);
            socket_->httpMessage()->emit(OutgoingMessage::EVENT_CLOSE);
            return libj::Status::OK;
        }

        void setSocket(net::SocketImpl* socket) {
            socket_ = socket;
        }

     private:
        net::SocketImpl* socket_;

        SocketOnClose() : socket_(NULL) {}
    };

    String::CPtr toHex(Size val) {
        Size n;
        to<Size>(val, &n);
        const Size kLen = ((sizeof(Size) << 3) / 3) + 3;
        char s[kLen];
        snprintf(s, kLen, "%zx", n);
        return String::create(s);
    }

    Boolean send(const Value& data, Buffer::Encoding enc = Buffer::NONE) {
        if (hasFlag(HEADER_SENT)) {
            return writeRaw(data, enc);
        } else {
            setFlag(HEADER_SENT);
            String::CPtr str = toCPtr<String>(data);
            if (str) {
                assert(header_);
                return writeRaw(header_->concat(str), enc);
            } else {
                output_->add(0, header_);
                outputEncodings_->addTyped(0, Buffer::UTF8);
                return writeRaw(data, enc);
            }
        }
    }

    Boolean writeRaw(const Value& data, Buffer::Encoding enc) {
        String::CPtr str = toCPtr<String>(data);
        Buffer::CPtr buf = toCPtr<Buffer>(data);
        if (str) {
            if (str->isEmpty()) return true;
        } else if (buf) {
            if (buf->isEmpty()) return true;
        } else {
            return false;
        }

        if (socket_ &&
            socket_->httpMessage() == this &&
            socket_->writable()) {
            while (output_->length()) {
                if (!socket_->writable()) {
                    buffer(data, enc);
                    return false;
                }
                Value c = output_->remove(0);
                Buffer::Encoding e = outputEncodings_->removeTyped(0);
                socket_->write(c, e);
            }
            return socket_->write(data, enc);
        } else {
            buffer(data, enc);
            return false;
        }
    }

    void buffer(const Value& data, Buffer::Encoding enc) {
        String::CPtr str = toCPtr<String>(data);
        if (str && str->isEmpty()) return;

        Buffer::CPtr buf = toCPtr<Buffer>(data);
        if (buf && buf->isEmpty()) return;

        Size length = output_->length();

        if (!length || !str) {
            output_->add(data);
            outputEncodings_->addTyped(enc);
            return;
        }

        Buffer::Encoding lastEnc = outputEncodings_->getTyped(length - 1);
        Value lastData = output_->get(length - 1);

        if ((enc != Buffer::NONE && lastEnc == enc) ||
            (enc == Buffer::NONE && data.type() == lastData.type())) {
            if (str) {
                output_->set(
                    length - 1,
                    toCPtr<String>(lastData)->concat(str));
            } else if (buf) {
                output_->set(
                    length -1,
                    toCPtr<Buffer>(lastData)->concat(buf));
            } else {
                assert(false);
            }
        }

        output_->add(data);
        outputEncodings_->addTyped(enc);
    }

    void storeHeader(String::CPtr firstLine, JsObject::CPtr headers) {
        LIBJ_STATIC_SYMBOL_DEF(symClose,     "close");
        LIBJ_STATIC_SYMBOL_DEF(symChunked,   "chunked");

        Boolean sentConnectionHeader = false;
        Boolean sentContentLengthHeader = false;
        Boolean sentTransferEncodingHeader = false;
        Boolean sentDateHeader = false;
        Boolean sentExpect = false;

        StringBuffer::Ptr messageHader = StringBuffer::create();
        messageHader->append(firstLine);
        String::CPtr field;
        Value value;

        #define LIBNODE_OUTMSG_STORE(F, V) { \
            messageHader->append(F); \
            messageHader->appendCStr(": "); \
            messageHader->append(V); \
            messageHader->appendCStr("\r\n"); \
            String::CPtr lowerField = F->toLowerCase(); \
            if (lowerField->equals(LHEADER_CONNECTION)) { \
                sentConnectionHeader = true; \
                if (V.equals(symClose)) { \
                    setFlag(LAST); \
                } else { \
                    setFlag(SHOULD_KEEP_ALIVE); \
                } \
            } else if (lowerField->equals(LHEADER_TRANSFER_ENCODING)) { \
                sentTransferEncodingHeader = true; \
                if (V.equals(symChunked)) { \
                    setFlag(CHUNKED_ENCODING); \
                } \
            } else if (lowerField->equals(LHEADER_CONTENT_LENGTH)) { \
                sentContentLengthHeader = true; \
            } else if (lowerField->equals(LHEADER_DATE)) { \
                sentDateHeader = true; \
            } else if (lowerField->equals(LHEADER_EXPECT)) { \
                sentExpect = true; \
            } \
        }

        if (headers) {
            Set::CPtr keys = headers->keySet();
            Iterator::Ptr itr = keys->iterator();
            while (itr->hasNext()) {
                field = toCPtr<String>(itr->next());
                assert(field);
                value = headers->get(field);

                JsArray::CPtr ary = toCPtr<JsArray>(value);
                if (ary) {
                    Size len = ary->length();
                    for (Size i = 0; i < len; i++) {
                        Value v = ary->get(i);
                        LIBNODE_OUTMSG_STORE(field, v);
                    }
                } else {
                    LIBNODE_OUTMSG_STORE(field, value);
                }
            }
        }

        #undef LIBNODE_OUTMSG_STORE

        // TODO(plenluno): date header

        if (!sentConnectionHeader) {
            Boolean shouldSendKeepAlive =
                hasFlag(SHOULD_KEEP_ALIVE) &&
                (sentContentLengthHeader ||
                 hasFlag(USE_CHUNKED_ENCODING_BY_DEFAULT) ||
                 true);  // this.agent
            messageHader->append(HEADER_CONNECTION);
            messageHader->appendCStr(": ");
            if (shouldSendKeepAlive) {
                messageHader->appendCStr("keep-alive");
            } else {
                setFlag(LAST);
                messageHader->appendCStr("close");
            }
            messageHader->appendCStr("\r\n");
        }

        if (!sentContentLengthHeader && !sentTransferEncodingHeader) {
            if (hasFlag(HAS_BODY)) {
                if (hasFlag(USE_CHUNKED_ENCODING_BY_DEFAULT)) {
                    messageHader->append(HEADER_TRANSFER_ENCODING);
                    messageHader->appendCStr(": chunked\r\n");
                    setFlag(CHUNKED_ENCODING);
                } else {
                    setFlag(LAST);
                }
            } else {
                unsetFlag(CHUNKED_ENCODING);
            }
        }

        messageHader->appendCStr("\r\n");
        header_ = messageHader->toString();
        unsetFlag(HEADER_SENT);

        if (sentExpect) send(String::create());
    }

    JsObject::Ptr renderHeaders() {
        JsObject::Ptr headers = JsObject::create();
        Set::CPtr keys = headers_->keySet();
        Iterator::Ptr itr = keys->iterator();
        while (itr->hasNext()) {
            String::CPtr key = toCPtr<String>(itr->next());
            headers->put(headerNames_->get(key), headers_->get(key));
        }
        return headers;
    }

    void finish() {
        LIBJ_STATIC_SYMBOL_DEF(EVENT_FINISH, "finish");
        assert(socket_);
        emit(EVENT_FINISH);
    }

    void flush() {
        if (!socket_) return;

        Boolean ret = false;
        while (output_->length()) {
            if (!socket_->writable()) return;

            Value data = output_->remove(0);
            Buffer::Encoding enc = outputEncodings_->removeTyped(0);
            ret = socket_->write(data, enc);
        }

        if (hasFlag(FINISHED)) {
            finish();
        } else if (ret) {
            emit(EVENT_DRAIN);
        }
    }

    void implicitHeader() {
        if (hasFlag(SERVER_RESPONSE)) {
            writeHead(statusCode_);
        } else {
            assert(method_ && path_);
            StringBuffer::Ptr sb = StringBuffer::create();
            sb->append(method_);
            sb->appendChar(' ');
            sb->append(path_);
            sb->appendCStr(" HTTP/1.1\r\n");
            String::CPtr firstLine = sb->toString();
            storeHeader(firstLine, renderHeaders());
        }
    }

 public:
    enum Flag {
        WRITABLE                        = 1 << 0,
        LAST                            = 1 << 1,
        CHUNKED_ENCODING                = 1 << 2,
        SHOULD_KEEP_ALIVE               = 1 << 3,
        USE_CHUNKED_ENCODING_BY_DEFAULT = 1 << 4,
        SEND_DATE                       = 1 << 5,
        HAS_BODY                        = 1 << 6,
        FINISHED                        = 1 << 7,
        HEADER_SENT                     = 1 << 8,
        EXPECT_CONTINUE                 = 1 << 9,
        SENT_100                        = 1 << 10,
        SERVER_RESPONSE                 = 1 << 11,
    };

 private:
    typedef TypedLinkedList<Buffer::Encoding> EncodingList;

    net::SocketImpl::Ptr socket_;
    Int statusCode_;
    String::CPtr method_;
    String::CPtr path_;
    String::CPtr header_;
    String::CPtr trailer_;
    JsObject::Ptr headers_;
    JsObject::Ptr headerNames_;
    LinkedList::Ptr output_;
    EncodingList::Ptr outputEncodings_;
    SocketOnClose::Ptr socketOnClose_;
    EventEmitter::Ptr ee_;

    OutgoingMessage()
        : socket_(net::SocketImpl::null())
        , statusCode_(Status::OK)
        , method_(String::null())
        , path_(String::null())
        , header_(String::create())
        , trailer_(String::create())
        , headers_(JsObject::create())
        , headerNames_(JsObject::create())
        , output_(LinkedList::create())
        , outputEncodings_(EncodingList::create())
        , ee_(EventEmitter::create()) {
        static SocketOnClose::Ptr socketOnClose = SocketOnClose::create();
        socketOnClose_ = socketOnClose;

        setFlag(WRITABLE);
        setFlag(SHOULD_KEEP_ALIVE);
        setFlag(USE_CHUNKED_ENCODING_BY_DEFAULT);
        setFlag(HAS_BODY);
    }

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

#define LIBNODE_HTTP_OUTGOING_MESSAGE_IMPL(O) \
    LIBNODE_WRITABLE_STREAM_IMPL(O); \
    virtual void writeHead( \
        Int statusCode, \
        String::CPtr reasonPhrase = String::null(), \
        JsObject::CPtr headers = JsObject::null()) { \
        return O->writeHead(statusCode, reasonPhrase, headers); \
    } \
    virtual Int statusCode() const { \
        return O->statusCode(); \
    } \
    virtual void setHeader(String::CPtr name, String::CPtr value) { \
        O->setHeader(name, value); \
    } \
    virtual String::CPtr getHeader(String::CPtr name) const { \
        return O->getHeader(name); \
    } \
    virtual void removeHeader(String::CPtr name) { \
        O->removeHeader(name); \
    }

}  // namespace http
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_HTTP_OUTGOING_MESSAGE_H_
