// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_HTTP_SERVER_H_
#define LIBNODE_DETAIL_HTTP_SERVER_H_

#include <libnode/http/method.h>
#include <libnode/http/server.h>
#include <libnode/detail/net/server.h>
#include <libnode/detail/net/socket.h>
#include <libnode/detail/http/parser.h>
#include <libnode/detail/http/server_request.h>
#include <libnode/detail/http/server_response.h>

#include <assert.h>

namespace libj {
namespace node {
namespace detail {
namespace http {

class Server : public net::Server<node::http::Server> {
 public:
    LIBJ_MUTABLE_DEFS(Server, node::http::Server);

    static Ptr create(JsFunction::Ptr requestListener) {
        LIBJ_STATIC_SYMBOL_DEF(EVENT_DESTROY, "destroy");

        Server* server = new Server();

        server->on(
            EVENT_CONNECTION,
            JsFunction::Ptr(new ServerOnConnection(server)));
        server->on(
            EVENT_CLIENT_ERROR,
            JsFunction::Ptr(new ServerOnClientError()));

        if (requestListener) {
            server->on(EVENT_REQUEST, requestListener);
        }

        Ptr srv(server);
        server->on(EVENT_DESTROY, JsFunction::Ptr(new OnDestroy(srv)));
        return srv;
    }

    virtual Size maxHeadersCount() const {
        return maxHeadersCount_;
    }

    virtual void setMaxHeadersCount(Size max) {
        maxHeadersCount_ = max;
    }

 private:
    static void freeParser(Parser* parser) {
        assert(parser);
        parser->free();
        delete parser;
    }

    static void abortIncoming(JsArray::Ptr incomings) {
        LIBJ_STATIC_SYMBOL_DEF(EVENT_ABORTED, "aborted");

        while (incomings->length()) {
            IncomingMessage::Ptr req =
                toPtr<IncomingMessage>(incomings->shift());
            req->emit(EVENT_ABORTED);
            req->emit(IncomingMessage::EVENT_CLOSE);
#ifdef LIBNODE_REMOVE_LISTENER
            // 'close' only once
            req->removeAllListeners(IncomingMessage::EVENT_CLOSE);
            // no 'data' and 'end' after 'close'
            req->removeAllListeners(IncomingMessage::EVENT_DATA);
            req->removeAllListeners(IncomingMessage::EVENT_END);
#endif
        }
    }

 private:
    class SocketOnClose : LIBJ_JS_FUNCTION(SocketOnClose)
     public:
        SocketOnClose(Parser* parser, JsArray::Ptr incomings)
            : parser_(parser)
            , incomings_(incomings) {}

        virtual Value operator()(JsArray::Ptr args) {
            freeParser(parser_);
            parser_ = NULL;
            abortIncoming(incomings_);
            return Status::OK;
        }

     private:
        Parser* parser_;
        JsArray::Ptr incomings_;
    };

    class SocketOnTimeout : LIBJ_JS_FUNCTION(SocketOnTimeout)
     public:
        SocketOnTimeout(net::Socket* sock) : socket_(sock) {}

        virtual Value operator()(JsArray::Ptr args) {
            socket_->destroy();
            return Status::OK;
        }

     private:
        net::Socket* socket_;
    };

    class SocketOnError : LIBJ_JS_FUNCTION(SocketOnError)
     public:
        SocketOnError(
            Server* srv,
            net::Socket* sock)
            : self_(srv)
            , socket_(sock) {}

        virtual Value operator()(JsArray::Ptr args) {
            Error::CPtr err = args->getCPtr<Error>(0);
            self_->emit(EVENT_CLIENT_ERROR, err, socket_);
            return Status::OK;
        }

     private:
        Server* self_;
        net::Socket* socket_;
    };

    class SocketOnData : LIBJ_JS_FUNCTION(SocketOnData)
     public:
        SocketOnData(
            Server* srv,
            net::Socket* sock,
            Parser* parser,
            JsFunction::Ptr onClose)
            : self_(srv)
            , socket_(sock)
            , parser_(parser)
            , onClose_(onClose) {}

        virtual Value operator()(JsArray::Ptr args) {
            Buffer::CPtr buf = args->getCPtr<Buffer>(0);
            Int bytesParsed = parser_->execute(buf);
            IncomingMessage::Ptr req = parser_->incoming();
            if (bytesParsed < 0) {
                Error::Code err = Error::ILLEGAL_DATA_FORMAT;
                socket_->destroy(Error::create(err));
                return err;
            } else if (req && req->hasFlag(IncomingMessage::UPGRADE)) {
                JsFunction::Ptr prev = socket_->setOnData(JsFunction::null());
                assert(&(*prev) == this);
                socket_->setOnEnd(JsFunction::null());
                socket_->removeListener(net::Socket::EVENT_CLOSE, onClose_);

                assert(parser_);
                parser_->finish();
                freeParser(parser_);
                parser_ = NULL;

                Boolean isConnect =
                    req->method()->equals(node::http::METHOD_CONNECT);
                String::CPtr event = isConnect ? EVENT_CONNECT : EVENT_UPGRADE;
                if (self_->listeners(event)->length()) {
                    self_->emit(
                        event,
                        ServerRequest::create(req),
                        req->socket(),
                        buf->slice(bytesParsed));
                } else {
                    socket_->destroy();
                }
            }
            return Status::OK;
        }

     private:
        Server* self_;
        net::Socket* socket_;
        Parser* parser_;
        JsFunction::Ptr onClose_;
    };

    class SocketOnEnd : LIBJ_JS_FUNCTION(SocketOnEnd)
     public:
        SocketOnEnd(
            Server* srv,
            net::Socket* sock,
            Parser* parser,
            JsArray::Ptr incomings,
            JsArray::Ptr outgoings)
            : self_(srv)
            , socket_(sock)
            , parser_(parser)
            , incomings_(incomings)
            , outgoings_(outgoings) {}

        virtual Value operator()(JsArray::Ptr args) {
            if (!parser_->finish()) {
                Error::Code err = Error::ILLEGAL_DATA_FORMAT;
                socket_->destroy(Error::create(err));
                return err;
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
            return Status::OK;
        }

     private:
        Server* self_;
        net::Socket* socket_;
        Parser* parser_;
        JsArray::Ptr incomings_;
        JsArray::Ptr outgoings_;
    };

    class OutgoingOnFinish : LIBJ_JS_FUNCTION(OutgoingOnFinish)
     public:
        OutgoingOnFinish(
            OutgoingMessage* res,
            net::Socket::Ptr socket,
            JsArray::Ptr incomings,
            JsArray::Ptr outgoings)
            : res_(res)
            , socket_(socket)
            , incomings_(incomings)
            , outgoings_(outgoings) {}

        virtual Value operator()(JsArray::Ptr args) {
            incomings_->shift();

            res_->detachSocket(socket_);

            // no event after 'finish'
            res_->removeAllListeners();

            if (res_->hasFlag(OutgoingMessage::LAST)) {
                socket_->destroySoon();
            } else {
                OutgoingMessage::Ptr msg =
                    toPtr<OutgoingMessage>(outgoings_->shift());
                if (msg) {
                    msg->assignSocket(msg, socket_);
                }
            }
            return Status::OK;
        }

     private:
        OutgoingMessage* res_;
        net::Socket::Ptr socket_;
        JsArray::Ptr incomings_;
        JsArray::Ptr outgoings_;
    };

    class ParserOnIncoming : LIBJ_JS_FUNCTION(ParserOnIncoming)
     public:
        ParserOnIncoming(
            Server* srv,
            net::Socket::Ptr sock,
            JsArray::Ptr incomings,
            JsArray::Ptr outgoings)
            : self_(srv)
            , socket_(sock)
            , incomings_(incomings)
            , outgoings_(outgoings) {}

        virtual Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(EVENT_FINISH, "finish");
            LIBJ_STATIC_SYMBOL_DEF(symContExp,   "100-continue");

            IncomingMessage::Ptr in = args->getPtr<IncomingMessage>(0);
            Boolean shouldKeepAlive = false;
            to<Boolean>(args->get(1), &shouldKeepAlive);

            incomings_->push(in);

            OutgoingMessage::Ptr out = OutgoingMessage::createInServer(in);
            if (shouldKeepAlive) {
                out->setFlag(OutgoingMessage::SHOULD_KEEP_ALIVE);
            } else {
                out->unsetFlag(OutgoingMessage::SHOULD_KEEP_ALIVE);
            }

            if (socket_->httpMessage()) {
                outgoings_->push(out);
            } else {
                out->assignSocket(out, socket_);
            }

            JsFunction::Ptr onFinish(new OutgoingOnFinish(
                    &(*out), socket_, incomings_, outgoings_));
            out->on(EVENT_FINISH, onFinish);

            ServerRequest::Ptr req = ServerRequest::create(in);
            ServerResponse::Ptr res = ServerResponse::create(out);
            String::CPtr expectHeader =
                in->getHeader(node::http::LHEADER_EXPECT);
            if (in->httpVersionMajor() == 1 &&
                in->httpVersionMinor() == 1 &&
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
        Server* self_;
        net::Socket::Ptr socket_;
        JsArray::Ptr incomings_;
        JsArray::Ptr outgoings_;
    };

    class ServerOnConnection : LIBJ_JS_FUNCTION(ServerOnConnection)
     public:
        ServerOnConnection(Server* srv) : self_(srv) {}

        virtual Value operator()(JsArray::Ptr args) {
            net::Socket::Ptr socket = toPtr<net::Socket>(args->get(0));
            assert(socket);

            JsArray::Ptr incomings = JsArray::create();
            JsArray::Ptr outgoings = JsArray::create();

            OutgoingMessage::httpSocketSetup(socket);

            socket->setTimeout(2 * 60 * 1000);
            socket->once(
                net::Socket::EVENT_TIMEOUT,
                JsFunction::Ptr(new SocketOnTimeout(&(*socket))));

            Size maxHeadersCount;
            if (self_->maxHeadersCount_) {
                maxHeadersCount = self_->maxHeadersCount_;
            } else {
                maxHeadersCount = 2000 >> 1;
            }

            Parser* parser = new Parser(HTTP_REQUEST, socket, maxHeadersCount);
            socket->setParser(parser);

            JsFunction::Ptr onClose(new SocketOnClose(parser, incomings));
            socket->on(net::Socket::EVENT_CLOSE, onClose);

            JsFunction::Ptr onError(new SocketOnError(self_, &(*socket)));
            socket->on(net::Socket::EVENT_ERROR, onError);

            JsFunction::Ptr onData(new SocketOnData(
                    self_, &(*socket), parser, onClose));
            socket->setOnData(onData);

            JsFunction::Ptr onEnd(new SocketOnEnd(
                    self_, &(*socket), parser, incomings, outgoings));
            socket->setOnEnd(onEnd);

            JsFunction::Ptr onIncoming(new ParserOnIncoming(
                    self_, socket, incomings, outgoings));
            parser->setOnIncoming(onIncoming);

            return Status::OK;
        }

     private:
        Server* self_;
    };

    class ServerOnClientError : LIBJ_JS_FUNCTION(ServerOnClientError)
     public:
        virtual Value operator()(JsArray::Ptr args) {
            Error::CPtr err = args->getCPtr<Error>(0);
            net::Socket* conn = NULL;
            to<net::Socket*>(args->get(1), &conn);
            if (conn) conn->destroy(err);
            return Status::OK;
        }
    };

 private:
    Size maxHeadersCount_;

    Server() : maxHeadersCount_(0) {
        setFlag(ALLOW_HALF_OPEN);
    }
};

}  // namespace http
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_HTTP_SERVER_H_
