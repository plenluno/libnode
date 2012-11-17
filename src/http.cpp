// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/http.h"

#include "./http/client_request_impl.h"
#include "./http/outgoing_message.h"

namespace libj {
namespace node {
namespace http {

Agent::Ptr globalAgent = Agent::create();

Server::Ptr createServer(JsFunction::Ptr requestListener) {
    Server::Ptr srv = Server::create();
    if (requestListener) {
        srv->on(Server::EVENT_REQUEST, requestListener);
    }
    return srv;
}

ClientRequest::Ptr request(
    JsObject::CPtr options,
    JsFunction::Ptr callback) {
    LIBJ_STATIC_SYMBOL_DEF(symHttp,     "http:");
    LIBJ_STATIC_SYMBOL_DEF(symProtocol, "protocol");

    String::CPtr protocol = options->getCPtr<String>(symProtocol);
    if (protocol && !protocol->equals(symHttp)) {
        return ClientRequest::null();
    }

    OutgoingMessage::Ptr msg = OutgoingMessage::create(options, callback);
    return ClientRequestImpl::create(msg);
}

ClientRequest::Ptr get(
    JsObject::CPtr options,
    JsFunction::Ptr callback) {
    ClientRequest::Ptr req = request(options, callback);
    req->end();
    return req;
}

}  // namespace http
}  // namespace node
}  // namespace libj
