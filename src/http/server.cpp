// Copyright (c) 2012 Plenluno All rights reserved.

#include <assert.h>

#include "libnode/http/method.h"
#include "libnode/http/server.h"

#include "./parser.h"
#include "./server_request_impl.h"
#include "./server_response_impl.h"
#include "../net/server_impl.h"
#include "../net/socket_impl.h"

namespace libj {
namespace node {
namespace http {

class ServerImpl : public FlagMixin, public Server {
 public:
    static Ptr create() {
        ServerImpl* httpSrv = new ServerImpl();
        httpSrv->server_->setFlag(net::ServerImpl::ALLOW_HALF_OPEN);
        httpSrv->addListener(
            EVENT_CONNECTION,
            ServerOnConnection::create(httpSrv));
        httpSrv->addListener(
            EVENT_CLIENT_ERROR,
            ServerOnClientError::create());
        return Ptr(httpSrv);
    }

 private:
    static OutgoingMessage::Ptr createOutgoing(IncomingMessage::Ptr req) {
        LIBJ_STATIC_SYMBOL_DEF(symChunked, "chunked");

        OutgoingMessage::Ptr out = OutgoingMessage::create();
        out->setFlag(OutgoingMessage::SERVER_SIDE);
        out->setFlag(OutgoingMessage::SEND_DATE);

        if (req->method()->equals(METHOD_HEAD)) {
            out->unsetFlag(OutgoingMessage::HAS_BODY);
        }

        if (req->httpVersionMajor() < 1 ||
            (req->httpVersionMajor() == 1 &&
             req->httpVersionMinor() == 0)) {
            out->unsetFlag(OutgoingMessage::SHOULD_KEEP_ALIVE);
            if (req->getHeader(HEADER_TE)->equals(symChunked)) {
                out->setFlag(OutgoingMessage::USE_CHUNKED_ENCODING_BY_DEFAULT);
            }
        }
        return out;
    }

    static void freeParser(Parser* parser) {
        assert(parser);
        parser->free();
        delete parser;
    }

    static void httpSocketSetup(net::SocketImpl::Ptr socket) {
        SocketOnDrain::Ptr onDrain = SocketOnDrain::create(socket);
        socket->removeAllListeners(net::Socket::EVENT_DRAIN);
        socket->on(net::Socket::EVENT_DRAIN, onDrain);
    }

    static void abortIncoming(JsArray::Ptr incomings) {
        LIBJ_STATIC_SYMBOL_DEF(EVENT_ABORTED, "aborted");

        while (incomings->length()) {
            IncomingMessage::Ptr req =
                toPtr<IncomingMessage>(incomings->shift());
            req->emit(EVENT_ABORTED);
            req->emit(IncomingMessage::EVENT_CLOSE);
        }
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
        static Ptr create(Parser* parser, JsArray::Ptr incomings) {
            return Ptr(new SocketOnClose(parser, incomings));
        }

        Value operator()(JsArray::Ptr args) {
            freeParser(parser_);
            parser_ = NULL;
            abortIncoming(incomings_);
            return libj::Status::OK;
        }

     private:
        Parser* parser_;
        JsArray::Ptr incomings_;

        SocketOnClose(Parser* parser, JsArray::Ptr incomings)
            : parser_(parser)
            , incomings_(incomings) {}
    };

    class SocketOnTimeout : LIBJ_JS_FUNCTION(SocketOnTimeout)
     public:
        static Ptr create(net::SocketImpl::Ptr socket) {
            return Ptr(new SocketOnTimeout(&(*socket)));
        }

        Value operator()(JsArray::Ptr args) {
            socket_->destroy();
            return libj::Status::OK;
        }

     private:
        net::SocketImpl* socket_;

        SocketOnTimeout(net::SocketImpl* sock) : socket_(sock) {}
    };

    class SocketOnError : LIBJ_JS_FUNCTION(SocketOnError)
     public:
        static Ptr create(
            ServerImpl* server,
            net::SocketImpl::Ptr socket) {
            return Ptr(new SocketOnError(server, &(*socket)));
        }

        Value operator()(JsArray::Ptr args) {
            Error::CPtr err = args->getCPtr<Error>(0);
            self_->emit(EVENT_CLIENT_ERROR, err, socket_);
            return libj::Status::OK;
        }

     private:
        ServerImpl* self_;
        net::SocketImpl* socket_;

        SocketOnError(
            ServerImpl* srv,
            net::SocketImpl* sock)
            : self_(srv)
            , socket_(sock) {}
    };

    class SocketOnData : LIBJ_JS_FUNCTION(SocketOnData)
     public:
        static Ptr create(
            ServerImpl* server,
            Parser* parser,
            net::SocketImpl::Ptr socket,
            JsFunction::Ptr onClose) {
            return Ptr(new SocketOnData(server, parser, &(*socket), onClose));
        }

        Value operator()(JsArray::Ptr args) {
            Buffer::CPtr buf = args->getCPtr<Buffer>(0);
            Int bytesParsed = parser_->execute(buf);
            IncomingMessage::Ptr req = parser_->incoming();
            if (bytesParsed < 0) {
                libj::Error::CPtr err =
                    libj::Error::create(libj::Error::ILLEGAL_DATA_FORMAT);
                socket_->destroy(err);
            } else if (req && req->hasFlag(IncomingMessage::UPGRADE)) {
                socket_->setOnData(JsFunction::null());
                socket_->setOnEnd(JsFunction::null());
                socket_->removeListener(net::Socket::EVENT_CLOSE, onClose_);

                parser_->finish();
                freeParser(parser_);
                parser_ = NULL;

                Boolean isConnect = req->method()->equals(METHOD_CONNECT);
                String::CPtr event = isConnect ? EVENT_CONNECT : EVENT_UPGRADE;
                if (self_->listeners(event)->length()) {
                    self_->emit(
                        event,
                        ServerRequestImpl::create(req),
                        req->socket(),
                        buf->slice(bytesParsed));
                } else {
                    socket_->destroy();
                }
            }
            return libj::Status::OK;
        }

     private:
        ServerImpl* self_;
        Parser* parser_;
        net::SocketImpl* socket_;
        JsFunction::Ptr onClose_;

        SocketOnData(
            ServerImpl* srv,
            Parser* parser,
            net::SocketImpl* sock,
            JsFunction::Ptr onClose)
            : self_(srv)
            , parser_(parser)
            , socket_(sock)
            , onClose_(onClose) {}
    };

    class SocketOnEnd : LIBJ_JS_FUNCTION(SocketOnEnd)
     public:
        static Ptr create(
            ServerImpl* server,
            Parser* parser,
            net::SocketImpl::Ptr socket,
            JsArray::Ptr incomings,
            JsArray::Ptr outgoings) {
            return Ptr(new SocketOnEnd(
                server, parser, &(*socket), incomings, outgoings));
        }

        Value operator()(JsArray::Ptr args) {
            if (!parser_->finish()) {
                libj::Error::CPtr err =
                    libj::Error::create(libj::Error::ILLEGAL_DATA_FORMAT);
                socket_->destroy(err);
            }

            if (!self_->hasFlag(HTTP_ALLOW_HALF_OPEN)) {
                abortIncoming(incomings_);
                if (socket_->writable()) socket_->end();
            } else if (outgoings_->length()) {
                Size lastIndex = outgoings_->length() - 1;
                OutgoingMessage::Ptr lastMsg =
                    outgoings_->getPtr<OutgoingMessage>(lastIndex);
                lastMsg->setFlag(OutgoingMessage::LAST);
            } else if (socket_->httpMessage()) {
                socket_->httpMessage()->setFlag(OutgoingMessage::LAST);
            } else if (socket_->writable()) {
                socket_->end();
            }
            return libj::Status::OK;
        }

     private:
        ServerImpl* self_;
        Parser* parser_;
        net::SocketImpl* socket_;
        JsArray::Ptr incomings_;
        JsArray::Ptr outgoings_;

        SocketOnEnd(
            ServerImpl* srv,
            Parser* parser,
            net::SocketImpl* sock,
            JsArray::Ptr incomings,
            JsArray::Ptr outgoings)
            : self_(srv)
            , parser_(parser)
            , socket_(sock)
            , incomings_(incomings)
            , outgoings_(outgoings) {}
    };

    class OutgoingMessageOnFinish : LIBJ_JS_FUNCTION(OutgoingMessageOnFinish)
     public:
        static Ptr create(
            OutgoingMessage::Ptr res,
            net::SocketImpl::Ptr socket,
            JsArray::Ptr incomings,
            JsArray::Ptr outgoings) {
            return Ptr(new OutgoingMessageOnFinish(
                &(*res), socket, incomings, outgoings));
        }

        Value operator()(JsArray::Ptr args) {
            incomings_->shift();
            res_->detachSocket(socket_);
            if (res_->hasFlag(OutgoingMessage::LAST)) {
                socket_->destroySoon();
            } else {
                OutgoingMessage::Ptr msg =
                    toPtr<OutgoingMessage>(outgoings_->shift());
                if (msg) {
                    msg->assignSocket(msg, socket_);
                }
            }
            return libj::Status::OK;
        }

     private:
        OutgoingMessage* res_;
        net::SocketImpl::Ptr socket_;
        JsArray::Ptr incomings_;
        JsArray::Ptr outgoings_;

        OutgoingMessageOnFinish(
            OutgoingMessage* res,
            net::SocketImpl::Ptr socket,
            JsArray::Ptr incomings,
            JsArray::Ptr outgoings)
            : res_(res)
            , socket_(socket)
            , incomings_(incomings)
            , outgoings_(outgoings) {}
    };

    class ParserOnIncoming : LIBJ_JS_FUNCTION(ParserOnIncoming)
     public:
        static Ptr create(
            ServerImpl* server,
            net::SocketImpl::Ptr socket,
            JsArray::Ptr incomings,
            JsArray::Ptr outgoings) {
            return Ptr(new ParserOnIncoming(
                server, socket, incomings, outgoings));
        }

        Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_FINISH, "finish");
            LIBJ_STATIC_SYMBOL_DEF(symV11,       "1.1");
            LIBJ_STATIC_SYMBOL_DEF(symExpect,    "expect");
            LIBJ_STATIC_SYMBOL_DEF(symContExp,   "100-continue");

            IncomingMessage::Ptr in = args->getPtr<IncomingMessage>(0);
            Boolean shouldKeepAlive = false;
            to<Boolean>(args->get(1), &shouldKeepAlive);

            OutgoingMessage::Ptr out = createOutgoing(in);
            incomings_->push(in);
            if (shouldKeepAlive) {
                out->setFlag(OutgoingMessage::SHOULD_KEEP_ALIVE);
            } else {
                out->unsetFlag(OutgoingMessage::SHOULD_KEEP_ALIVE);
            }

            OutgoingMessage* httpMessage = socket_->httpMessage();
            if (httpMessage) {
                outgoings_->push(out);
            } else {
                out->assignSocket(out, socket_);
            }

            out->on(
                EVENT_FINISH,
                OutgoingMessageOnFinish::create(
                    out, socket_, incomings_, outgoings_));

            ServerRequest::Ptr req = ServerRequestImpl::create(in);
            ServerResponse::Ptr res = ServerResponseImpl::create(out);
            String::CPtr expectHeader = in->getHeader(symExpect);
            if (in->httpVersion()->equals(symV11) &&
                expectHeader &&
                expectHeader->toLowerCase()->equals(symContExp)) {
                out->setFlag(OutgoingMessage::EXPECT_CONTINUE);
                if (self_->listeners(EVENT_CHECK_CONTINUE)->length()) {
                    self_->emit(EVENT_CHECK_CONTINUE, req, res);
                } else {
                    out->writeContinue();
                    self_->emit(EVENT_REQUEST, req, res);
                }
            } else {
                self_->emit(EVENT_REQUEST, req, res);
            }
            return false;
        }

     private:
        ServerImpl* self_;
        net::SocketImpl::Ptr socket_;
        JsArray::Ptr incomings_;
        JsArray::Ptr outgoings_;

        ParserOnIncoming(
            ServerImpl* srv,
            net::SocketImpl::Ptr sock,
            JsArray::Ptr incomings,
            JsArray::Ptr outgoings)
            : self_(srv)
            , socket_(sock)
            , incomings_(incomings)
            , outgoings_(outgoings) {}
    };

    class ServerOnConnection : LIBJ_JS_FUNCTION(ServerOnConnection)
     public:
        static Ptr create(ServerImpl* srv) {
            return Ptr(new ServerOnConnection(srv));
        }

        Value operator()(JsArray::Ptr args) {
            net::SocketImpl::Ptr socket = toPtr<net::SocketImpl>(args->get(0));
            assert(socket);

            JsArray::Ptr incomings = JsArray::create();
            JsArray::Ptr outgoings = JsArray::create();

            httpSocketSetup(socket);

#if 0
            socket->setTimeout(2 * 60 * 1000);
            socket->once(
                net::Socket::EVENT_TIMEOUT,
                SocketOnTimeout::create(socket));
#endif

            Size maxHeaders;
            if (self_->maxHeadersCount_) {
                maxHeaders = self_->maxHeadersCount_;
            } else {
                maxHeaders = 2000;
            }
            Parser* parser = new Parser(HTTP_REQUEST, socket, maxHeaders);
            socket->setParser(parser);

            JsFunction::Ptr socketOnClose =
                SocketOnClose::create(parser, incomings);
            socket->addListener(
                EVENT_ERROR,
                SocketOnError::create(self_, socket));
            socket->addListener(
                EVENT_CLOSE,
                socketOnClose);

            socket->setOnData(
                SocketOnData::create(
                    self_, parser, socket, socketOnClose));
            socket->setOnEnd(
                SocketOnEnd::create(
                    self_, parser, socket, incomings, outgoings));

            parser->setOnIncoming(
                ParserOnIncoming::create(
                    self_, socket, incomings, outgoings));
            return libj::Status::OK;
        }

     private:
        ServerImpl* self_;

        ServerOnConnection(ServerImpl* srv) : self_(srv) {}
    };

    class ServerOnClientError : LIBJ_JS_FUNCTION(ServerOnClientError)
     public:
        static Ptr create() {
            return Ptr(new ServerOnClientError());
        }

        Value operator()(JsArray::Ptr args) {
            libj::Error::CPtr err = args->getCPtr<libj::Error>(0);
            net::SocketImpl* socket = NULL;
            to<net::SocketImpl*>(args->get(1), &socket);
            assert(socket);
            socket->destroy(err);
            return libj::Status::OK;
        }
    };

 public:
    enum Flag {
        HTTP_ALLOW_HALF_OPEN = 1 << 0,
    };

 private:
    net::ServerImpl::Ptr server_;
    Size maxHeadersCount_;

    ServerImpl()
        : server_(net::ServerImpl::create())
        , maxHeadersCount_(0) {}

    LIBNODE_NET_SERVER_IMPL(server_);
};

LIBJ_SYMBOL_DEF(Server::EVENT_REQUEST,        "request");
LIBJ_SYMBOL_DEF(Server::EVENT_CHECK_CONTINUE, "checkContinue");
LIBJ_SYMBOL_DEF(Server::EVENT_CONNECT,        "connect");
LIBJ_SYMBOL_DEF(Server::EVENT_UPGRADE,        "upgrade");
LIBJ_SYMBOL_DEF(Server::EVENT_CLIENT_ERROR,   "clientError");

Server::Ptr Server::create() {
    return ServerImpl::create();
}

}  // namespace http
}  // namespace node
}  // namespace libj
