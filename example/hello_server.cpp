// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/http.h>
#include <libnode/node.h>
#include <libj/js_closure.h>

namespace libj {
namespace node {
namespace example {

class OnRequest : LIBJ_JS_FUNCTION(OnRequest)
    virtual Value operator()(JsArray::Ptr args) {
        LIBJ_STATIC_CONST_STRING_DEF(TEXT_PLAIN,  "text/plain");
        LIBJ_STATIC_CONST_STRING_DEF(HELLO_WORLD, "Hello World\n");

        http::ServerResponse::Ptr res = args->getPtr<http::ServerResponse>(1);
        res->setHeader(http::HEADER_CONTENT_TYPE, TEXT_PLAIN);
        res->end(HELLO_WORLD);
        return UNDEFINED;
    }
};

void helloServer() {
    http::Server::Ptr srv =
        http::Server::create(JsFunction::Ptr(new OnRequest()));
    srv->listen(1337, String::create("127.0.0.1"));
    node::run();
}

}  // namespace exampel
}  // namespace node
}  // namespace libj

int main() {
    namespace node = libj::node;
    node::example::helloServer();
    return 0;
}
