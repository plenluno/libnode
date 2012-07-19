// Copyright (c) 2012 Plenluno All rights reserved.

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
        if (isOpen_ || !hostName || backlog <= 0)
            return false;
        std::string addr;
        for (Size i = 0; i < hostName->length(); i++)
            addr += static_cast<char>(hostName->charAt(i));
        server_.data = this;
        isOpen_ = !(uv_tcp_bind(
                    &server_,
                    uv_ip4_addr(addr.c_str(), port))) &&
                  !uv_listen(
                    reinterpret_cast<uv_stream_t*>(&server_),
                    backlog,
                    ServerImpl::onConnection);
        return isOpen_;
    }

    void close() {
        if (isOpen_) {
            uv_close(
                reinterpret_cast<uv_handle_t*>(&server_),
                ServerImpl::onClose);
            isOpen_ = false;
        }
    }

 private:
    static http_parser_settings settings;

    static void onConnection(uv_stream_t* stream, int status) {
        if (!settings.on_url) {
            settings.on_url = ServerImpl::onUrl;
            settings.on_header_field = ServerImpl::onHeaderField;
            settings.on_header_value = ServerImpl::onHeaderValue;
            settings.on_headers_complete = ServerImpl::onHeadersComplete;
            settings.on_message_begin = ServerImpl::onMessageBegin;
            settings.on_body = ServerImpl::onBody;
            settings.on_message_complete = ServerImpl::onMessageComplete;
        }

        ServerImpl* server = static_cast<ServerImpl*>(stream->data);
        ServerContext* context = new ServerContext(server);
        uv_tcp_t* tcp = context->socket->getUvTcp();

        if (uv_accept(stream, reinterpret_cast<uv_stream_t*>(tcp)))
            return;

        http_parser_init(&context->parser, HTTP_REQUEST);

        tcp->data = context;
        context->parser.data = context;

        uv_read_start(
            reinterpret_cast<uv_stream_t*>(tcp),
            ServerImpl::onAlloc,
            ServerImpl::onRead);

        JsArray::Ptr args = JsArray::create();
        args->add(context->socket);
        server->emit(EVENT_CONNECTION, args);
    }

    static uv_buf_t onAlloc(uv_handle_t* handle, size_t suggestedSize) {
        uv_buf_t buf;
        buf.base = static_cast<char*>(malloc(suggestedSize));
        buf.len = suggestedSize;
        return buf;
    }

    static void onClose(uv_handle_t* handle) {
    }

    static void onRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
        ServerContext* context = static_cast<ServerContext*>(stream->data);
        if (!context->request) {
            ServerRequestImpl::Ptr p(new ServerRequestImpl(context));
            context->request = p;
        }
        if (!context->response) {
            ServerResponseImpl::Ptr p(new ServerResponseImpl(context));
            context->response = p;
        }

        if (nread >= 0) {
            size_t parsed = http_parser_execute(
                                &context->parser,
                                &settings,
                                buf.base,
                                nread);
            if (parsed < static_cast<size_t>(nread)) {
                // parse error
                uv_close(
                    reinterpret_cast<uv_handle_t*>(stream),
                    ServerImpl::onClose);
            }
        } else {
            // uv_err_t err = uv_last_error(uv_default_loop());
            // assert(err.code == UV_EOF);
            uv_close(
                reinterpret_cast<uv_handle_t*>(stream),
                ServerImpl::onClose);
        }
        free(buf.base);
    }

    static int onUrl(http_parser* parser, const char* at, size_t length) {
        ServerContext* context = static_cast<ServerContext*>(parser->data);
        context->request->setUrl(String::create(at, String::ASCII, length));
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
        ServerImpl* server = static_cast<ServerImpl*>(context->server);
        server->emit(Server::EVENT_REQUEST, args);
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
    uv_tcp_t server_;
    EventEmitter::Ptr ee_;
    bool isOpen_;

    ServerImpl()
        : ee_(EventEmitter::create())
        , isOpen_(false) {
        uv_tcp_init(uv_default_loop(), &server_);
    }

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
