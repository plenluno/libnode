// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_HTTP_OUTGOING_MESSAGE_H_
#define LIBNODE_SRC_HTTP_OUTGOING_MESSAGE_H_

#include <assert.h>
#include <stdio.h>
#include <libj/typed_linked_list.h>

#include "libnode/http.h"
#include "libnode/stream/writable_stream.h"

#include "./parser.h"
#include "./incoming_message.h"
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

    static Ptr create(JsObject::CPtr options, JsFunction::Ptr callback);

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

    void abort() {
        if (socket_) {
            socket_->destroy();
        } else {
            JsFunction::Ptr method(new Destroy(this));
            deferToConnect(method);
        }
    }

    void onSocket(net::SocketImpl::Ptr socket) {
        JsFunction::Ptr onSocket(new OnSocket(this, socket));
        process::nextTick(onSocket);
    }

    void setTimeout(UInt msecs, JsFunction::Ptr callback) {
        LIBJ_STATIC_SYMBOL_DEF(EVENT_SOCKET,  "socket");
        LIBJ_STATIC_SYMBOL_DEF(EVENT_TIMEOUT, "timeout");
        LIBJ_STATIC_SYMBOL_DEF(EVENT_CONNECT, "connect");

        if (callback) once(EVENT_TIMEOUT, callback);

        JsFunction::Ptr emitTimeout(new EmitTimeout(this));

        if (socket_ && socket_->hasFlag(WRITABLE)) {
            if (timeoutCb_) {
                socket_->setTimeout(0, timeoutCb_);
            }
            timeoutCb_ = emitTimeout;
            socket_->setTimeout(msecs, emitTimeout);
            return;
        } else if (socket_) {
            JsFunction::Ptr onConnect(
                new SetTimeout(this, msecs, emitTimeout));
            socket_->once(EVENT_CONNECT, onConnect);
            return;
        } else {
            JsFunction::Ptr onSocket(
                new SetTimeout(this, msecs, emitTimeout));
            once(EVENT_SOCKET, onSocket);
            return;
        }
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

 private:
    class SocketOnDrain : LIBJ_JS_FUNCTION(SocketOnDrain)
        static Ptr create(net::SocketImpl::Ptr socket) {
            return Ptr(new SocketOnDrain(&(*socket)));
        }

        Value operator()(JsArray::Ptr args) {
            OutgoingMessage* httpMessage = socket_->httpMessage();
            if (httpMessage)
                httpMessage->emit(OutgoingMessage::EVENT_DRAIN);
            return libj::Status::OK;
        }

     private:
        net::SocketImpl* socket_;

        SocketOnDrain(net::SocketImpl* sock) : socket_(sock) {}
    };

    class SocketOnClose : LIBJ_JS_FUNCTION(SocketOnClose)
     public:
        SocketOnClose() : socket_(NULL) {}

        Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(symHttpMessage, "httpMessage");

            assert(socket_);
            socket_->httpMessage()->emit(OutgoingMessage::EVENT_CLOSE);
            socket_->remove(symHttpMessage);
            return libj::Status::OK;
        }

        void setSocket(net::SocketImpl* socket) {
            socket_ = socket;
        }

     private:
        net::SocketImpl* socket_;
    };

    class CloseResponse : LIBJ_JS_FUNCTION(CloseResponse)
     public:
        CloseResponse(IncomingMessage::Ptr res) : res_(res) {}

        Value operator()(JsArray::Ptr args) {
            res_->emitEnd();
            res_->emit(IncomingMessage::EVENT_CLOSE);
            return Status::OK;
        }

     private:
        IncomingMessage::Ptr res_;
    };

    class SocketCloseListener : LIBJ_JS_FUNCTION(SocketCloseListener)
     public:
        SocketCloseListener() : socket_(NULL) {}

        Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_ABORTED, "aborted");

            OutgoingMessage* req = socket_->httpMessage();
            req->emit(EVENT_CLOSE);
            if (req->res_ && req->res_->hasFlag(IncomingMessage::READABLE)) {
                IncomingMessage::Ptr res = req->res_;
                res->emit(EVENT_ABORTED);
                JsFunction::Ptr closeResponse(new CloseResponse(res));
                res->emitPending(closeResponse);
            } else if (!req->res_ && !req->hasFlag(HAD_ERROR)) {
                req->emit(OutgoingMessage::EVENT_ERROR, createHangUpError());
            }

            Parser* parser = req->parser_;
            if (parser) {
                parser->finish();
                freeParser(parser, req);
            }
            return Status::OK;
        }

        void setSocket(net::SocketImpl* socket) {
            socket_ = socket;
        }

     private:
        net::SocketImpl* socket_;
    };

    class SocketErrorListener : LIBJ_JS_FUNCTION(SocketErrorListener)
     public:
        SocketErrorListener() : socket_(NULL) {}

        Value operator()(JsArray::Ptr args) {
            libj::Error::CPtr err = args->getCPtr<libj::Error>(0);
            OutgoingMessage* req = socket_->httpMessage();
            if (req) {
                req->emit(OutgoingMessage::EVENT_ERROR, err);
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

        void setSocket(net::SocketImpl* socket) {
            socket_ = socket;
        }

     private:
        net::SocketImpl* socket_;
    };

    class SocketOnEnd : LIBJ_JS_FUNCTION(SocketOnEnd)
     public:
        SocketOnEnd(net::SocketImpl::Ptr sock) : socket_(&(*sock)) {}

        Value operator()(JsArray::Ptr args) {
            OutgoingMessage* req = socket_->httpMessage();
            if (!req->res_) {
                req->emit(
                    OutgoingMessage::EVENT_ERROR,
                    createHangUpError());
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

     private:
        net::SocketImpl* socket_;
    };

    class  SocketOnData : LIBJ_JS_FUNCTION(SocketOnData)
     public:
        SocketOnData(
            OutgoingMessage* msg,
            net::SocketImpl::Ptr sock)
            : self_(msg)
            , socket_(&(*sock)) {}

        Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_UPGRADE,       "upgrade");
            LIBJ_STATIC_SYMBOL_DEF(EVENT_CONNECT,       "connect");
            LIBJ_STATIC_SYMBOL_DEF(EVENT_AGENT_REMOVED, "agentRemoved");

            OutgoingMessage* req = socket_->httpMessage();
            Parser* parser = socket_->parser();
            Buffer::CPtr buf = args->getCPtr<Buffer>(0);
            Int bytesParsed = parser->execute(buf);
            if (bytesParsed < 0) {
                freeParser(parser, req);
                socket_->destroy();
                req->emit(
                    EVENT_ERROR,
                    libj::Error::create(libj::Error::ILLEGAL_RESPONSE));
                req->setFlag(HAD_ERROR);
            } else if (parser->incoming() &&
                parser->incoming()->hasFlag(IncomingMessage::UPGRADE)) {
                IncomingMessage::Ptr res = parser->incoming();
                req->res_ = res;

                socket_->setOnData(JsFunction::null());
                socket_->setOnEnd(JsFunction::null());
                parser->finish();

                Buffer::Ptr bodyHead = toPtr<Buffer>(buf->slice(bytesParsed));
                Boolean isConnect = req->method_->equals(METHOD_CONNECT);
                String::CPtr event = isConnect ? EVENT_CONNECT : EVENT_UPGRADE;
                if (req->listeners(event)->isEmpty()) {
                    socket_->destroy();
                } else {
                    req->setFlag(UPGRADE_OR_CONNECT);
                    socket_->emit(EVENT_AGENT_REMOVED);
                    self_->socketCloseListener_->setSocket(socket_);
                    self_->socketErrorListener_->setSocket(socket_);
                    socket_->removeListener(
                        EVENT_CLOSE, self_->socketCloseListener_);
                    socket_->removeListener(
                        EVENT_ERROR, self_->socketErrorListener_);
                    req->emit(event, res, socket_, bytesParsed);
                    req->emit(EVENT_CLOSE);
                }
            } else if (parser->incoming() &&
                parser->incoming()->hasFlag(IncomingMessage::COMPLETE) &&
                parser->incoming()->statusCode() != 100) {
                freeParser(parser, req);
            }
            return Status::OK;
        }

     private:
        OutgoingMessage* self_;
        net::SocketImpl* socket_;
    };

    class ResponseOnEnd : LIBJ_JS_FUNCTION(ResponseOnEnd)
     public:
        ResponseOnEnd(IncomingMessage::Ptr res) : res_(res) {}

        Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_FREE, "free");

            OutgoingMessage* req = res_->req();
            net::SocketImpl::Ptr socket = req->socket_;
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
                    net::SocketImpl::EVENT_CLOSE,
                    req->socketCloseListener_);
                socket->removeListener(
                    net::SocketImpl::EVENT_ERROR,
                    req->socketErrorListener_);
                socket->emit(EVENT_FREE);
            }
            return Status::OK;
        }

     private:
        IncomingMessage::Ptr res_;
    };

    class ParserOnIncomingClient : LIBJ_JS_FUNCTION(ParserOnIncomingClient)
     public:
        ParserOnIncomingClient(
            Parser* parser,
            net::SocketImpl::Ptr socket)
            : parser_(parser)
            , socket_(socket) {}

        Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_CONTINUE, "continue");
            LIBJ_STATIC_SYMBOL_DEF(EVENT_RESPONSE, "response");

            IncomingMessage::Ptr res = args->getPtr<IncomingMessage>(0);
            Boolean shouldKeepAlive = false;
            to<Boolean>(args->get(1), &shouldKeepAlive);

            OutgoingMessage* req = socket_->httpMessage();
            if (req->res_) {
                socket_->destroy();
                return true;
            }
            req->res_ = res;

            if (req->method_->equals(METHOD_CONNECT)) {
                res->setFlag(IncomingMessage::UPGRADE);
                return true;
            }

            if (res->statusCode() == 100) {
                req->res_ = IncomingMessage::null();
                req->emit(EVENT_CONTINUE);
                return true;
            }

            if (req->hasFlag(SHOULD_KEEP_ALIVE) &&
                !shouldKeepAlive &&
                !req->hasFlag(UPGRADE_OR_CONNECT)) {
                req->unsetFlag(SHOULD_KEEP_ALIVE);
            }

            req->emit(EVENT_RESPONSE, res);
            req->res_ = res;
            res->setReq(req);

            return req->method_->equals(METHOD_HEAD);
        }

     private:
        Parser* parser_;
        net::SocketImpl::Ptr socket_;
    };

    class OnSocket : LIBJ_JS_FUNCTION(SocketOnClose)
     public:
        OnSocket(
            OutgoingMessage* self,
            net::SocketImpl::Ptr socket)
            : self_(self)
            , socket_(socket) {}

        Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_SOCKET, "socket");

            Parser* parser = new Parser(HTTP_RESPONSE, socket_);
            self_->socket_ = socket_;
            self_->parser_ = parser;

            socket_->setParser(parser);
            socket_->setHttpMessage(self_);

            httpSocketSetup(socket_);

            self_->socketErrorListener_->setSocket(&(*socket_));
            self_->socketCloseListener_->setSocket(&(*socket_));

            socket_->on(
                net::SocketImpl::EVENT_ERROR,
                self_->socketErrorListener_);
            socket_->on(
                net::SocketImpl::EVENT_CLOSE,
                self_->socketCloseListener_);

            JsFunction::Ptr socketOnEnd(new SocketOnEnd(socket_));
            JsFunction::Ptr socketOnData(new SocketOnData(self_, socket_));
            socket_->setOnEnd(socketOnEnd);
            socket_->setOnData(socketOnData);

            JsFunction::Ptr parserOnIncomingClient(
                new ParserOnIncomingClient(parser, socket_));
            parser->setOnIncoming(parserOnIncomingClient);

            // Int len = self_->listeners(EVENT_SOCKET)->length();
            // console::printf(console::DEBUG, "emit socket %d\n", len);

            self_->emit(EVENT_SOCKET, socket_);
            return Status::OK;
        }

     private:
        OutgoingMessage* self_;
        net::SocketImpl::Ptr socket_;
    };

    class EmitTimeout : LIBJ_JS_FUNCTION(EmitTimeout)
     public:
        EmitTimeout(OutgoingMessage* msg) : self_(msg) {}

        Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_TIMEOUT, "timeout");
            self_->emit(EVENT_TIMEOUT);
            return Status::OK;
        }

     private:
        OutgoingMessage* self_;
    };

    class Destroy : LIBJ_JS_FUNCTION(Destroy)
     public:
        Destroy(OutgoingMessage* msg) : self_(msg) {}

        Value operator()(JsArray::Ptr args) {
            return self_->destroy();
        }

     private:
        OutgoingMessage* self_;
    };

    class SetTimeout : LIBJ_JS_FUNCTION(EmitTimeout)
     public:
        SetTimeout(
            OutgoingMessage* msg,
            Int msecs,
            JsFunction::Ptr callback)
            : self_(msg)
            , msecs_(msecs)
            , callback_(callback) {}

        Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_TIMEOUT, "timeout");
            self_->setTimeout(msecs_, callback_);
            return Status::OK;
        }

     private:
        OutgoingMessage* self_;
        Int msecs_;
        JsFunction::Ptr callback_;
    };

    class SetNoDelay : LIBJ_JS_FUNCTION(SetNoDelay)
     public:
        SetNoDelay(
            net::SocketImpl* socket,
            Boolean noDelay)
            : socket_(socket)
            , noDelay_(noDelay) {}

        Value operator()(JsArray::Ptr args) {
            return socket_->setNoDelay(noDelay_);
        }

     private:
        net::SocketImpl* socket_;
        Boolean noDelay_;
    };

    class SetSocketKeepAlive : LIBJ_JS_FUNCTION(SetSocketKeepAlive)
     public:
        SetSocketKeepAlive(
            net::SocketImpl* socket,
            Boolean enable,
            UInt initialDelay)
            : socket_(socket)
            , enable_(enable)
            , initialDelay_(initialDelay) {}

        Value operator()(JsArray::Ptr args) {
            return socket_->setKeepAlive(enable_, initialDelay_);
        }

     private:
        net::SocketImpl* socket_;
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

        Value operator()(JsArray::Ptr args) {
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

 private:
    static void freeParser(Parser* parser, OutgoingMessage* req) {
        assert(parser);
        parser->free();
        if (req) {
            req->parser_ = NULL;
        }
        delete parser;
    }

    static uv::Error::CPtr createHangUpError() {
        return uv::Error::create(uv::Error::_ECONNRESET);
    }

    static String::CPtr toHex(Size val) {
        Size n;
        to<Size>(val, &n);
        const Size kLen = ((sizeof(Size) << 3) / 3) + 3;
        char s[kLen];
        snprintf(s, kLen, "%zx", n);
        return String::create(s);
    }

    static void httpSocketSetup(net::SocketImpl::Ptr socket) {
        SocketOnDrain::Ptr onDrain = SocketOnDrain::create(socket);
        socket->removeAllListeners(net::Socket::EVENT_DRAIN);
        socket->on(net::Socket::EVENT_DRAIN, onDrain);
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

    void deferToConnect(
        JsFunction::Ptr method,
        JsFunction::Ptr callback = JsFunction::null()) {
        LIBJ_STATIC_SYMBOL_DEF(EVENT_SOCKET, "socket");

        JsFunction::Ptr onSocket(new DeferToConnect(this, method, callback));
        if (socket_) {
            (*onSocket)();
        } else {
            once(EVENT_SOCKET, onSocket);
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
        HAD_ERROR                       = 1 << 11,
        UPGRADE_OR_CONNECT              = 1 << 12,
        SERVER_RESPONSE                 = 1 << 13,
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
    Parser* parser_;
    Agent::Ptr agent_;
    String::CPtr socketPath_;
    IncomingMessage::Ptr res_;
    JsFunction::Ptr timeoutCb_;
    SocketOnClose::Ptr socketOnClose_;
    SocketCloseListener::Ptr socketCloseListener_;
    SocketErrorListener::Ptr socketErrorListener_;
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
        , parser_(NULL)
        , agent_(Agent::null())
        , socketPath_(String::null())
        , res_(IncomingMessage::null())
        , timeoutCb_(JsFunction::null())
        , socketOnClose_(new SocketOnClose())
        , socketCloseListener_(new SocketCloseListener())
        , socketErrorListener_(new SocketErrorListener())
        , ee_(EventEmitter::create()) {
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
