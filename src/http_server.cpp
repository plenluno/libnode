// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/http_server.h"
#include "./http_server_context.h"

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
    
    bool listen(Int port) {
        server_.data = this;
        return !(uv_tcp_bind(
                    &server_,
                    uv_ip4_addr("0.0.0.0", port))) &&
               !uv_listen(
                    reinterpret_cast<uv_stream_t*>(&server_),
                    128,
                    ServerImpl::onConnection);
    }
    
    void close() {
        // TODO: implement
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
        
        uv_tcp_init(uv_default_loop(), &context->tcp);
        if (uv_accept(stream, reinterpret_cast<uv_stream_t*>(&context->tcp)))
            return;
            
        http_parser_init(&context->parser, HTTP_REQUEST);

        context->tcp.data = context;
        context->parser.data = context;

        uv_read_start(
            reinterpret_cast<uv_stream_t*>(&context->tcp),
            ServerImpl::onAlloc,
            ServerImpl::onRead);
        
        JsArray::Ptr args = JsArray::create();
        // TODO: add socket
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
        if (!context->request)
            context->request = new ServerRequestImpl(context);
        if (!context->response)
            context->response = new ServerResponseImpl(context);

        if (nread >= 0) {
            size_t parsed = http_parser_execute(
                                &context->parser,
                                &settings,
                                buf.base,
                                nread);
            if (parsed < nread) {
                // parse error
                uv_close(
                    reinterpret_cast<uv_handle_t*>(stream),
                    ServerImpl::onClose);
            }
        } else { 
            uv_err_t err = uv_last_error(uv_default_loop());
            //assert(err.code == UV_EOF);
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
    
    static int onHeaderField(http_parser* parser, const char* at, size_t length) {
        ServerContext* context = static_cast<ServerContext*>(parser->data);
        headerName = String::create(at, String::ASCII, length);
        return 0;
    }
    
    static int onHeaderValue(http_parser* parser, const char* at, size_t length) {
        ServerContext* context = static_cast<ServerContext*>(parser->data);
        context->request->setHeader(headerName, String::create(at, String::ASCII, length));
        return 0;
    }
    
    static int onHeadersComplete(http_parser* parser) {
        ServerContext* context = static_cast<ServerContext*>(parser->data);
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
    
    ServerImpl()
        : ee_(EventEmitter::create()) {
        uv_tcp_init(uv_default_loop(), &server_);
    }

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

LIBJ_NULL_CPTR(String, ServerImpl::headerName);
http_parser_settings ServerImpl::settings = {};

String::CPtr Server::EVENT_REQUEST = String::create("request");
String::CPtr Server::EVENT_CONNECTION = String::create("connection");
String::CPtr Server::EVENT_CLOSE = String::create("close");

Server::Ptr Server::create() {
    return ServerImpl::create();
}

Server::Ptr Server::create(JsFunction::Ptr requestListener) {
    return ServerImpl::create(requestListener);
}

}  // namespace http
}  // namespace node
}  // namespace libj
