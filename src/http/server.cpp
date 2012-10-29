// Copyright (c) 2012 Plenluno All rights reserved.

#include <assert.h>
#include <http_parser.h>
#include <uv.h>

#include "libnode/http/server.h"
#include "./parser.h"
#include "./server_request_impl.h"
#include "./server_response_impl.h"
#include "../net/server_impl.h"
#include "../net/socket_impl.h"

namespace libj {
namespace node {
namespace http {

class ServerImpl : public Server {
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
    static void httpSocketSetup(net::SocketImpl::Ptr socket) {
        SocketOnDrain::Ptr onDrain = SocketOnDrain::create(socket);
        socket->removeListener(net::Socket::EVENT_DRAIN, onDrain);
        socket->on(net::Socket::EVENT_DRAIN, onDrain);
    }

    static void abortIncoming(JsArray::Ptr incomings) {
        static const String::CPtr strAborted = String::intern("aborted");

        while (incomings->length()) {
            IncomingMessage::Ptr req =
                toPtr<IncomingMessage>(incomings->shift());
            req->emit(strAborted);
            req->emit(IncomingMessage::EVENT_CLOSE);
        }
    }

 private:
    class SocketOnDrain : LIBJ_JS_FUNCTION(SocketOnDrain)
        static Ptr create(net::SocketImpl::Ptr socket) {
            return Ptr(new SocketOnDrain(&(*socket)));
        }

        Value operator()(JsArray::Ptr args) {
            static const String::CPtr strHttpMsg =
                String::intern("httpMessage");

            OutgoingMessage* httpMessage = NULL;
            to<OutgoingMessage*>(socket_->get(strHttpMsg), &httpMessage);
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
            parser_->free();
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
        static Ptr create() {
            return Ptr(new SocketOnData());
        }

        Value operator()(JsArray::Ptr args) {
            Buffer::CPtr buf = args->getCPtr<Buffer>(0);
            Int bytesParsed = parser_->execute(buf);
            IncomingMessage::Ptr req = parser_->incoming();
            if (bytesParsed < 0) {  // error
                // socket_->destroy(error);
            } else if (req && req->hasFlag(IncomingMessage::UPGRADE)) {
            }
            return libj::Status::OK;
        }

     private:
        Parser* parser_;
        net::SocketImpl::Ptr socket_;

        SocketOnData() {}
    };

    class SocketOnEnd : LIBJ_JS_FUNCTION(SocketOnEnd)
     public:
        static Ptr create() {
            return Ptr(new SocketOnEnd());
        }

        Value operator()(JsArray::Ptr args) {
            return libj::Status::OK;
        }
    };

    class ParserOnIncoming : LIBJ_JS_FUNCTION(ParserOnIncoming)
     public:
        static Ptr create() {
            return Ptr(new ParserOnIncoming());
        }

        Value operator()(JsArray::Ptr args) {
            return libj::Status::OK;
        }
    };

    class ServerOnConnection : LIBJ_JS_FUNCTION(ServerOnConnection)
     public:
        static Ptr create(ServerImpl* srv) {
            return Ptr(new ServerOnConnection(srv));
        }

        Value operator()(JsArray::Ptr args) {
            static const String::CPtr strParser = String::intern("parser");

            net::SocketImpl::Ptr socket = toPtr<net::SocketImpl>(args->get(0));
            assert(socket);

            JsArray::Ptr incomings = JsArray::create();
            JsArray::Ptr outgoings = JsArray::create();

            httpSocketSetup(socket);

            socket->setTimeout(2 * 60 * 1000);
            socket->once(
                net::Socket::EVENT_TIMEOUT,
                SocketOnTimeout::create(socket));

            Parser*  parser = new Parser(HTTP_REQUEST, socket);
            socket->put(strParser, parser);

            socket->addListener(
                EVENT_ERROR,
                SocketOnError::create(self_, socket));
            socket->addListener(
                EVENT_CLOSE,
                SocketOnClose::create(parser, incomings));

            socket->setOnData(SocketOnData::create());
            socket->setOnEnd(SocketOnEnd::create());

            parser->setOnIncoming(ParserOnIncoming::create());
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
            Error::CPtr err = args->getCPtr<Error>(0);
            net::SocketImpl* socket = NULL;
            to<net::SocketImpl*>(args->get(1), &socket);
            assert(socket);
            socket->destroy(err);
            return libj::Status::OK;
        }
    };

 private:
    net::ServerImpl::Ptr server_;

    ServerImpl() : server_(net::ServerImpl::create()) {}

    LIBNODE_NET_SERVER_IMPL(server_);
};

const String::CPtr Server::EVENT_REQUEST =
    String::intern("request");
const String::CPtr Server::EVENT_CHECK_CONTINUE =
    String::intern("checkContinue");
const String::CPtr Server::EVENT_CONNECT =
    String::intern("connect");
const String::CPtr Server::EVENT_UPGRADE =
    String::intern("upgrade");
const String::CPtr Server::EVENT_CLIENT_ERROR =
    String::intern("clientError");

Server::Ptr Server::create() {
    return ServerImpl::create();
}

}  // namespace http
}  // namespace node
}  // namespace libj
