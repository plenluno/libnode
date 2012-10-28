// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_OUTCOMING_MESSAGE_H_
#define LIBNODE_SRC_HTTP_OUTCOMING_MESSAGE_H_

#include <assert.h>
#include <libj/typed_linked_list.h>

#include "libnode/http.h"
#include "libnode/stream.h"

#include "../net/socket_impl.h"

namespace libj {
namespace node {
namespace http {

class OutcomingMessage : LIBNODE_WRITABLE_STREAM(OutcomingMessage)
 public:
    static Ptr create() {
        return Ptr(new OutcomingMessage());
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
        if (header_) return false;

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
        static const String::CPtr strCRLF = String::intern("\r\n");

        if (!header_) {
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
                // TODO(plenluno): toHex(len)
                String::CPtr data = String::valueOf(len)->concat(strCRLF);
                data = data->concat(str)->concat(strCRLF);
                return send(data, enc);
            } else {
                Size len = buf->length();
                send(String::valueOf(len)->concat(strCRLF));
                send(buf);
                return send(strCRLF);
            }
        } else {
            return send(chunk, enc);
        }
    }

    Boolean end(const Value& data, Buffer::Encoding enc) {
        static const String::CPtr strCRLF = String::intern("\r\n");
        static const String::CPtr str0CRLF = String::create("0\r\n");
        static const String::CPtr strCRLF0CRLF = String::create("\r\n0\r\n");

        if (hasFlag(FINISHED)) return false;

        if (!header_) {
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
            socket_->getHttpMessage() == this;

        Boolean ret;
        if (hot) {
            if (hasFlag(CHUNKED_ENCODING)) {
                Size len = Buffer::byteLength(str, enc);
                // TODO(plenluno): toHex(len)
                String::CPtr chunk =
                    header_->concat(String::valueOf(len))
                           ->concat(strCRLF)
                           ->concat(str)
                           ->concat(strCRLF0CRLF)
                           ->concat(trailer_)
                           ->concat(strCRLF);
                ret = socket_->write(chunk, enc);
            } else {
                ret = socket_->write(header_->concat(str), enc);
            }
            setFlag(HEADER_SENT);
        } else if (!d.isUndefined()) {
            ret = write(d, enc);
        }

        if (!hot) {
            if (hasFlag(CHUNKED_ENCODING)) {
                String::CPtr chunk =
                    str0CRLF->concat(trailer_)->concat(strCRLF);
                ret = send(chunk);
            } else {
                ret = send(String::create());
            }
        }

        setFlag(FINISHED);
        if (output_->length() == 0 && socket_->getHttpMessage() == this) {
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
        // statusLine->appendCStr("HTTP/1.1 ");
        statusLine->append(status->code());
        // statusLine->appendChar(" ");
        statusLine->append(status->message());
        // statusLine->appendCStr("\r\n");

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

 private:
    Boolean send(const Value& data, Buffer::Encoding enc = Buffer::NONE) {
        Value d;
        if (!hasFlag(HEADER_SENT)) {
            String::CPtr str = toCPtr<String>(data);
            if (str) {
                assert(header_);
                d = header_->concat(str);
            } else {
                d = data;
                output_->add(0, header_);
                outputEncodings_->addTyped(0, Buffer::UTF8);
            }
            setFlag(HEADER_SENT);
        }
        return writeRaw(d, enc);
    }

    Boolean writeRaw(const Value& data, Buffer::Encoding enc) {
        String::CPtr str = toCPtr<String>(data);
        if (str && str->isEmpty()) return true;

        Buffer::CPtr buf = toCPtr<Buffer>(data);
        if (buf && buf->isEmpty()) return true;

        if (socket_ &&
            socket_->getHttpMessage() == this &&
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
        static const String::CPtr strColon = String::intern(": ");
        static const String::CPtr strCRLF = String::intern("\r\n");
        static const String::CPtr strClose = String::intern("close");
        static const String::CPtr strKeepAlive = String::intern("keep-alive");
        static const String::CPtr strChunked = String::intern("chunked");

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
            messageHader->append(strColon); \
            messageHader->append(V); \
            messageHader->append(strCRLF); \
            String::CPtr lowerField = F->toLowerCase(); \
            if (lowerField->equals(LHEADER_CONNECTION)) { \
                sentConnectionHeader = true; \
                if (V.equals(strClose)) { \
                    setFlag(LAST); \
                } else { \
                    setFlag(SHOULD_KEEP_ALIVE); \
                } \
            } else if (lowerField->equals(LHEADER_TRANSFER_ENCODING)) { \
                sentTransferEncodingHeader = true; \
                if (V.equals(strChunked)) { \
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
            messageHader->append(strColon);
            if (shouldSendKeepAlive) {
                messageHader->append(strKeepAlive);
            } else {
                setFlag(LAST);
                messageHader->append(strClose);
            }
            messageHader->append(strCRLF);
        }

        if (!sentContentLengthHeader && !sentTransferEncodingHeader) {
            if (hasFlag(HAS_BODY)) {
                if (hasFlag(USE_CHUNKED_ENCODING_BY_DEFAULT)) {
                    messageHader->append(HEADER_TRANSFER_ENCODING);
                    messageHader->append(strColon);
                    messageHader->append(strChunked);
                    messageHader->append(strCRLF);
                    setFlag(CHUNKED_ENCODING);
                } else {
                    setFlag(LAST);
                }
            } else {
                unsetFlag(CHUNKED_ENCODING);
            }
        }

        messageHader->append(strCRLF);
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
        static const String::CPtr strFinish = String::intern("finish");
        assert(socket_);
        emit(strFinish);
    }

    void flush() {
        if (!socket_) return;

        Boolean ret;
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
        static const String::CPtr strMethod = String::intern("method");
        static const String::CPtr strPath = String::intern("path");

        if (hasFlag(SERVER_RESPONSE)) {
            writeHead(statusCode_);
        } else {
            String::CPtr method = getCPtr<String>(strMethod);
            String::CPtr path = getCPtr<String>(strPath);
            assert(method && path);
            StringBuffer::Ptr sb = StringBuffer::create();
            sb->append(method);
            // sb->appendChar(' ');
            sb->append(path);
            // sb->appendCStr(" HTTP/1.1\r\n");
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

    LIBNODE_FLAG_METHODS(Flag, flags_);

 private:
    typedef TypedLinkedList<Buffer::Encoding> EncodingList;

    UInt flags_;
    net::SocketImpl::Ptr socket_;
    Int statusCode_;
    String::CPtr header_;
    String::CPtr trailer_;
    JsObject::Ptr headers_;
    JsObject::Ptr headerNames_;
    LinkedList::Ptr output_;
    EncodingList::Ptr outputEncodings_;
    EventEmitter::Ptr ee_;

 public:
    OutcomingMessage()
        : flags_(0)
        , socket_(net::SocketImpl::null())
        , statusCode_(Status::OK)
        , header_(String::create())
        , trailer_(String::create())
        , headers_(JsObject::create())
        , headerNames_(JsObject::create())
        , output_(LinkedList::create())
        , outputEncodings_(EncodingList::create())
        , ee_(EventEmitter::create()) {
        setFlag(WRITABLE);
        setFlag(SHOULD_KEEP_ALIVE);
        setFlag(USE_CHUNKED_ENCODING_BY_DEFAULT);
        setFlag(HAS_BODY);
    }

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

#define LIBNODE_HTTP_OUTCOMING_MESSAGE_IMPL(O) \
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

#endif  // LIBNODE_SRC_HTTP_OUTCOMING_MESSAGE_H_
