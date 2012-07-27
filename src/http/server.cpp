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

    static Ptr create(JsFunction::Ptr requestListener) {
        Ptr p = create();
        p->on(EVENT_REQUEST, requestListener);
        return p;
    }

    Boolean listen(Int port, String::CPtr hostName, Int backlog) {
        server_->on(
            net::Server::EVENT_LISTENING,
            OnListening::create());
        server_->on(
            net::Server::EVENT_CONNECTION,
            OnConnection::create(this));
        return server_->listen(port, hostName, backlog);
    }

    void close() {
        server_->close();
    }

 private:
    static http_parser_settings settings;

    class OnListening : LIBJ_JS_FUNCTION(OnListening)
     public:
        static Ptr create() {
            Ptr p(new OnListening());
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

    class OnConnection : LIBJ_JS_FUNCTION(OnConnection)
     public:
        static Ptr create(ServerImpl* srv) {
            Ptr p(new OnConnection(srv));
            return p;
        }

        Value operator()(JsArray::Ptr args) {
            net::SocketImpl::Ptr socket = toPtr<net::SocketImpl>(args->get(0));
            assert(socket);
            ServerContext* context = new ServerContext(server_, socket);
            http_parser_init(&context->parser, HTTP_REQUEST);
            socket->on(net::Socket::EVENT_DATA, OnData::create(context));
            return Status::OK;
        }

     private:
        ServerImpl* server_;

        OnConnection(ServerImpl* srv) : server_(srv) {}
    };

    class OnData : LIBJ_JS_FUNCTION(OnData)
     public:
        static Ptr create(ServerContext* ctxt) {
            Ptr p(new OnData(ctxt));
            return p;
        }

        Value operator()(JsArray::Ptr args) {
            Buffer::Ptr buf = toPtr<Buffer>(args->get(0));
            assert(buf);
            size_t numRead = buf->length();
            size_t numParsed = http_parser_execute(
                                &context_->parser,
                                &settings,
                                static_cast<const char*>(buf->data()),
                                numRead);
            if (numParsed < numRead) {
                // parse error
                context_->socket->destroy();  // TODO(plenluno): uv_close
            }
            return Status::OK;
        }

     private:
        ServerContext* context_;

        OnData(ServerContext* ctxt) : context_(ctxt) {}
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

Server::Ptr Server::create() {
    return ServerImpl::create();
}

Server::Ptr Server::create(JsFunction::Ptr requestListener) {
    return ServerImpl::create(requestListener);
}

}  // namespace http
}  // namespace node
}  // namespace libj
