// Copyright (c) 2012 Plenluno All rights reserved.

#include "libj/json.h"
#include "libnode/http_server.h"
#include "./http_server_request_impl.h"
#include "./http_server_response_impl.h"
#include <uv.h>
#include <http_parser.h>

namespace libj {
namespace node {
namespace http {

class ServerImpl : public Server {
 private:
    class Client {
     public:
        Client(ServerImpl* srv)
            : server(srv)
            , request(static_cast<ServerRequestImpl*>(0))
            , response(static_cast<ServerResponseImpl*>(0)) {
        }
        
        uv_tcp_t tcp;
        http_parser parser;
        uv_write_t write;
        ServerImpl* server;
        Type<ServerRequestImpl>::Ptr request;
        Type<ServerResponseImpl>::Ptr response;
    };

 public:
    static Ptr create() {
        Ptr p(new ServerImpl());
        return p;
    }

    static Ptr create(Type<JsFunction>::Ptr requestListener) {
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
    static http_parser_settings* settings;
    
    static void onConnection(uv_stream_t* stream, int status) {
        if (!settings) {
            settings = static_cast<http_parser_settings*>(
                        malloc(sizeof(http_parser_settings)));
            settings->on_url = ServerImpl::onUrl;
            settings->on_header_field = ServerImpl::onHeaderField;
            settings->on_header_value = ServerImpl::onHeaderValue;
            settings->on_headers_complete = ServerImpl::onHeadersComplete;
            settings->on_message_begin = ServerImpl::onMessageBegin;
            settings->on_body = ServerImpl::onBody;
            settings->on_message_complete = ServerImpl::onMessageComplete;
        }
        
        ServerImpl* server = static_cast<ServerImpl*>(stream->data);
        Client* client = new Client(server);
        
        uv_tcp_init(uv_default_loop(), &client->tcp);
        if (uv_accept(stream, reinterpret_cast<uv_stream_t*>(&client->tcp)))
            return;
            
        http_parser_init(&client->parser, HTTP_REQUEST);

        client->tcp.data = client;
        client->parser.data = client;

        uv_read_start(
            reinterpret_cast<uv_stream_t*>(&client->tcp),
            ServerImpl::onAlloc,
            ServerImpl::onRead);
        
        Type<JsArray>::Ptr args = JsArray::create();
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
        free(handle);
    }
    
    static void onRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
        Client* client = static_cast<Client*>(stream->data);
        if (!client->request)
            client->request = ServerRequestImpl::create();
        if (!client->response)
            client->response = ServerResponseImpl::create(
                                    &client->write,
                                    reinterpret_cast<uv_stream_t*>(&client->tcp));

        if (nread >= 0) {
            size_t parsed = http_parser_execute(
                                &client->parser,
                                settings,
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
            assert(err.code == UV_EOF);
            uv_close(
                reinterpret_cast<uv_handle_t*>(stream),
                ServerImpl::onClose);
        }
        free(buf.base);
    }
    
    static int onUrl(http_parser* parser, const char* at, size_t length) {
        Client* client = static_cast<Client*>(parser->data);
        client->request->setUrl(String::create(at, String::ASCII, length));
        return 0;
    }
    
    static Type<String>::Cptr headerName;
    
    static int onHeaderField(http_parser* parser, const char* at, size_t length) {
        Client* client = static_cast<Client*>(parser->data);
        headerName = String::create(at, String::ASCII, length);
        return 0;
    }
    
    static int onHeaderValue(http_parser* parser, const char* at, size_t length) {
        Client* client = static_cast<Client*>(parser->data);
        client->request->setHeader(headerName, String::create(at, String::ASCII, length));
        return 0;
    }
    
    static int onHeadersComplete(http_parser* parser) {
        Client* client = static_cast<Client*>(parser->data);
        Type<JsArray>::Ptr args = JsArray::create();
        Type<ServerRequest>::Ptr req = client->request;
        Type<ServerResponse>::Ptr res = client->response;
        args->add(req);
        args->add(res);
        client->server->emit(Server::EVENT_REQUEST, args);
        return 0;
    }
    
    static int onMessageBegin(http_parser* parser) {
        return 0;
    }
    
    static int onBody(http_parser* parser, const char* at, size_t length) {
        Client* client = static_cast<Client*>(parser->data);
        Type<String>::Cptr chunk = String::create(at, String::ASCII, length);
        Type<JsArray>::Ptr args = JsArray::create();
        args->add(chunk);
        client->request->emit(ServerRequest::EVENT_DATA, args);
        return 0;
    }
    
    static int onMessageComplete(http_parser* parser) {
        Client* client = static_cast<Client*>(parser->data);
        Type<JsArray>::Ptr args = JsArray::create();
        client->request->emit(ServerRequest::EVENT_END, args);
        // TypeId id;
        // std::cout
        //     << static_cast<const char*>(json::stringify(client->request)->data(&id))
        //     << std::endl;
        return 0;
    }
    
 private:
    uv_tcp_t server_;
    Type<EventEmitter>::Ptr ee_;
    
    ServerImpl()
        : ee_(EventEmitter::create()) {
        uv_tcp_init(uv_default_loop(), &server_);
    }

    LIBNODE_EVENT_EMITTER_IMPL(ee_);
};

http_parser_settings* ServerImpl::settings = 0;
LIBJ_NULL_CPTR(String, ServerImpl::headerName);

Type<String>::Cptr Server::EVENT_REQUEST = String::create("request");
Type<String>::Cptr Server::EVENT_CONNECTION = String::create("connection");
Type<String>::Cptr Server::EVENT_CLOSE = String::create("close");

Type<Server>::Ptr Server::create() {
    return ServerImpl::create();
}

Type<Server>::Ptr Server::create(Type<JsFunction>::Ptr requestListener) {
    return ServerImpl::create(requestListener);
}

}  // namespace http
}  // namespace node
}  // namespace libj
