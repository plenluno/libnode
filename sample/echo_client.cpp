// Copyright (c) 2012 Plenluno All rights reserved.

#include <libj/console.h>
#include <libj/json.h>
#include <libnode/buffer.h>
#include <libnode/http.h>
#include <libnode/node.h>
#include <libnode/url.h>

namespace libj {
namespace node {
namespace echo {

class OnData : LIBJ_JS_FUNCTION(OnData)
 public:
    Value operator()(JsArray::Ptr args) {
        console::log(args->get(0));
        return Status::OK;
    }
};

class OnEnd : LIBJ_JS_FUNCTION(OnEnd)
 public:
    Value operator()(JsArray::Ptr args) {
        console::log(String::create("event 'end'"));
        return Status::OK;
    }
};

class OnClose :  LIBJ_JS_FUNCTION(OnClose)
 public:
    Value operator()(JsArray::Ptr args) {
        console::log(String::create("event 'close'"));
        return Status::OK;
    }
};

class OnResponse : LIBJ_JS_FUNCTION(OnResponse)
 public:
    Value operator()(JsArray::Ptr args) {
        http::ClientResponse::Ptr res =
            toPtr<http::ClientResponse>(args->get(0));

        OnData::Ptr onData(new OnData());
        OnEnd::Ptr onEnd(new OnEnd());
        OnClose::Ptr onClose(new OnClose());
        res->setEncoding(Buffer::UTF8);
        res->on(http::ClientResponse::EVENT_DATA, onData);
        res->on(http::ClientResponse::EVENT_END, onEnd);
        res->on(http::ClientResponse::EVENT_CLOSE, onClose);

        console::printv(console::NORMAL,
            "status: %v\n", res->statusCode());
        console::printv(console::NORMAL,
            "version: HTTP/%v\n", res->httpVersion());
        console::printv(console::NORMAL,
            "headers: %v\n", json::stringify(res->headers()));
        return Status::OK;
    }
};

}  // namespace echo
}  // namespace node
}  // namespace libj

int main() {
    namespace node = libj::node;
    namespace http = node::http;
    namespace url = node::url;
    namespace echo = node::echo;

    libj::String::CPtr body = libj::String::create("xyz");

    libj::JsObject::Ptr headers = libj::JsObject::create();
    headers->put(
        http::HEADER_CONNECTION,
        libj::String::create("close"));
    headers->put(
        http::HEADER_CONTENT_LENGTH,
        libj::String::valueOf(node::Buffer::byteLength(body)));

    libj::JsObject::Ptr options = url::parse(
        libj::String::create("http://127.0.0.1:10000/abc"));
    options->put(libj::String::create("headers"), headers);

    echo::OnResponse::Ptr onResponse(new echo::OnResponse());
    http::ClientRequest::Ptr req = http::request(options, onResponse);
    req->write(body);
    req->end();
    node::run();
    return 0;
}
