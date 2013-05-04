// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/http.h>
#include <libnode/url.h>
#include <libnode/detail/http/client_request.h>

namespace libj {
namespace node {
namespace http {

Agent::Ptr globalAgent() {
    return detail::http::globalAgent();
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
    LIBJ_STATIC_SYMBOL_DEF(symHttp, "http:");

    JsObject::CPtr opts = JsObject::null();
    String::CPtr url = toCPtr<String>(options);
    if (url) {
        opts = url::parse(url);
    } else {
        opts = toCPtr<JsObject>(options);
    }

    if (opts) {
        String::CPtr protocol = opts->getCPtr<String>(url::PROTOCOL);
        if (protocol && !protocol->equals(symHttp)) {
            return ClientRequest::null();
        }
    }

    return detail::http::ClientRequest::create(opts, callback);
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
