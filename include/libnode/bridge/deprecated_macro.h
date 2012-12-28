// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_BRIDGE_DEPRECATED_MACRO_H_
#define LIBNODE_BRIDGE_DEPRECATED_MACRO_H_

#include <libj/bridge/deprecated_macro.h>

#define LIBNODE_EVENT_EMITTER_IMPL(EE) \
    LIBJ_JS_OBJECT_IMPL(EE); \
public: \
    void on(String::CPtr event, JsFunction::Ptr listener) { \
        EE->on(event, listener); \
    } \
    void once(String::CPtr event, JsFunction::Ptr listener) { \
        EE->once(event, listener); \
    } \
    void addListener(String::CPtr event, JsFunction::Ptr listener) { \
        EE->addListener(event, listener); \
    } \
    void removeListener(String::CPtr event, JsFunction::CPtr listener) { \
        EE->removeListener(event, listener); \
    } \
    void removeAllListeners() { \
        EE->removeAllListeners(); \
    } \
    void removeAllListeners(String::CPtr event) { \
        EE->removeAllListeners(event); \
    } \
    JsArray::Ptr listeners(String::CPtr event) { \
        return EE->listeners(event); \
    } \
    void emit(String::CPtr event, JsArray::Ptr args = JsArray::null()) { \
        EE->emit(event, args); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1) { \
        EE->emit(event, v1); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2) { \
        EE->emit(event, v1, v2); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2, const Value& v3) { \
        EE->emit(event, v1, v2, v3); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2, const Value& v3, \
        const Value& v4) { \
        EE->emit(event, v1, v2, v3, v4); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2, const Value& v3, \
        const Value& v4, const Value& v5) { \
        EE->emit(event, v1, v2, v3, v4, v5); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2, const Value& v3, \
        const Value& v4, const Value& v5, const Value& v6) { \
        EE->emit(event, v1, v2, v3, v4, v5, v6); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2, const Value& v3, \
        const Value& v4, const Value& v5, const Value& v6, \
        const Value& v7) { \
        EE->emit(event, v1, v2, v3, v4, v5, v6, v7); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2, const Value& v3, \
        const Value& v4, const Value& v5, const Value& v6, \
        const Value& v7, const Value& v8) { \
        EE->emit(event, v1, v2, v3, v4, v5, v6, v7, v8); \
    } \
    void emit( \
        String::CPtr event, \
        const Value& v1, const Value& v2, const Value& v3, \
        const Value& v4, const Value& v5, const Value& v6, \
        const Value& v7, const Value& v8, const Value& v9) { \
        EE->emit(event, v1, v2, v3, v4, v5, v6, v7, v8, v9); \
    }

#define LIBNODE_STREAM_IMPL(S) \
    LIBNODE_EVENT_EMITTER_IMPL(S); \
public: \
    virtual Boolean destroy() { \
        return S->destroy(); \
    }

#define LIBNODE_READABLE_STREAM_IMPL(S) \
    LIBNODE_STREAM_IMPL(S); \
public: \
    virtual Boolean readable() const { \
        return S->readable(); \
    } \
    virtual Boolean pause() { \
        return S->pause(); \
    } \
    virtual Boolean resume() { \
        return S->resume(); \
    } \
    virtual Boolean setEncoding(Buffer::Encoding enc) { \
        return S->setEncoding(enc); \
    }

#define LIBNODE_WRITABLE_STREAM_IMPL(S) \
    LIBNODE_STREAM_IMPL(S); \
public: \
    virtual Boolean writable() const { \
        return S->writable(); \
    } \
    virtual Boolean write( \
        const Value& data, \
        Buffer::Encoding enc = Buffer::NONE) { \
        return S->write(data, enc); \
    } \
    virtual Boolean end( \
        const Value& data = UNDEFINED, \
        Buffer::Encoding enc = Buffer::NONE) { \
        return S->end(data, enc); \
    } \
    virtual Boolean destroySoon() { \
        return S->destroySoon(); \
    }

#define LIBNODE_DUPLEX_STREAM_IMPL(S) \
    LIBNODE_STREAM_IMPL(S); \
public: \
    virtual Boolean readable() const { \
        return S->readable(); \
    } \
    virtual Boolean pause() { \
        return S->pause(); \
    } \
    virtual Boolean resume() { \
        return S->resume(); \
    } \
    virtual Boolean setEncoding(Buffer::Encoding enc) { \
        return S->setEncoding(enc); \
    } \
    virtual Boolean writable() const { \
        return S->writable(); \
    } \
    virtual Boolean write( \
        const Value& data, \
        Buffer::Encoding enc = Buffer::NONE) { \
        return S->write(data, enc); \
    } \
    virtual Boolean end( \
        const Value& data = UNDEFINED, \
        Buffer::Encoding enc = Buffer::NONE) { \
        return S->end(data, enc); \
    } \
    virtual Boolean destroySoon() { \
        return S->destroySoon(); \
    }

#define LIBNODE_NET_SERVER_IMPL(S) \
    LIBNODE_EVENT_EMITTER_IMPL(S) \
    virtual Value address() { \
        return S->address(); \
    } \
    virtual Size connections() const { \
        return S->connections(); \
    } \
    virtual Size maxConnections() const { \
        return S->maxConnections(); \
    } \
    virtual void setMaxConnections(Size max) { \
        return S->setMaxConnections(max); \
    } \
    virtual Boolean listen( \
        Int port, \
        String::CPtr host = IN_ADDR_ANY, \
        Int backlog = 511, \
        JsFunction::Ptr callback = JsFunction::null()) { \
        return S->listen(port, host, backlog, callback); \
    } \
    virtual Boolean listen( \
        String::CPtr path, \
        JsFunction::Ptr callback = JsFunction::null()) { \
        return S->listen(path, callback); \
    } \
    virtual Boolean close( \
        JsFunction::Ptr callback = JsFunction::null()) { \
        return S->close(callback); \
    }

#define LIBNODE_NET_SOCKET_IMPL(S) \
    LIBNODE_DUPLEX_STREAM_IMPL(S); \
public: \
    virtual JsObject::Ptr address() { \
        return S->address(); \
    } \
    virtual String::CPtr remoteAddress() { \
        return S->remoteAddress(); \
    } \
    virtual Int remotePort() { \
        return S->remotePort(); \
    } \
    virtual Boolean setNoDelay(Boolean noDelay = true) { \
        return S->setNoDelay(noDelay); \
    } \
    virtual Boolean setKeepAlive( \
        Boolean enable = false, UInt initialDelay = 0) { \
        return S->setKeepAlive(enable, initialDelay); \
    } \
    virtual void setTimeout( \
        UInt timeout, \
        JsFunction::Ptr callback = JsFunction::null()) { \
        S->setTimeout(timeout, callback); \
    }

#define LIBNODE_HTTP_SERVER_REQUEST_IMPL(SR) \
    LIBNODE_READABLE_STREAM_IMPL(SR); \
public: \
    String::CPtr method() const { \
        return SR->method(); \
    } \
    String::CPtr url() const { \
        return SR->url(); \
    } \
    JsObject::CPtr headers() const { \
        return SR->headers(); \
    } \
    String::CPtr httpVersion() const { \
        return SR->httpVersion(); \
    } \
    net::Socket::Ptr connection() const { \
        return SR->connection(); \
    }

#define LIBNODE_HTTP_SERVER_RESPONSE_IMPL(SR) \
    LIBNODE_WRITABLE_STREAM_IMPL(SR); \
public: \
    virtual void writeHead( \
        Int statusCode, \
        String::CPtr reasonPhrase = String::null(), \
        JsObject::CPtr headers = JsObject::null()) { \
        SR->writeHead(statusCode, reasonPhrase, headers); \
    } \
    virtual Int statusCode() const { \
        return SR->statusCode(); \
    } \
    virtual void setHeader(String::CPtr name, String::CPtr value) { \
        SR->setHeader(name, value); \
    } \
    virtual String::CPtr getHeader(String::CPtr name) const { \
        return SR->getHeader(name); \
    } \
    virtual void removeHeader(String::CPtr name) { \
        SR->removeHeader(name); \
    }

#define LIBNODE_HTTP_CLIENT_REQUEST_IMPL(CR) \
    LIBNODE_WRITABLE_STREAM_IMPL(CR); \
public: \
    virtual void abort() { \
        CR->abort(); \
    } \
    virtual void setNoDelay(Boolean noDelay = true) { \
        CR->setNoDelay(noDelay); \
    } \
    virtual void setSocketKeepAlive( \
        Boolean enable = false, UInt initialDelay = 0) { \
        CR->setSocketKeepAlive(enable, initialDelay); \
    } \
    virtual void setTimeout( \
        UInt timeout, \
        JsFunction::Ptr callback = JsFunction::null()) { \
        CR->setTimeout(timeout, callback); \
    }

#define LIBNODE_HTTP_CLIENT_RESPONSE_IMPL(CR) \
    LIBNODE_READABLE_STREAM_IMPL(CR); \
public: \
    virtual Int statusCode() const { \
        return CR->statusCode(); \
    } \
    virtual String::CPtr httpVersion() const { \
        return CR->httpVersion(); \
    } \
    virtual JsObject::CPtr headers() const { \
        return CR->headers(); \
    } \
    virtual JsObject::CPtr trailers() const { \
        return CR->trailers(); \
    }

#define LIBNODE_NET_SOCKET(T) \
    public libj::node::net::Socket { \
    LIBJ_MUTABLE_DEFS(T, libj::node::net::Socket)

#define LIBNODE_HTTP_SERVER_REQUEST(T) \
    public libj::node::http::ServerRequest { \
    LIBJ_MUTABLE_DEFS(T, libj::node::http::ServerRequest)

#define LIBNODE_HTTP_SERVER_RESPONSE(T) \
    public libj::node::http::ServerResponse { \
    LIBJ_MUTABLE_DEFS(T, libj::node::http::ServerResponse)

#define LIBNODE_HTTP_CLIENT_REQUEST(T) \
    public libj::node::http::ClientRequest { \
    LIBJ_MUTABLE_DEFS(T, libj::node::http::ClientRequest)

#define LIBNODE_HTTP_CLIENT_RESPONSE(T) \
    public libj::node::http::ClientResponse { \
    LIBJ_MUTABLE_DEFS(T, libj::node::http::ClientResponse)

#endif  // LIBNODE_BRIDGE_DEPRECATED_MACRO_H_
