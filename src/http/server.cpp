// Copyright (c) 2012 Plenluno All rights reserved.

#include <assert.h>
#include <uv.h>
#include <string>

#include "libnode/http/server.h"
#include "./server_context.h"

namespace libj {
namespace node {
namespace http {

class ServerImpl : public Server {
 public:
    static Ptr create() {
        Ptr p(new ServerImpl());
        return p;
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
            Ptr p(new OnServerClose(srv));
            return p;
        }

        Value operator()(JsArray::Ptr args) {
            server_->emit(EVENT_CLOSE, JsArray::create());
            return Status::OK;
        }

     private:
        ServerImpl* server_;

        OnServerClose(ServerImpl* srv) : server_(srv) {}
    };

    class OnServerListening : LIBJ_JS_FUNCTION(OnServerListening)
     public:
        static Ptr create() {
            Ptr p(new OnServerListening());
            return p;
        }

        Value operator()(JsArray::Ptr args) {
            settings.on_url = ServerImpl::onUrl;
            settings.on_header_field = ServerImpl::onHeaderField;
            settings.on_header_value = ServerImpl::onHeaderValue;
            settings.on_headers_complete = ServerImpl::onHeadersComplete;
            settings.on_message_begin = ServerImpl::onMessageBegin;
            settings.on_body = ServerImpl::onBody;
            settings.on_message_complete = ServerImpl::onMessageComplete;
            return Status::OK;
        }
    };

    class OnServerConnection : LIBJ_JS_FUNCTION(OnServerConnection)
     public:
        static Ptr create(ServerImpl* srv) {
            Ptr p(new OnServerConnection(srv));
            return p;
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
            return Status::OK;
        }

     private:
        ServerImpl* server_;

        OnServerConnection(ServerImpl* srv) : server_(srv) {}
    };

    class OnSocketData : LIBJ_JS_FUNCTION(OnSocketData)
     public:
        static Ptr create(ServerContext* ctxt) {
            assert(ctxt);
            Ptr p(new OnSocketData(ctxt));
            return p;
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
            if (httpParser->upgrade) {
                context_->socket->removeAllListeners();
                JsArray::Ptr args = JsArray::create();
                args->add(context_->request);
                args->add(context_->socket);
                args->add(buf->slice(numParsed));
                context_->server->emit(EVENT_UPGRADE, args);
                delete context_;
            } else if (numParsed < numRead) {  // parse error
                JsArray::Ptr emptyArgs = JsArray::create();
                context_->request->emit(ServerRequest::EVENT_CLOSE, emptyArgs);
                context_->response->emit(ServerRequest::EVENT_CLOSE, emptyArgs);
                context_->socket->destroy();
            }
            return Status::OK;
        }

     private:
        ServerContext* context_;

        OnSocketData(ServerContext* ctxt) : context_(ctxt) {}
    };

    class OnSocketClose : LIBJ_JS_FUNCTION(OnSocketClose)
     public:
        static Ptr create(ServerContext* ctxt) {
            assert(ctxt);
            Ptr p(new OnSocketClose(ctxt));
            return p;
        }

        Value operator()(JsArray::Ptr args) {
            delete context_;
            return Status::OK;
        }

     private:
        ServerContext* context_;

        OnSocketClose(ServerContext* ctxt) : context_(ctxt) {}
    };

    static int onUrl(http_parser* parser, const char* at, size_t length) {
        ServerContext* context = static_cast<ServerContext*>(parser->data);
        String::CPtr url = String::create(at, String::ASCII, length);
        context->request->setUrl(url);
        return 0;
    }

    static String::CPtr headerName;

    static int onHeaderField(
        http_parser* parser, const char* at, size_t length) {
        headerName = String::create(at, String::ASCII, length);
        return 0;
    }

    static int onHeaderValue(
        http_parser* parser, const char* at, size_t length) {
        ServerContext* context = static_cast<ServerContext*>(parser->data);
        context->request->setHeader(
            headerName,
            String::create(at, String::ASCII, length));
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

        JsArray::Ptr args = JsArray::create();
        ServerRequest::Ptr req(context->request);
        ServerResponse::Ptr res(context->response);
        args->add(req);
        args->add(res);
        assert(context->server);
        context->server->emit(Server::EVENT_REQUEST, args);
        return 0;
    }

    static int onMessageBegin(http_parser* parser) {
        return 0;
    }

    static int onBody(http_parser* parser, const char* at, size_t length) {
        ServerContext* context = static_cast<ServerContext*>(parser->data);
        if (context->request) {
            String::CPtr chunk = String::create(at, String::ASCII, length);
            JsArray::Ptr args = JsArray::create();
            args->add(chunk);
            context->request->emit(ServerRequest::EVENT_DATA, args);
        }
        return 0;
    }

    static int onMessageComplete(http_parser* parser) {
        ServerContext* context = static_cast<ServerContext*>(parser->data);
        if (context->request) {
            JsArray::Ptr args = JsArray::create();
            context->request->emit(ServerRequest::EVENT_END, args);
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

LIBJ_NULL_CPTR(String, ServerImpl::headerName);
http_parser_settings ServerImpl::settings = {};

const String::CPtr Server::IN_ADDR_ANY = String::create("0.0.0.0");
const String::CPtr Server::EVENT_REQUEST = String::create("request");
const String::CPtr Server::EVENT_CONNECTION = String::create("connection");
const String::CPtr Server::EVENT_CLOSE = String::create("close");
const String::CPtr Server::EVENT_UPGRADE = String::create("upgrade");

Server::Ptr Server::create() {
    return ServerImpl::create();
}

}  // namespace http
}  // namespace node
}  // namespace libj
