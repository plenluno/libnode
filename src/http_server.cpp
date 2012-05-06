// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/http_server.h"
#include <uv.h>
#include <http_parser.h>

namespace libj {
namespace node {
namespace http {

class ServerImpl : public Server {
 private:
     struct Client {
       uv_tcp_t tcp;
       http_parser parser;
       uv_write_t write;
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
    static uv_buf_t* responseBuf;
    
    static void onConnection(uv_stream_t* server, int status) {
        if (!settings) {
            settings = static_cast<http_parser_settings*>(
                        malloc(sizeof(http_parser_settings)));
            settings->on_message_begin = 0;
            settings->on_header_field = 0;
            settings->on_header_value = 0;
            settings->on_url = 0;
            settings->on_body = 0;
            settings->on_headers_complete = ServerImpl::onHeadersComplete;
            settings->on_message_complete = 0;
        }
        
        if (!responseBuf) {
            const char response[] =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 6\r\n"
                "\r\n"
                "hello\n";
            responseBuf = static_cast<uv_buf_t*>(malloc(sizeof(uv_buf_t)));
            responseBuf->base = static_cast<char*>(malloc(sizeof(response)));
            responseBuf->len = sizeof(response);
            strncpy(responseBuf->base, response, sizeof(response));
        }
        
        Client* client = new Client();
        
        uv_tcp_init(uv_default_loop(), &client->tcp);
        if (uv_accept(server, reinterpret_cast<uv_stream_t*>(&client->tcp)))
            return;
            
        http_parser_init(&client->parser, HTTP_REQUEST);

        client->tcp.data = client;
        client->parser.data = client;

        uv_read_start(
            reinterpret_cast<uv_stream_t*>(&client->tcp),
            ServerImpl::onAlloc,
            ServerImpl::onRead);
    }
    
    static uv_buf_t onAlloc(uv_handle_t* handle, size_t suggestedSize) {
        uv_buf_t buf;
        buf.base = static_cast<char*>(malloc(suggestedSize));
        buf.len = suggestedSize;
        return buf;
    }
    
    static void onRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
        Client* client = static_cast<Client*>(stream->data);
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
    
    static void onClose(uv_handle_t* handle) {
        free(handle);
    }
    
    static int onHeadersComplete(http_parser* parser) {
        Client* client = static_cast<Client*>(parser->data);
        uv_write(
            &client->write,
            reinterpret_cast<uv_stream_t*>(&client->tcp),
            responseBuf,
            1,
            ServerImpl::afterWrite);
        return 1;
    }
    
    static void afterWrite(uv_write_t* write, int status) {
        uv_close(
            reinterpret_cast<uv_handle_t*>(write->handle),
            ServerImpl::onClose);
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
uv_buf_t* ServerImpl::responseBuf = 0;

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
