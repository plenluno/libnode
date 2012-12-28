// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_HTTP_IMPL_CLIENT_REQUEST_H_
#define LIBNODE_DETAIL_HTTP_IMPL_CLIENT_REQUEST_H_

#include <libnode/http/option.h>
#include <libnode/detail/http/agent.h>

#include <libj/cast.h>

namespace libj {
namespace node {
namespace detail {
namespace http {

inline OutgoingMessage::Ptr OutgoingMessage::createInClient(
    libj::JsObject::CPtr options,
    libj::JsFunction::Ptr callback) {
    LIBJ_STATIC_SYMBOL_DEF(OPTION_SET_HOST,     "setHost");
    LIBJ_STATIC_SYMBOL_DEF(OPTION_DEFAULT_PORT, "defaultPort");
    LIBJ_STATIC_SYMBOL_DEF(sym80,               "80");
    LIBJ_STATIC_SYMBOL_DEF(symSlash,            "/");
    LIBJ_STATIC_SYMBOL_DEF(symLocalhost,        "localhost");

    OutgoingMessage::Ptr self(new OutgoingMessage());

    if (!options) options = libj::JsObject::create();

    self->agent_ =
        options->getPtr<node::http::Agent>(node::http::OPTION_AGENT);
    if (!self->agent_) self->agent_ = node::http::globalAgent();

    String::CPtr defaultPort = sym80;
    const Value& vDefaultPort = options->get(OPTION_DEFAULT_PORT);
    if (!vDefaultPort.isUndefined()) {
        String::CPtr sDefaultPort = String::valueOf(vDefaultPort);
        if (sDefaultPort) defaultPort = sDefaultPort;
    }

    String::CPtr port = defaultPort;
    const Value& vPort = options->get(node::http::OPTION_PORT);
    if (!vPort.isUndefined()) {
        String::CPtr sPort = String::valueOf(vPort);
        if (sPort) port = sPort;
    }

    String::CPtr host = options->getCPtr<String>(node::http::OPTION_HOSTNAME);
    if (!host) host = options->getCPtr<String>(node::http::OPTION_HOST);
    if (!host) host = symLocalhost;

    String::CPtr method = options->getCPtr<String>(node::http::OPTION_METHOD);
    if (method) {
        method = method->toUpperCase();
    } else {
        method = node::http::METHOD_GET;
    }
    self->method_ = method;

    self->path_ = options->getCPtr<String>(node::http::OPTION_PATH);
    if (!self->path_) self->path_ = symSlash;

    if (callback) {
        self->once(ClientRequest::EVENT_RESPONSE, callback);
    }

    libj::JsObject::CPtr headers =
        options->getCPtr<libj::JsObject>(node::http::OPTION_HEADERS);
    if (headers) {
        Set::CPtr keys = headers->keySet();
        Iterator::Ptr itr = keys->iterator();
        while (itr->hasNext()) {
            String::CPtr key = toCPtr<String>(itr->next());
            self->setHeader(key, headers->getCPtr<String>(key));
        }
    }

    Boolean setHost = !options->getCPtr<String>(OPTION_SET_HOST);
    if (host && !self->getHeader(node::http::LHEADER_HOST) && setHost) {
        if (port->equals(defaultPort)) {
            self->setHeader(node::http::HEADER_HOST, host);
        } else {
            StringBuffer::Ptr hostHeader = StringBuffer::create();
            hostHeader->append(host);
            hostHeader->appendChar(':');
            hostHeader->append(port);
            self->setHeader(node::http::HEADER_HOST, hostHeader->toString());
        }
    }

    String::CPtr auth = options->getCPtr<String>(node::http::OPTION_AUTH);
    if (auth && !self->getHeader(node::http::HEADER_AUTHORIZATION)) {
        Buffer::Ptr authBuf = Buffer::create(auth);
        StringBuffer::Ptr basicAuth = StringBuffer::create();
        basicAuth->appendCStr("Basic ");
        basicAuth->append(authBuf->toString(Buffer::BASE64));
        self->setHeader(
            node::http::HEADER_AUTHORIZATION,
            basicAuth->toString());
    }

    if (method->equals(node::http::METHOD_GET) ||
        method->equals(node::http::METHOD_HEAD) ||
        method->equals(node::http::METHOD_CONNECT)) {
        self->unsetFlag(OutgoingMessage::USE_CHUNKED_ENCODING_BY_DEFAULT);
    } else {
        self->setFlag(OutgoingMessage::USE_CHUNKED_ENCODING_BY_DEFAULT);
    }

    if (self->getHeader(node::http::HEADER_EXPECT)) {
        StringBuffer::Ptr firstLine = StringBuffer::create();
        firstLine->append(method);
        firstLine->appendChar(' ');
        firstLine->append(self->path_);
        firstLine->appendCStr(" HTTP/1.1\r\n");
        self->storeHeader(firstLine->toString(), self->renderHeaders());
    }

    self->socketPath_ =
        options->getCPtr<String>(node::http::OPTION_SOCKET_PATH);
    if (self->socketPath_) {
        self->setFlag(OutgoingMessage::LAST);
        self->unsetFlag(OutgoingMessage::SHOULD_KEEP_ALIVE);

        net::Socket::Ptr sock = net::Socket::create();
        sock->connect(self->socketPath_);
        self->onSocket(sock);
    } else if (self->agent_) {
        self->unsetFlag(OutgoingMessage::LAST);
        self->setFlag(OutgoingMessage::SHOULD_KEEP_ALIVE);
        http::Agent::Ptr agent =
            LIBJ_STATIC_PTR_CAST(http::Agent)(self->agent_);
        agent->addRequest(
            self,
            host,
            port,
            options->getCPtr<String>(node::http::OPTION_LOCAL_ADDRESS));
    } else {
        self->setFlag(OutgoingMessage::LAST);
        self->unsetFlag(OutgoingMessage::SHOULD_KEEP_ALIVE);

        libj::JsObject::Ptr opt = libj::JsObject::create();
        opt->put(node::http::OPTION_PORT, port);
        opt->put(node::http::OPTION_HOST, host);
        opt->put(
            node::http::OPTION_LOCAL_ADDRESS,
            options->getCPtr<String>(node::http::OPTION_LOCAL_ADDRESS));

        net::Socket::Ptr sock = net::Socket::create();
        sock->connect(opt);
        self->onSocket(sock);
    }

    JsFunction::Ptr flush(new Flush(&(*self)));
    self->deferToConnect(JsFunction::null(), flush);
    return self;
}

inline ClientRequest::Ptr ClientRequest::create(OutgoingMessage::Ptr msg) {
    if (msg) {
        return Ptr(new ClientRequest(msg));
    } else {
        return null();
    }
}

inline ClientRequest::Ptr ClientRequest::create(
    libj::JsObject::CPtr options,
    libj::JsFunction::Ptr callback) {
    return ClientRequest::create(
        OutgoingMessage::createInClient(options, callback));
}

}  // namespace http
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_HTTP_IMPL_CLIENT_REQUEST_H_
