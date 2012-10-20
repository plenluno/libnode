// Copyright (c) 2012 Plenluno All rights reserved.

#include <assert.h>
#include <http_parser.h>
#include <uv.h>

#include "libnode/http/server.h"
#include "./server_request_impl.h"
#include "./server_response_impl.h"
#include "../net/server_impl.h"
#include "../net/socket_impl.h"

namespace libj {
namespace node {
namespace http {

class ParserContext {
 public:
    ParserContext()
        : url(String::create())
        , headers(JsArray::create()) {}

    http_parser parser;
    String::CPtr url;
    JsArray::Ptr headers;
    IncomingMessage::Ptr incoming;
};

class ServerImpl;

class ServerContext {
 public:
    ServerContext(
        ServerImpl* srv,
        net::SocketImpl::Ptr sock)
        : server(srv)
        , request(ServerRequestImpl::create(sock))
        , response(ServerResponseImpl::create(sock)) {
        parser.data = this;
    }

    ServerImpl* server;
    ServerRequestImpl::Ptr request;
    ServerResponseImpl::Ptr response;
    http_parser parser;
};

class ServerImpl : public Server {
 public:
    static Ptr create() {
        return Ptr(new ServerImpl());
    }

    Boolean listen(Int port, String::CPtr hostName, Int backlog) {
        server_->on(
            net::Server::EVENT_CLOSE,
            OnServerClose::create(this));
        server_->on(
            net::Server::EVENT_LISTENING,
            OnServerListening::create());
        server_->on(
            net::Server::EVENT_CONNECTION,
            OnServerConnection::create(this));
        return server_->listen(port, hostName, backlog);
    }

    void close() {
        server_->close();
    }

 private:
    static http_parser_settings settings;

    class OnServerClose : LIBJ_JS_FUNCTION(OnServerClose)
     public:
        static Ptr create(ServerImpl* srv) {
            return Ptr(new OnServerClose(srv));
        }

        Value operator()(JsArray::Ptr args) {
            server_->emit(EVENT_CLOSE);
            return libj::Status::OK;
        }

     private:
        ServerImpl* server_;

        OnServerClose(ServerImpl* srv) : server_(srv) {}
    };

    class OnServerListening : LIBJ_JS_FUNCTION(OnServerListening)
     public:
        static Ptr create() {
            return Ptr(new OnServerListening());
        }

        Value operator()(JsArray::Ptr args) {
            settings.on_url = ServerImpl::onUrl;
            settings.on_header_field = ServerImpl::onHeaderField;
            settings.on_header_value = ServerImpl::onHeaderValue;
            settings.on_headers_complete = ServerImpl::onHeadersComplete;
            settings.on_message_begin = ServerImpl::onMessageBegin;
            settings.on_body = ServerImpl::onBody;
            settings.on_message_complete = ServerImpl::onMessageComplete;
            return libj::Status::OK;
        }
    };

    class OnServerConnection : LIBJ_JS_FUNCTION(OnServerConnection)
     public:
        static Ptr create(ServerImpl* srv) {
            return Ptr(new OnServerConnection(srv));
        }

        Value operator()(JsArray::Ptr args) {
            net::SocketImpl::Ptr socket = toPtr<net::SocketImpl>(args->get(0));
            assert(socket);
            ServerContext* context = new ServerContext(server_, socket);
            http_parser_init(&context->parser, HTTP_REQUEST);
            socket->on(
                net::Socket::EVENT_DATA,
                OnSocketData::create(context));
            socket->on(
                net::Socket::EVENT_CLOSE,
                OnSocketClose::create(context));
            server_->emit(EVENT_CONNECTION, args);
            return libj::Status::OK;
        }

     private:
        ServerImpl* server_;

        OnServerConnection(ServerImpl* srv) : server_(srv) {}
    };

    class OnSocketData : LIBJ_JS_FUNCTION(OnSocketData)
     public:
        static Ptr create(ServerContext* ctxt) {
            assert(ctxt);
            return Ptr(new OnSocketData(ctxt));
        }

        Value operator()(JsArray::Ptr args) {
            http_parser* httpParser = &context_->parser;
            Buffer::Ptr buf = toPtr<Buffer>(args->get(0));
            assert(buf);
            size_t numRead = buf->length();
            size_t numParsed = http_parser_execute(
                                httpParser,
                                &settings,
                                static_cast<const char*>(buf->data()),
                                numRead);
            ServerRequestImpl::Ptr request = context_->request;
            net::SocketImpl::Ptr socket =
                LIBJ_STATIC_PTR_CAST(net::SocketImpl)(request->connection());
            if (httpParser->upgrade) {
                socket->removeAllListeners();
                context_->server->emit(
                    EVENT_UPGRADE,
                    request, socket, buf->slice(numParsed));
                delete context_;
            } else if (numParsed < numRead) {  // parse error
                socket->destroy(
                    libj::Error::create(libj::Error::ILLEGAL_REQUEST));
            }
            return libj::Status::OK;
        }

     private:
        ServerContext* context_;

        OnSocketData(ServerContext* ctxt) : context_(ctxt) {}
    };

    class OnSocketClose : LIBJ_JS_FUNCTION(OnSocketClose)
     public:
        static Ptr create(ServerContext* ctxt) {
            assert(ctxt);
            return Ptr(new OnSocketClose(ctxt));
        }

        Value operator()(JsArray::Ptr args) {
            if (context_) {
                context_->request->emit(ServerRequest::EVENT_CLOSE);
                context_->response->emit(ServerResponse::EVENT_CLOSE);
                delete context_;
                context_ = NULL;
            }
            return libj::Status::OK;
        }

     private:
        ServerContext* context_;

        OnSocketClose(ServerContext* ctxt) : context_(ctxt) {}
    };

    static int onUrl(http_parser* parser, const char* at, size_t length) {
        ServerContext* context = static_cast<ServerContext*>(parser->data);
        String::CPtr url = String::create(at, String::UTF8, length);
        context->request->setUrl(url);
        return 0;
    }

    static String::CPtr headerName;

    static int onHeaderField(
        http_parser* parser, const char* at, size_t length) {
        headerName = String::create(at, String::UTF8, length);
        return 0;
    }

    static int onHeaderValue(
        http_parser* parser, const char* at, size_t length) {
        ServerContext* context = static_cast<ServerContext*>(parser->data);
        context->request->setHeader(
            headerName,
            String::create(at, String::UTF8, length));
        return 0;
    }

    static int onHeadersComplete(http_parser* parser) {
        static const String::CPtr methodDelete = String::create("DELETE");
        static const String::CPtr methodGet = String::create("GET");
        static const String::CPtr methodHead = String::create("HEAD");
        static const String::CPtr methodPost = String::create("POST");
        static const String::CPtr methodPut = String::create("PUT");
        static const String::CPtr methodConnect = String::create("CONNECT");
        static const String::CPtr methodOptions = String::create("OPTIONS");
        static const String::CPtr methodTrace = String::create("TRACE");
        static const String::CPtr dot = String::create(".");

        ServerContext* context = static_cast<ServerContext*>(parser->data);
        assert(context);
        switch (parser->method) {
        case HTTP_DELETE:
            context->request->setMethod(methodDelete);
            break;
        case HTTP_GET:
            context->request->setMethod(methodGet);
            break;
        case HTTP_HEAD:
            context->request->setMethod(methodHead);
            break;
        case HTTP_POST:
            context->request->setMethod(methodPost);
            break;
        case HTTP_PUT:
            context->request->setMethod(methodPut);
            break;
        case HTTP_CONNECT:
            context->request->setMethod(methodConnect);
            break;
        case HTTP_OPTIONS:
            context->request->setMethod(methodOptions);
            break;
        case HTTP_TRACE:
            context->request->setMethod(methodTrace);
            break;
        default:
            context->request->setMethod(String::create());
        }

        Int majorVer = static_cast<Int>(parser->http_major);
        Int minorVer = static_cast<Int>(parser->http_minor);
        String::CPtr httpVer = String::valueOf(majorVer);
        httpVer = httpVer->concat(dot);
        httpVer = httpVer->concat(String::valueOf(minorVer));
        context->request->setHttpVersion(httpVer);

        ServerRequest::Ptr req(context->request);
        ServerResponse::Ptr res(context->response);
        assert(context->server);
        context->server->emit(Server::EVENT_REQUEST, req, res);
        return 0;
    }

    static int onMessageBegin(http_parser* parser) {
        return 0;
    }

    static int onBody(http_parser* parser, const char* at, size_t length) {
        ServerContext* context = static_cast<ServerContext*>(parser->data);
        ServerRequestImpl::Ptr request = context->request;
        if (request) {
            Buffer::Ptr chunk = Buffer::create(at, length);
            JsArray::Ptr args = JsArray::create();
            if (request->hasEncoding()) {
                Buffer::Encoding enc = request->getEncoding();
                args->add(chunk->toString(enc));
            } else {
                args->add(chunk);
            }
            request->emit(ServerRequest::EVENT_DATA, args);
        }
        return 0;
    }

    static int onMessageComplete(http_parser* parser) {
        ServerContext* context = static_cast<ServerContext*>(parser->data);
        ServerRequestImpl::Ptr request = context->request;
        if (request) {
            request->emit(ServerRequest::EVENT_END);
        }
        return 0;
    }

 private:
    EventEmitter::Ptr ee_;
    net::ServerImpl::Ptr server_;

    ServerImpl()
        : ee_(EventEmitter::create())
        , server_(net::Server::create()) {}

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

http_parser_settings ServerImpl::settings = {};
String::CPtr ServerImpl::headerName = String::null();

const String::CPtr Server::IN_ADDR_ANY = String::intern("0.0.0.0");
const String::CPtr Server::EVENT_REQUEST = String::intern("request");
const String::CPtr Server::EVENT_CONNECTION = String::intern("connection");
const String::CPtr Server::EVENT_CLOSE = String::intern("close");
const String::CPtr Server::EVENT_UPGRADE = String::intern("upgrade");

Server::Ptr Server::create() {
    return ServerImpl::create();
}

}  // namespace http
}  // namespace node
}  // namespace libj
