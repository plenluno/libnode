// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_HTTP_OUTGOING_MESSAGE_H_
#define LIBNODE_DETAIL_HTTP_OUTGOING_MESSAGE_H_

#include <libnode/http/agent.h>
#include <libnode/http/status.h>
#include <libnode/http/client_request.h>
#include <libnode/debug_print.h>
#include <libnode/detail/http/parser.h>
#include <libnode/detail/http/client_response.h>

#include <libj/js_date.h>
#include <libj/typed_linked_list.h>
#include <libj/typed_value_holder.h>

#ifdef LIBJ_PF_WINDOWS
    #include <libj/platform/windows.h>
#endif

#include <assert.h>
#include <stdio.h>

namespace libj {
namespace node {
namespace detail {
namespace http {

class OutgoingMessage : public events::EventEmitter<WritableStream> {
 public:
    LIBJ_MUTABLE_DEFS(OutgoingMessage, WritableStream);

    static Ptr createInServer(IncomingMessage::Ptr req);

    static Ptr createInClient(
        libj::JsObject::CPtr options, JsFunction::Ptr callback);

    virtual Boolean writable() const {
        return socket_ && socket_->writable();
    }

    virtual Boolean write(const Value& chunk, Buffer::Encoding enc) {
        LIBJ_STATIC_SYMBOL_DEF(symCRLF, "\r\n");

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
            if (!buf->length()) return false;
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

    virtual Boolean end(const Value& data, Buffer::Encoding enc) {
        if (hasFlag(FINISHED)) return false;

        if (!header_) {
            implicitHeader();
        }

        Value d;
        if (hasFlag(HAS_BODY)) {
            d = data;
        } else {
            d = UNDEFINED;
        }

        String::CPtr str = toCPtr<String>(d);
        Boolean hot =
            !hasFlag(HEADER_SENT) &&
            str &&
            !str->isEmpty() &&
            output_->isEmpty() &&
            socket_ &&
            socket_->writable() &&
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
        if (output_->isEmpty() && socket_->httpMessage() == this) {
            finish();
        }
        return ret;
    }

    virtual Boolean destroy() {
        return socket_ && socket_->destroy();
    }

    virtual Boolean destroySoon() {
        return socket_ && socket_->destroySoon();
    }

 public:
    Int statusCode() const {
        return statusCode_;
    }

    String::CPtr getHeader(String::CPtr name) const {
        if (name && headers_) {
            return headers_->getCPtr<String>(name->toLowerCase());
        } else {
            return String::null();
        }
    }

    Boolean setHeader(String::CPtr name, String::CPtr value) {
        if (header_) return false;
        if (!name || !value) return false;

        if (!headers_) {
            assert(!headerNames_);
            headers_ = libj::JsObject::create();
            headerNames_ = libj::JsObject::create();
        }

        String::CPtr key = name->toLowerCase();
        headers_->put(key, value);
        headerNames_->put(key, name);
        return true;
    }

    Boolean removeHeader(String::CPtr name) {
        if (header_) return false;
        if (!name || !headers_) return false;

        String::CPtr key = name->toLowerCase();
        headers_->remove(key);
        headerNames_->remove(key);
        return true;
    }

    Boolean addTrailers(libj::JsObject::CPtr headers) {
        if (!headers) return false;

        StringBuffer::Ptr trailer = StringBuffer::create();
        Set::CPtr keys = headers->keySet();
        Iterator::Ptr itr = keys->iterator();
        while (itr->hasNext()) {
            String::CPtr key = toCPtr<String>(itr->next());
            String::CPtr val = headers->getCPtr<String>(key);
            if (key && val) {
                trailer->append(key);
                trailer->appendCStr(": ");
                trailer->append(val);
                trailer->appendCStr("\r\n");
            }
        }
        trailer_ = trailer->toString();
        return true;
    }

    void writeContinue() {
        LIBJ_STATIC_CONST_STRING_DEF(header, "HTTP/1.1 100 Continue\r\n\r\n");
        writeRaw(header, Buffer::UTF8);
        setFlag(SENT_100);
    }

    void writeHead(
        Int statusCode,
        String::CPtr reasonPhrase = String::null(),
        libj::JsObject::CPtr obj = libj::JsObject::null()) {
        statusCode_ = statusCode;
        node::http::Status::CPtr status =
            node::http::Status::create(statusCode, reasonPhrase);

        libj::JsObject::CPtr headers;
        if (obj && headers_) {
            libj::JsObject::Ptr hs = renderHeaders();
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

    Boolean headersSent() const {
        return !!header_;
    }

    Boolean sendDate() const {
        return hasFlag(SEND_DATE);
    }

    void setSendDate(Boolean send) {
        if (send) {
            setFlag(SEND_DATE);
        } else {
            unsetFlag(SEND_DATE);
        }
    }

    void abort() {
        if (socket_) {
            socket_->destroy();
        } else {
            JsFunction::Ptr method(new Destroy(this));
            deferToConnect(method);
        }
    }

    void setTimeout(UInt timeout, JsFunction::Ptr callback) {
        LIBJ_STATIC_SYMBOL_DEF(EVENT_TIMEOUT, "timeout");

        if (callback) once(EVENT_TIMEOUT, callback);

        JsFunction::Ptr emitTimeout(new EmitTimeout(this));

        if (socket_ && socket_->hasFlag(WRITABLE)) {
            if (timeoutCb_) {
                socket_->setTimeout(0, timeoutCb_);
            }
            timeoutCb_ = emitTimeout;
            socket_->setTimeout(timeout, emitTimeout);
            return;
        }

        if (socket_) {
            JsFunction::Ptr onConnect(
                new SetTimeout(this, timeout, emitTimeout));
            socket_->once(node::http::ClientRequest::EVENT_CONNECT, onConnect);
            return;
        }

        JsFunction::Ptr onSocket(new SetTimeout(this, timeout, emitTimeout));
        once(node::http::ClientRequest::EVENT_SOCKET, onSocket);
    }

    void setNoDelay(Boolean noDelay) {
        JsFunction::Ptr method(new SetNoDelay(&(*socket_), noDelay));
        deferToConnect(method);
    }

    void setSocketKeepAlive(Boolean enable, UInt initialDelay) {
        JsFunction::Ptr method(
            new SetSocketKeepAlive(&(*socket_), enable, initialDelay));
        deferToConnect(method);
    }

 public:
    static void httpSocketSetup(net::Socket::Ptr socket) {
        socket->setOnDrain(JsFunction::Ptr(new EmitDrain(&(*socket))));
    }

    void assignSocket(Ptr self, net::Socket::Ptr socket) {
        assert(!socket->httpMessage());
        socket->setHttpMessage(this);
        JsFunction::Ptr emitClose(new EmitClose(self));
        socket->setOnClose(emitClose);
        socket_ = socket;
        flush();
    }

    void detachSocket(net::Socket::Ptr socket) {
        assert(socket->httpMessage() == this);
        socket->setOnClose(JsFunction::null());
        socket->setHttpMessage(NULL);
        socket_ = net::Socket::null();
    }

    void onSocket(net::Socket::Ptr socket) {
        JsFunction::Ptr onSocket(new OnSocket(this, socket));
        process::nextTick(onSocket);
    }

    void clearTimeout(JsFunction::Ptr callback) {
        setTimeout(0, callback);
    }

 private:
    static String::CPtr toHex(Size val) {
        Size n;
        to<Size>(val, &n);
        const Size kLen = ((sizeof(Size) << 3) / 3) + 3;
        char s[kLen];
        snprintf(s, kLen, "%zx", n);
        return String::create(s);
    }

    static JsArray::Ptr freeParser(Parser* parser, OutgoingMessage* req) {
        assert(parser);
        assert(!req || !req->parser_ || req->parser_ == parser);
        JsArray::Ptr keeper = parser->free();
        delete parser;
        if (req) {
            req->parser_ = NULL;
        }
        return keeper;
    }

    static node::uv::Error::CPtr createHangUpError() {
        return node::uv::Error::create(node::uv::Error::_ECONNRESET);
    }

    typedef TypedValueHolder<String::CPtr> DateCache;

    static String::CPtr utcDate() {
        static DateCache::Ptr cache = DateCache::null();
        static JsFunction::Ptr clearCache = JsFunction::null();

        if (!cache) {
            assert(!clearCache);
            cache = DateCache::create(String::null());
            clearCache = JsFunction::Ptr(new ClearDateCache(cache));

            LIBJ_DEBUG_PRINT(
                "static: DateCache %p",
                LIBJ_DEBUG_OBJECT_PTR(cache));
            LIBJ_DEBUG_PRINT(
                "static: ClearDateCache %p",
                LIBJ_DEBUG_OBJECT_PTR(clearCache));
        }

        if (!cache->getTyped()) {
            LIBNODE_DEBUG_PRINT("set DateCache");
            JsDate::CPtr date = JsDate::create();
            cache->setTyped(date->toUTCString());
            node::setTimeout(
                clearCache,
                1000 - date->getMilliseconds());
        }
        return cache->getTyped();
    }

    Boolean send(const Value& data, Buffer::Encoding enc = Buffer::NONE) {
        if (hasFlag(HEADER_SENT)) {
            return writeRaw(data, enc);
        } else {
            assert(header_);
            setFlag(HEADER_SENT);
            String::CPtr str = toCPtr<String>(data);
            if (str) {
                return writeRaw(header_->concat(str), enc);
            } else {
                output_->unshift(header_);
                outputEncodings_->unshiftTyped(Buffer::UTF8);
                return writeRaw(data, enc);
            }
        }
    }

    Boolean writeRaw(const Value& data, Buffer::Encoding enc) {
        String::CPtr str = toCPtr<String>(data);
        if (str && str->isEmpty()) return true;

        Buffer::CPtr buf = toCPtr<Buffer>(data);
        if (buf && !buf->length()) return true;

        assert(str || buf);

        if (socket_ &&
            socket_->httpMessage() == this &&
            socket_->writable()) {
            while (output_->length()) {
                if (!socket_->writable()) {
                    buffer(data, enc);
                    return false;
                }
                Value c = output_->shift();
                Buffer::Encoding e = outputEncodings_->shiftTyped();
                socket_->write(c, e);
            }
            return socket_->write(data, enc);
        } else {
            buffer(data, enc);
            return false;
        }
    }

    Boolean buffer(const Value& data, Buffer::Encoding enc) {
        String::CPtr str = toCPtr<String>(data);
        if (str && str->isEmpty()) return true;

        Buffer::CPtr buf = toCPtr<Buffer>(data);
        if (buf && !buf->length()) return true;

        assert(str || buf);

        Size length = output_->length();

        if (!length || !str) {
            output_->push(data);
            outputEncodings_->pushTyped(enc);
            return false;
        }

        Buffer::Encoding lastEnc = outputEncodings_->getTyped(length - 1);
        Value lastData = output_->get(length - 1);

        if ((enc != Buffer::NONE && lastEnc == enc) ||
            (enc == Buffer::NONE && data.type() == lastData.type())) {
            if (str) {
                output_->set(
                    length - 1,
                    toCPtr<String>(lastData)->concat(str));
            } else {
                output_->set(
                    length -1,
                    toCPtr<Buffer>(lastData)->concat(buf));
            }
        }

        output_->push(data);
        outputEncodings_->pushTyped(enc);
        return false;
    }

    void store(
        StringBuffer::Ptr messageHeader,
        String::CPtr field,
        const Value& value) {
        LIBJ_STATIC_SYMBOL_DEF(symClose,   "close");
        LIBJ_STATIC_SYMBOL_DEF(symChunked, "chunked");

        messageHeader->append(field);
        messageHeader->appendCStr(": ");
        messageHeader->append(value);
        messageHeader->appendCStr("\r\n");

        String::CPtr lowerField = field->toLowerCase();
        if (lowerField->equals(node::http::LHEADER_CONNECTION)) {
            setFlag(SENT_CONNECTION_HEADER);
            if (value.equals(symClose)) {
                setFlag(LAST);
            } else {
                setFlag(SHOULD_KEEP_ALIVE);
            }
        } else if (lowerField->equals(node::http::LHEADER_TRANSFER_ENCODING)) {
            setFlag(SENT_TRANSFER_ENCODING_HEADER);
            if (value.equals(symChunked)) {
                setFlag(CHUNKED_ENCODING);
            }
        } else if (lowerField->equals(node::http::LHEADER_CONTENT_LENGTH)) {
            setFlag(SENT_CONTENT_LENGTH_HEADER);
        } else if (lowerField->equals(node::http::LHEADER_DATE)) {
            setFlag(SENT_DATE_HEADER);
        } else if (lowerField->equals(node::http::LHEADER_EXPECT)) {
            setFlag(SENT_EXPECT_HEADER);
        }
    }

    void storeHeader(String::CPtr firstLine, libj::JsObject::CPtr headers) {
        unsetFlag(SENT_CONNECTION_HEADER);
        unsetFlag(SENT_CONTENT_LENGTH_HEADER);
        unsetFlag(SENT_TRANSFER_ENCODING_HEADER);
        unsetFlag(SENT_DATE_HEADER);
        unsetFlag(SENT_EXPECT_HEADER);

        StringBuffer::Ptr messageHeader = StringBuffer::create();
        messageHeader->append(firstLine);

        if (headers) {
            Set::CPtr keys = headers->keySet();
            Iterator::Ptr itr = keys->iterator();
            while (itr->hasNext()) {
                String::CPtr field = toCPtr<String>(itr->next());
                Value value = headers->get(field);

                JsArray::CPtr ary = toCPtr<JsArray>(value);
                if (ary) {
                    Size len = ary->length();
                    for (Size i = 0; i < len; i++) {
                        store(messageHeader, field, ary->get(i));
                    }
                } else {
                    store(messageHeader, field, value);
                }
            }
        }

        if (hasFlag(SEND_DATE) && !hasFlag(SENT_DATE_HEADER)) {
            messageHeader->appendCStr("Date: ");
            messageHeader->append(utcDate());
            messageHeader->appendCStr("\r\n");
        }

        if (!hasFlag(SENT_CONNECTION_HEADER)) {
            Boolean shouldSendKeepAlive =
                hasFlag(SHOULD_KEEP_ALIVE) &&
                (hasFlag(SENT_CONTENT_LENGTH_HEADER) ||
                 hasFlag(USE_CHUNKED_ENCODING_BY_DEFAULT) ||
                 agent_);
            messageHeader->append(node::http::HEADER_CONNECTION);
            messageHeader->appendCStr(": ");
            if (shouldSendKeepAlive) {
                messageHeader->appendCStr("keep-alive");
            } else {
                setFlag(LAST);
                messageHeader->appendCStr("close");
            }
            messageHeader->appendCStr("\r\n");
        }

        if (!hasFlag(SENT_CONTENT_LENGTH_HEADER) &&
            !hasFlag(SENT_TRANSFER_ENCODING_HEADER)) {
            if (hasFlag(HAS_BODY)) {
                if (hasFlag(USE_CHUNKED_ENCODING_BY_DEFAULT)) {
                    messageHeader->append(
                        node::http::HEADER_TRANSFER_ENCODING);
                    messageHeader->appendCStr(": chunked\r\n");
                    setFlag(CHUNKED_ENCODING);
                } else {
                    setFlag(LAST);
                }
            } else {
                unsetFlag(CHUNKED_ENCODING);
            }
        }

        messageHeader->appendCStr("\r\n");
        header_ = messageHeader->toString();
        unsetFlag(HEADER_SENT);

        if (hasFlag(SENT_EXPECT_HEADER)) send(String::create());
    }

    libj::JsObject::Ptr renderHeaders() {
        if (header_) return libj::JsObject::null();
        if (!headers_) return libj::JsObject::create();

        libj::JsObject::Ptr headers = libj::JsObject::create();
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

            Value data = output_->shift();
            Buffer::Encoding enc = outputEncodings_->shiftTyped();
            ret = socket_->write(data, enc);
        }

        if (hasFlag(FINISHED)) {
            finish();
        } else if (ret) {
            emit(EVENT_DRAIN);
        }
    }

    void implicitHeader() {
        if (hasFlag(SERVER_SIDE)) {
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

    void deferToConnect(
        JsFunction::Ptr method,
        JsFunction::Ptr callback = JsFunction::null()) {
        JsFunction::Ptr onSocket(new DeferToConnect(this, method, callback));
        if (socket_) {
            (*onSocket)();
        } else {
            once(node::http::ClientRequest::EVENT_SOCKET, onSocket);
        }
    }

 private:
    class EmitDrain : LIBJ_JS_FUNCTION(EmitDrain)
        EmitDrain(net::Socket* sock) : socket_(sock) {}

        virtual Value operator()(JsArray::Ptr args) {
            OutgoingMessage* httpMessage = socket_->httpMessage();
            if (httpMessage) {
                httpMessage->emit(OutgoingMessage::EVENT_DRAIN);
            }
            return Status::OK;
        }

     private:
        net::Socket* socket_;
    };

    class EmitClose : LIBJ_JS_FUNCTION(EmitClose)
     public:
        EmitClose(OutgoingMessage::Ptr out) : self_(out) {}

        virtual Value operator()(JsArray::Ptr args) {
            self_->emit(OutgoingMessage::EVENT_CLOSE);
#ifdef LIBNODE_REMOVE_LISTENER
            // no event after 'close'
            self_->removeAllListeners();
#endif
            self_ = OutgoingMessage::null();
            return Status::OK;
        }

     private:
        // retain self_ until detachSocket or socket 'close'
        OutgoingMessage::Ptr self_;
    };

    class EmitTimeout : LIBJ_JS_FUNCTION(EmitTimeout)
     public:
        EmitTimeout(OutgoingMessage* msg) : self_(msg) {}

        virtual Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_TIMEOUT, "timeout");
            self_->emit(EVENT_TIMEOUT);
            return Status::OK;
        }

     private:
        OutgoingMessage* self_;
    };

    class IncomingEmitClose : LIBJ_JS_FUNCTION(IncomingEmitClose)
     public:
        IncomingEmitClose(IncomingMessage::Ptr res) : res_(res) {}

        virtual Value operator()(JsArray::Ptr args) {
            res_->emitEnd();
            res_->emit(IncomingMessage::EVENT_CLOSE);
#ifdef LIBNODE_REMOVE_LISTENER
            // no event after 'end' and 'close'
            res_->removeAllListeners();
#endif
            res_ = IncomingMessage::null();
            return Status::OK;
        }

     private:
        IncomingMessage::Ptr res_;
    };

    class SocketCloseListener : LIBJ_JS_FUNCTION(SocketCloseListener)
     public:
        SocketCloseListener() : socket_(NULL) {}

        virtual Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_ABORTED, "aborted");

            OutgoingMessage* req = socket_->httpMessage();
            req->emit(EVENT_CLOSE);

            IncomingMessage::Ptr res = req->res_;
            if (res && res->hasFlag(IncomingMessage::READABLE)) {
                res->emit(EVENT_ABORTED);
                JsFunction::Ptr emitClose(new IncomingEmitClose(res));
                res->emitPending(emitClose);
            } else if (!res && !req->hasFlag(HAD_ERROR)) {
                req->emit(EVENT_ERROR, createHangUpError());
            }

            Parser* parser = req->parser_;
            if (parser) {
                parser->finish();
                freeParser(parser, req);
            }
            return Status::OK;
        }

        void setSocket(net::Socket* socket) {
            socket_ = socket;
        }

     private:
        net::Socket* socket_;
    };

    class SocketErrorListener : LIBJ_JS_FUNCTION(SocketErrorListener)
     public:
        SocketErrorListener() : socket_(NULL) {}

        virtual Value operator()(JsArray::Ptr args) {
            Error::CPtr err = args->getCPtr<Error>(0);
            OutgoingMessage* req = socket_->httpMessage();
            if (req) {
                req->emit(EVENT_ERROR, err);
                req->setFlag(HAD_ERROR);
            }

            Parser* parser = socket_->parser();
            if (parser) {
                parser->finish();
                freeParser(parser, req);
            }

            socket_->destroy();
            return Status::OK;
        }

        void setSocket(net::Socket* socket) {
            socket_ = socket;
        }

     private:
        net::Socket* socket_;
    };

    class SocketOnEnd : LIBJ_JS_FUNCTION(SocketOnEnd)
     public:
        SocketOnEnd(net::Socket::Ptr sock) : socket_(&(*sock)) {}

        virtual Value operator()(JsArray::Ptr args) {
            OutgoingMessage* req = socket_->httpMessage();
            if (!req->res_) {
                req->emit(EVENT_ERROR, createHangUpError());
                req->setFlag(HAD_ERROR);
            }

            Parser* parser = socket_->parser();
            JsArray::Ptr keeper = JsArray::null();
            if (parser) {
                parser->finish();
                keeper = freeParser(parser, req);
                assert(keeper);
            }

            socket_->destroy();
            return Status::OK;
        }

     private:
        net::Socket* socket_;
    };

    class  SocketOnData : LIBJ_JS_FUNCTION(SocketOnData)
     public:
        SocketOnData(
            OutgoingMessage* msg,
            net::Socket::Ptr sock)
            : self_(msg)
            , socket_(&(*sock)) {}

        virtual Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_AGENT_REMOVE, "agentRemove");

            Parser* parser = socket_->parser();
            OutgoingMessage* req = socket_->httpMessage();
            IncomingMessage::Ptr res = parser->incoming();
            Buffer::CPtr buf = args->getCPtr<Buffer>(0);
            Int bytesParsed = parser->execute(buf);
            if (bytesParsed < 0) {
                freeParser(parser, req);
                socket_->destroy();
                req->emit(
                    EVENT_ERROR,
                    Error::create(Error::ILLEGAL_RESPONSE));
                req->setFlag(HAD_ERROR);
            } else if (res && res->hasFlag(IncomingMessage::UPGRADE)) {
                req->res_ = res;

                JsFunction::Ptr prev = socket_->setOnData(JsFunction::null());
                assert(&(*prev) == this);
                socket_->setOnEnd(JsFunction::null());
                parser->finish();

                Buffer::Ptr bodyHead = toPtr<Buffer>(buf->slice(bytesParsed));

                Boolean isConnect =
                    req->method_->equals(node::http::METHOD_CONNECT);
                String::CPtr event = isConnect
                    ? node::http::ClientRequest::EVENT_CONNECT
                    : node::http::ClientRequest::EVENT_UPGRADE;
                if (req->listeners(event)->isEmpty()) {
                    socket_->destroy();
                } else {
                    req->setFlag(UPGRADE_OR_CONNECT);
                    socket_->emit(EVENT_AGENT_REMOVE);
                    socket_->removeListener(
                        EVENT_CLOSE, self_->socketCloseListener_);
                    socket_->removeListener(
                        EVENT_ERROR, self_->socketErrorListener_);
                    req->emit(event, res, socket_, bodyHead);
                    req->emit(EVENT_CLOSE);
                }

                freeParser(parser, req);
            } else if (res &&
                res->hasFlag(IncomingMessage::COMPLETE) &&
                res->statusCode() != 100) {
                freeParser(parser, req);
            }
            return Status::OK;
        }

     private:
        OutgoingMessage* self_;
        net::Socket* socket_;
    };

    class ResponseOnEnd : LIBJ_JS_FUNCTION(ResponseOnEnd)
     public:
        ResponseOnEnd(IncomingMessage* res) : res_(res) {}

        virtual Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_FREE, "free");

            OutgoingMessage* req = res_->req();
            net::Socket::Ptr socket = req->socket_;
            if (!req->hasFlag(SHOULD_KEEP_ALIVE)) {
                if (socket->hasFlag(WRITABLE)) {
                    socket->destroySoon();
                }
                assert(!socket->hasFlag(WRITABLE));
            } else {
                if (req->timeoutCb_) {
                    socket->setTimeout(0, req->timeoutCb_);
                    req->timeoutCb_ = JsFunction::null();
                }
                socket->removeListener(
                    net::Socket::EVENT_CLOSE,
                    req->socketCloseListener_);
                socket->removeListener(
                    net::Socket::EVENT_ERROR,
                    req->socketErrorListener_);
                socket->emit(EVENT_FREE);
            }
            return Status::OK;
        }

     private:
        IncomingMessage* res_;
    };

    class ParserOnIncomingClient : LIBJ_JS_FUNCTION(ParserOnIncomingClient)
     public:
        ParserOnIncomingClient(net::Socket::Ptr socket) : socket_(socket) {}

        virtual Value operator()(JsArray::Ptr args) {
            IncomingMessage::Ptr res = args->getPtr<IncomingMessage>(0);
            Boolean shouldKeepAlive = false;
            to<Boolean>(args->get(1), &shouldKeepAlive);

            OutgoingMessage* req = socket_->httpMessage();
            if (req->res_) {
                socket_->destroy();
                return false;
            }
            req->res_ = res;

            if (req->method_->equals(node::http::METHOD_CONNECT)) {
                res->setFlag(IncomingMessage::UPGRADE);
                return true;
            }

            if (res->statusCode() == 100) {
                req->res_ = IncomingMessage::null();
                req->emit(node::http::ClientRequest::EVENT_CONTINUE);
                return true;
            }

            if (req->hasFlag(SHOULD_KEEP_ALIVE) &&
                !shouldKeepAlive &&
                !req->hasFlag(UPGRADE_OR_CONNECT)) {
                req->unsetFlag(SHOULD_KEEP_ALIVE);
            }

            req->emit(
                node::http::ClientRequest::EVENT_RESPONSE,
                ClientResponse::create(res));
            req->res_ = res;
            res->setReq(req);

            JsFunction::Ptr responseOnEnd(new ResponseOnEnd(&(*res)));
            res->on(IncomingMessage::EVENT_END, responseOnEnd);

            return req->method_->equals(node::http::METHOD_HEAD);
        }

     private:
        net::Socket::Ptr socket_;
    };

    class OnSocket : LIBJ_JS_FUNCTION(OnSocket)
     public:
        OnSocket(
            OutgoingMessage* self,
            net::Socket::Ptr socket)
            : self_(self)
            , socket_(socket) {}

        virtual Value operator()(JsArray::Ptr args) {
            Parser* parser = socket_->parser();
            if (parser) {
                freeParser(parser, self_);
            }

            parser = new Parser(HTTP_RESPONSE, socket_);
            self_->socket_ = socket_;
            self_->parser_ = parser;

            socket_->setParser(parser);
            socket_->setHttpMessage(self_);

            httpSocketSetup(socket_);

            if (!self_->socketCloseListener_) {
                assert(!self_->socketErrorListener_);
                self_->socketCloseListener_ =
                    SocketCloseListener::Ptr(new SocketCloseListener());
                self_->socketErrorListener_ =
                    SocketErrorListener::Ptr(new SocketErrorListener());
            }

            self_->socketCloseListener_->setSocket(&(*socket_));
            self_->socketErrorListener_->setSocket(&(*socket_));

            socket_->on(
                net::Socket::EVENT_CLOSE,
                self_->socketCloseListener_);
            socket_->on(
                net::Socket::EVENT_ERROR,
                self_->socketErrorListener_);

            JsFunction::Ptr socketOnEnd(new SocketOnEnd(socket_));
            JsFunction::Ptr socketOnData(new SocketOnData(self_, socket_));
            socket_->setOnEnd(socketOnEnd);
            socket_->setOnData(socketOnData);

            JsFunction::Ptr parserOnIncomingClient(
                new ParserOnIncomingClient(socket_));
            parser->setOnIncoming(parserOnIncomingClient);

            self_->emit(node::http::ClientRequest::EVENT_SOCKET, socket_);
            return Status::OK;
        }

     private:
        OutgoingMessage* self_;
        net::Socket::Ptr socket_;
    };

    class Destroy : LIBJ_JS_FUNCTION(Destroy)
     public:
        Destroy(OutgoingMessage* msg) : self_(msg) {}

        virtual Value operator()(JsArray::Ptr args) {
            return self_->destroy();
        }

     private:
        OutgoingMessage* self_;
    };

    class Flush : LIBJ_JS_FUNCTION(Flush)
     public:
        Flush(OutgoingMessage* self) : self_(self) {}

        virtual Value operator()(JsArray::Ptr args) {
            self_->flush();
            return Status::OK;
        }

     private:
        OutgoingMessage* self_;
    };

    class SetTimeout : LIBJ_JS_FUNCTION(EmitTimeout)
     public:
        SetTimeout(
            OutgoingMessage* msg,
            UInt timeout,
            JsFunction::Ptr callback)
            : self_(msg)
            , timeout_(timeout)
            , callback_(callback) {}

        virtual Value operator()(JsArray::Ptr args) {
            self_->setTimeout(timeout_, callback_);
            return Status::OK;
        }

     private:
        OutgoingMessage* self_;
        UInt timeout_;
        JsFunction::Ptr callback_;
    };

    class SetNoDelay : LIBJ_JS_FUNCTION(SetNoDelay)
     public:
        SetNoDelay(
            net::Socket* socket,
            Boolean noDelay)
            : socket_(socket)
            , noDelay_(noDelay) {}

        virtual Value operator()(JsArray::Ptr args) {
            return socket_->setNoDelay(noDelay_);
        }

     private:
        net::Socket* socket_;
        Boolean noDelay_;
    };

    class SetSocketKeepAlive : LIBJ_JS_FUNCTION(SetSocketKeepAlive)
     public:
        SetSocketKeepAlive(
            net::Socket* socket,
            Boolean enable,
            UInt initialDelay)
            : socket_(socket)
            , enable_(enable)
            , initialDelay_(initialDelay) {}

        virtual Value operator()(JsArray::Ptr args) {
            return socket_->setKeepAlive(enable_, initialDelay_);
        }

     private:
        net::Socket* socket_;
        Boolean enable_;
        UInt initialDelay_;
    };

    class DeferToConnect : LIBJ_JS_FUNCTION(DeferToConnect)
     public:
        DeferToConnect(
            OutgoingMessage* msg,
            JsFunction::Ptr method,
            JsFunction::Ptr callback)
            : self_(msg)
            , method_(method)
            , callback_(callback)
            , onConnect_(false) {}

        virtual Value operator()(JsArray::Ptr args) {
            if (self_->socket_->hasFlag(WRITABLE) || onConnect_) {
                if (method_) (*method_)();
                if (callback_) (*callback_)();
            } else {
                DeferToConnect::Ptr onConnect(
                    new DeferToConnect(self_, method_, callback_));
                onConnect->onConnect_ = true;
                self_->socket_->once(net::Socket::EVENT_CONNECT, onConnect);
            }
            return Status::OK;
        }

     private:
        OutgoingMessage* self_;
        JsFunction::Ptr method_;
        JsFunction::Ptr callback_;
        Boolean onConnect_;
    };

    class ClearDateCache : LIBJ_JS_FUNCTION(ClearDateCache)
     public:
        ClearDateCache(DateCache::Ptr cache) : cache_(cache) {}

        virtual Value operator()(JsArray::Ptr args) {
            cache_->setTyped(String::null());
            LIBNODE_DEBUG_PRINT("unset DateCache");
            return Status::OK;
        }

     private:
        DateCache::Ptr cache_;
    };

 public:
    enum Flag {
        SERVER_SIDE                     = 1 << 0,
        WRITABLE                        = 1 << 1,
        LAST                            = 1 << 2,
        SHOULD_KEEP_ALIVE               = 1 << 3,
        CHUNKED_ENCODING                = 1 << 4,
        USE_CHUNKED_ENCODING_BY_DEFAULT = 1 << 5,
        SEND_DATE                       = 1 << 6,
        HAS_BODY                        = 1 << 7,
        FINISHED                        = 1 << 8,
        HEADER_SENT                     = 1 << 9,
        EXPECT_CONTINUE                 = 1 << 10,
        SENT_100                        = 1 << 11,
        HAD_ERROR                       = 1 << 12,
        UPGRADE_OR_CONNECT              = 1 << 13,
        SENT_CONNECTION_HEADER          = 1 << 14,
        SENT_CONTENT_LENGTH_HEADER      = 1 << 15,
        SENT_TRANSFER_ENCODING_HEADER   = 1 << 16,
        SENT_DATE_HEADER                = 1 << 17,
        SENT_EXPECT_HEADER              = 1 << 18,
    };

 private:
    typedef TypedLinkedList<Buffer::Encoding> EncodingList;

    net::Socket::Ptr socket_;
    Int statusCode_;
    String::CPtr method_;
    String::CPtr path_;
    String::CPtr header_;
    String::CPtr trailer_;
    libj::JsObject::Ptr headers_;
    libj::JsObject::Ptr headerNames_;
    LinkedList::Ptr output_;
    EncodingList::Ptr outputEncodings_;
    Parser* parser_;
    node::http::Agent::Ptr agent_;
    String::CPtr socketPath_;
    IncomingMessage::Ptr res_;
    JsFunction::Ptr timeoutCb_;
    SocketCloseListener::Ptr socketCloseListener_;
    SocketErrorListener::Ptr socketErrorListener_;

    OutgoingMessage()
        : socket_(net::Socket::null())
        , statusCode_(node::http::Status::OK)
        , method_(String::null())
        , path_(String::null())
        , header_(String::null())
        , trailer_(String::create())
        , headers_(libj::JsObject::null())
        , headerNames_(libj::JsObject::null())
        , output_(LinkedList::create())
        , outputEncodings_(EncodingList::create())
        , parser_(NULL)
        , agent_(node::http::Agent::null())
        , socketPath_(String::null())
        , res_(IncomingMessage::null())
        , timeoutCb_(JsFunction::null())
        , socketCloseListener_(SocketCloseListener::null())
        , socketErrorListener_(SocketErrorListener::null()) {
        setFlag(WRITABLE);
        setFlag(SHOULD_KEEP_ALIVE);
        setFlag(USE_CHUNKED_ENCODING_BY_DEFAULT);
        setFlag(HAS_BODY);
    }
};

}  // namespace http
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_HTTP_OUTGOING_MESSAGE_H_
