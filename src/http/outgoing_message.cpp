// Copyright (c) 2012 Plenluno All rights reserved.

#include <libj/cast.h>

#include "./agent_impl.h"
#include "./outgoing_message.h"

namespace libj {
namespace node {
namespace http {

OutgoingMessage::Ptr OutgoingMessage::createInServer(
    IncomingMessage::Ptr req) {
    LIBJ_STATIC_SYMBOL_DEF(symChunked, "chunked");

    OutgoingMessage::Ptr self(new OutgoingMessage());
    self->setFlag(OutgoingMessage::SERVER_SIDE);
    self->setFlag(OutgoingMessage::SEND_DATE);

    if (req->method()->equals(METHOD_HEAD)) {
        self->unsetFlag(OutgoingMessage::HAS_BODY);
    }

    if (req->httpVersionMajor() < 1 ||
        (req->httpVersionMajor() == 1 &&
         req->httpVersionMinor() == 0)) {
        self->unsetFlag(OutgoingMessage::SHOULD_KEEP_ALIVE);
        if (req->getHeader(HEADER_TE)->equals(symChunked)) {
            self->setFlag(OutgoingMessage::USE_CHUNKED_ENCODING_BY_DEFAULT);
        }
    }
    return self;
}

OutgoingMessage::Ptr OutgoingMessage::createInClient(
    JsObject::CPtr options,
    JsFunction::Ptr callback) {
    LIBJ_STATIC_SYMBOL_DEF(symAgent,        "agent");
    LIBJ_STATIC_SYMBOL_DEF(symMethod,       "method");
    LIBJ_STATIC_SYMBOL_DEF(symPath,         "path");
    LIBJ_STATIC_SYMBOL_DEF(symSlash,        "/");
    LIBJ_STATIC_SYMBOL_DEF(symAuth,         "auth");
    LIBJ_STATIC_SYMBOL_DEF(symPort,         "port");
    LIBJ_STATIC_SYMBOL_DEF(sym80,           "80");
    LIBJ_STATIC_SYMBOL_DEF(symDefaultPort,  "defaultPort");
    LIBJ_STATIC_SYMBOL_DEF(symHost,         "host");
    LIBJ_STATIC_SYMBOL_DEF(symHostName,     "hostname");
    LIBJ_STATIC_SYMBOL_DEF(symLocalHost,    "localhost");
    LIBJ_STATIC_SYMBOL_DEF(symSetHost,      "setHost");
    LIBJ_STATIC_SYMBOL_DEF(symSocketPath,   "socketPath");
    LIBJ_STATIC_SYMBOL_DEF(symHeaders,      "headers");
    LIBJ_STATIC_SYMBOL_DEF(symLocalAddress, "localAddress");
    LIBJ_STATIC_SYMBOL_DEF(EVENT_RESPONSE,  "response");

    OutgoingMessage::Ptr self(new OutgoingMessage());

    if (!options) options = JsObject::create();

    self->agent_ = options->getPtr<Agent>(symAgent);
    if (!self->agent_) self->agent_ = globalAgent();

    String::CPtr defaultPort = sym80;
    const Value& vDefaultPort = options->get(symDefaultPort);
    if (!vDefaultPort.isUndefined()) {
        String::CPtr sDefaultPort = String::valueOf(vDefaultPort);
        if (sDefaultPort) defaultPort = sDefaultPort;
    }

    String::CPtr port = defaultPort;
    const Value& vPort = options->get(symPort);
    if (!vPort.isUndefined()) {
        String::CPtr sPort = String::valueOf(vPort);
        if (sPort) port = sPort;
    }

    String::CPtr host = options->getCPtr<String>(symHostName);
    if (!host) host = options->getCPtr<String>(symHost);
    if (!host) host = symLocalHost;

    String::CPtr method = options->getCPtr<String>(symMethod);
    if (method) {
        method = method->toUpperCase();
    } else {
        method = METHOD_GET;
    }
    self->method_ = method;

    self->path_ = options->getCPtr<String>(symPath);
    if (!self->path_) self->path_ = symSlash;

    if (callback) self->once(EVENT_RESPONSE, callback);

    JsObject::CPtr headers = options->getCPtr<JsObject>(symHeaders);
    if (headers) {
        Set::CPtr keys = headers->keySet();
        Iterator::Ptr itr = keys->iterator();
        while (itr->hasNext()) {
            String::CPtr key = toCPtr<String>(itr->next());
            self->setHeader(key, headers->getCPtr<String>(key));
        }
    }

    Boolean setHost = !options->getCPtr<String>(symSetHost);
    if (host && !self->getHeader(LHEADER_HOST) && setHost) {
        if (port->equals(defaultPort)) {
            self->setHeader(HEADER_HOST, host);
        } else {
            StringBuffer::Ptr hostHeader = StringBuffer::create();
            hostHeader->append(host);
            hostHeader->appendChar(':');
            hostHeader->append(port);
            self->setHeader(HEADER_HOST, hostHeader->toString());
        }
    }

    String::CPtr auth = options->getCPtr<String>(symAuth);
    if (auth && !self->getHeader(HEADER_AUTHORIZATION)) {
        Buffer::Ptr authBuf = Buffer::create(auth);
        StringBuffer::Ptr basicAuth = StringBuffer::create();
        basicAuth->appendCStr("Basic ");
        basicAuth->append(authBuf->toString(Buffer::BASE64));
        self->setHeader(HEADER_AUTHORIZATION, basicAuth->toString());
    }

    if (method->equals(METHOD_GET) ||
        method->equals(METHOD_HEAD) ||
        method->equals(METHOD_CONNECT)) {
        self->unsetFlag(OutgoingMessage::USE_CHUNKED_ENCODING_BY_DEFAULT);
    } else {
        self->setFlag(OutgoingMessage::USE_CHUNKED_ENCODING_BY_DEFAULT);
    }

    if (self->getHeader(HEADER_EXPECT)) {
        StringBuffer::Ptr firstLine = StringBuffer::create();
        firstLine->append(method);
        firstLine->appendChar(' ');
        firstLine->append(self->path_);
        firstLine->appendCStr(" HTTP/1.1\r\n");
        self->storeHeader(firstLine->toString(), self->renderHeaders());
    }

    self->socketPath_ = options->getCPtr<String>(symSocketPath);
    if (self->socketPath_) {
        self->setFlag(OutgoingMessage::LAST);
        self->unsetFlag(OutgoingMessage::SHOULD_KEEP_ALIVE);

        net::SocketImpl::Ptr sock = net::SocketImpl::create();
        sock->connect(self->socketPath_);
        self->onSocket(sock);
    } else if (self->agent_) {
        self->unsetFlag(OutgoingMessage::LAST);
        self->setFlag(OutgoingMessage::SHOULD_KEEP_ALIVE);
        AgentImpl::Ptr agent = LIBJ_STATIC_PTR_CAST(AgentImpl)(self->agent_);
        agent->addRequest(
            self,
            host,
            port,
            options->getCPtr<String>(symLocalAddress));
    } else {
        self->setFlag(OutgoingMessage::LAST);
        self->unsetFlag(OutgoingMessage::SHOULD_KEEP_ALIVE);

        JsObject::Ptr opt = JsObject::create();
        opt->put(symPort, port);
        opt->put(symHost, host);
        opt->put(symLocalAddress, options->getCPtr<String>(symLocalAddress));

        net::SocketImpl::Ptr sock = net::SocketImpl::create();
        sock->connect(opt);
        self->onSocket(sock);
    }

    JsFunction::Ptr flush(new Flush(&(*self)));
    self->deferToConnect(JsFunction::null(), flush);
    return self;
}

}  // namespace http
}  // namespace node
}  // namespace libj
