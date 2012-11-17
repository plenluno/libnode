// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/http.h"
#include "libnode/url.h"

#include "./http/client_request_impl.h"
#include "./http/outgoing_message.h"

namespace libj {
namespace node {
namespace http {

Agent::Ptr globalAgent() {
    static Agent::Ptr agent = Agent::create();
    return agent;
}

Server::Ptr createServer(JsFunction::Ptr requestListener) {
    Server::Ptr srv = Server::create();
    if (requestListener) {
        srv->on(Server::EVENT_REQUEST, requestListener);
    }
    return srv;
}

ClientRequest::Ptr request(
    const Value& options,
    JsFunction::Ptr callback) {
    LIBJ_STATIC_SYMBOL_DEF(symHttp,     "http:");
    LIBJ_STATIC_SYMBOL_DEF(symProtocol, "protocol");

    JsObject::CPtr opt = JsObject::null();
    String::CPtr url = toCPtr<String>(options);
    if (url) {
        opt = url::parse(url);
    } else {
        opt = toCPtr<JsObject>(options);
    }

    if (opt) {
        String::CPtr protocol = opt->getCPtr<String>(symProtocol);
        if (protocol && !protocol->equals(symHttp)) {
            return ClientRequest::null();
        }
    }

    OutgoingMessage::Ptr msg = OutgoingMessage::create(opt, callback);
    return ClientRequestImpl::create(msg);
}

ClientRequest::Ptr get(
    const Value& options,
    JsFunction::Ptr callback) {
    ClientRequest::Ptr req = request(options, callback);
    req->end();
    return req;
}

}  // namespace http
}  // namespace node
}  // namespace libj
