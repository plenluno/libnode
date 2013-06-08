// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/http.h>
#include <libnode/node.h>
#include <libnode/trace.h>
#include <libj/js_closure.h>

namespace libj {
namespace node {
namespace example {

void helloServer() {
    auto srv = http::Server::create(
        JsClosure::create([] (JsArray::Ptr args) {
            LIBJ_STATIC_CONST_STRING_DEF(TEXT_PLAIN,  "text/plain");
            LIBJ_STATIC_CONST_STRING_DEF(HELLO_WORLD, "Hello World\n");

            auto res = args->getPtr<http::ServerResponse>(1);
            res->setHeader(http::HEADER_CONTENT_TYPE, TEXT_PLAIN);
            res->end(HELLO_WORLD);
            return UNDEFINED;
        }));
    srv->listen(1337, String::create("127.0.0.1"));
    node::run();
}

}  // namespace exampel
}  // namespace node
}  // namespace libj

int main() {
    LIBNODE_DEBUG_TRACE_ON;
    namespace node = libj::node;
    node::example::helloServer();
    LIBNODE_DEBUG_TRACE_OFF;
    return 0;
}
